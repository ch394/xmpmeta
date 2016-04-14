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

#include "xdmlib/camera.h"

#include <libxml/tree.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/port.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "xdmlib/audio.h"
#include "xdmlib/camera_pose.h"
#include "xdmlib/image.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/file.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/xmp_const.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/utils.h"

using testing::Pair;
using testing::UnorderedElementsAre;
using testing::StrEq;
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

const char kPrefix[] = "Camera";
const char kAudioPrefix[] = "Audio";
const char kNamespaceHref[] = "http://ns.xdm.org/photos/1.0/camera/";
const char kAudioNamespaceHref[] = "http://ns.xdm.org/photos/1.0/audio/";
const char kCameraDataPath[] = "xdm/camera_testdata.txt";

const char kMediaData[] = "123ABC456DEF";

// Convenience function for creating an XML node.
xmlNodePtr NewNode(const string& node_name) {
  return xmlNewNode(nullptr, ToXmlChar(node_name.data()));
}

// Convenience function for creating an XML namespace.
xmlNsPtr NewNs(const string& href, const string& ns_name) {
  return xmlNewNs(nullptr, href.empty() ? nullptr : ToXmlChar(href.data()),
                  ToXmlChar(ns_name.data()));
}

std::unique_ptr<Audio> CreateAudio() {
  return Audio::FromData(kMediaData, "audio/mp4");
}

std::unique_ptr<Image> CreateImage() {
  return Image::FromData(kMediaData, "image/jpeg");
}

std::unique_ptr<CameraPose> CreateCameraPose() {
  double axis_x = 1;
  double axis_y = 2;
  double axis_z = 1.5;
  double angle = 1.57;
  std::vector<double> orientation = { axis_x, axis_y, axis_z, angle };
  return CameraPose::FromData(std::vector<double>(), orientation, -1);
}

TEST(Camera, GetNamespaces) {
  std::unordered_map<string, string> ns_name_href_map;
  string prefix(kPrefix);

  std::unique_ptr<Audio> audio = CreateAudio();
  std::unique_ptr<Camera> camera =
      Camera::FromData(std::move(audio), nullptr, nullptr);
  ASSERT_NE(nullptr, camera);

  ASSERT_TRUE(ns_name_href_map.empty());
  camera->GetNamespaces(&ns_name_href_map);
  EXPECT_THAT(ns_name_href_map,
              UnorderedElementsAre(
                  Pair(StrEq(prefix), StrEq(kNamespaceHref)),
                  Pair(StrEq(kAudioPrefix), StrEq(kAudioNamespaceHref))));
}


// TODO(miraleung): Add corner case tests when the rest of the elements are
// checked in.
TEST(Camera, FromData) {
  std::unique_ptr<Audio> audio = CreateAudio();
  std::unique_ptr<Image> image = CreateImage();
  std::unique_ptr<CameraPose> camera_pose = CreateCameraPose();

  const string audio_mime = audio->GetMime();
  const string image_mime = image->GetMime();

  std::unique_ptr<Camera> camera =
      Camera::FromData(std::move(audio), std::move(image),
                       std::move(camera_pose));
  ASSERT_NE(nullptr, camera);

  const Audio* read_audio = camera->GetAudio();
  ASSERT_NE(nullptr, read_audio);
  EXPECT_EQ(kMediaData, read_audio->GetData());
  EXPECT_EQ(audio_mime, read_audio->GetMime());

  const Image* read_image = camera->GetImage();
  ASSERT_NE(nullptr, read_image);
  EXPECT_EQ(kMediaData, read_image->GetData());
  EXPECT_EQ(image_mime, read_image->GetMime());

  const CameraPose* read_pose = camera->GetCameraPose();
  ASSERT_NE(nullptr, read_pose);
}

TEST(Camera, FromDataWithNullElements) {
  std::unique_ptr<Image> image = CreateImage();
  const string image_mime = image->GetMime();

  std::unique_ptr<Camera> camera = Camera::FromData(nullptr, std::move(image),
                                                    nullptr);
  ASSERT_NE(nullptr, camera);

  const Audio* read_audio = camera->GetAudio();
  ASSERT_EQ(nullptr, read_audio);

  const Image* read_image = camera->GetImage();
  EXPECT_EQ(kMediaData, read_image->GetData());
  EXPECT_EQ(image_mime, read_image->GetMime());

  EXPECT_EQ(nullptr, camera->GetCameraPose());
}

