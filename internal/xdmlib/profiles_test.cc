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

#include <libxml/tree.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "xdmlib/profile.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/file.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::CreateXmpData;
using xmpmeta::xml::Deserializer;
using xmpmeta::xml::DeserializerImpl;
using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::Serializer;
using xmpmeta::xml::SerializerImpl;
using xmpmeta::xml::ToXmlChar;
using xmpmeta::xml::XmlConst;
using xmpmeta::xml::XmlDocToString;

namespace xmpmeta {
namespace xdm {
namespace {

const char kNodeName[] = "Profiles";
const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/profile/";

// Test data constants.
const char kProfilesDataPath[] = "xdm/profiles_testdata.txt";

// Convenience function for creating an XML node.
xmlNodePtr NewNode(const string& node_name) {
  return xmlNewNode(nullptr, ToXmlChar(node_name.data()));
}

// Convenience function for creating an XML namespace.
xmlNsPtr NewNs(const string& href, const string& ns_name) {
  return xmlNewNs(nullptr,
                  href.empty() ? nullptr : ToXmlChar(href.data()),
                  ToXmlChar(ns_name.data()));
}

// Returns an XML rdf:Seq node with a list of the given values.
xmlNodePtr SetupRdfSeqOfIndices(const std::vector<int>& values,
                                const xmlNsPtr rdf_ns) {
  xmlNodePtr rdf_seq_node =
      xmlNewNode(rdf_ns, ToXmlChar(XmlConst::RdfSeq()));
  for (int value : values) {
    xmlNodePtr rdf_li_node =
        xmlNewNode(rdf_ns, ToXmlChar(XmlConst::RdfLi()));
    xmlNodeSetContent(
        rdf_li_node, ToXmlChar(std::to_string(value).data()));
    xmlAddChild(rdf_seq_node, rdf_li_node);
  }
  return rdf_seq_node;
}

std::unique_ptr<Profile> CreateVrPhotoProfile() {
  return Profile::FromData("VRPhoto", {0, 1});
}

std::unique_ptr<Profile> CreateDepthPhotoProfile() {
  return Profile::FromData("DepthPhoto", {0});
}

void ExpectProfileEquals(const Profile& profile_1, const Profile& profile_2) {
  EXPECT_EQ(profile_1.GetType(), profile_2.GetType());
  std::vector<int> indices_1 = profile_1.GetCameraIndices();
  std::vector<int> indices_2 = profile_2.GetCameraIndices();
  ASSERT_EQ(indices_1.size(), indices_2.size());
  for (int i = 0; i < indices_1.size(); i++) {
    EXPECT_EQ(indices_1[i], indices_2[i]);
  }
}

TEST(Profiles, GetNamespaces) {
  std::unique_ptr<Profile> profile = CreateVrPhotoProfile();
  std::vector<std::unique_ptr<Profile>> profile_list;
  profile_list.emplace_back(std::move(profile));
  std::unique_ptr<Profiles> profiles =
      Profiles::FromProfileArray(&profile_list);
  ASSERT_NE(nullptr, profiles);

  string prefix("Profile");
  std::unordered_map<string, string> ns_name_href_map;
  ASSERT_TRUE(ns_name_href_map.empty());
  profiles->GetNamespaces(&ns_name_href_map);
  EXPECT_EQ(1, ns_name_href_map.size());
  EXPECT_EQ(string(kNamespaceHref), string(ns_name_href_map[prefix]));

  // Same namespace will not be added again.
  profiles->GetNamespaces(&ns_name_href_map);
  EXPECT_EQ(1, ns_name_href_map.size());
  EXPECT_EQ(string(kNamespaceHref), string(ns_name_href_map[prefix]));
}

TEST(Profiles, FromProfileArray) {
  std::unique_ptr<Profile> vr_profile = CreateVrPhotoProfile();
  std::unique_ptr<Profile> depth_profile = CreateDepthPhotoProfile();
  std::vector<std::unique_ptr<Profile>> profile_list;
  profile_list.emplace_back(std::move(vr_profile));
  profile_list.emplace_back(std::move(depth_profile));
  int num_profiles = profile_list.size();

  std::unique_ptr<Profiles> profiles =
      Profiles::FromProfileArray(&profile_list);
  ASSERT_NE(nullptr, profiles);
  const std::vector<const Profile*> read_profile_list = profiles->GetProfiles();
  EXPECT_EQ(num_profiles, read_profile_list.size());
  // Pointer ownership of vr_profile and depth_profile has been transferred to
  // profiles.
  ExpectProfileEquals(*CreateVrPhotoProfile(), *read_profile_list[0]);
  ExpectProfileEquals(*CreateDepthPhotoProfile(), *read_profile_list[1]);
}

TEST(Profiles, FromEmptyProfileList) {
  std::vector<std::unique_ptr<Profile>> profile_list;
  std::unique_ptr<Profiles> profiles =
      Profiles::FromProfileArray(&profile_list);
  ASSERT_EQ(nullptr, profiles);
}

TEST(Profiles, SerializeWithoutRdfPrefix) {
  std::unique_ptr<Profile> vr_profile = CreateVrPhotoProfile();
  std::unique_ptr<Profile> depth_profile = CreateDepthPhotoProfile();
  std::vector<std::unique_ptr<Profile>> profile_list;
  profile_list.emplace_back(std::move(vr_profile));
  profile_list.emplace_back(std::move(depth_profile));

  std::unique_ptr<Profiles> profiles =
      Profiles::FromProfileArray(&profile_list);
  ASSERT_NE(nullptr, profiles);

  // Create XML serializer.
  const char device_name[] = "Device";
  const char profile_name[] = "Profile";
  const char namespaceHref[] = "http://notarealh.ref";

  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(device_name, NewNs(namespaceHref, device_name));
  namespaces.emplace(profile_name, NewNs(kNamespaceHref, profile_name));

  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(device_name, NewNs("", device_name));
  prefixes.emplace(profile_name, NewNs("", profile_name));

  xmlNodePtr device_node = NewNode(device_name);
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, device_node);

