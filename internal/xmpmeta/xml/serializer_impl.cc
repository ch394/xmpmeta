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

#include <libxml/tree.h>

#include "glog/logging.h"
#include "strings/numbers.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/utils.h"

namespace xmpmeta {
namespace xml {

// Methods specific to SerializerImpl.
SerializerImpl::SerializerImpl(
    const std::unordered_map<string, xmlNsPtr>& namespaces,
    const std::unordered_map<string, xmlNsPtr>& prefixes,
    const string& node_name, xmlNodePtr node) :
    node_name_(node_name), node_(node), namespaces_(namespaces),
    prefixes_(prefixes) {
  CHECK(node_ != nullptr) << "Node cannot be null";
  CHECK(!node_name_.empty()) << "Node name cannot be empty";
  CHECK(node_->name != nullptr) << "Name in the XML node cannot be null";
}

bool SerializerImpl::SerializeNamespaces() {
  if (namespaces_.empty()) {
    return true;
  }
  if (node_->ns == nullptr && !namespaces_.empty()) {
    return false;
  }
  // Check that the namespaces all have hrefs and that there is a value
  // for the key node_name.
  // Set the namespaces in the root node.
  xmlNsPtr node_ns = node_->ns;
  for (const auto& entry : namespaces_) {
    CHECK(entry.second->href != nullptr) << "Namespace href cannot be null";
    if (node_ns != nullptr) {
      node_ns->next = entry.second;
    }
    node_ns = entry.second;
  }
  return true;
}

std::unique_ptr<SerializerImpl> SerializerImpl::FromDataAndSerializeNamespaces(
    const std::unordered_map<string, xmlNsPtr>& namespaces,
    const std::unordered_map<string, xmlNsPtr>& prefixes,
    const string& node_name, xmlNodePtr node) {
  std::unique_ptr<SerializerImpl> serializer =
      std::unique_ptr<SerializerImpl>(
          new SerializerImpl(namespaces, prefixes, node_name, node));
  if (!serializer->SerializeNamespaces()) {
    LOG(ERROR) << "Could not serialize namespace for node " << node_name;
    return nullptr;
  }
  return serializer;
}

// Implemented methods.
std::unique_ptr<Serializer>
SerializerImpl::CreateSerializer(const string& node_name) const {
  if (node_name.empty()) {
    LOG(ERROR) << "Node name is empty";
    return nullptr;
  }

  if (prefixes_.count(node_name_) == 0) {
    LOG(ERROR) << "Prefix " << node_name_ << " not found in prefix list";
    return nullptr;
  }

  xmlNodePtr new_node =
      xmlNewNode(prefixes_.at(node_name_), ToXmlChar(node_name.data()));
  xmlAddChild(node_, new_node);
  return std::unique_ptr<Serializer>(
      new SerializerImpl(namespaces_, prefixes_, node_name, new_node));
}

std::unique_ptr<Serializer>
SerializerImpl::CreateItemSerializer(const string& item_name) const {
  if (prefixes_.count(XmlConst::RdfPrefix()) == 0 ||
      prefixes_.at(XmlConst::RdfPrefix()) == nullptr) {
    LOG(ERROR) << "No RDF prefix namespace found";
    return nullptr;
  }
  if (strcmp(XmlConst::RdfSeq(), FromXmlChar(node_->name)) != 0) {
    LOG(ERROR) << "No rdf:Seq node for serializing this item";
    return nullptr;
  }

  xmlNsPtr rdf_prefix_ns = prefixes_.at(string(XmlConst::RdfPrefix()));
  xmlNodePtr li_node =
      xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfLi()));
  xmlNodePtr new_node =
      xmlNewNode(prefixes_.at(node_name_), ToXmlChar(item_name.data()));
  xmlSetNs(li_node, rdf_prefix_ns);
  xmlAddChild(node_, li_node);
  xmlAddChild(li_node, new_node);
  return std::unique_ptr<Serializer>(
      new SerializerImpl(namespaces_, prefixes_, item_name, new_node));
}

std::unique_ptr<Serializer>
SerializerImpl::CreateListSerializer(const string& list_name) const {
  if (prefixes_.count(XmlConst::RdfPrefix()) == 0 ||
      prefixes_.at(XmlConst::RdfPrefix()) == nullptr) {
    LOG(ERROR) << "No RDF prefix namespace found";
    return nullptr;
  }

  if (prefixes_.count(node_name_) == 0 ||
      prefixes_.at(node_name_) == nullptr) {
    LOG(ERROR) << "Node name " << node_name_ << " not found in prefixes";
    return nullptr;
  }

  xmlNsPtr list_prefix_ns = prefixes_.at(node_name_);
  xmlNodePtr list_node =
      xmlNewNode(list_prefix_ns, ToXmlChar(list_name.data()));
  xmlNsPtr rdf_prefix_ns = prefixes_.at(string(XmlConst::RdfPrefix()));
  xmlNodePtr seq_node = xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfSeq()));
  xmlSetNs(seq_node, rdf_prefix_ns);
  xmlAddChild(list_node, seq_node);
  xmlAddChild(node_, list_node);
  return std::unique_ptr<Serializer>(
      new SerializerImpl(namespaces_, prefixes_, node_name_, seq_node));
}

