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

#ifndef XMPMETA_XML_SERIALIZER_H_
#define XMPMETA_XML_SERIALIZER_H_

#include <memory>
#include <string>
#include <vector>

#include "base/port.h"

namespace xmpmeta {
namespace xml {

// Serializes properties for a hierarchy of objects.
// Example:
//  BookSerializer serializer();
//  // Serialize a list of objects.
//  std::unique_ptr<Serializer> book_list_serializer =
//      serializer->CreateListSerializer("Books");
//  for (Book *book : book_list) {
//    std::unique_ptr<Serializer> book_serializer =
//        cameras_serializer->CreateItemSerializer("Book");
//    success &= book->Serialize(book_serializer.get());
//
//    // Write properties in an object.
//    // This would be called from the Book class.
//    string book_name("Book");
//    std::unique_ptr<Serializer> book_info_serializer =
//        book_serializer->CreateSerializer("Info");
//    book_info_serializer->WriteProperty("Author", "Cereal Eyser");
//    book_info_serializer->WriteProperty("ISBN", "314159265359");
//    std::unique_ptr<Serializer> genre_serializer =
//        book_serializer->CreateSeralizer("Genre", true);
//    std::unique_ptr<Serializer> fantasy_serializer =
//        genre_serializer->CreateSerialzer("Fantasy");
//    // Serialize genre properties here.
//  }

class Serializer {
 public:
  virtual ~Serializer() {}

  // Returns a Serializer for an object that is an item in a list.
  virtual std::unique_ptr<Serializer>
      CreateItemSerializer(const string& item_name) const = 0;

  // Returns a Serializer for a list of objects.
  virtual std::unique_ptr<Serializer>
      CreateListSerializer(const string& list_name) const = 0;

  // Creates a serializer from the current serializer.
  virtual std::unique_ptr<Serializer>
      CreateSerializer(const string& node_name) const = 0;

  // Serializes a property.
  // Example: <NodeName NodeName:PropertyName="PropertyValue" />
  virtual bool WriteProperty(const string& name,
                             const string& value) const = 0;

  // Serializes a property with the given prefix.
  // Example: <NodeName PropertyPrefix:PropertyName="PropertyValue" />
  virtual bool WriteBoolProperty(const string& name, bool value) const = 0;
  virtual bool WritePropertyWithPrefix(const string& prefix, const string& name,
                                       const string& value) const = 0;

  // Serializes the collection of values.
  virtual bool WriteIntArray(const string& array_name,
                             const std::vector<int>& values) const = 0;
  virtual bool WriteDoubleArray(const string& array_name,
                                const std::vector<double>& values) const = 0;
};

}  // namespace xml
}  // namespace xmpmeta

#endif  // XMPMETA_XML_SERIALIZER_H_
