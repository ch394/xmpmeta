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

#include "glog/logging.h"
#include "xmpmeta/xmp_parser.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/serializer.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::PanoMetaData;
using xmpmeta::xml::DeserializerImpl;
using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::ToXmlChar;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace {

const char kPrefix[] = "GPano";
const char kCroppedAreaLeftPixels[] = "CroppedAreaLeftPixels";
const char kCroppedAreaTopPixels[] = "CroppedAreaTopPixels";
const char kCroppedAreaImageWidthPixels[] = "CroppedAreaImageWidthPixels";
const char kCroppedAreaImageHeightPixels[] = "CroppedAreaImageHeightPixels";
const char kFullPanoWidthPixels[] = "FullPanoWidthPixels";
const char kFullPanoHeightPixels[] = "FullPanoHeightPixels";
const char kInitialViewHeadingDegrees[] = "InitialViewHeadingDegrees";
const char kFullPanoWidthPixelsDeprecated[] = "FullPanoImageWidthPixels";
const char kFullPanoHeightPixelsDeprecated[] = "FullPanoImageHeightPixels";
const char kNamespaceHref[] = "http://ns.google.com/photos/1.0/panorama/";

// Extracts metadata from xmp.
bool ParseGPanoFields(const XmpData& xmp, PanoMetaData* meta_data) {
  DeserializerImpl std_deserializer(XmlConst::RdfDescription(),
                                    GetFirstDescriptionElement(
                                        xmp.StandardSection()));
  if (!std_deserializer.ParseInt(kPrefix, kCroppedAreaLeftPixels,
                                 &meta_data->cropped_left)) {
    return false;
  }
  if (!std_deserializer.ParseInt(kPrefix, kCroppedAreaTopPixels,
                                 &meta_data->cropped_top)) {
    return false;
  }
  if (!std_deserializer.ParseInt(kPrefix, kCroppedAreaImageWidthPixels,
                                 &meta_data->cropped_width)) {
    return false;
  }
  if (!std_deserializer.ParseInt(kPrefix, kCroppedAreaImageHeightPixels,
                                 &meta_data->cropped_height)) {
    return false;
  }
  if (!std_deserializer.ParseInt(kPrefix, kFullPanoWidthPixels,
                                 &meta_data->full_width) &&
      !std_deserializer.ParseInt(kPrefix, kFullPanoWidthPixelsDeprecated,
                                 &meta_data->full_width)) {
    return false;
  }
  if (!std_deserializer.ParseInt(kPrefix, kFullPanoHeightPixels,
                                 &meta_data->full_height) &&
      !std_deserializer.ParseInt(kPrefix, kFullPanoHeightPixelsDeprecated,
                                 &meta_data->full_height)) {
    return false;
  }
  if (!std_deserializer.ParseInt(kPrefix, kInitialViewHeadingDegrees,
                                 &meta_data->initial_heading_degrees)) {
    // If kInitialViewHeadingDegrees is not defined, set it to the center of the
    // cropped panorama.
    meta_data->initial_heading_degrees =
        (meta_data->cropped_left + meta_data->cropped_width / 2) * 360 /
        meta_data->full_width;
  }

  return true;
}

}  // namespace

GPano::GPano() {}

void GPano::GetNamespaces(
    std::unordered_map<string, string>* ns_name_href_map) const {
  if (ns_name_href_map == nullptr) {
    LOG(ERROR) << "Namespace list or own namespace is null";
    return;
  }
  ns_name_href_map->emplace(kPrefix, kNamespaceHref);
}

const PanoMetaData& GPano::GetPanoMetaData() const { return meta_data_; }

std::unique_ptr<GPano> GPano::CreateFromData(const PanoMetaData& metadata) {
  std::unique_ptr<GPano> gpano(new GPano());
  gpano->meta_data_ = metadata;
  return gpano;
}

std::unique_ptr<GPano> GPano::FromXmp(const XmpData& xmp) {
  std::unique_ptr<GPano> gpano(new GPano());
  const bool success = ParseGPanoFields(xmp, &gpano->meta_data_);
  return success ? std::move(gpano) : nullptr;
}

std::unique_ptr<GPano> GPano::FromJpegFile(const string& filename) {
  XmpData xmp;
  const bool kSkipExtended = true;
  if (!ReadXmpHeader(filename, kSkipExtended, &xmp)) {
    return nullptr;
  }
  return FromXmp(xmp);
}

bool GPano::Serialize(xml::Serializer* serializer) const {
  if (serializer == nullptr) {
    LOG(ERROR) << "Serializer is null";
    return false;
  }

  const string cropped_left_str = std::to_string(meta_data_.cropped_left);
  const string cropped_top_str = std::to_string(meta_data_.cropped_top);
  const string cropped_width_str = std::to_string(meta_data_.cropped_width);
  const string cropped_height_str = std::to_string(meta_data_.cropped_height);
  const string full_width_str = std::to_string(meta_data_.full_width);
  const string full_height_str = std::to_string(meta_data_.full_height);
  const string init_heading_degrees_str =
      std::to_string(meta_data_.initial_heading_degrees);
  // Short-circuiting ensures that serialization halts at the first error if any
  // occurs.
  if (!serializer->WritePropertyWithPrefix(kPrefix, kCroppedAreaLeftPixels,
                                           cropped_left_str) ||
      !serializer->WritePropertyWithPrefix(kPrefix, kCroppedAreaTopPixels,
                                           cropped_top_str) ||
      !serializer->WritePropertyWithPrefix(kPrefix,
                                           kCroppedAreaImageWidthPixels,
                                           cropped_width_str) ||
      !serializer->WritePropertyWithPrefix(kPrefix,
                                           kCroppedAreaImageHeightPixels,
                                           cropped_height_str) ||
      !serializer->WritePropertyWithPrefix(kPrefix, kFullPanoWidthPixels,
                                           full_width_str) ||
      !serializer->WritePropertyWithPrefix(kPrefix, kFullPanoHeightPixels,
                                           full_height_str) ||
      !serializer->WritePropertyWithPrefix(kPrefix, kInitialViewHeadingDegrees,
                                           init_heading_degrees_str)) {
    return false;
  }
  return true;
}

}  // namespace xmpmeta
