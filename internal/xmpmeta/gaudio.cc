// xmpmeta. A fast XMP metadata parsing and writing library.
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

#include "xmpmeta/gaudio.h"

#include <libxml/tree.h>

#include "glog/logging.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/xmp_parser.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::DeserializerImpl;
using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace {

const char kPrefix[] = "GAudio";
const char kMime[] = "Mime";
const char kData[] = "Data";
const char kNamespaceHref[] = "http://ns.google.com/photos/1.0/audio/";

}  // namespace

GAudio::GAudio() {}

void GAudio::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) const {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list or own namespace is null";
    return;
  }
  ns_name_href_map->emplace(kPrefix, kNamespaceHref);
}

const string& GAudio::GetData() const { return data_; }

const string& GAudio::GetMime() const { return mime_; }

std::unique_ptr<GAudio> GAudio::CreateFromData(const string& data,
                                               const string& mime) {
  if (data.empty() || mime.empty()) {
    return nullptr;
  }
  std::unique_ptr<GAudio> gaudio(new GAudio());
  gaudio->data_ = data;
  gaudio->mime_ = mime;
  return gaudio;
}

std::unique_ptr<GAudio> GAudio::FromXmp(const XmpData& xmp) {
  DeserializerImpl std_deserializer(XmlConst::RdfDescription(),
                                    GetFirstDescriptionElement(
                                        xmp.StandardSection()));
  std::unique_ptr<GAudio> gaudio(new GAudio());
  if (!std_deserializer.ParseString(kPrefix, kMime, &gaudio->mime_)) {
    return nullptr;
  }
  xml::DeserializerImpl
      ext_deserializer(XmlConst::RdfDescription(),
                       GetFirstDescriptionElement(xmp.ExtendedSection()));
  if (!ext_deserializer.ParseBase64(kPrefix, kData, &gaudio->data_)) {
    return nullptr;
  }
  return gaudio;
}

std::unique_ptr<GAudio> GAudio::FromJpegFile(const string& filename) {
  XmpData xmp;
  const bool kSkipExtended = false;
  if (!ReadXmpHeader(filename, kSkipExtended, &xmp)) {
    return nullptr;
  }
  return FromXmp(xmp);
}

bool GAudio::IsPresent(const XmpData& xmp) {
  DeserializerImpl std_deserializer(XmlConst::RdfDescription(),
                                    GetFirstDescriptionElement(
                                        xmp.StandardSection()));
  string mime;
  return std_deserializer.ParseString(kPrefix, kMime, &mime);
}

bool GAudio::IsPresent(const string& filename) {
  XmpData xmp;
  const bool kSkipExtended = true;
  if (!ReadXmpHeader(filename, kSkipExtended, &xmp)) {
    return false;
  }
  return IsPresent(xmp);
}

bool GAudio::Serialize(xml::Serializer* std_serializer,
                       xml::Serializer* ext_serializer) const {
  if (std_serializer == nullptr || ext_serializer == nullptr) {
    LOG(ERROR) << "Serializer for standard section or extended section is null";
    return false;
  }

  string encoded;
  if (!EncodeBase64(data_, &encoded)) {
    LOG(WARNING) << "Data encoding failed";
    return false;
  }

  if (!std_serializer->WritePropertyWithPrefix(kPrefix, kMime, mime_)) {
    return false;
  }
  return ext_serializer->WritePropertyWithPrefix(kPrefix, kData, encoded);
}

}  // namespace xmpmeta
