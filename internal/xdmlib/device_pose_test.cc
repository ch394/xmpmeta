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

#include "xdmlib/device_pose.h"

#include <libxml/tree.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "xmpmeta/file.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/xmp_const.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/const.h"
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

const char kPrefix[] = "DevicePose";
const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/devicepose/";
const char kParentName[] = "Device";
const char kFakeHref[] = "http://notarealh.ref";

const char kTestDataFull[] = "xdm/device_pose_testdata_full.txt";
const char kTestDataPosition[] = "xdm/device_pose_testdata_position.txt";
const char kTestDataOrientation[] = "xdm/device_pose_testdata_orientation.txt";

// Same as NormalizeAxisAngle in device_pose.cc
const std::vector<double>
NormalizeAxisAngle(const std::vector<double>& coords) {
  if (coords.size() < 4) {
    return std::vector<double>();
  }
  double length = sqrt((coords[0] * coords[0]) +
                       (coords[1] * coords[1]) +
                       (coords[2] * coords[2]));
  const std::vector<double> normalized =
      { coords[0] / length, coords[1] / length, coords[2] / length, coords[3] };
  return normalized;
}

std::unordered_map<string, xmlNsPtr> SetupNamespaces() {
  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(kParentName, xmlNewNs(nullptr, ToXmlChar(kFakeHref),
                                           ToXmlChar(kParentName)));
  namespaces.emplace(kPrefix, xmlNewNs(nullptr, ToXmlChar(kFakeHref),
                                       ToXmlChar(kPrefix)));
  return namespaces;
}

std::unordered_map<string, xmlNsPtr> SetupPrefixes() {
  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(kParentName,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(kParentName)));
  prefixes.emplace(kPrefix,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(kPrefix)));
  return prefixes;
}

void FreeNamespaces(std::unordered_map<string, xmlNsPtr> xml_ns_map) {
  for (const auto& entry : xml_ns_map) {
    xmlFreeNs(entry.second);
  }
}

TEST(DevicePose, GetNamespaces) {
  std::unordered_map<string, string> ns_name_href_map;
  string prefix(kPrefix);

  double x = -85.32;
  double y = -135.20341;
  double z = 1.203;
  std::vector<double> init_position { x, y, z };
  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(init_position, std::vector<double>(), 0);
  ASSERT_NE(nullptr, pose);

  ASSERT_TRUE(ns_name_href_map.empty());
  pose->GetNamespaces(&ns_name_href_map);
  EXPECT_EQ(1, ns_name_href_map.size());
  EXPECT_EQ(kNamespaceHref, ns_name_href_map[prefix]);
}

TEST(DevicePose, FromDataAllEmpty) {
  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(std::vector<double>(), std::vector<double>(), 0);
  ASSERT_EQ(nullptr, pose);
}

TEST(DevicePose, FromDataPosition) {
  double x = -85.32;
  double y = -135.20341;
  double z = 1.203;
  std::vector<double> init_position = { x, y, z };

  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(init_position, std::vector<double>(), 0);
  ASSERT_NE(nullptr, pose);

  std::vector<double> position = pose->GetPosition();
  ASSERT_TRUE(pose->HasPosition());
  ASSERT_EQ(3, position.size());
  EXPECT_EQ(init_position.at(0), position.at(0));
  EXPECT_EQ(init_position.at(1), position.at(1));
  EXPECT_EQ(init_position.at(2), position.at(2));

  std::vector<double> orientation = pose->GetOrientationRotationXYZAngle();
  ASSERT_FALSE(pose->HasOrientation());
  ASSERT_EQ(0, orientation.size());

  ASSERT_EQ(0, pose->GetTimestamp());
}

TEST(DevicePose, FromOrientation) {
  double axis_x = 0;
  double axis_y = 0;
  double axis_z = 1;
  double angle = M_PI_2;
  int64 timestamp = 1455818790;
  std::vector<double> init_orientation = { axis_x, axis_y, axis_z, angle };
  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(std::vector<double>(), init_orientation,
                              timestamp);
  ASSERT_NE(nullptr, pose);

  std::vector<double> orientation = pose->GetOrientationRotationXYZAngle();
  ASSERT_EQ(4, orientation.size());
  const std::vector<double> normalized = NormalizeAxisAngle(init_orientation);
  EXPECT_EQ(normalized.at(0), orientation.at(0));
  EXPECT_EQ(normalized.at(1), orientation.at(1));
  EXPECT_EQ(normalized.at(2), orientation.at(2));
  EXPECT_EQ(angle, orientation.at(3));

  std::vector<double> position = pose->GetPosition();
  ASSERT_FALSE(pose->HasPosition());
  ASSERT_EQ(0, position.size());

  ASSERT_EQ(timestamp, pose->GetTimestamp());
}

