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
const char kPrefixOne[] = "Name";
const char kPrefixTwo[] = "NodeOne";
const char kPrefixThree[] = "NodeTwo";

std::unordered_map<string, xmlNsPtr> CreateNamespaces(bool add_rdf_namespace) {
  std::unordered_map<string, xmlNsPtr> namespaces;
  if (add_rdf_namespace) {
    namespaces.emplace(XmlConst::RdfPrefix(),
                       xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                                ToXmlChar(XmlConst::RdfPrefix())));
  }

  namespaces.emplace(kPrefixOne,
                     xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                              ToXmlChar(kPrefixOne)));
  namespaces.emplace(kPrefixTwo, xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                                          ToXmlChar(kPrefixTwo)));
  namespaces.emplace(kPrefixThree, xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                                            ToXmlChar(kPrefixThree)));
  return namespaces;
}

void FreeXmlNamespaces(std::unordered_map<string, xmlNsPtr> namespace_map) {
  for (const auto& entry : namespace_map) {
    xmlFreeNs(entry.second);
  }
}

TEST(SerializerImpl, CreateSerializerEmptyNodeName) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateSerializer(kPrefixTwo, "");
  ASSERT_EQ(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateSerializerPrefixNotNullAndNonexistent) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateSerializer("SomeNonexistentPrefix", "Node2");
  ASSERT_EQ(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateSerializer) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateSerializer(kPrefixTwo, kPrefixOne);
  ASSERT_NE(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateItemSerializerNoRdfPrefix) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateItemSerializer("SomeNode", kPrefixOne);
  ASSERT_EQ(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateItemSerializerPrefixNotInPrefixes) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfSeq()));
  SerializerImpl serializer(namespaces, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateItemSerializer("SomeNameNotInPrefixes", "ItemName");
  ASSERT_EQ(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, CreateItemSerializerPrefixEmpty) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfSeq()));
  SerializerImpl serializer(namespaces, node);

  std::unique_ptr<Serializer> created_serializer =
      serializer.CreateItemSerializer("", "SomeName");
  ASSERT_NE(nullptr, created_serializer.get());

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}


