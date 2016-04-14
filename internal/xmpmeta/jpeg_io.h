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

// Functions for doing parsing and writing on JPEG files.

#ifndef XMPMETA_JPEG_IO_H_
#define XMPMETA_JPEG_IO_H_

#include <string>
#include <vector>

#include "base/port.h"

namespace xmpmeta {

// Contains the data for a section in a JPEG file.
// A JPEG file contains many sections in addition to image data.
struct Section {
  // Constructors.
  Section() = default;
  explicit Section(const string& buffer);

  // Returns true if the section's marker matches an APP1 marker.
  bool IsMarkerApp1();

  int marker;
  bool is_image_section;
  string data;
};

// Parses the JPEG image file.
// If read_meta_only is true, keeps only the EXIF and XMP sections (with
// marker kApp1) and ignores others. Otherwise, keeps everything including
// image data.
// @param section_header The href that comes before an XMP section; ignored
// if empty.
std::vector<Section> Parse(std::istream* input_stream, bool read_meta_only,
                           const string& section_header);

// Writes JPEG data sections to a file.
void WriteSections(const std::vector<Section>& sections,
                   std::ostream* output_stream);

}  // namespace xmpmeta

#endif  // XMPMETA_JPEG_IO_H_
