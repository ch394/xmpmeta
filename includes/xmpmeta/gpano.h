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

#ifndef XMPMETA_GPANO_H_
#define XMPMETA_GPANO_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xmpmeta/pano_meta_data.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {

class GPano {
 public:
  // Appends GPano's XML namespace name and href to the given collection.
  // Key: Name of the namespace.
  // Value: Full namespace URL.
  // Example: ("GPano", "http://ns.google.com/photos/1.0/panorama/").
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) const;

  // Creates a GPano from the given PanoMetaData.
  static std::unique_ptr<GPano>
      CreateFromData(const PanoMetaData& pano_meta_data);

  // Creates a GPano from pre-extracted XMP metadata. Returns null if parsing
  // fails. Extended XMP is not needed.
  static std::unique_ptr<GPano> FromXmp(const XmpData& xmp);

  // Creates a GPano by extracting XMP metadata from a JPEG and parsing it. If
  // using XMP for other things as well, FromXmp() should be used instead to
  // prevent redundant extraction of XMP from the JPEG.
  static std::unique_ptr<GPano> FromJpegFile(const string& filename);

  // Returns the GPano data formatted as PanoMetaData.
  const PanoMetaData& GetPanoMetaData() const;

  // Serializes properties to XML.
  bool Serialize(xml::Serializer* serializer) const;

  // Disallow copying.
  GPano(const GPano&) = delete;
  void operator=(const GPano&) = delete;

 private:
  GPano();

  xmpmeta::PanoMetaData meta_data_;
};

}  // namespace xmpmeta

#endif  // XMPMETA_GPANO_H_
