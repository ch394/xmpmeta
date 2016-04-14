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

#include "xdmlib/device.h"

#include <libxml/tree.h>

#include "glog/logging.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/search.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/utils.h"
#include "xmpmeta/xmp_parser.h"

using xmpmeta::xml::DepthFirstSearch;
using xmpmeta::xml::Deserializer;
using xmpmeta::xml::DeserializerImpl;
using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::Serializer;
using xmpmeta::xml::SerializerImpl;
using xmpmeta::xml::ToXmlChar;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace xdm {
namespace {

const char kDevicePose[] = "DevicePose";
const char kPropertyPrefix[] = "Device";
const char kRevision[] = "Revision";
const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/device/";

}  // namespace

// Private constructor.
Device::Device() {}

// Public methods.
Device::~Device() {
  for (const auto& entry : prefixes_) {
    xmlFreeNs(entry.second);
  }
}

std::unique_ptr<Device> Device::FromData(
    const string& revision,
    std::unique_ptr<DevicePose> device_pose,
    std::unique_ptr<Profiles> profiles,
    std::unique_ptr<Cameras> cameras) {
  if (revision.empty()) {
    LOG(ERROR) << "Revision field cannot be empty";
    return nullptr;
  }
  std::unique_ptr<Device> device(new Device());

  // Populate the fields.
  device->revision_ = revision;

  // Populate the XDM elements.
  device->device_pose_ = std::move(device_pose);
  device->profiles_ = std::move(profiles);
  device->cameras_ = std::move(cameras);

  return device;
}

std::unique_ptr<Device> Device::FromXmp(const XmpData& xmp) {
  std::unique_ptr<Device> device(new Device());
  if (!device->ParseFields(xmp)) {
    return nullptr;
  }
  return device;
}

std::unique_ptr<Device> Device::FromJpegFile(const string& filename) {
  XmpData xmp;
  const bool kSkipExtended = false;
  if (!ReadXmpHeader(filename, kSkipExtended, &xmp)) {
    return nullptr;
  }
  return FromXmp(xmp);
}

const string& Device::GetRevision() const { return revision_; }
const Cameras* Device::GetCameras() const { return cameras_.get(); }
const DevicePose* Device::GetDevicePose() const { return device_pose_.get(); }
const Profiles* Device::GetProfiles() const { return profiles_.get(); }

// This cannot be const because of memory management for the namespaces.
// namespaces_ are freed when the XML document(s) in xmp are freed.
// If namespaces_ and prefixes_ are populated at object creation time and this
// object is serialized, freeing the xmlNs objects in the destructor will result
// memory management errors.
bool Device::SerializeToXmp(XmpData* xmp) {
  if (xmp == nullptr || xmp->StandardSection() == nullptr
      || xmp->ExtendedSection() == nullptr) {
    LOG(ERROR) << "XmpData or its sections are null";
    return false;
  }

  xmlNodePtr root_node =
      GetFirstDescriptionElement(*xmp->MutableExtendedSection());
  if (root_node == nullptr) {
    LOG(ERROR) << "Extended section has no rdf:Description node";
    return false;
  }

  // Create a node here instead of through a new deserializer, otherwise
  // an extraneous prefix will be written to the node name.
  xmlNodePtr device_node = xmlNewNode(nullptr, ToXmlChar(kPropertyPrefix));
  xmlAddChild(root_node, device_node);

  PopulateNamespacesAndPrefixes();
  xmlNsPtr prev_ns = root_node->ns;
  for (const auto& entry : namespaces_) {
    if (prev_ns != nullptr) {
      prev_ns->next = entry.second;
    }
    prev_ns = entry.second;
  }

  // Set up serialization on the first description node in the extended section.
  SerializerImpl device_serializer(namespaces_, prefixes_, kPropertyPrefix,
                                   device_node);

  // Serialize fields.
  if (!device_serializer.WriteProperty(kRevision, revision_)) {
    return false;
  }

  // Serialize elements.
  if (device_pose_) {
    std::unique_ptr<Serializer> pose_serializer =
        device_serializer.CreateSerializer(kDevicePose);
    if (!device_pose_->Serialize(pose_serializer.get())) {
      return false;
    }
  }
  if (profiles_ && !profiles_->Serialize(&device_serializer)) {
    return false;
  }
  if (cameras_ && !cameras_->Serialize(&device_serializer)) {
    return false;
  }

  // TODO(miraleung): Add other elements here.
  return true;
}


// Private methods.
void Device::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) const {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list is null";
    return;
  }
  ns_name_href_map->emplace(XmlConst::RdfPrefix(), "");
  ns_name_href_map->emplace(kPropertyPrefix, kNamespaceHref);
  if (device_pose_) {
    device_pose_->GetNamespaces(ns_name_href_map);
  }
  if (profiles_) {
    profiles_->GetNamespaces(ns_name_href_map);
  }
  if (cameras_) {
    cameras_->GetNamespaces(ns_name_href_map);
  }
}

bool Device::ParseFields(const XmpData& xmp) {
  if (xmp.ExtendedSection() == nullptr) {
    LOG(ERROR) << "XMP extended section is null";
    return false;
  }

  // Find and parse the Device node.
  // Only these two fields are required to be present; the rest are optional.
  // TODO(miraleung): Search for Device by namespace.
  xmlNodePtr device_node =
      DepthFirstSearch(xmp.ExtendedSection(), kPropertyPrefix);
  if (device_node == nullptr) {
    LOG(ERROR) << "No device node found";
    return false;
  }
  const DeserializerImpl deserializer(kPropertyPrefix, device_node);
  if (!deserializer.ParseString(kRevision, &revision_)) {
    return false;
  }

  // XDM elements.
  cameras_ = Cameras::FromDeserializer(deserializer);
  device_pose_ = DevicePose::FromDeserializer(deserializer);
  profiles_ = Profiles::FromDeserializer(deserializer);

  return true;
}

// Gathers all the namespaces and prefixes of child elements.
void Device::PopulateNamespacesAndPrefixes() {
  std::unordered_map<string, string> ns_name_href_map;
  GetNamespaces(&ns_name_href_map);
  for (const auto& entry : ns_name_href_map) {
    if (namespaces_.count(entry.first) == 0 && !entry.second.empty()) {
      namespaces_.emplace(entry.first,
                          xmlNewNs(nullptr, ToXmlChar(entry.second.data()),
                                   ToXmlChar(entry.first.data())));
    }
    if (prefixes_.count(entry.first) == 0) {
      prefixes_.emplace(entry.first,
                        xmlNewNs(nullptr, nullptr,
                                 ToXmlChar(entry.first.data())));
    }
  }
}

}  // namespace xdm
}  // namespace xmpmeta
