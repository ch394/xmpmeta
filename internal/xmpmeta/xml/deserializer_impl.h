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

#ifndef XMPMETA_XML_DESERIALIZER_IMPL_H_
#define XMPMETA_XML_DESERIALIZER_IMPL_H_

#include <map>
#include <mutex>
#include <string>

#include <libxml/tree.h>

#include "base/port.h"
#include "xmpmeta/xml/deserializer.h"

namespace xmpmeta {
namespace xml {

// Deserializes an XML node.
// Example:
//   xmlNodePtr device_node =
//       DepthFirstSearch(xmp.ExtendedSection(), "Description");
//   std::string device_name("Device");
//   DeserializerImpl deserializer(device_name, device_node);
//   string revision;
//   deserializer.ParseString("Revision", &revision);
// TODO(miraleung): Add example for list node deserializer.
class DeserializerImpl : public Deserializer {
 public:
  // Creates a deserializer with a null rdf:Seq node.
  DeserializerImpl(const string& node_name, const xmlNodePtr node);

  // Returns a Deserializer.
  // child_name is the name of the next node to deserialize.
  std::unique_ptr<Deserializer>
      CreateDeserializer(const string& child_name) const override;

  // Returns a Deserializer from a list element node, if one is available as
  // a descendant of node_.
  // Returns null if seq_node_ is null or if the index is out of range.
  std::unique_ptr<Deserializer>
      CreateDeserializerFromListElementAt(const string& list_name,
                                          int index) const override;

  // Parsers for XML properties prefixed with node_name_.
  bool ParseBase64(const string& name, string* value) const override;
  bool ParseBoolean(const string& name, bool* value) const override;
  bool ParseDouble(const string& name, double* value) const override;
  bool ParseInt(const string& name, int* value) const override;
  bool ParseLong(const string& name, int64* value) const override;
  bool ParseString(const string& name, string* value) const override;

  // Parsers for XML properties prefixed with the given prefix.
  bool ParseBase64(const string& prefix, const string& name,
                   string* value) const override;
  bool ParseBoolean(const string& prefix, const string& name,
                    bool* value) const override;
  bool ParseDouble(const string& prefix, const string& name,
                   double* value) const override;
  bool ParseInt(const string& prefix, const string& name,
                int* value) const override;
  bool ParseLong(const string& prefix, const string& name,
                 int64* value) const override;
  bool ParseString(const string& prefix, const string& name,
                   string* value) const override;

  // Parses the numbers in an rdf:Seq list into the values collection.
  // The given collection is cleared of any existing values, and the
  // parsed numbers are written to it.
  bool ParseIntArray(const string& list_name,
                     std::vector<int>* values) const override;
  bool ParseDoubleArray(const string& list_name,
                        std::vector<double>* values) const override;

  // Disallow copying.
  DeserializerImpl(const DeserializerImpl&) = delete;
  void operator=(const DeserializerImpl&) = delete;

 private:
  string node_name_;
  xmlNodePtr node_;
  // Remembers the parent node of the last deserializer created on the rdf:Seq
  // node. For performance reasons only, to avoid unnessarily traversing
  // the XML document tree.
  mutable xmlNodePtr list_node_;
  // Lock modifications of list_node_ in const functions to make it thread-safe.
  mutable std::mutex mtx_;
};

}  // namespace xml
}  // namespace xmpmeta

#endif  // XMPMETA_XML_DESERIALIZER_IMPL_H_
