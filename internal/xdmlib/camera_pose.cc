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

#include "xdmlib/camera_pose.h"

#include <math.h>

#include "glog/logging.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {
namespace {

const char kPropertyPrefix[] = "CameraPose";
const char kPositionX[] = "PositionX";
const char kPositionY[] = "PositionY";
const char kPositionZ[] = "PositionZ";
const char kRotationAxisX[] = "RotationAxisX";
const char kRotationAxisY[] = "RotationAxisY";
const char kRotationAxisZ[] = "RotationAxisZ";
const char kRotationAngle[] = "RotationAngle";
const char kTimestamp[] = "Timestamp";
const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/camerapose/";

const std::vector<double>
NormalizeAxisAngle(const std::vector<double>& coords) {
  if (coords.size() < 4) {
    return std::vector<double>();
  }
  double length = sqrt((coords[0] * coords[0]) +
                       (coords[1] * coords[1]) +
                       (coords[2] * coords[2]));
  const std::vector<double> normalized =
      { coords[0] / length, coords[1] / length, coords[2] / length, coords[3] };
  return normalized;
}

}  // namespace

// Private constructor.
CameraPose::CameraPose() : timestamp_(-1) {}

// Public methods.
void CameraPose::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list or own namespace is null";
    return;
  }
  ns_name_href_map->emplace(kPropertyPrefix, kNamespaceHref);
}

std::unique_ptr<CameraPose>
CameraPose::FromData(const std::vector<double>& position,
                     const std::vector<double>& orientation,
                     const int64 timestamp) {
  if (position.empty() && orientation.empty()) {
    LOG(ERROR) << "Either position or orientation must be provided";
    return nullptr;
  }

  std::unique_ptr<CameraPose> camera_pose(new CameraPose());
  if (position.size() >= 3) {
    camera_pose->position_ = position;
  }

  if (orientation.size() >= 4) {
    camera_pose->orientation_ = NormalizeAxisAngle(orientation);
  }

  if (timestamp >= 0) {
    camera_pose->timestamp_ = timestamp;
  }

  return camera_pose;
}

std::unique_ptr<CameraPose>
CameraPose::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Deserializer> deserializer =
      parent_deserializer.CreateDeserializer(kPropertyPrefix);
  if (deserializer == nullptr) {
    return nullptr;
  }
  std::unique_ptr<CameraPose> camera_pose(new CameraPose());
  if (!camera_pose->ParseCameraPoseFields(*deserializer)) {
    return nullptr;
  }
  return camera_pose;
}

bool CameraPose::HasPosition() const { return position_.size() == 3; }
bool CameraPose::HasOrientation() const { return orientation_.size() == 4; }

const std::vector<double>&
CameraPose::GetPositionXYZ() const { return position_; }

const std::vector<double>&
CameraPose::GetOrientationRotationXYZAngle() const { return orientation_; }

int64 CameraPose::GetTimestamp() const { return timestamp_; }

bool CameraPose::Serialize(Serializer* serializer) const {
  if (serializer == nullptr) {
    LOG(ERROR) << "Serializer is null";
    return false;
  }

  if (!HasPosition() && !HasOrientation()) {
    LOG(ERROR) << "Camera pose has neither position nor orientation";
    return false;
  }

  bool success = true;
  if (position_.size() == 3) {
    success &=
        serializer->WriteProperty(kPositionX, std::to_string(position_[0])) &&
        serializer->WriteProperty(kPositionY, std::to_string(position_[1])) &&
        serializer->WriteProperty(kPositionZ, std::to_string(position_[2]));
  }

  if (orientation_.size() == 4) {
    success &=
        serializer->WriteProperty(kRotationAxisX,
                                  std::to_string(orientation_[0])) &&
        serializer->WriteProperty(kRotationAxisY,
                                  std::to_string(orientation_[1])) &&
        serializer->WriteProperty(kRotationAxisZ,
                                  std::to_string(orientation_[2])) &&
        serializer->WriteProperty(kRotationAngle,
                                  std::to_string(orientation_[3]));
  }

  if (timestamp_ >= 0) {
    serializer->WriteProperty(kTimestamp, std::to_string(timestamp_));
  }

  return success;
}

// Private methods.
bool CameraPose::ParseCameraPoseFields(const Deserializer& deserializer) {
  double x, y, z;
  // If a position field is present, the rest must be as well.
  if (deserializer.ParseDouble(kPositionX, &x)) {
    if (!deserializer.ParseDouble(kPositionY, &y)) {
      return false;
    }
    if (!deserializer.ParseDouble(kPositionZ, &z)) {
      return false;
    }
    position_ = { x, y, z };
  }

  // Same for orientation.
  if (deserializer.ParseDouble(kRotationAxisX, &x)) {
    if (!deserializer.ParseDouble(kRotationAxisY, &y)) {
      return false;
    }
    if (!deserializer.ParseDouble(kRotationAxisZ, &z)) {
      return false;
    }
    double angle;
    if (!deserializer.ParseDouble(kRotationAngle, &angle)) {
      return false;
    }
    std::vector<double> axis_angle = { x, y, z, angle };
    orientation_ = NormalizeAxisAngle(axis_angle);
  }

  if (position_.size() < 3 && orientation_.size() < 4) {
    return false;
  }

  deserializer.ParseLong(kTimestamp, &timestamp_);
  return true;
}

}  // namespace xdm
}  // namespace xmpmeta