TEST(SerializerImpl, CreateItemSerializer) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl initial_serializer(namespaces, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateListSerializer(kPrefixOne, "ListName");

  std::unique_ptr<Serializer> created_serializer =
      serializer->CreateItemSerializer(kPrefixOne, "ItemName");
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
  ASSERT_NE(nullptr,
            serializer->CreateItemSerializer(kPrefixOne, "ItemName").get());
  ASSERT_NE(nullptr,
            serializer->CreateItemSerializer(kPrefixTwo, "ItemName").get());
  ASSERT_NE(nullptr,
            serializer->CreateItemSerializer(kPrefixThree, "ItemName").get());
  i = 0;
  count = 0;
  for (xmlNodePtr child = GetElementAt(seq_node, i);
       child != nullptr;
       child = GetElementAt(seq_node, ++i)) {
    count++;
  }
  ASSERT_EQ(4, count);


  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WritePropertyNullChildNode) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  ASSERT_FALSE(serializer.WriteProperty("NonExistentPrefix", "Name", "Value"));

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WritePropertyEmptyNameEmptyValue) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl initial_serializer(namespaces, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kPrefixThree, kPrefixTwo);

  ASSERT_FALSE(serializer->WriteProperty(kPrefixTwo, "", ""));
  ASSERT_FALSE(serializer->WriteProperty(kPrefixTwo, "", "Value"));
  ASSERT_TRUE(serializer->WriteProperty(kPrefixTwo, "Name", ""));
  ASSERT_TRUE(serializer->WriteProperty("", "AnotherName", ""));
  ASSERT_TRUE(serializer->WriteProperty(kPrefixThree, "SomeOtherName", ""));

  xmlNodePtr node_one = DepthFirstSearch(node, kPrefixTwo);
  ASSERT_NE(nullptr, node_one);
  string value;
  DeserializerImpl deserializer(node_one);
  ASSERT_TRUE(deserializer.ParseString(kPrefixTwo, "Name", &value));
  ASSERT_TRUE(value.empty());
  ASSERT_TRUE(deserializer.ParseString("", "AnotherName", &value));
  ASSERT_TRUE(value.empty());
  ASSERT_FALSE(deserializer.ParseString(kPrefixTwo, "SomeOtherName", &value));
  ASSERT_TRUE(deserializer.ParseString(kPrefixThree, "SomeOtherName", &value));
  ASSERT_TRUE(value.empty());

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteProperty) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl initial_serializer(namespaces, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kPrefixOne, kPrefixTwo);

  const char* property_name = "Name";
  const char* property_value = "Value";
  ASSERT_TRUE(serializer->WriteProperty(kPrefixTwo, property_name,
                                        property_value));
  string value;
  xmlNodePtr node_one = DepthFirstSearch(node, kPrefixTwo);
  DeserializerImpl deserializer(node_one);
  ASSERT_TRUE(deserializer.ParseString(kPrefixTwo, property_name, &value));
  ASSERT_EQ(property_value, value);

  // Write another property.
  const char* property_name2 = "AnotherName";
  const char* property_value2 = "AnotherValue";
  ASSERT_TRUE(serializer->WriteProperty(kPrefixTwo, property_name2,
                                        property_value2));
  ASSERT_TRUE(deserializer.ParseString(kPrefixTwo, property_name2, &value));
  ASSERT_EQ(property_value2, value);

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayNoRdfPrefix) {
  bool add_rdf_namespace = false;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  xmlNsPtr mock_ns = xmlNewNs(nullptr, ToXmlChar("http://fakeh.ref"),
                              ToXmlChar(kPrefixOne));
  xmlSetNs(node, mock_ns);
  ASSERT_NE(nullptr, node->ns);
  EXPECT_EQ(string(kPrefixOne), string(FromXmlChar(node->ns->prefix)));
  int num_nses = 0;
  xmlNsPtr xml_ns = node->ns;
  while (xml_ns) {
    num_nses++;
    xml_ns = xml_ns->next;
  }
  EXPECT_EQ(1, num_nses);
  std::unique_ptr<SerializerImpl> serializer =
      SerializerImpl::FromDataAndSerializeNamespaces(namespaces, node);

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
  ASSERT_FALSE(serializer->WriteDoubleArray(kPrefixTwo, "Parent", values));

  FreeXmlNamespaces(namespaces);
  xmlFreeNs(mock_ns);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayPrefixNotInNamespaces) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  const std::vector<double> values = { 1, 2, 3 };
  ASSERT_FALSE(serializer.WriteDoubleArray("NonExistentPrefix", "Parent",
                                           values));

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayEmptyValues) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  const std::vector<double> values;
  const string parent_name("Parent");
  ASSERT_FALSE(serializer.WriteDoubleArray(kPrefixTwo, parent_name, values));

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayEmptyParentName) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl serializer(namespaces, node);

  const std::vector<double> values = { 1, 2, 3 };
  ASSERT_FALSE(serializer.WriteDoubleArray(kPrefixOne, "", values));

  FreeXmlNamespaces(namespaces);
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteIntArrayEmptyPrefix) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl initial_serializer(namespaces, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kPrefixThree, kPrefixTwo);

  const std::vector<int> values = {1, 2, 3, 4, 5};
  const string parent_name("Parent");
  ASSERT_TRUE(serializer->WriteIntArray("", parent_name, values));

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
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteIntArray) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl initial_serializer(namespaces, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kPrefixThree, kPrefixTwo);

  const std::vector<int> values = {1, 2, 3, 4, 5};
  const string parent_name("Parent");
  ASSERT_TRUE(serializer->WriteIntArray(kPrefixTwo, parent_name, values));

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
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArrayEmptyPrefix) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl initial_serializer(namespaces, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kPrefixThree, kPrefixTwo);

  const std::vector<double> values = { 1.023, 2.112, 3.12, 4.25, 5.67 };
  const string parent_name("Parent");
  ASSERT_TRUE(serializer->WriteDoubleArray("", parent_name, values));

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
  xmlFreeNode(node);
}

TEST(SerializerImpl, WriteDoubleArray) {
  bool add_rdf_namespace = true;
  std::unordered_map<string, xmlNsPtr> namespaces =
      CreateNamespaces(add_rdf_namespace);
  xmlNodePtr node = xmlNewNode(nullptr, ToXmlChar(kPrefixOne));
  SerializerImpl initial_serializer(namespaces, node);
  std::unique_ptr<Serializer> serializer =
      initial_serializer.CreateSerializer(kPrefixThree, kPrefixTwo);

  const std::vector<double> values = { 1.023, 2.112, 3.12, 4.25, 5.67 };
  const string parent_name("Parent");
  ASSERT_TRUE(serializer->WriteDoubleArray(kPrefixTwo, parent_name, values));

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
  xmlFreeNode(node);
}

}  // namespace
}  // namespace xml
}  // namespace xmpmeta
