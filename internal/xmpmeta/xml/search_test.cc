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

#include "xmpmeta/xml/search.h"

#include <libxml/tree.h>

#include "gtest/gtest.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::FromXmlChar;
using xmpmeta::xml::ToXmlChar;

namespace xmpmeta {
namespace xml {
namespace {

TEST(Search, DepthFirstSearchDocForExistingNode) {
  // Set up XML structure.
  xmlNodePtr root_node = xmlNewNode(nullptr, ToXmlChar("NodeName"));
  xmlNodePtr child_node = xmlNewNode(nullptr, ToXmlChar("ChildNode"));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar("1.0"));

  xmlDocSetRootElement(xml_doc, root_node);
  xmlAddChild(root_node, child_node);

  xmlNodePtr found_node = xml::DepthFirstSearch(xml_doc, "ChildNode");
  ASSERT_NE(nullptr, found_node);
  EXPECT_EQ(string("ChildNode"), string(FromXmlChar(found_node->name)));

  xmlFreeDoc(xml_doc);
}

TEST(Search, DepthFirstSearchDocForNonexistentNode) {
// Set up XML structure.
  xmlNodePtr root_node = xmlNewNode(nullptr, ToXmlChar("NodeName"));
  xmlNodePtr child_node = xmlNewNode(nullptr, ToXmlChar("ChildNode"));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar("1.0"));

  xmlDocSetRootElement(xml_doc, root_node);
  xmlAddChild(root_node, child_node);

  xmlNodePtr found_node = xml::DepthFirstSearch(xml_doc, "NoSuchNode");
  EXPECT_EQ(nullptr, found_node);

  xmlFreeDoc(xml_doc);
}

TEST(Search, DepthFirstSearchNodeForExistingNode) {
// Set up XML structure.
  xmlNodePtr root_node = xmlNewNode(nullptr, ToXmlChar("NodeName"));
  xmlNodePtr child_node = xmlNewNode(nullptr, ToXmlChar("ChildNode"));

  xmlAddChild(root_node, child_node);

  xmlNodePtr found_node = xml::DepthFirstSearch(root_node, "ChildNode");
  ASSERT_NE(nullptr, found_node);
  EXPECT_EQ(string("ChildNode"), string(FromXmlChar(found_node->name)));

  xmlFreeNode(root_node);
}

TEST(Search, DepthFirstSearchNodeForSelf) {
// Set up XML structure.
  xmlNodePtr root_node = xmlNewNode(nullptr, ToXmlChar("NodeName"));

  xmlNodePtr found_node = xml::DepthFirstSearch(root_node, "NodeName");
  ASSERT_NE(nullptr, found_node);
  EXPECT_EQ(string("NodeName"), string(FromXmlChar(found_node->name)));

  xmlFreeNode(root_node);
}

TEST(Search, DepthFirstSearchNodeForNonexistentNode) {
// Set up XML structure.
  xmlNodePtr root_node = xmlNewNode(nullptr, ToXmlChar("NodeName"));
  xmlNodePtr child_node = xmlNewNode(nullptr, ToXmlChar("ChildNode"));

  xmlAddChild(root_node, child_node);

  xmlNodePtr found_node = xml::DepthFirstSearch(root_node, "NoSuchode");
  EXPECT_EQ(nullptr, found_node);

  xmlFreeNode(root_node);
}

}  // namespace
}  // namespace xml
}  // namespace xmpmeta