TEST(DevicePose, FromPositionAndOrientation) {
  double x = -85.32;
  double y = -135.20341;
  double z = 1.203;
  double axis_x = 0;
  double axis_y = 0;
  double axis_z = 1;
  double angle = M_PI_2;
  int64 timestamp = 1455818790;

  std::vector<double> init_position = { x, y, z };
  std::vector<double> init_orientation = { axis_x, axis_y, axis_z, angle };
  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(init_position, init_orientation, timestamp);
  ASSERT_NE(nullptr, pose);
  std::vector<double> orientation = pose->GetOrientationRotationXYZAngle();
  ASSERT_TRUE(pose->HasPosition());
  ASSERT_EQ(4, orientation.size());
  std::vector<double> normalized = NormalizeAxisAngle(init_orientation);
  EXPECT_EQ(normalized.at(0), orientation.at(0));
  EXPECT_EQ(normalized.at(1), orientation.at(1));
  EXPECT_EQ(normalized.at(2), orientation.at(2));
  EXPECT_EQ(angle, orientation.at(3));

  std::vector<double> position = pose->GetPosition();
  ASSERT_TRUE(pose->HasOrientation());
  ASSERT_EQ(3, position.size());
  EXPECT_EQ(init_position.at(0), position.at(0));
  EXPECT_EQ(init_position.at(1), position.at(1));
  EXPECT_EQ(init_position.at(2), position.at(2));

  ASSERT_EQ(timestamp, pose->GetTimestamp());
}

TEST(DevicePose, SerializePosition) {
  double x = -85.32;
  double y = -135.20341;
  double z = 1.203;
  int64 timestamp = 1455818790;
  std::vector<double> init_position = { x, y, z };

  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(init_position, std::vector<double>(), timestamp);
  ASSERT_NE(nullptr, pose);

  // Set up XML structure.
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar(kParentName));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, camera_node);

  // Create serializer.
  std::unordered_map<string, xmlNsPtr> namespaces = SetupNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = SetupPrefixes();
  SerializerImpl serializer(namespaces, prefixes, kParentName, camera_node);
  std::unique_ptr<Serializer> pose_serializer =
      serializer.CreateSerializer(kPrefix);
  ASSERT_NE(nullptr, pose_serializer);

  ASSERT_TRUE(pose->Serialize(pose_serializer.get()));

  const string testdata_path = TestFileAbsolutePath(kTestDataPosition);
  std::string expected_xdm_data;
  ReadFileToStringOrDie(testdata_path, &expected_xdm_data);
  EXPECT_EQ(expected_xdm_data, XmlDocToString(xml_doc));

  // Clean up.
  FreeNamespaces(namespaces);
  FreeNamespaces(prefixes);
  xmlFreeDoc(xml_doc);
}

TEST(DevicePose, SerializeOrientation) {
  double axis_x = 1;
  double axis_y = 2;
  double axis_z = 1.5;
  double angle = 1.57;
  std::vector<double> init_orientation = { axis_x, axis_y, axis_z, angle };

  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(std::vector<double>(), init_orientation);
  ASSERT_NE(nullptr, pose);

  // Set up XML structure.
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar(kParentName));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, camera_node);

  // Create serializer.
  std::unordered_map<string, xmlNsPtr> namespaces = SetupNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = SetupPrefixes();
  SerializerImpl serializer(namespaces, prefixes, kParentName, camera_node);
  std::unique_ptr<Serializer> pose_serializer =
      serializer.CreateSerializer(kPrefix);
  ASSERT_NE(nullptr, pose_serializer);

  ASSERT_TRUE(pose->Serialize(pose_serializer.get()));

  const string testdata_path = TestFileAbsolutePath(kTestDataOrientation);
  std::string expected_xdm_data;
  ReadFileToStringOrDie(testdata_path, &expected_xdm_data);
  EXPECT_EQ(expected_xdm_data, XmlDocToString(xml_doc));

  // Clean up.
  FreeNamespaces(namespaces);
  FreeNamespaces(prefixes);
  xmlFreeDoc(xml_doc);
}


