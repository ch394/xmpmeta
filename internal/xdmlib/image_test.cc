// xdmlib. A fast XDM parsing and writing library.
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

#include "xdmlib/image.h"

#include <libxml/tree.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "xmpmeta/file.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/xmp_const.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::CreateXmpData;
using xmpmeta::xml::Deserializer;
using xmpmeta::xml::DeserializerImpl;
using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::Serializer;
using xmpmeta::xml::SerializerImpl;
using xmpmeta::xml::ToXmlChar;
using xmpmeta::xml::XmlConst;
using xmpmeta::xml::XmlDocToString;

namespace xmpmeta {
namespace xdm {
namespace {

const char kPrefix[] = "Image";
const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/image/";
const char kImageDataPath[] = "xdm/image_testdata.txt";

TEST(Image, GetNamespaces) {
  std::unordered_map<string, string> ns_name_href_map;
  string prefix(kPrefix);

  string data("123ABC456DEF");
  string mime("image/jpeg");
  std::unique_ptr<Image> image = Image::FromData(data, mime);
  ASSERT_NE(nullptr, image);

  ASSERT_TRUE(ns_name_href_map.empty());
  image->GetNamespaces(&ns_name_href_map);
  EXPECT_EQ(1, ns_name_href_map.size());
  EXPECT_EQ(string(kNamespaceHref), string(ns_name_href_map[prefix]));

  // Same namespace will not be added again.
  image->GetNamespaces(&ns_name_href_map);
  EXPECT_EQ(1, ns_name_href_map.size());
  EXPECT_EQ(string(kNamespaceHref), string(ns_name_href_map[prefix]));
}

TEST(Image, FromData) {
  string data("123ABC456DEF");
  string mime("image/jpeg");
  std::unique_ptr<Image> image = Image::FromData(data, mime);
  ASSERT_NE(nullptr, image);
  // Set to default mimetype.
  EXPECT_EQ(mime, image->GetMime());
  EXPECT_EQ(data, image->GetData());
}

TEST(Image, FromEmptyData) {
  string data("123ABC456DEF");
  string mime("image/jpeg");
  std::unique_ptr<Image> image = Image::FromData(string(""), string(""));
  EXPECT_EQ(nullptr, image);
  image = Image::FromData(data, string(""));
  EXPECT_EQ(nullptr, image);
  image = Image::FromData(string(""), mime);
  EXPECT_EQ(nullptr, image);
}

TEST(Image, Serialize) {
  string data("123ABC456DEF");
  string mime("image/jpeg");

  std::unique_ptr<Image> image = Image::FromData(data, mime);

  // Create XML serializer.
  const char device_name[] = "Device";
  const char camera_name[] = "Camera";
  const char image_name[] = "Image";
  const char namespaceHref[] = "http://notarealh.ref";
  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(device_name, xmlNewNs(nullptr, ToXmlChar(namespaceHref),
                                           ToXmlChar(device_name)));
  namespaces.emplace(camera_name, xmlNewNs(nullptr, ToXmlChar(namespaceHref),
                                           ToXmlChar(camera_name)));
  namespaces.emplace(image_name, xmlNewNs(nullptr, ToXmlChar(namespaceHref),
                                          ToXmlChar(image_name)));
  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(device_name,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(device_name)));
  prefixes.emplace(camera_name,
                   xmlNewNs(nullptr, nullptr, ToXmlChar(camera_name)));
  xmlNodePtr device_node = xmlNewNode(nullptr, ToXmlChar(device_name));
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, device_node);

  // Create serializer.
  SerializerImpl serializer(namespaces, prefixes, device_name, device_node);
  std::unique_ptr<Serializer> camera_serializer =
      serializer.CreateSerializer(camera_name);
  ASSERT_NE(nullptr, camera_serializer);
  std::unique_ptr<Serializer> image_serializer =
      camera_serializer->CreateSerializer(kPrefix);
  ASSERT_NE(nullptr, image_serializer);

  ASSERT_TRUE(image->Serialize(image_serializer.get()));

  const string xdm_image_data_path = TestFileAbsolutePath(kImageDataPath);
  std::string expected_xdm_data;
  ReadFileToStringOrDie(xdm_image_data_path, &expected_xdm_data);
  EXPECT_EQ(expected_xdm_data, XmlDocToString(xml_doc));

  // Free all XML objects.
  for (const auto& entry : namespaces) {
    xmlFreeNs(entry.second);
  }
  for (const auto& entry : prefixes) {
    xmlFreeNs(entry.second);
  }
  xmlFreeDoc(xml_doc);
}

TEST(Image, ReadMetadata) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  // Mock an XdmCamera node.
  xmlNodePtr camera_node = xmlNewNode(nullptr, ToXmlChar("Camera"));
  xmlAddChild(description_node, camera_node);

  string prefix(kPrefix);
  xmlNodePtr image_node = xmlNewNode(nullptr, ToXmlChar(prefix.data()));
  xmlAddChild(camera_node, image_node);

  string data("123ABC456DEF");
  string base64_encoded;
  ASSERT_TRUE(EncodeBase64(data, &base64_encoded));
  string mime("image/jpeg");
  xmlNsPtr image_ns = xmlNewNs(nullptr, ToXmlChar(kNamespaceHref),
                               ToXmlChar(prefix.data()));
  xmlSetNsProp(image_node, image_ns, ToXmlChar("Mime"), ToXmlChar(mime.data()));
  xmlSetNsProp(image_node, image_ns, ToXmlChar("Data"),
               ToXmlChar(base64_encoded.data()));

  // Create an Image from the metadata.
  DeserializerImpl deserializer(prefix, description_node);
  std::unique_ptr<Image> image = Image::FromDeserializer(deserializer);
  ASSERT_NE(nullptr, image.get());
  EXPECT_EQ(mime, image->GetMime());
  EXPECT_EQ(data, image->GetData());

  xmlFreeNs(image_ns);
}

}  // namespace
}  // namespace xdm
}  // namespace xmpmeta
