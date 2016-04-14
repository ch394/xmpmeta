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

#ifndef XMPMETA_XDM_CAMERAS_H_
#define XMPMETA_XDM_CAMERAS_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xdmlib/camera.h"
#include "xdmlib/element.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xdm {

// Implements the Device:Cameras field from the XDM specification, with
// serialization and deserialization for its child Camera elements.
class Cameras : public Element {
 public:
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  bool Serialize(xml::Serializer* serializer) const override;

  // Creates this object from the given cameras. Returns null if the list is
  // empty.
  // If creation succeeds, ownership of the Camera objects are transferred to
  // the resulting Cameras object. The vector of Camera objects will be cleared.
  static std::unique_ptr<Cameras>
      FromCameraArray(std::vector<std::unique_ptr<Camera>>* cameras);

  // Returns the deserialized cameras in a Cameras object, null if parsing
  // failed for all the cameras.
  static std::unique_ptr<Cameras>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Returns the list of cameras.
  const std::vector<const Camera*> GetCameras() const;

  // Disallow copying.
  Cameras(const Cameras&) = delete;
  void operator=(const Cameras&) = delete;

 private:
  Cameras();

  std::vector<std::unique_ptr<Camera>> camera_list_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_CAMERAS_H_
