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

#ifndef XMPMETA_GIMAGE_H_
#define XMPMETA_GIMAGE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {

class GImage {
 public:
  // Appends GImage's XML namespace name and href to the given collection.
  // Key: Name of the namespace.
  // Value: Full namespace URL.
  // Example: ("GImage", "http://ns.google.com/photos/1.0/image/").
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) const;

  // Creates a GImage from the given fields. Returns null if either field is
  // empty. The fields are copied to the new object.
  // Data is NOT base64-encoded, and is the image data of the right eye.
  // Mime is the mimetype of the image data, such as image/jpeg.
  static std::unique_ptr<GImage> CreateFromData(const string& data,
                                                const string& mime);

  // Creates a GImage from pre-extracted XMP metadata. Returns null if parsing
  // fails. Both standard and extended XMP are required.
  static std::unique_ptr<GImage> FromXmp(const XmpData& xmp);

  // Creates a GImage by extracting XMP metadata from a JPEG and parsing it. If
  // using XMP for other things as well, FromXmp() should be used instead to
  // prevent redundant extraction of XMP from the JPEG.
  static std::unique_ptr<GImage> FromJpegFile(const string& filename);

  // Determines whether the requisite fields are present in the XMP metadata.
  // Only the Mime field is checked in order to make this fast. Therefore,
  // extended XMP is not needed.
  static bool IsPresent(const XmpData& xmp);

  // Same as above but extracts XMP metadata from a file instead of reusing
  // already extracted metadata.
  static bool IsPresent(const string& filename);

  // Returns the GImage data, which has been base-64 decoded but is still
  // encoded according to the mime type of the GImage.
  const string& GetData() const;

  // Returns the GImage mime type.
  const string& GetMime() const;

  // Serializes properties to XML.
  // The first serializer is assumed to be for the standard section in XMP, and
  // the second for the extended section.
  bool Serialize(xml::Serializer* std_section_serializer,
                 xml::Serializer* ext_section_serializer) const;

  // Disallow copying.
  GImage(const GImage&) = delete;
  void operator=(const GImage&) = delete;

 private:
  GImage();

  string data_;
  string mime_;
};

}  // namespace xmpmeta

#endif  // XMPMETA_GIMAGE_H_
