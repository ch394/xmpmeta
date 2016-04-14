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

#include "xmpmeta/xml/deserializer_impl.h"

#include <memory>
#include <string>
#include <vector>

#include <libxml/tree.h>

#include "gtest/gtest.h"
#include "strings/numbers.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::ToXmlChar;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace xml {
namespace {

// Creates a new XML node.
xmlNodePtr NewNode(const char* node_name) {
  return xmlNewNode(nullptr, ToXmlChar(node_name));
}

// Creates a new namespace.
xmlNsPtr NewNamespace(const char* href, const char* ns_name) {
  return xmlNewNs(nullptr, ToXmlChar(href), ToXmlChar(ns_name));
}

template <typename T>
xmlNodePtr RdfSeqNodeFromArray(const std::vector<T> data) {
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  for (int i = 0; i < data.size(); i++) {
    xmlNodePtr li_node = NewNode(XmlConst::RdfLi());
    xmlNodeSetContent(li_node, ToXmlChar(std::to_string(data[i]).data()));
    xmlAddChild(seq_node, li_node);
  }
  return seq_node;
}

TEST(DeserializerImpl, CreateDeserializerEmptyChildName) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);

  DeserializerImpl deserializer(node_name, node);
  std::unique_ptr<Deserializer> created_deserializer =
      deserializer.CreateDeserializer("");
  ASSERT_EQ(nullptr, created_deserializer.get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, CreateDeserializerNoChildNode) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  DeserializerImpl deserializer(node_name, node);

  const string child_name("ChildName");
  std::unique_ptr<Deserializer> created_deserializer =
      deserializer.CreateDeserializer(child_name);
  ASSERT_EQ(nullptr, created_deserializer.get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, CreateDeserializer) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  const string child_name("ChildName");
  xmlNodePtr child_node = NewNode(child_name.data());
  xmlAddChild(node, child_node);

  DeserializerImpl deserializer(node_name, node);
  std::unique_ptr<Deserializer> created_deserializer =
      deserializer.CreateDeserializer(child_name);
  ASSERT_NE(nullptr, created_deserializer.get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, CreateDeserializerFromListElementEmptyParentName) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_parent_node = NewNode("Parent");
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node = NewNode(XmlConst::RdfLi());
  xmlAddChild(seq_node, li_node);
  xmlAddChild(seq_parent_node, seq_node);
  xmlAddChild(node, seq_parent_node);

  DeserializerImpl deserializer(node_name, node);
  std::unique_ptr<Deserializer> created_deserializer =
      deserializer.CreateDeserializerFromListElementAt("", 0);
  ASSERT_EQ(nullptr, created_deserializer.get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, CreateDeserializerFromListElementNegativeIndex) {
  const char* node_name = "NodeName";
  const char* parent_name = "Parent";
  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_parent_node = NewNode(parent_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node = NewNode(XmlConst::RdfLi());
  xmlAddChild(seq_node, li_node);
  xmlAddChild(seq_parent_node, seq_node);
  xmlAddChild(node, seq_parent_node);

  DeserializerImpl deserializer(node_name, node);
  std::unique_ptr<Deserializer> created_deserializer =
      deserializer.CreateDeserializerFromListElementAt(parent_name, -1);
  ASSERT_EQ(nullptr, created_deserializer.get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, CreateDeserializerFromListElementNoSeqNode) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);

  DeserializerImpl deserializer(node_name, node);
  std::unique_ptr<Deserializer> created_deserializer =
      deserializer.CreateDeserializerFromListElementAt(node_name, 0);
  ASSERT_EQ(nullptr, created_deserializer.get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, CreateDeserializerFromListElementBadLiElementIndex) {
  const char* parent_name = "Parent";
  xmlNodePtr node = NewNode("NodeName");
  xmlNodePtr seq_parent_node = NewNode(parent_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node = NewNode(XmlConst::RdfLi());
  xmlAddChild(seq_node, li_node);
  xmlAddChild(seq_parent_node, seq_node);
  xmlAddChild(node, seq_parent_node);

  DeserializerImpl deserializer("NodeName", node);
  std::unique_ptr<Deserializer> created_deserializer =
      deserializer.CreateDeserializerFromListElementAt(parent_name, 1);
  ASSERT_EQ(nullptr, created_deserializer.get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, CreateDeserializerFromListElement) {
  const char* node_name = "NodeName";
  const char* parent_name = "Parent";

  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_parent_node = NewNode(parent_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node1 = NewNode(XmlConst::RdfLi());
  xmlNodePtr li_node2 = NewNode(XmlConst::RdfLi());
  xmlNodePtr li_node3 = NewNode(XmlConst::RdfLi());
  xmlAddChild(seq_node, li_node1);
  xmlAddChild(seq_node, li_node2);
  xmlAddChild(seq_node, li_node3);
  xmlAddChild(seq_parent_node, seq_node);
  xmlAddChild(node, seq_parent_node);

  // Test different indices.
  DeserializerImpl deserializer(node_name, node);
  ASSERT_NE(nullptr,
            deserializer.CreateDeserializerFromListElementAt(parent_name,
                                                             0).get());
  ASSERT_NE(nullptr,
            deserializer.CreateDeserializerFromListElementAt(parent_name,
                                                             1).get());
  ASSERT_NE(nullptr,
            deserializer.CreateDeserializerFromListElementAt(parent_name,
                                                             2).get());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseDoubleArrayNoSeqParentNodeElement) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);

  DeserializerImpl deserializer(node_name, node);
  std::vector<double> values;
  ASSERT_FALSE(deserializer.ParseDoubleArray("nonexistentnode", &values));
  ASSERT_TRUE(values.empty());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseDoubleArrayNoSeqElement) {
const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);

  DeserializerImpl deserializer(node_name, node);
  std::vector<double> values;
  ASSERT_FALSE(deserializer.ParseDoubleArray(node_name, &values));
  ASSERT_TRUE(values.empty());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseDoubleArrayNoListElements) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlAddChild(node, seq_node);

  DeserializerImpl deserializer(node_name, node);
  std::vector<double> values;
  ASSERT_TRUE(deserializer.ParseDoubleArray(node_name, &values));
  ASSERT_TRUE(values.empty());

  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseTwoArraysOfReals) {
  const std::vector<double> expected_values_one = { 1.234, 5.678, 8.9011 };
  xmlNodePtr seq_node_one = RdfSeqNodeFromArray(expected_values_one);

  const std::vector<double> expected_values_two = { 3.21, 5.291 };
  xmlNodePtr seq_node_two = RdfSeqNodeFromArray(expected_values_two);

  // Create and set parents of the rdf:Seq nodes.
  const char* child_one_name = "ChildOne";
  const char* child_two_name = "ChildTwo";

  xmlNodePtr node = NewNode("NodeName");
  xmlNodePtr child_one_node = NewNode(child_one_name);
  xmlNodePtr child_two_node = NewNode(child_two_name);
  xmlAddChild(node, child_one_node);
  xmlAddChild(node, child_two_node);
  xmlAddChild(child_one_node, seq_node_one);
  xmlAddChild(child_two_node, seq_node_two);

  // Deserialize the first child node.
  DeserializerImpl deserializer("NodeName", node);
  std::vector<double> values;
  ASSERT_TRUE(deserializer.ParseDoubleArray(child_one_name, &values));
  EXPECT_EQ(expected_values_one, values);

  // Deserialize the second child node.
  ASSERT_TRUE(deserializer.ParseDoubleArray(child_two_name, &values));
  EXPECT_EQ(expected_values_two, values);

  // Clean up. This also frees all of the node's children.
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseTwoArraysOfInts) {
  const std::vector<int> expected_values_one = {20, 30, 40};
  xmlNodePtr seq_node_one = RdfSeqNodeFromArray(expected_values_one);

  const std::vector<int> expected_values_two = {55, 77};
  xmlNodePtr seq_node_two = RdfSeqNodeFromArray(expected_values_two);

  // Create and set parents of the rdf:Seq nodes.
  const char* child_one_name = "ChildOne";
  const char* child_two_name = "ChildTwo";

  xmlNodePtr node = NewNode("NodeName");
  xmlNodePtr child_one_node = NewNode(child_one_name);
  xmlNodePtr child_two_node = NewNode(child_two_name);
  xmlAddChild(node, child_one_node);
  xmlAddChild(node, child_two_node);
  xmlAddChild(child_one_node, seq_node_one);
  xmlAddChild(child_two_node, seq_node_two);

  // Deserialize the first child node.
  DeserializerImpl deserializer("NodeName", node);
  std::vector<int> values;
  ASSERT_TRUE(deserializer.ParseIntArray(child_one_name, &values));
  EXPECT_EQ(expected_values_one, values);

  // Deserialize the second child node.
  ASSERT_TRUE(deserializer.ParseIntArray(child_two_name, &values));
  EXPECT_EQ(expected_values_two, values);

  // Clean up. This also frees all of the node's children.
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseIntegerArrayWithDoubleTypes) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node1 = NewNode(XmlConst::RdfLi());
  xmlNodePtr li_node2 = NewNode(XmlConst::RdfLi());

  // Set list node values.
  xmlNodeSetContent(li_node1, ToXmlChar("1"));
  xmlNodeSetContent(li_node2, ToXmlChar("5.234"));

  // Set up node descendants.
  xmlAddChild(seq_node, li_node1);
  xmlAddChild(seq_node, li_node2);

  xmlAddChild(node, seq_node);
  DeserializerImpl deserializer(node_name, node);
  std::vector<int> values;
  ASSERT_FALSE(deserializer.ParseIntArray(node_name, &values));
  EXPECT_EQ(std::vector<int>({1}), values);

  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseIntegerArrayWithStringType) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node1 = NewNode(XmlConst::RdfLi());
  xmlNodePtr li_node2 = NewNode(XmlConst::RdfLi());

  // Set list node values.
  xmlNodeSetContent(li_node1, ToXmlChar("1"));
  xmlNodeSetContent(li_node2, ToXmlChar("ThisIsNotAnInteger"));

  // Set up node descendants.
  xmlAddChild(seq_node, li_node1);
  xmlAddChild(seq_node, li_node2);

  xmlAddChild(node, seq_node);
  DeserializerImpl deserializer(node_name, node);
  std::vector<int> values;
  EXPECT_FALSE(deserializer.ParseIntArray(node_name, &values));

  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseDoubleArray) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node1 = NewNode(XmlConst::RdfLi());
  xmlNodePtr li_node2 = NewNode(XmlConst::RdfLi());
  xmlNodePtr li_node3 = NewNode(XmlConst::RdfLi());

  // Set list node values.
  const std::vector<double> expected_values = {1.234, 5.678, 8.9011};
  xmlNodeSetContent(li_node1,
                    ToXmlChar(std::to_string(expected_values[0]).data()));
  xmlNodeSetContent(li_node2,
                    ToXmlChar(std::to_string(expected_values[1]).data()));
  xmlNodeSetContent(li_node3,
                    ToXmlChar(std::to_string(expected_values[2]).data()));

  // Set up node descendants.
  xmlAddChild(seq_node, li_node1);
  xmlAddChild(seq_node, li_node2);
  xmlAddChild(seq_node, li_node3);

  xmlAddChild(node, seq_node);
  DeserializerImpl deserializer(node_name, node);
  std::vector<double> values;
  ASSERT_TRUE(deserializer.ParseDoubleArray(node_name, &values));
  EXPECT_EQ(expected_values, values);

  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseDoubleArrayWithStringType) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlNodePtr seq_node = NewNode(XmlConst::RdfSeq());
  xmlNodePtr li_node1 = NewNode(XmlConst::RdfLi());
  xmlNodePtr li_node2 = NewNode(XmlConst::RdfLi());

  // Set list node values.
  const std::vector<double> expected_values = {1.234, 5.678, 8.9011};
  xmlNodeSetContent(li_node1, ToXmlChar("1.23"));
  xmlNodeSetContent(li_node2, ToXmlChar("NotADouble"));

  // Set up node descendants.
  xmlAddChild(seq_node, li_node1);
  xmlAddChild(seq_node, li_node2);

  xmlAddChild(node, seq_node);
  DeserializerImpl deserializer(node_name, node);
  std::vector<double> values;
  EXPECT_FALSE(deserializer.ParseDoubleArray(node_name, &values));

  xmlFreeNode(node);
}

