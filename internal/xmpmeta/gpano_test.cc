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

#include "xmpmeta/gpano.h"

#include <memory>
#include <string>

#include <libxml/tree.h>

#include "glog/logging.h"
#include "xmpmeta/pano_meta_data.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/serializer_impl.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::PanoMetaData;
using xmpmeta::xml::SerializerImpl;
using xmpmeta::xml::ToXmlChar;

namespace xmpmeta {
namespace {

const int kCroppedLeft = 2420;
const int kCroppedTop = 1396;
const int kCroppedWidth = 3782;
const int kCroppedHeight = 1566;
const int kFullWidth = 8192;
const int kFullHeight = 4096;
const int kInitialHeadingDegrees = 189;

std::unique_ptr<SerializerImpl> CreateSerializer(const XmpData& xmp_data,
                                                 const xmlNsPtr xml_ns) {
  std::unordered_map<string, xmlNsPtr> namespaces;
  // Prefix map can be empty since new nodes will not be created on this
  // serializer.
  std::unordered_map<string, xmlNsPtr> prefixes;
  namespaces.emplace("GPano", xml_ns);
  std::unique_ptr<SerializerImpl> serializer =
      SerializerImpl::FromDataAndSerializeNamespaces(
          namespaces, prefixes, xml::XmlConst::RdfDescription(),
          xml::GetFirstDescriptionElement(xmp_data.StandardSection()));
  return serializer;
}

TEST(GPano, ParseVrPhotoXmp) {
  const string left_path = TestFileAbsolutePath("left_with_xmp.jpg");
  std::unique_ptr<GPano> gpano = GPano::FromJpegFile(left_path);
  ASSERT_NE(nullptr, gpano);

  const PanoMetaData& meta_data = gpano->GetPanoMetaData();
  EXPECT_EQ(kCroppedLeft, meta_data.cropped_left);
  EXPECT_EQ(kCroppedTop, meta_data.cropped_top);
  EXPECT_EQ(kCroppedWidth, meta_data.cropped_width);
  EXPECT_EQ(kCroppedHeight, meta_data.cropped_height);
  EXPECT_EQ(kFullWidth, meta_data.full_width);
  EXPECT_EQ(kFullHeight, meta_data.full_height);
  EXPECT_EQ(kInitialHeadingDegrees, meta_data.initial_heading_degrees);
}

TEST(GPano, BadPath) {
  std::unique_ptr<GPano> gpano = GPano::FromJpegFile("bad_path.jpg");
  ASSERT_EQ(nullptr, gpano);
}

TEST(GPano, ToVrPhotoXmp) {
  PanoMetaData new_meta_data;
  new_meta_data.cropped_left = kCroppedLeft;
  new_meta_data.cropped_top = kCroppedTop;
  new_meta_data.cropped_width = kCroppedWidth;
  new_meta_data.cropped_height = kCroppedHeight;
  new_meta_data.full_width = kFullWidth;
  new_meta_data.full_height = kFullHeight;
  new_meta_data.initial_heading_degrees = kInitialHeadingDegrees;

  // Doesn't matter whether there's an extended section or not.
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(false);
  std::unique_ptr<GPano> new_gpano = GPano::CreateFromData(new_meta_data);
  ASSERT_NE(nullptr, new_gpano);

  xmlNsPtr xml_ns =
      xmlNewNs(nullptr, ToXmlChar("http://fakehref.com/"), ToXmlChar("GPano"));
  std::unique_ptr<SerializerImpl> serializer =
      CreateSerializer(*xmp_data, xml_ns);
  ASSERT_TRUE(new_gpano->Serialize(serializer.get()));

  std::unique_ptr<GPano> gpano_from_xmp = GPano::FromXmp(*xmp_data);
  ASSERT_NE(nullptr, gpano_from_xmp);

  const PanoMetaData& meta_data = gpano_from_xmp->GetPanoMetaData();
  EXPECT_EQ(kCroppedLeft, meta_data.cropped_left);
  EXPECT_EQ(kCroppedTop, meta_data.cropped_top);
  EXPECT_EQ(kCroppedWidth, meta_data.cropped_width);
  EXPECT_EQ(kCroppedHeight, meta_data.cropped_height);
  EXPECT_EQ(kFullWidth, meta_data.full_width);
  EXPECT_EQ(kFullHeight, meta_data.full_height);
  EXPECT_EQ(kInitialHeadingDegrees, meta_data.initial_heading_degrees);
}

TEST(GPano, ToVrPhotoXmpWithNullSerializer) {
  PanoMetaData new_meta_data;
  std::unique_ptr<GPano> new_gpano = GPano::CreateFromData(new_meta_data);
  ASSERT_FALSE(new_gpano->Serialize(nullptr));
}

}  // namespace
}  // namespace xmpmeta
