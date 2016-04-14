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

#ifndef XMPMETA_XML_DESERIALIZER_H_
#define XMPMETA_XML_DESERIALIZER_H_

#include <memory>
#include <string>
#include <vector>

#include "base/integral_types.h"
#include "base/port.h"

namespace xmpmeta {
namespace xml {

// Performs deserialization.
// Example:
//   Deserializer deserializer();
//   string revision;
//   deserializer.ParseString("Revision", &revision);
class Deserializer {
 public:
  virtual ~Deserializer() {}

  // Returns a Deserializer.
  // child_name is the name of the next node to deserialize.
  virtual std::unique_ptr<Deserializer>
      CreateDeserializer(const string& child_name) const = 0;

  // Returns a Deserializer from a list element node.
  virtual std::unique_ptr<Deserializer>
      CreateDeserializerFromListElementAt(const string& list_name,
                                             int index) const = 0;

  // Parsers for property types.
  // Parses a node such as <Prefix Prefix:Name="Value" />
  virtual bool ParseBase64(const string& name, string* value) const = 0;
  virtual bool ParseBoolean(const string& name, bool* value) const = 0;
  virtual bool ParseInt(const string& name, int* value) const = 0;
  virtual bool ParseDouble(const string& name, double* value) const = 0;
  virtual bool ParseLong(const string& name, int64* value) const = 0;
  virtual bool ParseString(const string& name, string* value) const = 0;

  // Parsers for properties with the given prefix.
  // Parses a node such as <NodeName Prefix:Name="Value" />
  virtual bool ParseBase64(const string& prefix, const string& name,
                           string* value) const = 0;
  virtual bool ParseBoolean(const string& prefix, const string& name,
                            bool* value) const = 0;
  virtual bool ParseInt(const string& prefix, const string& name,
                        int* value) const = 0;
  virtual bool ParseDouble(const string& prefix, const string& name,
                           double* value) const = 0;
  virtual bool ParseLong(const string& prefix, const string& name,
                         int64* value) const = 0;
  virtual bool ParseString(const string& prefix, const string& name,
                           string* value) const = 0;

  // Parsers for arrays.
  virtual bool ParseIntArray(const string& list_name,
                             std::vector<int>* values) const = 0;
  virtual bool ParseDoubleArray(const string& list_name,
                                std::vector<double>* values) const = 0;
};

}  // namespace xml
}  // namespace xmpmeta

#endif  // XMPMETA_XML_DESERIALIZER_H_