// Variations on a theme of property parser tests.
// Doubles.
TEST(DeserializerImpl, ParseDoubleEmptyName) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlSetProp(node, ToXmlChar("Name"), ToXmlChar(std::to_string(1.234).data()));
  DeserializerImpl deserializer(node_name, node);
  double value;
  ASSERT_FALSE(deserializer.ParseDouble("", &value));
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseDoubleNoProperty) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  DeserializerImpl deserializer(node_name, node);
  double value;
  ASSERT_FALSE(deserializer.ParseDouble("Name", &value));
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseDouble) {
  const char* node_name = "NodeName";
  xmlNsPtr node_ns = NewNamespace("http://somehref.com/", node_name);
  xmlNodePtr node = NewNode(node_name);
  string property_name("Name");
  double property_value = 1.24;

  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
               ToXmlChar(std::to_string(property_value).data()));
  DeserializerImpl deserializer(node_name, node);

  double value;
  ASSERT_TRUE(deserializer.ParseDouble(property_name, &value));
  EXPECT_FLOAT_EQ(property_value, value);

  xmlFreeNs(node_ns);
  xmlFreeNode(node);
}

// String.
TEST(DeserializerImpl, ParseStringEmptyName) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlSetProp(node, ToXmlChar("Name"), ToXmlChar("Value"));
  DeserializerImpl deserializer(node_name, node);
  string value;
  ASSERT_FALSE(deserializer.ParseString("", &value));
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseStringNoProperty) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  DeserializerImpl deserializer(node_name, node);
  string value;
  ASSERT_FALSE(deserializer.ParseString("Name", &value));
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseString) {
  xmlNsPtr node_ns = NewNamespace("http://somehref.com/", "NodeName");
  xmlNodePtr node = NewNode("NodeName");
  string property_name("Name");
  string property_value("Value");

  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
             ToXmlChar(property_value.data()));
  DeserializerImpl deserializer("NodeName", node);

  string value;
  ASSERT_TRUE(deserializer.ParseString(property_name, &value));
  ASSERT_EQ(property_value, value);

  xmlFreeNs(node_ns);
  xmlFreeNode(node);
}

