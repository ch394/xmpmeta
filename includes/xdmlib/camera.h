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

#ifndef XMPMETA_XDM_CAMERA_H_
#define XMPMETA_XDM_CAMERA_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xdmlib/audio.h"
#include "xdmlib/camera_pose.h"
#include "xdmlib/image.h"
#include "xdmlib/element.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xdm {

// Implements the Camera element from the XDM specification, with
// serialization and deserialization.
class Camera : public Element {
 public:
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  bool Serialize(xml::Serializer* serializer) const override;

  // Creates a Camera from the given objects.
  // Optional Camera elements can be null.
  // TODO(miraleung): Describe restrictions here when all objects are
  // checked in.
  static std::unique_ptr<Camera>
      FromData(std::unique_ptr<Audio> audio, std::unique_ptr<Image> image,
               std::unique_ptr<CameraPose> camera_pose);

  // Returns the deserialized Camera object, null if parsing fails.
  // TODO(miraleung): Describe restrictions here when all objects are
  // checked in.
  static std::unique_ptr<Camera>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Getters.
  const Audio* GetAudio() const;
  const Image* GetImage() const;
  const CameraPose* GetCameraPose() const;

  // Disallow copying.
  Camera(const Camera&) = delete;
  void operator=(const Camera&) = delete;

 private:
  Camera() = default;

  bool ParseChildElements(const xml::Deserializer& deserializer);

  // TODO(miraleung): Add rest of the objects here when they're checked in.
  std::unique_ptr<Audio> audio_;
  std::unique_ptr<Image> image_;

  // Optional elements.
  std::unique_ptr<CameraPose> camera_pose_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_CAMERA_H_
