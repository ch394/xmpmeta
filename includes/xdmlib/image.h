// Copyright 2016 The XMPMeta Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef XMPMETA_XDM_IMAGE_H_
#define XMPMETA_XDM_IMAGE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xdmlib/element.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xdm {

/**
 * An Image element for an XDM device.
 */
class Image : public Element {
 public:
  // Appends child elements' namespaces' and their respective hrefs to the
  // given collection, and any parent nodes' names to prefix_names.
  // Key: Name of the namespace.
  // Value: Full namespace URL.
  // Example: ("Image", "http://ns.xdm.org/photos/1.0/image/")
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  // Serializes this object.
  bool Serialize(xml::Serializer* serializer) const override;

  // Creates an Image from the given fields. Returns null if
  // either field is empty. The fields are copied to the new object.
  // Data is NOT base64-encoded, and is the image data of the right eye.
  // Mime is the mimetype of the image data.
  static std::unique_ptr<Image> FromData(const string& data,
                                         const string& mime);

  // Returns the deserialized Image; null if parsing fails.
  static std::unique_ptr<Image>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Returns the Image data, which has been base-64 decoded but is still
  // encoded according to the mime type of the Image.
  const string& GetData() const;

  // Returns the Image mime type.
  const string& GetMime() const;

  // Disallow copying.
  Image(const Image&) = delete;
  void operator=(const Image&) = delete;

 private:
  Image();

  // Extracts image fields.
  bool ParseImageFields(const xml::Deserializer& deserializer);

  string data_;  // The raw data, i.e. not base64-encoded.
  string mime_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_IMAGE_H_