TEST(DevicePose, SerializePositionAndOrientation) {
  double x = -85.32;
  double y = -135.20341;
  double z = 1.203;

  double axis_x = 1;
  double axis_y = 2;
  double axis_z = 1.5;
  double angle = 1.57;

  int64 timestamp = 1455818790;
  std::vector<double> init_position = { x, y, z };
  std::vector<double> init_orientation = { axis_x, axis_y, axis_z, angle };

  std::unique_ptr<DevicePose> pose =
      DevicePose::FromData(init_position, init_orientation, timestamp);
  ASSERT_NE(nullptr, pose);

  // Set up XML structure.
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar(kParentName));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, camera_node);

  // Create serializer.
  std::unordered_map<string, xmlNsPtr> namespaces = SetupNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = SetupPrefixes();
  SerializerImpl serializer(namespaces, prefixes, kParentName, camera_node);
  std::unique_ptr<Serializer> pose_serializer =
      serializer.CreateSerializer(kPrefix);
  ASSERT_NE(nullptr, pose_serializer);

  ASSERT_TRUE(pose->Serialize(pose_serializer.get()));

  const string testdata_path = TestFileAbsolutePath(kTestDataFull);
  std::string expected_xdm_data;
  ReadFileToStringOrDie(testdata_path, &expected_xdm_data);
  EXPECT_EQ(expected_xdm_data, XmlDocToString(xml_doc));

  // Clean up.
  FreeNamespaces(namespaces);
  FreeNamespaces(prefixes);
  xmlFreeDoc(xml_doc);
}

TEST(DevicePose, ReadMetadataPositionOrientation) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  // Set up the XML structure.
  xmlNodePtr parent_node =
      xmlNewNode(nullptr, ToXmlChar(kParentName));
  xmlAddChild(description_node, parent_node);

  xmlNodePtr pose_node = xmlNewNode(nullptr, ToXmlChar(kPrefix));
  xmlAddChild(parent_node, pose_node);
  xmlNsPtr pose_ns = xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                              ToXmlChar(kPrefix));

  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("Latitude"),
               ToXmlChar("1.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("Longitude"),
               ToXmlChar("2.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("Altitude"),
               ToXmlChar("-1"));

  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisX"),
               ToXmlChar("1.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisY"),
               ToXmlChar("2.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisZ"),
               ToXmlChar("1.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAngle"),
               ToXmlChar("1.57"));

  // Create an DevicePose from the metadata.
  DeserializerImpl deserializer(kPrefix, description_node);
  std::unique_ptr<DevicePose> pose =
      DevicePose::FromDeserializer(deserializer);
  ASSERT_NE(nullptr, pose.get());

  std::vector<double> values = pose->GetPosition();
  ASSERT_EQ(3, values.size());
  EXPECT_EQ(1.5, values[0]);
  EXPECT_EQ(2.5, values[1]);
  EXPECT_EQ(-1, values[2]);

  values = pose->GetOrientationRotationXYZAngle();
  ASSERT_EQ(4, values.size());
  std::vector<double> raw_values({1.5, 2.5, 1.5, 1.57});
  std::vector<double> normalized = NormalizeAxisAngle(raw_values);
  EXPECT_EQ(normalized[0], values[0]);
  EXPECT_EQ(normalized[1], values[1]);
  EXPECT_EQ(normalized[2], values[2]);
  EXPECT_EQ(normalized[3], values[3]);

  EXPECT_EQ(-1, pose->GetTimestamp());

  xmlFreeNs(pose_ns);
}

TEST(DevicePose, ReadMetadataOrientation) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  // Set up the XML structure.
  xmlNodePtr parent_node = xmlNewNode(nullptr, ToXmlChar(kParentName));
  xmlAddChild(description_node, parent_node);

  xmlNodePtr pose_node = xmlNewNode(nullptr, ToXmlChar(kPrefix));
  xmlAddChild(parent_node, pose_node);
  xmlNsPtr pose_ns = xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                              ToXmlChar(kPrefix));

  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisX"),
               ToXmlChar("1.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisY"),
               ToXmlChar("2.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisZ"),
               ToXmlChar("1.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAngle"),
               ToXmlChar("1.57"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("Timestamp"),
               ToXmlChar("1455818790"));

  // Create an DevicePose from the metadata.
  DeserializerImpl deserializer(kPrefix, description_node);
  std::unique_ptr<DevicePose> pose =
      DevicePose::FromDeserializer(deserializer);
  ASSERT_NE(nullptr, pose.get());

  std::vector<double> values = pose->GetOrientationRotationXYZAngle();
  ASSERT_EQ(4, values.size());
  std::vector<double> raw_values({1.5, 2.5, 1.5, 1.57});
  std::vector<double> normalized = NormalizeAxisAngle(raw_values);
  EXPECT_EQ(normalized[0], values[0]);
  EXPECT_EQ(normalized[1], values[1]);
  EXPECT_EQ(normalized[2], values[2]);
  EXPECT_EQ(normalized[3], values[3]);

  ASSERT_EQ(1455818790, pose->GetTimestamp());

  xmlFreeNs(pose_ns);
}

}  // namespace
}  // namespace xdm
}  // namespace xmpmeta
