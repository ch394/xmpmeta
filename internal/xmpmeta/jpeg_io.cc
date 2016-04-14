// xmpmeta. A fast XMP metadata parsing and writing library.
// Copyright 2016 Google Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: miraleung@google.com (Mira Leung)

#include "xmpmeta/jpeg_io.h"

#include <fstream>
#include <sstream>

#include "glog/logging.h"
#include "strings/util.h"

namespace xmpmeta {
namespace {

// File markers.
// See: http://www.fileformat.info/format/jpeg/egff.htm or
// https://en.wikipedia.org/wiki/JPEG
const int kSoi = 0xd8;   // Start of image marker.
const int kApp1 = 0xe1;  // Start of EXIF section.
const int kSos = 0xda;   // Start of scan.

// Number of bytes used to store a section's length in a JPEG file.
const int kSectionLengthByteSize = 2;

// Returns the number of bytes available to be read. Sets the seek position
// to the place it was in before calling this function.
int GetBytesAvailable(std::istream* input_stream) {
  const std::streampos pos = input_stream->tellg();
  input_stream->seekg(0, std::ios::end);
  const std::streamsize len = input_stream->tellg() - pos;
  input_stream->seekg(pos);
  return len;
}

// Returns the first byte in the stream cast to an integer.
size_t ReadByteAsInt(std::istream* input_stream) {
  unsigned char byte;
  if (!input_stream->read(reinterpret_cast<char*>(&byte), 1)) {
    // Return an invalid value - no byte can be read as -1.
    return 0;
  }
  return static_cast<int>(byte);
}

// Reads the length of a section from 2 bytes.
int Read2ByteLength(std::istream* input_stream) {
  const int length_high = ReadByteAsInt(input_stream);
  const int length_low = ReadByteAsInt(input_stream);
  if (length_high == -1 || length_low == -1) {
    return -1;
  }
  return length_high << 8 | length_low;
}

}  // namespace

Section::Section(const string& buffer) {
  marker = kApp1;
  is_image_section = false;
  data = buffer;
}

bool Section::IsMarkerApp1() {
  return marker == kApp1;
}

std::vector<Section> Parse(std::istream* input_stream, bool read_meta_only,
                           const string& section_header) {
  std::vector<Section> sections;
  // Return early if this is not the start of a JPEG section.
  if (ReadByteAsInt(input_stream) != 0xff ||
      ReadByteAsInt(input_stream) != kSoi) {
    LOG(WARNING) << "File's first two bytes does not match the sequence \xff"
                 << kSoi;
    return std::vector<Section>();
  }

  int chr;  // Short for character.
  while ((chr = ReadByteAsInt(input_stream)) != -1) {
    if (chr != 0xff) {
      LOG(WARNING) << "Read non-padding byte: " << chr;
      return sections;
    }
    // Skip padding bytes.
    while ((chr = ReadByteAsInt(input_stream)) == 0xff) {
    }
    if (chr == -1) {
      LOG(WARNING) << "No more bytes in file available to be read.";
      return sections;
    }

    const int marker = chr;
    if (marker == kSos) {
      // kSos indicates the image data will follow and no metadata after that,
      // so read all data at one time.
      if (!read_meta_only) {
        Section section;
        section.marker = marker;
        section.is_image_section = true;
        const int bytes_available = GetBytesAvailable(input_stream);
        section.data.resize(bytes_available);
       if (input_stream->read(&section.data[0], bytes_available) &&
          (section_header.empty() ||
           HasPrefixString(section.data, section_header))) {
          sections.push_back(section);
        }
      }
      // All sections have been read.
      return sections;
    }

    const size_t length = Read2ByteLength(input_stream);
    if (length < kSectionLengthByteSize) {
      // No sections to read.
      LOG(WARNING) << "No sections to read; section length is " << length;
      return sections;
    }

    if (!read_meta_only || marker == kApp1) {
      Section section;
      section.marker = marker;
      section.is_image_section = false;
      const size_t data_size = length - kSectionLengthByteSize;
      section.data.resize(data_size);
      if (section.data.size() != data_size) {
        LOG(WARNING) << "Discrepancy in section data size "
                     << section.data.size() << "and data size " << data_size;
        return sections;
      }
      if (input_stream->read(&section.data[0], section.data.size()) &&
          (section_header.empty() ||
           HasPrefixString(section.data, section_header))) {
        sections.push_back(section);
      }
    } else {
      // Skip this section since all EXIF/XMP meta will be in kApp1 section.
      input_stream->ignore(length - kSectionLengthByteSize);
    }
  }
  return sections;
}

void WriteSections(const std::vector<Section>& sections,
                   std::ostream* output_stream) {
  output_stream->put(0xff);
  output_stream->put(kSoi);
  for (const Section& section : sections) {
    output_stream->put(0xff);
    output_stream->put(section.marker);
    if (!section.is_image_section) {
      const int section_length = static_cast<int>(section.data.length()) + 2;
      // It's not the image data.
      const int lh = section_length >> 8;
      const int ll = section_length & 0xff;
      output_stream->put(lh);
      output_stream->put(ll);
    }
    output_stream->write(section.data.c_str(), section.data.length());
  }
}

}  // namespace xmpmeta
