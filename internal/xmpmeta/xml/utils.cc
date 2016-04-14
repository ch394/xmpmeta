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

#include "glog/logging.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/search.h"

namespace xmpmeta {
namespace xml {

xmlNodePtr GetFirstDescriptionElement(const xmlDocPtr parent) {
  return DepthFirstSearch(parent, XmlConst::RdfDescription());
}

xmlNodePtr GetFirstSeqElement(xmlDocPtr parent) {
  // DepthFirstSearch will perform the null check.
  return DepthFirstSearch(parent, XmlConst::RdfSeq());
}

// Returns the first rdf:Seq element found in the given node.
xmlNodePtr GetFirstSeqElement(xmlNodePtr parent) {
  // DepthFirstSearch will perform the null check.
  return DepthFirstSearch(parent, XmlConst::RdfSeq());
}

// Returns the ith (zero-indexed) element in the given node.
// {@code parent} is an rdf:Seq node.
xmlNodePtr GetElementAt(xmlNodePtr node, int index) {
  if (node == nullptr || index < 0) {
    LOG(ERROR) << "Node was null or index was negative";
    return nullptr;
  }
  const string node_name = FromXmlChar(node->name);
  if (strcmp(node_name.c_str(), XmlConst::RdfSeq())) {
    LOG(ERROR) << "Node is not an rdf:Seq node, was " << node_name;
    return nullptr;
  }
  int i = 0;
  for (xmlNodePtr child = node->children; child != nullptr && i <= index;
       child = child->next) {
    if (strcmp(FromXmlChar(child->name), XmlConst::RdfLi())) {
      // This is not an rdf:li node. This can occur because the node's content
      // is also treated as a node, and these should be ignored.
      continue;
    }
    if (i == index) {
      return child;
    }
    i++;
  }
  return nullptr;
}

const string GetLiNodeContent(xmlNodePtr node) {
  string value;
  if (node == nullptr || strcmp(FromXmlChar(node->name),
                                XmlConst::RdfLi())) {
    LOG(ERROR) << "Node is null or is not an rdf:li node";
    return value;
  }
  xmlChar* node_content = xmlNodeGetContent(node);
  value = FromXmlChar(node_content);
  free(node_content);
  return value;
}

const string XmlDocToString(const xmlDocPtr doc) {
  xmlChar* xml_doc_contents;
  int doc_size = 0;
  xmlDocDumpFormatMemoryEnc(doc, &xml_doc_contents, &doc_size,
                            XmlConst::EncodingStr(), 1);
  const string xml_doc_string(FromXmlChar(xml_doc_contents));
  xmlFree(xml_doc_contents);
  return xml_doc_string;
}

}  // namespace xml
}  // namespace xmpmeta
