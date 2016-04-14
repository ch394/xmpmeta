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

#include "xdmlib/cameras.h"

#include "glog/logging.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {

const char kNodeName[] = "Cameras";
const char kCameraName[] = "Camera";

// Private constructor.
Cameras::Cameras() {}

// Public methods.
void Cameras::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr || camera_list_.empty()) {
    LOG(ERROR) << "Namespace list is null or camera list is empty";
    return;
  }
  for (const auto& camera : camera_list_) {
    camera->GetNamespaces(ns_name_href_map);
  }
}

std::unique_ptr<Cameras>
Cameras::FromCameraArray(std::vector<std::unique_ptr<Camera>>* camera_list) {
  if (camera_list == nullptr || camera_list->empty()) {
    LOG(ERROR) << "Camera list is empty";
    return nullptr;
  }
  std::unique_ptr<Cameras> cameras(new Cameras());
  cameras->camera_list_ = std::move(*camera_list);
  return cameras;
}

std::unique_ptr<Cameras>
Cameras::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Cameras> cameras(new Cameras());
  int i = 0;
  std::unique_ptr<Deserializer> deserializer =
      parent_deserializer.CreateDeserializerFromListElementAt(kNodeName, 0);
  while (deserializer) {
    std::unique_ptr<Camera> camera = Camera::FromDeserializer(*deserializer);
    if (camera == nullptr) {
      LOG(ERROR) << "Unable to deserialize a camera";
      return nullptr;
    }
    cameras->camera_list_.emplace_back(std::move(camera));
    deserializer =
        parent_deserializer.CreateDeserializerFromListElementAt(kNodeName, ++i);
  }

  if (cameras->camera_list_.empty()) {
    return nullptr;
  }
  return cameras;
}

const std::vector<const Camera*> Cameras::GetCameras() const {
  std::vector<const Camera*> camera_list;
  for (const auto& camera : camera_list_) {
    camera_list.push_back(camera.get());
  }
  return camera_list;
}

bool Cameras::Serialize(Serializer* serializer) const {
  if (camera_list_.empty()) {
    LOG(ERROR) << "Camera list is empty";
    return false;
  }
  std::unique_ptr<Serializer> cameras_serializer =
      serializer->CreateListSerializer(kNodeName);
  if (cameras_serializer == nullptr) {
    // Error is logged in Serializer.
    return false;
  }
  for (int i = 0; i < camera_list_.size(); i++) {
    std::unique_ptr<Serializer> camera_serializer =
        cameras_serializer->CreateItemSerializer(kCameraName);
    if (camera_serializer == nullptr) {
      LOG(ERROR) << "Could not create a list item serializer for Camera";
      return false;
    }
    if (!camera_list_[i]->Serialize(camera_serializer.get())) {
      LOG(ERROR) << "Could not serialize camera " << i;
      return false;
    }
  }
  return true;
}

}  // namespace xdm
}  // namespace xmpmeta
