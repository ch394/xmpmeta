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

#ifndef XMPMETA_XDM_AUDIO_H_
#define XMPMETA_XDM_AUDIO_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xdmlib/element.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xdm {

// Implements the Audio element from the XDM specification, with
// serialization and deserialization.
class Audio : public Element {
 public:
  // Appends child elements' namespaces' and their respective hrefs to the
  // given collection, and any parent nodes' names to prefix_names.
  // Key: Name of the namespace.
  // Value: Full namespace URL.
  // Example: ("Audio", "http://ns.xdm.org/photos/1.0/audio/")
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  // Serializes this object.
  bool Serialize(xml::Serializer* serializer) const override;

  // Creates an Audio from the given fields. Returns null if
  // either field is empty. The fields are copied to the new object.
  // Data is NOT base64-encoded, and is the image data of the right eye.
  // Mime is the mimetype of the audio data.
  // The caller is given ownership of the returned pointer.
  static std::unique_ptr<Audio> FromData(const string& data,
                                         const string& mime);

  // Returns the deserialized Audio; null if parsing fails.
  // The returned pointer is owned by the caller.
  static std::unique_ptr<Audio>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Returns the Audio data, which has been base-64 decoded but is still
  // encoded according to the mime type of the Audio.
  const string& GetData() const;

  // Returns the Audio mime type.
  const string& GetMime() const;

  // Disallow copying.
  Audio(const Audio&) = delete;
  void operator=(const Audio&) = delete;

 private:
  Audio();

  // Extracts audio fields.
  bool ParseAudioFields(const xml::Deserializer& deserializer);

  string data_;  // The raw data, i.e. not base64-encoded.
  string mime_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_AUDIO_H_