TEST(Camera, Serialize) {
  std::unique_ptr<Audio> audio = CreateAudio();
  std::unique_ptr<Image> image = CreateImage();
  std::unique_ptr<CameraPose> camera_pose = CreateCameraPose();
  std::unique_ptr<Camera> camera =
      Camera::FromData(std::move(audio), std::move(image),
                       std::move(camera_pose));
  ASSERT_NE(nullptr, camera);

  // Create XML serializer.
  const char device_name[] = "Device";
  const char camera_name[] = "Camera";
  const char audio_name[] = "Audio";
  const char image_name[] = "Image";
  const char camera_pose_name[] = "CameraPose";
  const char namespaceHref[] = "http://notarealh.ref";

  std::unordered_map<string, xmlNsPtr> namespaces;
  namespaces.emplace(device_name, NewNs(namespaceHref, device_name));
  namespaces.emplace(camera_name, NewNs(kNamespaceHref, camera_name));
  namespaces.emplace(audio_name, NewNs(namespaceHref, audio_name));
  namespaces.emplace(image_name, NewNs(namespaceHref, image_name));
  namespaces.emplace(camera_pose_name, NewNs(namespaceHref, camera_pose_name));

  std::unordered_map<string, xmlNsPtr> prefixes;
  prefixes.emplace(device_name, NewNs("", device_name));
  prefixes.emplace(camera_name, NewNs("", camera_name));

  xmlNodePtr device_node = NewNode(device_name);
  xmlDocPtr xml_doc = xmlNewDoc(ToXmlChar(XmlConst::Version()));
  xmlDocSetRootElement(xml_doc, device_node);

  // Create serializer.
  SerializerImpl serializer(namespaces, prefixes, device_name, device_node);
  std::unique_ptr<Serializer> camera_serializer =
      serializer.CreateSerializer(camera_name);
  ASSERT_NE(nullptr, camera_serializer);

  ASSERT_TRUE(camera->Serialize(camera_serializer.get()));

  const string xdm_camera_data_path = TestFileAbsolutePath(kCameraDataPath);
  std::string expected_xdm_data;
  ReadFileToStringOrDie(xdm_camera_data_path, &expected_xdm_data);
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

TEST(Camera, ReadMetadata) {
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  xmlNodePtr description_node =
      GetFirstDescriptionElement(xmp_data->ExtendedSection());

  xmlNodePtr camera_node = NewNode(kPrefix);
  xmlAddChild(description_node, camera_node);

  // Set up Audio node.
  const char audio_name[] = "Audio";
  xmlNodePtr audio_node = NewNode(audio_name);
  xmlAddChild(camera_node, audio_node);

  // Set Audio properties.
  string base64_encoded;
  ASSERT_TRUE(EncodeBase64(kMediaData, &base64_encoded));
  const char namespaceHref[] = "http://notarealh.ref";
  const string audio_mime("audio/wav");
  xmlNsPtr audio_ns = NewNs(namespaceHref, audio_name);
  xmlSetNsProp(audio_node, audio_ns, ToXmlChar("Mime"),
               ToXmlChar(audio_mime.data()));
  xmlSetNsProp(audio_node, audio_ns, ToXmlChar("Data"),
               ToXmlChar(base64_encoded.data()));

  // Set up Image node.
  const char image_name[] = "Image";
  xmlNodePtr image_node = NewNode(image_name);
  xmlAddChild(camera_node, image_node);

  const string image_mime("image/jpeg");
  xmlNsPtr image_ns = NewNs(namespaceHref, "Image");
  xmlSetNsProp(image_node, image_ns, ToXmlChar("Mime"),
               ToXmlChar(image_mime.data()));
  xmlSetNsProp(image_node, image_ns, ToXmlChar("Data"),
               ToXmlChar(base64_encoded.data()));

  // Set up Camera Pose node.
  xmlNodePtr pose_node = NewNode("CameraPose");
  xmlAddChild(camera_node, pose_node);
  xmlNsPtr pose_ns = NewNs(namespaceHref, "CameraPose");

  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisX"), ToXmlChar("1"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisY"), ToXmlChar("2"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAxisZ"),
               ToXmlChar("1.5"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("RotationAngle"),
               ToXmlChar("1.57"));
  xmlSetNsProp(pose_node, pose_ns, ToXmlChar("Timestamp"),
               ToXmlChar("1455818790"));

  // Create an Camera from the metadata.
  DeserializerImpl deserializer(kPrefix, description_node);
  std::unique_ptr<Camera> camera = Camera::FromDeserializer(deserializer);
  ASSERT_NE(nullptr, camera.get());

  // Check elements.
  const Audio* read_audio = camera->GetAudio();
  ASSERT_NE(nullptr, read_audio);
  EXPECT_EQ(kMediaData, read_audio->GetData());
  EXPECT_EQ(audio_mime, read_audio->GetMime());

  const Image* read_image = camera->GetImage();
  ASSERT_NE(nullptr, read_image);
  EXPECT_EQ(kMediaData, read_image->GetData());
  EXPECT_EQ(image_mime, read_image->GetMime());

  const CameraPose* read_pose = camera->GetCameraPose();
  std::unique_ptr<CameraPose> pose = CreateCameraPose();
  ASSERT_NE(nullptr, read_pose);
  EXPECT_EQ(pose->GetOrientationRotationXYZAngle(),
            read_pose->GetOrientationRotationXYZAngle());
  EXPECT_EQ(pose->HasPosition(), read_pose->HasPosition());

  xmlFreeNs(audio_ns);
  xmlFreeNs(image_ns);
  xmlFreeNs(pose_ns);
}

}  // namespace
}  // namespace xdm
}  // namespace xmpmeta
