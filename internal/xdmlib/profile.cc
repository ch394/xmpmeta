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

#include "xdmlib/profile.h"

#include "glog/logging.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {
namespace {

const char kPropertyPrefix[] = "Profile";
const char kType[] = "Type";
const char kCameraIndices[] = "CameraIndices";

const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/profile/";

// Profile type names.
const char kVrPhoto[] = "VRPhoto";
const char kDepthPhoto[] = "DepthPhoto";

// Profile camera indices' sizes.
const size_t kVrPhotoIndicesSize = 2;
const size_t kDepthPhotoIndicesSize = 1;

// Returns true if the type is unsupported, or if the type is supported in the
// XDM Profile element and the size of the camera indices matches that
// specified in the spec.
bool ValidateKnownTypeAndIndices(const string& type,
                                 size_t camera_indices_size) {
  bool isVrPhoto = (kVrPhoto == type);
  bool isDepthPhoto = (kDepthPhoto == type);
  if (!isVrPhoto && !isDepthPhoto) {
    return true;
  }
  bool matches =
      (isVrPhoto && camera_indices_size >= kVrPhotoIndicesSize) ||
      (isDepthPhoto && camera_indices_size >= kDepthPhotoIndicesSize);
  if (!matches) {
    LOG(WARNING) << "Size of camera indices for "
                 << (isVrPhoto ? kVrPhoto : kDepthPhoto)
                 << " must be at least "
                 << (isVrPhoto ? kVrPhotoIndicesSize : kDepthPhotoIndicesSize);
  }
  return matches;
}

}  // namespace

// Private constructor.
Profile::Profile(const string& type, const std::vector<int>& camera_indices) :
    type_(type), camera_indices_(camera_indices) {}

// Public methods.
void Profile::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list or own namespace is null";
    return;
  }
  ns_name_href_map->emplace(kPropertyPrefix, kNamespaceHref);
}

std::unique_ptr<Profile>
Profile::FromData(const string& type, const std::vector<int>& camera_indices) {
  if (type.empty()) {
    LOG(ERROR) << "Profile must have a type";
    return nullptr;
  }
  // Check that the camera indices' length is at least the size of that
  // specified for the type. This has no restrictions on unsupported profile
  // types.
  if (!ValidateKnownTypeAndIndices(type, camera_indices.size())) {
    return nullptr;
  }

  return std::unique_ptr<Profile>(new Profile(type, camera_indices));
}

std::unique_ptr<Profile>
Profile::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Deserializer> deserializer =
      parent_deserializer.CreateDeserializer(kPropertyPrefix);
  if (deserializer == nullptr) {
    return nullptr;
  }
  std::unique_ptr<Profile> profile(new Profile("", {}));
  if (!deserializer->ParseString(kType, &profile->type_)) {
    return nullptr;
  }
  deserializer->ParseIntArray(kCameraIndices, &profile->camera_indices_);
  if (!ValidateKnownTypeAndIndices(profile->type_,
                                   profile->camera_indices_.size())) {
    return nullptr;
  }
  return profile;
}

const string& Profile::GetType() const { return type_; }

const std::vector<int>& Profile::GetCameraIndices() const {
  return camera_indices_;
}

bool Profile::Serialize(Serializer* serializer) const {
  if (serializer == nullptr) {
    LOG(ERROR) << "Serializer is null";
    return false;
  }
  if (!serializer->WriteProperty(kType, type_)) {
    return false;
  }
  if (camera_indices_.empty()) {
    return true;
  }
  return serializer->WriteIntArray(kCameraIndices, camera_indices_);
}

}  // namespace xdm
}  // namespace xmpmeta
