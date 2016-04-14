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

#include "xdmlib/point_cloud.h"

#include "glog/logging.h"
#include "strings/numbers.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {
namespace {

const char kPropertyPrefix[] = "PointCloud";
const char kCount[] = "Count";
const char kColor[] = "Color";
const char kPosition[] = "Position";
const char kMetric[] = "Metric";
const char kSoftware[] = "Software";

const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/pointcloud/";

}  // namespace

// Private constructor.
PointCloud::PointCloud() : count_(-1), metric_(false) { }

// Public methods.
void PointCloud::GetNamespaces(std::unordered_map<string,
                               string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list or own namespace is null";
    return;
  }
  ns_name_href_map->insert(std::pair<string, string>(kPropertyPrefix,
                                                     kNamespaceHref));
}

std::unique_ptr<PointCloud>
PointCloud::FromData(int count, const string& position, const string& color,
                     bool metric, const string& software) {
  if (position.empty()) {
    LOG(ERROR) << "No position data given";
    return nullptr;
  }
  std::unique_ptr<PointCloud> point_cloud(new PointCloud());
  point_cloud->count_ = count;
  point_cloud->position_ = position;
  point_cloud->metric_ = metric;
  point_cloud->color_ = color;
  point_cloud->software_ = software;
  return point_cloud;
}

std::unique_ptr<PointCloud>
PointCloud::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Deserializer> deserializer =
      parent_deserializer.CreateDeserializer(kPropertyPrefix);
  if (deserializer == nullptr) {
    return nullptr;
  }

  std::unique_ptr<PointCloud> point_cloud(new PointCloud());
  if (!point_cloud->ParseFields(*deserializer)) {
    return nullptr;
  }
  return point_cloud;
}

int PointCloud::GetCount() const { return count_; }
const string& PointCloud::GetPosition() const { return position_; }
const string& PointCloud::GetColor() const { return color_; }
bool PointCloud::GetMetric() const { return metric_; }
const string& PointCloud::GetSoftware() const { return software_; }

bool PointCloud::Serialize(Serializer* serializer) const {
  if (serializer == nullptr) {
    LOG(ERROR) << "Serializer is null";
    return false;
  }

  string base64_encoded_position;
  if (!EncodeBase64(position_, &base64_encoded_position)) {
    LOG(WARNING) << "Position encoding failed";
    return false;
  }

  // Write required fields.
  if (count_ < 0 || !serializer->WriteProperty(kCount, SimpleItoa(count_))) {
    return false;
  }
  if (!serializer->WriteProperty(kPosition, base64_encoded_position)) {
    return false;
  }

  // Write optional fields.
  serializer->WriteBoolProperty(kMetric, metric_);

  if (!color_.empty()) {
    string base64_encoded_color;
    if (!EncodeBase64(color_, &base64_encoded_color)) {
      LOG(ERROR) << "Base64 encoding of color failed";
    } else {
      serializer->WriteProperty(kColor, base64_encoded_color);
    }
  }

  if (!software_.empty()) {
    serializer->WriteProperty(kSoftware, software_);
  }
  return true;
}

// Private methods.
bool PointCloud::ParseFields(const Deserializer& deserializer) {
  // Required fields.
  if (!deserializer.ParseInt(kCount, &count_)) {
    return false;
  }
  if (!deserializer.ParseBase64(kPosition, &position_)) {
    return false;
  }

  // Optional fields.
  if (!deserializer.ParseBoolean(kMetric, &metric_)) {
    // Set it to the default value.
    metric_ = false;
  }
  deserializer.ParseBase64(kColor, &color_);
  deserializer.ParseString(kSoftware, &software_);
  return true;
}

}  // namespace xdm
}  // namespace xmpmeta
