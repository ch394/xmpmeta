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

#include "xmpmeta/vr_photo_writer.h"

#include <unordered_map>

#include <libxml/tree.h>

#include "base/port.h"
#include "glog/logging.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::Serializer;
using xmpmeta::xml::SerializerImpl;
using xmpmeta::xml::ToXmlChar;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace {

// Creates and populates a namespace map from a name, href mapping,
// with its name and href.
void PopulateNamespaceMap(
    const std::unordered_map<string, string>& ns_name_href_map,
    std::unordered_map<string, xmlNsPtr>* dest_map) {
  for (const auto& entry : ns_name_href_map) {
    if (dest_map->count(entry.first) == 0 && !entry.second.empty()) {
      dest_map->emplace(entry.first,
                        xmlNewNs(nullptr, ToXmlChar(entry.second.data()),
                                 ToXmlChar(entry.first.data())));
    }
  }
}

}  // namespace

bool WriteVrPhotoMetaToXmp(const GImage& gimage, const GPano& gpano,
                           const GAudio* gaudio, XmpData* xmp_data) {
  std::unordered_map<string, string> ns_name_href_map;
  gimage.GetNamespaces(&ns_name_href_map);
  if (gaudio != nullptr) {
    gaudio->GetNamespaces(&ns_name_href_map);
  }

  // Only GImage and GAudio data will be written to the extended section.
  std::unordered_map<string, xmlNsPtr> ext_namespaces;
  PopulateNamespaceMap(ns_name_href_map, &ext_namespaces);

  // The standard section will have GImage, GAudio, and GPano fields written to
  // it.
  gpano.GetNamespaces(&ns_name_href_map);
  std::unordered_map<string, xmlNsPtr> main_namespaces;
  PopulateNamespaceMap(ns_name_href_map, &main_namespaces);

  // Prefix map can be empty since new nodes will not be created on this
  // serializer.
  std::unordered_map<string, xmlNsPtr> prefixes;
  std::unique_ptr<SerializerImpl> main_serializer =
      SerializerImpl::FromDataAndSerializeNamespaces(
          main_namespaces, prefixes, XmlConst::RdfDescription(),
          GetFirstDescriptionElement(*xmp_data->MutableStandardSection()));
  if (!main_serializer) {
    return false;
  }

  if (!gpano.Serialize(main_serializer.get())) {
    LOG(ERROR) << "Could not serialize GPano to XmpData";
    return false;
  }

  std::unique_ptr<Serializer> ext_serializer =
      SerializerImpl::FromDataAndSerializeNamespaces(
          ext_namespaces, prefixes, XmlConst::RdfDescription(),
          GetFirstDescriptionElement(*xmp_data->MutableExtendedSection()));
  if (!ext_serializer) {
    return false;
  }

  if (gaudio != nullptr &&
      !gaudio->Serialize(main_serializer.get(), ext_serializer.get())) {
    LOG(ERROR) << "Could not serialize GAudio to XmpData";
    return false;
  }

  if (!gimage.Serialize(main_serializer.get(), ext_serializer.get())) {
    LOG(ERROR) << "Could not serialize GImage to XmpData";
    return false;
  }
  return true;
}

}  // namespace xmpmeta
