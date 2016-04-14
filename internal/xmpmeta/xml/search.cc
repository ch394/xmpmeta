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

#include <string>
#include <stack>

#include "glog/logging.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::FromXmlChar;

namespace xmpmeta {
namespace xml {

xmlNodePtr DepthFirstSearch(const xmlDocPtr parent, const char* name) {
  if (parent == nullptr || parent->children == nullptr) {
    LOG(ERROR) << "XML doc was null or has no XML nodes";
    return nullptr;
  }
  xmlNodePtr result;
  for (xmlNodePtr node = parent->children; node != nullptr;
       node = node->next) {
    result = DepthFirstSearch(node, name);
    if (result != nullptr) {
      return result;
    }
  }
  LOG(WARNING) << "No node matching the name " << name << " was found";
  return nullptr;
}

xmlNodePtr DepthFirstSearch(const xmlNodePtr parent, const char* name) {
  if (parent == nullptr) {
    LOG(ERROR) << "XML node was null";
    return nullptr;
  }
  std::stack<xmlNodePtr> node_stack;
  node_stack.push(parent);
  while (!node_stack.empty()) {
    const xmlNodePtr current_node = node_stack.top();
    node_stack.pop();
    if (strcmp(FromXmlChar(current_node->name), name) == 0) {
      return current_node;
    }
    std::stack<xmlNodePtr> stack_to_reverse;
    for (xmlNodePtr child = current_node->children; child != nullptr;
         child = child->next) {
      stack_to_reverse.push(child);
    }
    while (!stack_to_reverse.empty()) {
      node_stack.push(stack_to_reverse.top());
      stack_to_reverse.pop();
    }
  }
  return nullptr;
}

}  // namespace xml
}  // namespace xmpmeta
