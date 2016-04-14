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

#include "xdmlib/audio.h"

#include "glog/logging.h"
#include "xmpmeta/base64.h"

using xmpmeta::xml::Deserializer;
using xmpmeta::xml::Serializer;

namespace xmpmeta {
namespace xdm {
namespace {

const char kPropertyPrefix[] = "Audio";
const char kMime[] = "Mime";
const char kData[] = "Data";
const char kMimeMp4[] = "audio/mp4";

const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/audio/";

}  // namespace

// Private constructor.
Audio::Audio() {}

// Public methods.
void Audio::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list or own namespace is null";
    return;
  }
  ns_name_href_map->emplace(kPropertyPrefix, kNamespaceHref);
}

// Public methods.
std::unique_ptr<Audio> Audio::FromData(const string& data, const string& mime) {
  if (data.empty() || mime.empty()) {
    LOG(ERROR) << "No audio data or mimetype given";
    return nullptr;
  }
  std::unique_ptr<Audio> audio(new Audio());
  audio->data_ = data;
  audio->mime_ = mime;
  return audio;
}

std::unique_ptr<Audio>
Audio::FromDeserializer(const Deserializer& parent_deserializer) {
  std::unique_ptr<Deserializer> deserializer =
      parent_deserializer.CreateDeserializer(kPropertyPrefix);
  if (deserializer == nullptr) {
    return nullptr;
  }
  std::unique_ptr<Audio> audio(new Audio());
  if (!audio->ParseAudioFields(*deserializer)) {
    return nullptr;
  }
  return audio;
}

const string& Audio::GetData() const { return data_; }

const string& Audio::GetMime() const { return mime_; }

bool Audio::Serialize(Serializer* serializer) const {
  if (serializer == nullptr) {
    LOG(ERROR) << "Serializer is null";
    return false;
  }
  string base64_encoded;
  if (!EncodeBase64(data_, &base64_encoded)) {
    return false;
  }
  if (!serializer->WriteProperty(kMime, mime_)) {
    return false;
  }
  return serializer->WriteProperty(kData, base64_encoded);
}

// Private methods.
bool Audio::ParseAudioFields(const Deserializer& deserializer) {
  if (!deserializer.ParseString(kMime, &mime_)) {
    return false;
  }
  return deserializer.ParseBase64(kData , &data_);
}

}  // namespace xdm
}  // namespace xmpmeta
