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

#include "serializer_impl.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <libxml/tree.h>

#include "gtest/gtest.h"
#include "xmpmeta/xmp_parser.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/search.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::FromXmlChar;
using xmpmeta::xml::ToXmlChar;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace xml {
namespace {



const char kNamespaceHref[] = "http://somehref.com";
const char kNodeName[] = "Name";
const char kNodeOneName[] = "NodeOne";
const char kNodeTwoName[] = "NodeTwo";
const char kRdfName[] = "rdf";

std::unordered_map<string, xmlNsPtr> CreateNamespaces() {
  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(kNodeOneName, xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                                            ToXmlChar(kNodeOneName)));
  namespaces.emplace(kNodeTwoName, xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                                            ToXmlChar(kNodeTwoName)));
  return namespaces;
}

std::unordered_map<string, xmlNsPtr> CreatePrefixes() {
  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(kNodeName, xmlNewNs(nullptr, nullptr, ToXmlChar(kNodeName)));
  prefixes.emplace(kNodeOneName,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(kNodeOneName)));
  prefixes.emplace(kNodeTwoName,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(kNodeTwoName)));
  return prefixes;
}

void FreeXmlNamespaces(std::unordered_map<string, xmlNsPtr> namespace_map) {
  for (const auto& entry : namespace_map) {
    xmlFreeNs(entry.second);
  }
}

