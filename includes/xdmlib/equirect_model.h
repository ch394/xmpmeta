// xdmlib. A fast XDM parsing and writing library.
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

#ifndef XMPMETA_XDM_EQUIRECT_MODEL_H_
#define XMPMETA_XDM_EQUIRECT_MODEL_H_

#include <memory>
#include <unordered_map>

#include "xdmlib/dimension.h"
#include "xdmlib/element.h"
#include "xdmlib/point.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xdm {

// Implements the EquirectModel element in the XDM specification, with
// serialization and deserialization.
class EquirectModel : public Element {
 public:
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  bool Serialize(xml::Serializer* serializer) const override;

  // Creates an EquirectModel from the given fields. Returns null if
  // any of the required fields are not present (see fields below).
  // The order of numbers in cropped_origin is (left, top).
  // The order of numbers in cropped_size is (cropped width, cropped height).
  // The order of numbers in full_size = (full width, full height).
  static std::unique_ptr<EquirectModel>  FromData(const Point& cropped_origin,
                                                  const Dimension& cropped_size,
                                                  const Dimension& full_size);

  // Returns the deserialized equirect model, null if parsing fails.
  static std::unique_ptr<EquirectModel>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Getters.
  const Point& GetCroppedOrigin() const;
  const Dimension& GetCroppedSize() const;
  const Dimension& GetFullSize() const;

  // Disallow copying.
  EquirectModel(const EquirectModel&) = delete;
  void operator=(const EquirectModel&) = delete;

 private:
  EquirectModel(const Point& cropped_origin,
                const Dimension& cropped_size,
                const Dimension& full_size);

  // Cropped origin coordinates.
  // cropped_origin_.first: CroppedLeftAreaPixels
  // cropped_origin_.second: CroppedTopAreaPixels
  Point cropped_origin_;

  // Cropped size, in pixels.
  // cropped_size_.width: CroppedAreaImageWidthPixels
  // cropped_size_.height: CroppedAreaImageHeightPixels
  Dimension cropped_size_;

  // Full size, in pixels.
  // full_size_.width: FullImageAreaImageWidthPixels
  // full_size_.height: FullImageAreaImageHeightPixels
  Dimension full_size_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_EQUIRECT_MODEL_H_
