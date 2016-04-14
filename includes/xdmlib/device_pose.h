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

#ifndef XMPMETA_XDM_DEVICE_POSE_H_
#define XMPMETA_XDM_DEVICE_POSE_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "xdmlib/element.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xdm {

/**
 * Implements the DevicePose element in the XDM specification, with
 * serialization and deserialization.
 * See xdm.org.
 */
class DevicePose : public Element {
 public:
  // Appends child elements' namespaces' and their respective hrefs to the
  // given collection, and any parent nodes' names to prefix_names.
  // Key: Name of the namespace.
  // Value: Full namespace URL.
  // Example: ("DevicePose", "http://ns.xdm.org/photos/1.0/devicepose/")
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  // Serializes this object. Returns true on success.
  bool Serialize(xml::Serializer* serializer) const override;

  // Creates a DevicePose from the given data.
  // The order of values in position is latitude, longitude, altitude..
  // The order of values in orientation is the rotation x, y, z angle, where
  // x, y, z are the rotation axis coordinates, and angle is the rotation angle
  // in radians.
  // rotation angle in angle-axis format (radians).
  // Position and orientation are in raw coordinates, and will be stored as
  // normalied values. Please refer to the XDM specification for the device
  // coordinate system.
  // At least one valid position or orientation must be provided. These
  // arguments will be ignored if the vector is of the wrong size.
  static std::unique_ptr<DevicePose>
      FromData(const std::vector<double>& position,
               const std::vector<double>& orientation,
               const int64 timestamp = -1);

  // Returns the deserialized XdmAudio; null if parsing fails.
  // The returned pointer is owned by the caller.
  static std::unique_ptr<DevicePose>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Returns true if the device's position is provided.
  bool HasPosition() const;

  // Returns true if the device's orientation is provided.
  bool HasOrientation() const;

  // Returns the device's position fields, or an empty vector if they are
  // not present.
  const std::vector<double>& GetPosition() const;

  // Returns the device's orientation fields, or an empty vector if they are
  // not present.
  const std::vector<double>& GetOrientationRotationXYZAngle() const;

  // Timestamp.
  int64 GetTimestamp() const;

  // Disallow copying.
  DevicePose(const DevicePose&) = delete;
  void operator=(const DevicePose&) = delete;

 private:
  DevicePose();

  // Extracts device pose fields.
  bool ParseDevicePoseFields(const xml::Deserializer& deserializer);

  // Position variables, in meters relative to camera 0.
  // If providing position data, all three fields must be set.
  // Stored in normalized form.
  std::vector<double> position_;

  // Orientation variables.
  // If providing orientation data, all four fields must be set.
  // Stored in normalized form.
  std::vector<double> orientation_;

  // Timestamp is Epoch time in milliseconds.
  int64 timestamp_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_DEVICE_POSE_H_
