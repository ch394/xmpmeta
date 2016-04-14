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

#ifndef XMPMETA_GAUDIO_H_
#define XMPMETA_GAUDIO_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {

class GAudio {
 public:
  // Appends GAudio's XML namespace name and href to the given collection.
  // Key: Name of the namespace.
  // Value: Full namespace URL.
  // Example: ("GAudio", "http://ns.google.com/photos/1.0/audio/").
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) const;

  // Creates a GAudio from the given fields. Returns null if either field is
  // empty.
  // data is the base64 decoded audio data.
  // mime is the audio mimetype, e.g. audio/mp4.
  static std::unique_ptr<GAudio> CreateFromData(const string& data,
                                                const string& mime);

  // Creates a GAudio from pre-extracted XMP metadata. Returns null if parsing
  // fails. Both standard and extended XMP are required.
  static std::unique_ptr<GAudio> FromXmp(const XmpData& xmp);

  // Creates a GAudio by extracting XMP metadata from a JPEG and parsing it. If
  // using XMP for other things as well, FromXmp() should be used instead to
  // prevent redundant extraction of XMP from the JPEG.
  static std::unique_ptr<GAudio> FromJpegFile(const string& filename);

  // Determines whether the requisite fields are present in the XMP metadata.
  // Only the Mime field is checked in order to make this fast. Therefore,
  // extended XMP is not needed.
  static bool IsPresent(const XmpData& xmp);

  // Same as above but extracts XMP metadata from a file instead of reusing
  // already extracted metadata.
  static bool IsPresent(const string& filename);

  // Returns the GAudio data, which has been base-64 decoded but is still
  // encoded according to the mime type of the GAudio.
  const string& GetData() const;

  // Returns the GAudio mime type.
  const string& GetMime() const;

  // Serializes properties to XML.
  // The first serializer is assumed to be for the standard section in XMP, and
  // the second for the extended section.
  bool Serialize(xml::Serializer* std_section_serializer,
                 xml::Serializer* ext_section_serializer) const;

  // Disallow copying.
  GAudio(const GAudio&) = delete;
  void operator=(const GAudio&) = delete;

 private:
  GAudio();

  // The base64 decoded data.
  string data_;
  string mime_;
};

}  // namespace xmpmeta

#endif  // XMPMETA_GAUDIO_H_
