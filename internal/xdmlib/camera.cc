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

#include "xdmlib/camera.h"

#include "glog/logging.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {
namespace {

const char kPropertyPrefix[] = "Camera";
const char kAudio[] = "Audio";
const char kCameraPose[] = "CameraPose";
const char kImage[] = "Image";

const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/camera/";

}  // namespace

void Camera::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list is null";
    return;
  }
  ns_name_href_map->emplace(kPropertyPrefix, kNamespaceHref);
  if (audio_) {
    audio_->GetNamespaces(ns_name_href_map);
  }
  if (image_) {
    image_->GetNamespaces(ns_name_href_map);
  }
  if (camera_pose_) {
    camera_pose_->GetNamespaces(ns_name_href_map);
  }
}

std::unique_ptr<Camera>
Camera::FromData(std::unique_ptr<Audio> audio, std::unique_ptr<Image> image,
                 std::unique_ptr<CameraPose> camera_pose) {
  // TODO(miraleung): Add restrictive checks here for required elements
  // once all they are all checked in. Remove this check.
  if (audio == nullptr && image == nullptr) {
    LOG(ERROR) << "Camera must have at least one child element";
    return nullptr;
  }
  std::unique_ptr<Camera> camera(new Camera());
  camera->audio_ = std::move(audio);
  camera->image_ = std::move(image);
  camera->camera_pose_ = std::move(camera_pose);
  return camera;
}

std::unique_ptr<Camera>
Camera::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Deserializer> deserializer =
      parent_deserializer.CreateDeserializer(kPropertyPrefix);
  if (deserializer == nullptr) {
    return nullptr;
  }
  std::unique_ptr<Camera> camera(new Camera());
  if (!camera->ParseChildElements(*deserializer)) {
    return nullptr;
  }
  return camera;
}

const Audio* Camera::GetAudio() const { return audio_.get(); }
const Image* Camera::GetImage() const { return image_.get(); }
const CameraPose* Camera::GetCameraPose() const { return camera_pose_.get(); }

bool Camera::Serialize(Serializer* serializer) const {
  if (serializer == nullptr) {
    LOG(ERROR) << "Serializer is null";
    return false;
  }

  // At least one of the below elements are required, and hence must be
  // successfully serialized.
  bool success = false;
  if (audio_ != nullptr) {
    std::unique_ptr<Serializer> audio_serializer =
        serializer->CreateSerializer(kAudio);
    success |= audio_->Serialize(audio_serializer.get());
  }
  if (image_ != nullptr) {
    std::unique_ptr<Serializer> image_serializer =
        serializer->CreateSerializer(kImage);
    success |= image_->Serialize(image_serializer.get());
  }

  if (!success) {
    return false;
  }

  // Serialize optional elements.
  if (camera_pose_ != nullptr) {
    std::unique_ptr<Serializer> camera_pose_serializer =
        serializer->CreateSerializer(kCameraPose);
    success &= camera_pose_->Serialize(camera_pose_serializer.get());
  }

  return success;
}

// Private methods.
bool Camera::ParseChildElements(const Deserializer& deserializer) {
  // TODO(miraleung): Add restriction checks here once all elements are
  // checked in.

  // At least one of the elements below must be present in Camera, and hence
  // at least one of these parsings must be successful.
  bool success = false;
  std::unique_ptr<Audio> audio = Audio::FromDeserializer(deserializer);

  if (audio.get()) {
    success = true;
    audio_ = std::move(audio);
  }

  std::unique_ptr<Image> image = Image::FromDeserializer(deserializer);
  if (image.get()) {
    success = true;
    image_ = std::move(image);
  }

  if (!success) {
    return false;
  }

  // Parse optional elements.
  std::unique_ptr<CameraPose> camera_pose =
      CameraPose::FromDeserializer(deserializer);
  if (camera_pose.get()) {
    camera_pose_ = std::move(camera_pose);
  }

  return success;
}

}  // namespace xdm
}  // namespace xmpmeta
