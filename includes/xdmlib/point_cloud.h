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

#ifndef XMPMETA_XDM_POINT_CLOUD_H_
#define XMPMETA_XDM_POINT_CLOUD_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "xdmlib/element.h"
#include "xmpmeta/xml/deserializer.h"
#include "xmpmeta/xml/serializer.h"

// Implements the Point Cloud element from the XDM specification, with
// serialization and deserialization.
namespace xmpmeta {
namespace xdm {

class PointCloud : public Element {
 public:
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) override;

  bool Serialize(xml::Serializer* serializer) const override;

  // Creates a Point Cloud from the given fields. Returns null if position is
  // empty. The first two arguments are required fields, the rest are optional.
  // If the color or software fields are empty, they will not be serialized.
  static std::unique_ptr<PointCloud>
      FromData(int count, const string& position, const string& color,
               bool metric, const string& software);

  // Returns the deserialized PointCloud; null if parsing fails.
  // The returned pointer is owned by the caller.
  static std::unique_ptr<PointCloud>
      FromDeserializer(const xml::Deserializer& parent_deserializer);

  // Getters.
  int GetCount() const;
  const string& GetPosition() const;  // Raw data, i.e. not base64 encoded.
  const string& GetColor() const;
  bool GetMetric() const;
  const string& GetSoftware() const;

  PointCloud(const PointCloud&) = delete;
  void operator=(const PointCloud&) = delete;

 private:
  PointCloud();

  bool ParseFields(const xml::Deserializer& deserializer);

  // Required fields.
  int count_;
  string position_;  // Raw data, i.e. not base64 encoded.

  // Optional fields.
  bool metric_;
  string color_;  // Raw data, i.e. not base64 encoded.
  string software_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_POINT_CLOUD_H_
