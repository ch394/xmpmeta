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

#include "xdmlib/point_cloud.h"

#include <libxml/tree.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "xmpmeta/file.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/utils.h"

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

const char kPrefix[] = "PointCloud";
const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/pointcloud/";
const char kAllOptionalFieldsFile[] = "xdm/point_cloud_testdata_full.txt";
const char kMinimumFieldsFile[] = "xdm/point_cloud_testdata.txt";

TEST(PointCloud, GetNamespaces) {
  std::unordered_map<string, string> ns_name_href_map;
  string prefix(kPrefix);

  int count = 5;
  const string position("asdf123");
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromData(count, position, "", false, "");
  ASSERT_NE(nullptr, point_cloud);

  EXPECT_TRUE(ns_name_href_map.empty());
  point_cloud->GetNamespaces(&ns_name_href_map);
  EXPECT_EQ(1, ns_name_href_map.size());
  EXPECT_EQ(string(kNamespaceHref), string(ns_name_href_map[prefix]));

  // Same namespace will not be added again.
  point_cloud->GetNamespaces(&ns_name_href_map);
  EXPECT_EQ(1, ns_name_href_map.size());
  EXPECT_EQ(string(kNamespaceHref), string(ns_name_href_map[prefix]));
}

TEST(PointCloud, FromData) {
  int count = 5;
  const string position("asdf123");
  const string color("4567qwerty");
  const string software("software");
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromData(count, position, color, false, software);
  ASSERT_NE(nullptr, point_cloud);
  EXPECT_EQ(count, point_cloud->GetCount());
  EXPECT_EQ(position, point_cloud->GetPosition());
  EXPECT_FALSE(point_cloud->GetMetric());
  EXPECT_EQ(color, point_cloud->GetColor());
  EXPECT_EQ(software, point_cloud->GetSoftware());
}

TEST(PointCloud, FromDataEmptyPosition) {
  int count = 5;
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromData(count, "", "4567qwerty", false, "software");
  ASSERT_EQ(nullptr, point_cloud);
}

TEST(PointCloud, FromDataRequiredFieldsOnly) {
  int count = 5;
  const string position("asdf123");
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromData(count, position, "", false, "");
  ASSERT_NE(nullptr, point_cloud);
  EXPECT_EQ(count, point_cloud->GetCount());
  EXPECT_EQ(position, point_cloud->GetPosition());
  EXPECT_FALSE(point_cloud->GetMetric());
  EXPECT_TRUE(point_cloud->GetColor().empty());
  EXPECT_TRUE(point_cloud->GetSoftware().empty());
}

TEST(PointCloud, SerializeWithMinimumFields) {
  int count = 5;
  const string position("asdf123");
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromData(count, position, "", false, "");
  ASSERT_NE(nullptr, point_cloud);

  // Set up XML nodes.
  const char* camera_name = "Camera";
  const char* namespaceHref = "http://notarealh.ref";
  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(camera_name, xmlNewNs(nullptr, ToXmlChar(namespaceHref),
                                           ToXmlChar(camera_name)));
  namespaces.emplace(kPrefix, xmlNewNs(nullptr, ToXmlChar(namespaceHref),
                                       ToXmlChar(kPrefix)));

  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(camera_name,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(camera_name)));
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar(camera_name));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, camera_node);

  // Create serializer
  SerializerImpl serializer(namespaces, prefixes, camera_name, camera_node);
  std::unique_ptr<Serializer> point_cloud_serializer =
      serializer.CreateSerializer(kPrefix);
  ASSERT_NE(nullptr, point_cloud_serializer);
  ASSERT_TRUE(point_cloud->Serialize(point_cloud_serializer.get()));

  // Check the XML string.
  const string testdata_path = TestFileAbsolutePath(kMinimumFieldsFile);
  std::string expected_data;
  ReadFileToStringOrDie(testdata_path, &expected_data);
  ASSERT_EQ(expected_data, XmlDocToString(xml_doc));

  // Free all XML objects.
  for (const auto& entry : namespaces) {
    xmlFreeNs(entry.second);
  }
  for (const auto& entry : prefixes) {
    xmlFreeNs(entry.second);
  }
  xmlFreeDoc(xml_doc);
}

TEST(PointCloud, SerializeWithAllOptionalFields) {
  int count = 5;
  const string position("asdf123");
  const string color("4567qwerty");
  const string software("software");
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromData(count, position, color, true, software);
  ASSERT_NE(nullptr, point_cloud);

  // Set up XML nodes.
  const char* camera_name = "Camera";
  const char* namespaceHref = "http://notarealh.ref";
  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(camera_name, xmlNewNs(nullptr, ToXmlChar(namespaceHref),
                                           ToXmlChar(camera_name)));
  namespaces.emplace(kPrefix, xmlNewNs(nullptr, ToXmlChar(namespaceHref),
                                       ToXmlChar(kPrefix)));

  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(camera_name,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(camera_name)));
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar(camera_name));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, camera_node);

  // Create serializer
  SerializerImpl serializer(namespaces, prefixes, camera_name, camera_node);
  std::unique_ptr<Serializer> point_cloud_serializer =
      serializer.CreateSerializer(kPrefix);
  ASSERT_NE(nullptr, point_cloud_serializer);
  ASSERT_TRUE(point_cloud->Serialize(point_cloud_serializer.get()));

  // Check the XML string.
  const string testdata_path = TestFileAbsolutePath(kAllOptionalFieldsFile);
  std::string expected_data;
  ReadFileToStringOrDie(testdata_path, &expected_data);
  ASSERT_EQ(expected_data, XmlDocToString(xml_doc));

  // Free all XML objects.
  for (const auto& entry : namespaces) {
    xmlFreeNs(entry.second);
  }
  for (const auto& entry : prefixes) {
    xmlFreeNs(entry.second);
  }
  xmlFreeDoc(xml_doc);
}

