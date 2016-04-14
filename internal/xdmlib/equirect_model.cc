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

#include "xdmlib/equirect_model.h"

#include "glog/logging.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {
namespace {

const char kPropertyPrefix[] = "EquirectModel";
const char kNodePrefix[] = "ImagingModel";
const char kCroppedLeft[] = "CroppedAreaLeftPixels";
const char kCroppedTop[] = "CroppedAreaTopPixels";
const char kCroppedImageWidth[] = "CroppedAreaImageWidthPixels";
const char kCroppedImageHeight[] = "CroppedAreaImageHeightPixels";
const char kFullImageWidth[] = "FullImageWidthPixels";
const char kFullImageHeight[] = "FullImageHeightPixels";

const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/equirectmodel/";
const char kNodeNamespaceHref[] = "http://ns.xdm.org/photos/1.0/imagingmodel/";

std::unique_ptr<EquirectModel> ParseFields(const Deserializer& deserializer) {
  int cropped_left;
  int cropped_top;
  int cropped_image_width;
  int cropped_image_height;
  int full_image_width;
  int full_image_height;
  // Short-circuiting ensures unnecessary reads will not be performed.
  if (!deserializer.ParseInt(kCroppedLeft, &cropped_left) ||
      !deserializer.ParseInt(kCroppedTop, &cropped_top) ||
      !deserializer.ParseInt(kCroppedImageWidth, &cropped_image_width) ||
      !deserializer.ParseInt(kCroppedImageHeight, &cropped_image_height) ||
      !deserializer.ParseInt(kFullImageWidth, &full_image_width) ||
      !deserializer.ParseInt(kFullImageHeight, &full_image_height)) {
    return nullptr;
  }
  return EquirectModel::FromData(
      Point(cropped_left, cropped_top),
      Dimension(cropped_image_width, cropped_image_height),
      Dimension(full_image_width, full_image_height));
}

}  // namespace

// Private constructor.
EquirectModel::EquirectModel(const Point& cropped_origin,
                             const Dimension& cropped_size,
                             const Dimension& full_size) :
  cropped_origin_(cropped_origin), cropped_size_(cropped_size),
  full_size_(full_size) {}

// Public methods.
void EquirectModel::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list or own namespace is null";
    return;
  }
  ns_name_href_map->emplace(kNodePrefix, kNodeNamespaceHref);
  ns_name_href_map->emplace(kPropertyPrefix, kNamespaceHref);
}

std::unique_ptr<EquirectModel>
EquirectModel::FromData(const Point& cropped_origin,
                        const Dimension& cropped_size,
                        const Dimension& full_size) {
  return std::unique_ptr<EquirectModel>(
      new EquirectModel(cropped_origin, cropped_size, full_size));
}

std::unique_ptr<EquirectModel>
EquirectModel::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Deserializer> deserializer =
      parent_deserializer.CreateDeserializer(kPropertyPrefix);
  if (deserializer == nullptr) {
    return nullptr;
  }
  return ParseFields(*deserializer);
}

const Point&
EquirectModel::GetCroppedOrigin() const { return cropped_origin_; }

const Dimension&
EquirectModel::GetCroppedSize() const { return cropped_size_; }

const Dimension&
EquirectModel::GetFullSize() const { return full_size_; }

bool EquirectModel::Serialize(Serializer* serializer) const {
  if (serializer == nullptr) {
    LOG(ERROR) << "Serializer is null";
    return false;
  }

  // Short-circuiting ensures unnecessary writes will not be performed.
  if (!serializer->WriteProperty(kCroppedLeft,
                                 std::to_string(cropped_origin_.x)) ||
      !serializer->WriteProperty(kCroppedTop,
                                 std::to_string(cropped_origin_.y)) ||
      !serializer->WriteProperty(kCroppedImageWidth,
                                 std::to_string(cropped_size_.width)) ||
      !serializer->WriteProperty(kCroppedImageHeight,
                                 std::to_string(cropped_size_.height)) ||
      !serializer->WriteProperty(kFullImageWidth,
                                 std::to_string(full_size_.width)) ||
      !serializer->WriteProperty(kFullImageHeight,
                                 std::to_string(full_size_.height))) {
    return false;
  }
  return true;
}

}  // namespace xdm
}  // namespace xmpmeta
