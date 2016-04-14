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

#ifndef XMPMETA_XDM_PROFILES_H_
#define XMPMETA_XDM_PROFILES_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xdmlib/profile.h"
#include "xdmlib/element.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xdm {

// Implements the Device:Profiles field from the XDM specification, with
// serialization and deserialization for its child Profile elements.
class Profiles : public Element {
 public:
  // Interface methods.
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  bool Serialize(xml::Serializer* serializer) const override;

  // Static methods.

  // Creates this object from the given profiles. If the list is empty, returns
  // a unique_ptr owning nothing.
  static std::unique_ptr<Profiles>
      FromProfileArray(std::vector<std::unique_ptr<Profile>>* profiles);

  // Returns the deserialized profiles in a Profiles object, a unique_ptr owning
  // nothing if parsing failed for all the profiles.
  static std::unique_ptr<Profiles>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Non-static methods.

  // Returns the list of cameras.
  const std::vector<const Profile*> GetProfiles() const;

  // Disallow copying
  Profiles(const Profiles&) = delete;
  void operator=(const Profiles&) = delete;

 private:
  Profiles() = default;

  std::vector<std::unique_ptr<Profile>> profile_list_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_PROFILES_H_