TEST(PointCloud, ReadMetadataMissingRequiredField) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  // Mock an Camera node.
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar("Camera"));
  xmlAddChild(description_node, camera_node);

  xmlNodePtr point_cloud_node = xmlNewNode(nullptr, ToXmlChar(kPrefix));
  xmlAddChild(camera_node, point_cloud_node);

  const string position("asdf123");
  string base64_encoded_position;
  ASSERT_TRUE(EncodeBase64(position, &base64_encoded_position));

  xmlNsPtr point_cloud_ns = xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                                     ToXmlChar(kPrefix));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Position"),
               ToXmlChar(base64_encoded_position.data()));

  // Create an PointCloud from the metadata.
  DeserializerImpl deserializer(kPrefix, description_node);
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromDeserializer(deserializer);
  ASSERT_EQ(nullptr, point_cloud.get());

  xmlFreeNs(point_cloud_ns);
}

TEST(PointCloud, ReadMetadataMissingOptionalField) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  // Mock an Camera node.
  xmlNodePtr camera_node =
      xmlNewNode(nullptr, ToXmlChar("Camera"));
  xmlAddChild(description_node, camera_node);

  xmlNodePtr point_cloud_node = xmlNewNode(nullptr, ToXmlChar(kPrefix));
  xmlAddChild(camera_node, point_cloud_node);

  const string position("asdf123");
  const string color("4567qwerty");
  string base64_encoded_position;
  ASSERT_TRUE(EncodeBase64(position, &base64_encoded_position));
  string base64_encoded_color;
  ASSERT_TRUE(EncodeBase64(color, &base64_encoded_color));

  xmlNsPtr point_cloud_ns =
      xmlNewNs(nullptr, ToXmlChar(kNamespaceHref), ToXmlChar(kPrefix));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Count"),
               ToXmlChar("5"));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Position"),
               ToXmlChar(base64_encoded_position.data()));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Color"),
               ToXmlChar(base64_encoded_color.data()));

  // Create an PointCloud from the metadata.
  DeserializerImpl deserializer(kPrefix, description_node);
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromDeserializer(deserializer);
  ASSERT_NE(nullptr, point_cloud.get());

  ASSERT_EQ(5, point_cloud->GetCount());
  ASSERT_EQ(position, point_cloud->GetPosition());
  ASSERT_EQ(color, point_cloud->GetColor());
  ASSERT_TRUE(point_cloud->GetSoftware().empty());

  // Parses to the default value when not set.
  ASSERT_FALSE(point_cloud->GetMetric());

  xmlFreeNs(point_cloud_ns);
}

TEST(PointCloud, ReadMetadata) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  // Mock an Camera node.
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar("Camera"));
  xmlAddChild(description_node, camera_node);

  xmlNodePtr point_cloud_node = xmlNewNode(nullptr, ToXmlChar(kPrefix));
  xmlAddChild(camera_node, point_cloud_node);

  const string position("asdf123");
  const string color("4567qwerty");
  const string software("software");
  string base64_encoded_position;
  ASSERT_TRUE(EncodeBase64(position, &base64_encoded_position));
  string base64_encoded_color;
  ASSERT_TRUE(EncodeBase64(color, &base64_encoded_color));

  xmlNsPtr point_cloud_ns =
      xmlNewNs(nullptr, ToXmlChar(kNamespaceHref), ToXmlChar(kPrefix));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Count"),
               ToXmlChar("5"));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Position"),
               ToXmlChar(base64_encoded_position.data()));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Color"),
               ToXmlChar(base64_encoded_color.data()));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Software"),
               ToXmlChar(software.data()));
  xmlSetNsProp(point_cloud_node, point_cloud_ns, ToXmlChar("Metric"),
               ToXmlChar("true"));

  // Create an PointCloud from the metadata.
  DeserializerImpl deserializer(kPrefix, description_node);
  std::unique_ptr<PointCloud> point_cloud =
      PointCloud::FromDeserializer(deserializer);
  ASSERT_NE(nullptr, point_cloud.get());

  ASSERT_EQ(5, point_cloud->GetCount());
  ASSERT_EQ(position, point_cloud->GetPosition());
  ASSERT_TRUE(point_cloud->GetMetric());
  ASSERT_EQ(color, point_cloud->GetColor());
  ASSERT_EQ(software, point_cloud->GetSoftware());

  xmlFreeNs(point_cloud_ns);
}

}  // namespace
}  // namespace xdm
}  // namespace xmpmeta