TEST(SerializerImpl, CreateSerializerEmptyNodeName) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateSerializer("");
  ASSERT_EQ(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateSerializerPrefixNotNullAndNonexistent) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, "SomeNodeName", node);

  string node_name("SomeNonexistentPrefix");
  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateSerializer(node_name);
  ASSERT_EQ(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateSerializer) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateSerializer(kNodeName);
  ASSERT_NE(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateItemSerializerNoRdfPrefix) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateItemSerializer(kNodeName);
  ASSERT_EQ(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateItemSerializerParentNotInPrefixes) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  prefixes.emplace(kRdfName,
                   xmlNewNs(nullptr, nullptr,
                            ToXmlChar("http://fakerdfnamespace.com")));
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfSeq()));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateItemSerializer("SomeNameNotInPrefixes");
  ASSERT_NE(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateItemSerializer) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  prefixes.emplace(kRdfName,
                   xmlNewNs(nullptr, nullptr,
                            ToXmlChar("http://fakerdfnamespace.com")));
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl initial_serializer(namespaces, prefixes, kNodeName, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateListSerializer("ListName");

  std::unique_ptr<Serializer> created_serializer =
      serializer->CreateItemSerializer(kNodeName);
  ASSERT_NE(nullptr, created_serializer.get());
  xmlNodePtr seq_node = DepthFirstSearch(node, "Seq");
  ASSERT_NE(nullptr, seq_node);
  int i = 0;
  int count = 0;
  for (xmlNodePtr child = GetElementAt(seq_node, i);
       child != nullptr;
       child = GetElementAt(seq_node, ++i)) {
    count++;
  }
  ASSERT_EQ(1, count);

  // Create more list element nodes.
  ASSERT_NE(nullptr, serializer->CreateItemSerializer(kNodeName).get());
  ASSERT_NE(nullptr, serializer->CreateItemSerializer(kNodeName).get());
  ASSERT_NE(nullptr, serializer->CreateItemSerializer(kNodeName).get());
  i = 0;
  count = 0;
  for (xmlNodePtr child = GetElementAt(seq_node, i);
       child != nullptr;
       child = GetElementAt(seq_node, ++i)) {
    count++;
  }
  ASSERT_EQ(4, count);


  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WritePropertyNullChildNode) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  ASSERT_FALSE(serializer.WriteProperty("Name", "Value"));

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WritePropertyEmptyNameEmptyValue) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl initial_serializer(namespaces, prefixes, kNodeName, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kNodeOneName);

  ASSERT_FALSE(serializer->WriteProperty("", ""));
  ASSERT_FALSE(serializer->WriteProperty("", "Value"));
  ASSERT_FALSE(serializer->WriteProperty("Name", ""));

  xmlNodePtr node_one = DepthFirstSearch(node, kNodeOneName);
  ASSERT_NE(nullptr, node_one);
  string value;
  DeserializerImpl deserializer(kNodeOneName, node_one);
  ASSERT_FALSE(deserializer.ParseString("Name", &value));

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteProperty) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl initial_serializer(namespaces, prefixes, kNodeName, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kNodeOneName);

  const char* property_name = "Name";
  const char* property_value = "Value";
  ASSERT_TRUE(serializer->WriteProperty(property_name, property_value));
  string value;
  xmlNodePtr node_one = DepthFirstSearch(node, kNodeOneName);
  DeserializerImpl deserializer(kNodeOneName, node_one);
  ASSERT_TRUE(deserializer.ParseString(property_name, &value));
  ASSERT_EQ(property_value, value);

  // Write another property.
  const char* property_name2 = "AnotherName";
  const char* property_value2 = "AnotherValue";
  ASSERT_TRUE(serializer->WriteProperty(property_name2, property_value2));
  ASSERT_TRUE(deserializer.ParseString(property_name2, &value));
  ASSERT_EQ(property_value2, value);

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayNoRdfPrefix) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  xmlNsPtr mock_ns = xmlNewNs(nullptr, ToXmlChar("http://fakeh.ref"),
                              ToXmlChar(kNodeName));
  xmlSetNs(node, mock_ns);
  ASSERT_NE(nullptr, node->ns);
  EXPECT_EQ(string(kNodeName), string(FromXmlChar(node->ns->prefix)));
  int num_nses = 0;
  xmlNsPtr xml_ns = node->ns;
  while (xml_ns) {
    num_nses++;
    xml_ns = xml_ns->next;
  }
  EXPECT_EQ(1, num_nses);
  std::unique_ptr<SerializerImpl> serializer =
      SerializerImpl::FromDataAndSerializeNamespaces(namespaces, prefixes,
                                                     kNodeName, node);

  // Check that the namespaces are set in the root node.
  xmlNsPtr node_ns = node->ns->next;
  for (const auto& entry : namespaces) {
    ASSERT_NE(nullptr, node_ns);
    EXPECT_EQ(FromXmlChar(entry.second->prefix),
              FromXmlChar(node_ns->prefix));
    node_ns = node_ns->next;
    num_nses++;
  }
  EXPECT_EQ(namespaces.size() + 1, num_nses);

  const std::vector<double> values = { 1, 2, 3 };
  const string parent_name("Parent");
  ASSERT_FALSE(serializer->WriteDoubleArray(parent_name, values));

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNs(mock_ns);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayNullNode) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  prefixes.emplace(kRdfName,
                   xmlNewNs(nullptr, nullptr,
                            ToXmlChar(kNamespaceHref)));
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  const std::vector<double> values = { 1, 2, 3 };
  const string parent_name("Parent");
  ASSERT_FALSE(serializer.WriteDoubleArray(parent_name, values));

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayEmptyValues) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  prefixes.emplace(kRdfName, xmlNewNs(nullptr, nullptr,
                                      ToXmlChar(kNamespaceHref)));
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  const std::vector<double> values;
  const string parent_name("Parent");
  ASSERT_FALSE(serializer.WriteDoubleArray(parent_name, values));

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayEmptyParentName) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  prefixes.emplace(kRdfName, xmlNewNs(nullptr, nullptr,
                                      ToXmlChar(kNamespaceHref)));
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl serializer(namespaces, prefixes, kNodeName, node);

  const std::vector<double> values = { 1, 2, 3 };
  ASSERT_FALSE(serializer.WriteDoubleArray("", values));

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteIntArray) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  prefixes.emplace(kRdfName, xmlNewNs(nullptr, nullptr,
                                      ToXmlChar(kNamespaceHref)));
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl initial_serializer(namespaces, prefixes, kNodeName, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kNodeOneName);

  const std::vector<int> values = {1, 2, 3, 4, 5};
  const string parent_name("Parent");
  ASSERT_TRUE(serializer->WriteIntArray(parent_name, values));

  // Read from the array.
  xmlNodePtr seq_node = DepthFirstSearch(node, "Seq");
  int i = 0;
  int count = 0;
  for (xmlNodePtr li_node = GetElementAt(seq_node, i);
       li_node != nullptr;
       li_node = GetElementAt(seq_node, ++i)) {
    EXPECT_EQ(values.at(i), std::stoi(GetLiNodeContent(li_node).data()));
    count++;
  }
  ASSERT_EQ(5, count);

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArray) {
  std::unordered_map<string, xmlNsPtr> namespaces = CreateNamespaces();
  std::unordered_map<string, xmlNsPtr> prefixes = CreatePrefixes();
  prefixes.emplace(kRdfName, xmlNewNs(nullptr, nullptr,
                                      ToXmlChar(kNamespaceHref)));
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kNodeName));
  SerializerImpl initial_serializer(namespaces, prefixes, kNodeName, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kNodeOneName);

  const std::vector<double> values = { 1.023, 2.112, 3.12, 4.25, 5.67 };
  const string parent_name("Parent");
  ASSERT_TRUE(serializer->WriteDoubleArray(parent_name, values));

  // Read from the array.
  xmlNodePtr seq_node = DepthFirstSearch(node, "Seq");
  int i = 0;
  int count = 0;
  for (xmlNodePtr li_node = GetElementAt(seq_node, i);
       li_node != nullptr;
       li_node = GetElementAt(seq_node, ++i)) {
    EXPECT_EQ(values.at(i), std::stod(GetLiNodeContent(li_node)));
    count++;
  }
  ASSERT_EQ(5, count);

  FreeXmlNamespaces(namespaces);
  FreeXmlNamespaces(prefixes);
  xmlFreeNode(node);
}

}  // namespace
}  // namespace xml
}  // namespace xmpmeta