// Base64.
TEST(DeserializerImpl, ParseBase64EmptyName) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  xmlSetProp(node, ToXmlChar("Name"), ToXmlChar("123ABC"));
  DeserializerImpl deserializer(node_name, node);
  string value;
  ASSERT_FALSE(deserializer.ParseBase64("", &value));
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseBase64NoProperty) {
  const char* node_name = "NodeName";
  xmlNodePtr node = NewNode(node_name);
  DeserializerImpl deserializer(node_name, node);
  string value;
  ASSERT_FALSE(deserializer.ParseBase64("Name", &value));
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseBase64) {
  const char* node_name = "NodeName";
  xmlNsPtr node_ns = NewNamespace("http://somehref.com/", node_name);
  xmlNodePtr node = NewNode(node_name);
  string property_name("Name");
  string property_value("SomeValue");

  string base64_encoded;
  EncodeBase64(property_value, &base64_encoded);
  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
               ToXmlChar(base64_encoded.data()));
  DeserializerImpl deserializer(node_name, node);

  string value;
  ASSERT_TRUE(deserializer.ParseBase64(property_name, &value));
  ASSERT_EQ(property_value, value);

  xmlFreeNs(node_ns);
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseBoolean) {
  const char* node_name = "NodeName";
  xmlNsPtr node_ns = NewNamespace("http://somehref.com/", node_name);
  xmlNodePtr node = NewNode(node_name);
  string property_name("FlagProperty");

  // Lowercase.
  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
               ToXmlChar("false"));
  DeserializerImpl deserializer(node_name, node);

  bool value;
  EXPECT_TRUE(deserializer.ParseBoolean(property_name, &value));
  EXPECT_FALSE(value);

  // Uppercase.
  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
               ToXmlChar("TRUE"));
  EXPECT_TRUE(deserializer.ParseBoolean(property_name, &value));
  EXPECT_TRUE(value);

  // Camelcase.
  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
               ToXmlChar("fALse"));
  EXPECT_TRUE(deserializer.ParseBoolean(property_name, &value));
  EXPECT_FALSE(value);

  // Some other string.
  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
               ToXmlChar("falsies"));
  EXPECT_FALSE(deserializer.ParseBoolean(property_name, &value));

  xmlFreeNs(node_ns);
  xmlFreeNode(node);
}

TEST(DeserializerImpl, ParseInt) {
  const char* node_name = "NodeName";
  xmlNsPtr node_ns = NewNamespace("http://somehref.com/", node_name);
  xmlNodePtr node = NewNode(node_name);
  string property_name("Name");
  double property_value = 12345;

  xmlSetNsProp(node, node_ns, ToXmlChar(property_name.data()),
               ToXmlChar(std::to_string(property_value).data()));
  DeserializerImpl deserializer(node_name, node);

  double value;
  ASSERT_TRUE(deserializer.ParseDouble(property_name, &value));
  EXPECT_EQ(property_value, value);

  xmlFreeNs(node_ns);
  xmlFreeNode(node);
}

}  // namespace
}  // namespace xml
}  // namespace xmpmeta
