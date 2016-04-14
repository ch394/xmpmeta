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

#include "xdmlib/profiles.h"

#include "glog/logging.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {
namespace {

const char kNodeName[] = "Profiles";
const char kProfileName[] = "Profile";

}  // namespace

void Profiles::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr || profile_list_.empty()) {
    LOG(ERROR) << "Namespace list is null or profile list is empty";
    return;
  }
  for (const auto& profile : profile_list_) {
    profile->GetNamespaces(ns_name_href_map);
  }
}

std::unique_ptr<Profiles> Profiles::FromProfileArray(
    std::vector<std::unique_ptr<Profile>>* profile_list) {
  if (profile_list->empty()) {
    LOG(ERROR) << "Profile list is empty";
    return nullptr;
  }
  std::unique_ptr<Profiles> profiles(new Profiles());
  profiles->profile_list_ = std::move(*profile_list);
  return profiles;
}

std::unique_ptr<Profiles>
Profiles::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Profiles> profiles(new Profiles());
  int i = 0;
  for (std::unique_ptr<Deserializer> deserializer =
       parent_deserializer.CreateDeserializerFromListElementAt(kNodeName, i);
       deserializer != nullptr;
       deserializer =
          parent_deserializer.CreateDeserializerFromListElementAt(kNodeName,
                                                                  ++i)) {
    std::unique_ptr<Profile> profile = Profile::FromDeserializer(*deserializer);
    if (profile != nullptr) {
      profiles->profile_list_.emplace_back(std::move(profile));
    }
  }

  if (profiles->profile_list_.empty()) {
    return nullptr;
  }
  return profiles;
}

const std::vector<const Profile*> Profiles::GetProfiles() const {
  std::vector<const Profile*> profile_list;
  for (const auto& profile : profile_list_) {
    profile_list.push_back(profile.get());
  }
  return profile_list;
}

bool Profiles::Serialize(Serializer* serializer) const {
  if (profile_list_.empty()) {
    LOG(ERROR) << "Profile list is empty";
    return false;
  }
  bool success = true;
  int i = 0;
  std::unique_ptr<Serializer> profiles_serializer =
      serializer->CreateListSerializer(kNodeName);
  if (profiles_serializer == nullptr) {
    // Error is logged in Serializer.
    return false;
  }
  for (const auto& profile : profile_list_) {
    std::unique_ptr<Serializer> profile_serializer =
        profiles_serializer->CreateItemSerializer(kProfileName);
    if (profile_serializer == nullptr) {
      continue;
    }
    success &= profile->Serialize(profile_serializer.get());
    if (!success) {
      LOG(ERROR) << "Could not serialize profile " << i;
    }
    ++i;
  }
  return success;
}

}  // namespace xdm
}  // namespace xmpmeta
