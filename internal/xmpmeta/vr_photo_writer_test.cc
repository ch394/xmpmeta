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

#include "xmpmeta/vr_photo_writer.h"

#include <memory>
#include <vector>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "xmpmeta/file.h"
#include "xmpmeta/pano_meta_data.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xml/utils.h"

namespace xmpmeta {
namespace {

const char kStandardSectionWithAudioData[] =
    "vr_photo_with_audio_std_section_data.txt";
const char kExtendedSectionWithAudioData[] =
   "vr_photo_with_audio_ext_section_data.txt";

const char kStandardSectionWithoutAudioData[] =
    "vr_photo_no_audio_std_section_data.txt";
const char kExtendedSectionWithoutAudioData[] =
    "vr_photo_no_audio_ext_section_data.txt";

TEST(VrPhotoWriter, WriteWithAudio) {
  const string imageData = "ImageData";
  const string imageMime = "image/jpeg";
  std::unique_ptr<GImage> gimage = GImage::CreateFromData(imageData, imageMime);
  EXPECT_EQ(imageData, gimage->GetData());
  EXPECT_EQ(imageMime, gimage->GetMime());

  const string audioData = "AudioData";
  const string audioMime = "audio/mp4";
  std::unique_ptr<GAudio> gaudio = GAudio::CreateFromData(audioData, audioMime);
  EXPECT_EQ(audioData, gaudio->GetData());
  EXPECT_EQ(audioMime, gaudio->GetMime());

  PanoMetaData meta;
  meta.full_width = 10;
  meta.full_height = 20;
  meta.cropped_width = 5;
  meta.cropped_height = 10;
  meta.cropped_left = 0;
  meta.cropped_top = 3;
  std::unique_ptr<GPano> gpano = GPano::CreateFromData(meta);

  // Write the XMP.
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  ASSERT_NE(nullptr, xmp_data->ExtendedSection());
  ASSERT_TRUE(WriteVrPhotoMetaToXmp(*gimage, *gpano, gaudio.get(),
                                    xmp_data.get()));

  // Check that the XML document matches our expectations.
  const string std_section_data_path =
      TestFileAbsolutePath(kStandardSectionWithAudioData);
  std::string expected_data;
  ReadFileToStringOrDie(std_section_data_path, &expected_data);
  EXPECT_EQ(expected_data, xml::XmlDocToString(xmp_data->StandardSection()));
  const string ext_section_data_path =
      TestFileAbsolutePath(kExtendedSectionWithAudioData);
  ReadFileToStringOrDie(ext_section_data_path, &expected_data);
  EXPECT_EQ(expected_data, xml::XmlDocToString(xmp_data->ExtendedSection()));
}

TEST(VrPhotoWriter, WriteWithoutAudio) {
  const string imageData = "ImageData";
  const string imageMime = "image/jpeg";
  std::unique_ptr<GImage> gimage = GImage::CreateFromData(imageData, imageMime);
  EXPECT_EQ(imageData, gimage->GetData());
  EXPECT_EQ(imageMime, gimage->GetMime());

  PanoMetaData meta;
  meta.full_width = 10;
  meta.full_height = 20;
  meta.cropped_width = 5;
  meta.cropped_height = 10;
  meta.cropped_left = 0;
  meta.cropped_top = 3;
  std::unique_ptr<GPano> gpano = GPano::CreateFromData(meta);

  // Write the XMP.
  std::unique_ptr<XmpData> xmp_data = CreateXmpData(true);
  ASSERT_NE(nullptr, xmp_data->ExtendedSection());
  ASSERT_TRUE(WriteVrPhotoMetaToXmp(*gimage, *gpano, nullptr, xmp_data.get()));

  // Check that the XML document matches our expectations.
  const string std_section_data_path =
      TestFileAbsolutePath(kStandardSectionWithoutAudioData);
  std::string expected_data;
  ReadFileToStringOrDie(std_section_data_path, &expected_data);
  EXPECT_EQ(expected_data, xml::XmlDocToString(xmp_data->StandardSection()));
  const string ext_section_data_path =
      TestFileAbsolutePath(kExtendedSectionWithoutAudioData);
  ReadFileToStringOrDie(ext_section_data_path, &expected_data);
  EXPECT_EQ(expected_data, xml::XmlDocToString(xmp_data->ExtendedSection()));
}

}  // namespace
}  // namespace xmpmeta