  // Create serializer.
  SerializerImpl serializer(namespaces, prefixes, device_name, device_node);
  ASSERT_FALSE(profiles->Serialize(&serializer));

  // Free all XML objects.
  for (const auto& entry : namespaces) {
    xmlFreeNs(entry.second);
  }
  for (const auto& entry : prefixes) {
    xmlFreeNs(entry.second);
  }
  xmlFreeDoc(xml_doc);
}

TEST(Profiles, Serialize) {
  std::unique_ptr<Profile> vr_profile = CreateVrPhotoProfile();
  std::unique_ptr<Profile> depth_profile = CreateDepthPhotoProfile();
  std::vector<std::unique_ptr<Profile>> profile_list;
  profile_list.emplace_back(std::move(vr_profile));
  profile_list.emplace_back(std::move(depth_profile));

  std::unique_ptr<Profiles> profiles =
      Profiles::FromProfileArray(&profile_list);
  ASSERT_NE(nullptr, profiles);

  // Create XML serializer.
  const char device_name[] = "Device";
  const char profile_name[] = "Profile";
  const char namespaceHref[] = "http://notarealh.ref";

  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(device_name, NewNs(namespaceHref, device_name));
  namespaces.emplace(profile_name, NewNs(kNamespaceHref, profile_name));

  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(device_name, NewNs("", device_name));
  prefixes.emplace(profile_name, NewNs("", profile_name));
  prefixes.emplace(XmlConst::RdfPrefix(), NewNs("", XmlConst::RdfPrefix()));

  xmlNodePtr device_node = NewNode(device_name);
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, device_node);

  // Create serializer.
  SerializerImpl serializer(namespaces, prefixes, device_name, device_node);
  ASSERT_TRUE(profiles->Serialize(&serializer));

  const string testdata_path = TestFileAbsolutePath(kProfilesDataPath);
  std::string expected_xdm_data;
  ReadFileToStringOrDie(testdata_path, &expected_xdm_data);
  EXPECT_EQ(expected_xdm_data, XmlDocToString(xml_doc));

