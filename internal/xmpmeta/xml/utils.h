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

// Utility functions for writing and parsing XML metadata.

#ifndef XMPMETA_XML_UTILS_H_
#define XMPMETA_XML_UTILS_H_

#include <string>

#include <libxml/tree.h>

#include "base/port.h"

namespace xmpmeta {
namespace xml {

// Convenience function to convert an xmlChar* to a char*
inline const char* FromXmlChar(const xmlChar* in) {
  return reinterpret_cast<const char*>(in);
}

// Convenience function to convert a char* to an xmlChar*.
inline const xmlChar* ToXmlChar(const char* in) {
  return reinterpret_cast<const xmlChar*>(in);
}

// Returns the first rdf:Description node; null if not found.
xmlNodePtr GetFirstDescriptionElement(xmlDocPtr parent);

// Returns the first rdf:Seq element found in the XML document.
xmlNodePtr GetFirstSeqElement(xmlDocPtr parent);

// Returns the first rdf:Seq element found in the given node.
// Returns {@code parent} if that is itself an rdf:Seq node.
xmlNodePtr GetFirstSeqElement(xmlNodePtr parent);

// Returns the ith (zero-indexed) rdf:li node in the given rdf:Seq node.
// Returns null if either of {@code index} < 0, {@code node} is null, or is
// not an rdf:Seq node.
xmlNodePtr GetElementAt(xmlNodePtr node, int index);

// Returns the value in an rdf:li node. This is for a node whose value
// does not have a name, e.g. <rdf:li>value</rdf:li>.
// If the given rdf:li node has a nested node, it returns the string
// representation of the contents of those nodes, which replaces the XML
// tags with one whitespace character for each tag character.
// This is treated as undefined behavior; it is the caller's responsibility
// to remove any whitespace and newlines.
const string GetLiNodeContent(xmlNodePtr node);

// Returns the given XML doc serialized to a string.
// For debugging purposes.
const string XmlDocToString(const xmlDocPtr doc);

}  // namespace xml
}  // namespace xmpmeta

#endif  // XMPMETA_XML_UTILS_H_
