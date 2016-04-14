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

#include "xmpmeta/xml/utils.h"

#include <memory>
#include <string>
#include <vector>

#include <libxml/tree.h>

#include "gtest/gtest.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/test_xmp_creator.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_parser.h"
#include "xmpmeta/xmp_writer.h"

#include "const.h"

using xmpmeta::xml::FromXmlChar;

namespace xmpmeta {
namespace xml {
namespace {

// Constants.
const char kInFile[] = "in.jpg";

// XDM test data.
const char* kXmpBody =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\">"
    "     <rdf:Seq>\n"
    "       <rdf:li>1.0</rdf:li>\n"
    "       <rdf:li>2.0</rdf:li>\n"
    "       <rdf:li>\n"
    "         <rdf:Description>\n"
    "           <rdf:Seq>\n"
    "             <rdf:li>string</rdf:li>\n"
    "           </rdf:Seq>\n"
    "         </rdf:Description>\n"
    "       </rdf:li>\n"
    "     </rdf:Seq>\n"
    "   </rdf:Description>\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>\n";

void CreateMetadataFile(const string& in_filename) {
  std::vector<string> standard_xmp;
  standard_xmp.push_back(TestXmpCreator::CreateStandardXmpString(kXmpBody));
  TestXmpCreator::WriteJPEGFile(in_filename, standard_xmp);
}

TEST(XmlUtils, GetFirstSeqElementFromDoc) {
  // Create the metadata.
  const string in_filename = TempFileAbsolutePath(kInFile);
  CreateMetadataFile(in_filename);
  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(in_filename, true, &xmp_data));

  // Get the rdf:Seq element.
  const xmlNodePtr seq_node = GetFirstSeqElement(xmp_data.StandardSection());
  ASSERT_NE(nullptr, seq_node);
  ASSERT_EQ(string(XmlConst::RdfSeq()), string(FromXmlChar(seq_node->name)));

  // Check that it's the right rdf:Seq node.
  xmlNodePtr li_node = GetElementAt(seq_node, 0);
  ASSERT_NE(nullptr, li_node);
  ASSERT_EQ(string("1.0"), GetLiNodeContent(li_node));
}

TEST(XmlUtils, GetFirstSeqElementSeqElement) {
  // Create the metadata.
  const string in_filename = TempFileAbsolutePath(kInFile);
  CreateMetadataFile(in_filename);
  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(in_filename, true, &xmp_data));

  // Get the rdf:Seq element.
  const xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data.StandardSection());
  xmlNodePtr seq_node = GetFirstSeqElement(description_node);
  ASSERT_NE(nullptr, seq_node);
  ASSERT_EQ(string(XmlConst::RdfSeq()), string(FromXmlChar(seq_node->name)));

  // Check that this returns itself.
  ASSERT_NE(nullptr, seq_node);
  ASSERT_EQ(string(XmlConst::RdfSeq()), string(FromXmlChar(seq_node->name)));

  // Check that it's the right rdf:Seq node.
  xmlNodePtr li_node = GetElementAt(seq_node, 0);
  ASSERT_NE(nullptr, li_node);
  ASSERT_EQ(string("1.0"), GetLiNodeContent(li_node));
}

TEST(XmlUtils, GetFirstSeqElementFromNode) {
  // Create the metadata.
  const string in_filename = TempFileAbsolutePath(kInFile);
  CreateMetadataFile(in_filename);
  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(in_filename, true, &xmp_data));

  // Get the rdf:Seq element.
  const xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data.StandardSection());
  xmlNodePtr seq_node = GetFirstSeqElement(description_node);
  ASSERT_NE(nullptr, seq_node);
  ASSERT_EQ(string(XmlConst::RdfSeq()), string(FromXmlChar(seq_node->name)));

  // Get the inner rdf:Seq node.
  xmlNodePtr li_node = GetElementAt(seq_node, 2);
  ASSERT_NE(nullptr, li_node);
  seq_node = GetFirstSeqElement(li_node);
  ASSERT_NE(nullptr, seq_node);

  // Check that it's the right rdf:Seq node.
  li_node = GetElementAt(seq_node, 0);
  ASSERT_NE(nullptr, li_node);
  ASSERT_EQ(string("string"), GetLiNodeContent(li_node));

  // No more rdf:Seq nodes to be found.
  ASSERT_EQ(nullptr, GetFirstSeqElement(li_node));
}

TEST(XmlUtils, GetListElementAndContents) {
  // Create the metadata.
  const string in_filename = TempFileAbsolutePath(kInFile);
  CreateMetadataFile(in_filename);
  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(in_filename, true, &xmp_data));

  // Get the rdf:Seq element.
  xmlNodePtr seq_node = GetFirstSeqElement(xmp_data.StandardSection());
  ASSERT_NE(nullptr, seq_node);

  // Get the list elements and their contents.
  xmlNodePtr li_node = GetElementAt(seq_node, 0);
  ASSERT_NE(nullptr, li_node);
  ASSERT_EQ(string("1.0"), GetLiNodeContent(li_node));

  li_node = GetElementAt(seq_node, 1);
  ASSERT_NE(nullptr, li_node);
  ASSERT_EQ(string("2.0"), GetLiNodeContent(li_node));

  // Get content of an rdf:li node with a nested node.
  li_node = GetElementAt(seq_node, 2);
  ASSERT_NE(nullptr, li_node);
  ASSERT_TRUE(string("") != GetLiNodeContent(li_node));
}

TEST(XmlUtils, GetContentsFromNonListNode) {
  // Create the metadata.
  const string in_filename = TempFileAbsolutePath(kInFile);
  CreateMetadataFile(in_filename);
  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(in_filename, true, &xmp_data));

  // Get the rdf:Seq element.
  xmlNodePtr seq_node = GetFirstSeqElement(xmp_data.StandardSection());
  ASSERT_NE(nullptr, seq_node);
  ASSERT_EQ(0, GetLiNodeContent(seq_node).length());
}

TEST(XmlUtils, GetContentsFromNullNode) {
  ASSERT_EQ(0, GetLiNodeContent(nullptr).length());
}

TEST(XmlUtils, GetListElementFromNonSeqNode) {
  const string in_filename = TempFileAbsolutePath(kInFile);
  CreateMetadataFile(in_filename);
  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(in_filename, true, &xmp_data));
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data.StandardSection());
  ASSERT_EQ(nullptr, GetElementAt(description_node, 0));
}

TEST(XmlUtils, GetListElementAtNegativeIndex) {
  const string in_filename = TempFileAbsolutePath(kInFile);
  CreateMetadataFile(in_filename);
  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(in_filename, true, &xmp_data));
  xmlNodePtr seq_node = GetFirstSeqElement(xmp_data.StandardSection());
  ASSERT_EQ(nullptr, GetElementAt(seq_node, -1));
}

TEST(XmlUtils, GetListElementFromNullNode) {
  ASSERT_EQ(nullptr, GetElementAt(nullptr, 0));
}

}  // namespace
}  // namespace xml
}  // namespace xmpmeta