bool SerializerImpl::WriteBoolProperty(const string& name, bool value) const {
  const string& bool_str = (value ? "true" : "false");
  return WriteProperty(name, bool_str);
}

bool SerializerImpl::WriteProperty(const string& name,
                                   const string& value) const {
  return WritePropertyWithPrefix(node_name_, name, value);
}

bool SerializerImpl::WritePropertyWithPrefix(const string& prefix,
                                             const string& name,
                                             const string& value) const {
  if (!strcmp(XmlConst::RdfSeq(), FromXmlChar(node_->name))) {
    LOG(ERROR) << "Cannot write a property on an rdf:Seq node";
    return false;
  }
  if (prefix.empty() || name.empty() || value.empty()) {
    LOG(ERROR) << "Property value or name is empty";
    return false;
  }

  // Check that prefix has a corresponding namespace href.
  if (namespaces_.count(prefix) == 0) {
    LOG(ERROR) << "No namespace found for prefix " << prefix;
    return false;
  }

  // Serialize the property in the format Prefix:Name="Value".
  xmlNsPtr prefix_ns = namespaces_.at(prefix);
  xmlSetNsProp(node_, prefix_ns, ToXmlChar(name.data()),
               ToXmlChar(value.data()));
  return true;
}

bool SerializerImpl::WriteIntArray(const string& array_name,
                                   const std::vector<int>& values) const {
  if (!strcmp(XmlConst::RdfSeq(), FromXmlChar(node_->name))) {
    LOG(ERROR) << "Cannot write a property on an rdf:Seq node";
    return false;
  }
  if (values.empty()) {
    LOG(WARNING) << "No values to write";
    return false;
  }
  if (namespaces_.count(node_name_) == 0 ||
      namespaces_.at(node_name_) == nullptr) {
    LOG(ERROR) << "No prefix found for " << node_name_;
    return false;
  }
  if (prefixes_.count(XmlConst::RdfPrefix()) == 0 ||
      prefixes_.at(XmlConst::RdfPrefix()) == nullptr) {
    LOG(ERROR) << "No RDF prefix found";
    return false;
  }
  if (array_name.empty()) {
    LOG(ERROR) << "Parent name cannot be empty";
    return false;
  }

  xmlNsPtr node_ns = namespaces_.at(node_name_);
  xmlNodePtr array_parent_node =
      xmlNewNode(node_ns, ToXmlChar(array_name.data()));
  xmlAddChild(node_, array_parent_node);

  xmlNsPtr rdf_prefix_ns = prefixes_.at(XmlConst::RdfPrefix());
  xmlNodePtr seq_node = xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfSeq()));
  xmlSetNs(seq_node, rdf_prefix_ns);
  xmlAddChild(array_parent_node, seq_node);
  for (int value : values) {
    xmlNodePtr li_node = xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfLi()));
    xmlSetNs(li_node, rdf_prefix_ns);
    xmlAddChild(seq_node, li_node);
    xmlNodeSetContent(li_node, ToXmlChar(std::to_string(value).c_str()));
  }

  return true;
}

bool SerializerImpl::WriteDoubleArray(const string& array_name,
                                      const std::vector<double>& values) const {
  if (!strcmp(XmlConst::RdfSeq(), FromXmlChar(node_->name))) {
    LOG(ERROR) << "Cannot write a property on an rdf:Seq node";
    return false;
  }
  if (values.empty()) {
    LOG(WARNING) << "No values to write";
    return false;
  }
  if (namespaces_.count(node_name_) == 0 ||
      namespaces_.at(node_name_) == nullptr) {
    LOG(ERROR) << "No prefix found for " << node_name_;
    return false;
  }
  if (prefixes_.count(XmlConst::RdfPrefix()) == 0 ||
      prefixes_.at(XmlConst::RdfPrefix()) == nullptr) {
    LOG(ERROR) << "No RDF prefix found";
    return false;
  }
  if (array_name.empty()) {
    LOG(ERROR) << "Parent name cannot be empty";
    return false;
  }

  xmlNsPtr node_ns = namespaces_.at(node_name_);
  xmlNodePtr array_parent_node =
      xmlNewNode(node_ns, ToXmlChar(array_name.data()));
  xmlAddChild(node_, array_parent_node);

  xmlNsPtr rdf_prefix_ns = prefixes_.at(XmlConst::RdfPrefix());
  xmlNodePtr seq_node =
      xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfSeq()));
  xmlSetNs(seq_node, rdf_prefix_ns);
  xmlAddChild(array_parent_node, seq_node);
  for (float value : values) {
    xmlNodePtr li_node =
        xmlNewNode(nullptr, ToXmlChar(XmlConst::RdfLi()));
    xmlSetNs(li_node, rdf_prefix_ns);
    xmlAddChild(seq_node, li_node);
    xmlNodeSetContent(li_node, ToXmlChar(SimpleFtoa(value).c_str()));
  }

  return true;
}

}  // namespace xml
}  // namespace xmpmeta