  // Free all XML objects.
  for (const auto& entry : namespaces) {
    xmlFreeNs(entry.second);
  }
  for (const auto& entry : prefixes) {
    xmlFreeNs(entry.second);
  }
  xmlFreeDoc(xml_doc);
}

TEST(Profiles, ReadMetadata) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  // XDM Device node.
  xmlNodePtr device_node = NewNode("Device");
  xmlAddChild(description_node, device_node);

  // Device:Profiles node.
  xmlNodePtr profiles_node = NewNode(kNodeName);
  xmlAddChild(device_node, profiles_node);

  // rdf:Seq node.
  xmlNsPtr rdf_ns = NewNs("http://fakeh.ref", XmlConst::RdfPrefix());
  xmlNodePtr rdf_seq_node =
      xmlNewNode(rdf_ns, ToXmlChar(XmlConst::RdfSeq()));
  xmlAddChild(profiles_node, rdf_seq_node);

  // Set up Profile nodes.
  const char profile_ns_href[] = "http://ns.xdm.org/photos/1.0/profile/";
  const char profile_prefix[] = "Profile";
  xmlNsPtr profile_ns = NewNs(profile_ns_href, profile_prefix);


  // VR photo profile.
  std::vector<int> indices = {0, 1};
  xmlNodePtr vr_indices_rdf_seq_node = SetupRdfSeqOfIndices(indices, rdf_ns);
  xmlNodePtr vr_profile_camera_indices_node = NewNode("CameraIndices");
  xmlAddChild(vr_profile_camera_indices_node, vr_indices_rdf_seq_node);

  xmlNodePtr vr_profile_node = NewNode(profile_prefix);
  xmlAddChild(vr_profile_node, vr_profile_camera_indices_node);
  xmlSetNsProp(vr_profile_node, profile_ns, ToXmlChar("Type"),
               ToXmlChar("VRPhoto"));

  // Depth photo profile.
  indices = {0};
  xmlNodePtr depth_camera_indices_rdf_seq_node =
      SetupRdfSeqOfIndices(indices, rdf_ns);
  xmlNodePtr depth_profile_camera_indices_node = NewNode("CameraIndices");
  xmlAddChild(depth_profile_camera_indices_node,
              depth_camera_indices_rdf_seq_node);

  xmlNodePtr depth_profile_node = NewNode(profile_prefix);
  xmlAddChild(depth_profile_node, depth_profile_camera_indices_node);
  xmlSetNsProp(depth_profile_node, profile_ns, ToXmlChar("Type"),
               ToXmlChar("DepthPhoto"));


  // Insert profile nodes into the rdf:Seq list of profiles.
  xmlNodePtr vr_profile_rdf_li_node =
      xmlNewNode(rdf_ns, ToXmlChar(XmlConst::RdfLi()));
  xmlAddChild(vr_profile_rdf_li_node, vr_profile_node);
  xmlAddChild(rdf_seq_node, vr_profile_rdf_li_node);


  xmlNodePtr depth_profile_rdf_li_node =
      xmlNewNode(rdf_ns, ToXmlChar(XmlConst::RdfLi()));
  xmlAddChild(depth_profile_rdf_li_node, depth_profile_node);
  xmlAddChild(rdf_seq_node, depth_profile_rdf_li_node);

  // Create a Profiles object from the metadata.
  DeserializerImpl deserializer("Device", description_node);
  std::unique_ptr<Profiles> profiles = Profiles::FromDeserializer(deserializer);
  ASSERT_NE(nullptr, profiles.get());

  const std::vector<const Profile*> read_profile_list = profiles->GetProfiles();
  EXPECT_EQ(2, read_profile_list.size());
  const std::unique_ptr<Profile> vr_profile = CreateVrPhotoProfile();
  const std::unique_ptr<Profile> depth_profile = CreateDepthPhotoProfile();
  ExpectProfileEquals(*vr_profile, *read_profile_list[0]);
  ExpectProfileEquals(*depth_profile, *read_profile_list[1]);

  xmlFreeNs(profile_ns);
  xmlFreeNs(rdf_ns);
}

}  // namespace
}  // namespace xdm
}  // namespace xmpmeta
