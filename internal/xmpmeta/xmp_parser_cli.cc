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


// Command-line tool for extracting the XMP metadata for a VR photo.
// The PanoMetaData is printed out, and the right panorama and audio files are
// optionally saved to disk.
//
// Example usage:
//  xmp_parser_cli \
//    --input=<input_jpeg_file> \
//    --output_image=<output_jpeg_file> \
//    --output_audio=<output_audio_file> \
//    --alsologtostderr

#include <memory>
#include <string>

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "xmpmeta/file.h"
#include "xmpmeta/gaudio.h"
#include "xmpmeta/gimage.h"
#include "xmpmeta/gpano.h"
#include "xmpmeta/pano_meta_data.h"
#include "xmpmeta/xmp_data.h"
#include "xmpmeta/xmp_parser.h"
#include "xmpmeta/xml/utils.h"

DEFINE_string(input, "", "Jpeg file to be parsed.");
DEFINE_string(output_image, "", "Jpeg file for the right eye image.");
DEFINE_string(output_audio, "", "Audio file if present.");

using xmpmeta::PanoMetaData;
using xmpmeta::GAudio;
using xmpmeta::GImage;
using xmpmeta::GPano;
using xmpmeta::ReadXmpHeader;
using xmpmeta::WriteStringToFileOrDie;
using xmpmeta::XmpData;

// Prints the PanoMetaData to the log.
void PrintPanoMetaData(const PanoMetaData& meta_data) {
  LOG(INFO) << "Parsed PanoMetaData:";
  LOG(INFO) << " Cropped left: " << meta_data.cropped_left;
  LOG(INFO) << " Cropped top: " << meta_data.cropped_top;
  LOG(INFO) << " Cropped width: " << meta_data.cropped_width;
  LOG(INFO) << " Cropped height: " << meta_data.cropped_height;
  LOG(INFO) << " Full width: " << meta_data.full_width;
  LOG(INFO) << " Full height: " << meta_data.full_height;
  LOG(INFO) << " Initial heading: " << meta_data.initial_heading_degrees;
}

int main(int argc, char** argv) {
  InitGoogle(argv[0], &argc, &argv, true);
  QCHECK(!FLAGS_input.empty());

  // Parse the XMP.
  XmpData xmp;
  const bool skip_extended =
      FLAGS_output_image.empty() && FLAGS_output_audio.empty();
  QCHECK(ReadXmpHeader(FLAGS_input, skip_extended, &xmp));

  // Print the PanoMetaData.
  auto gpano = CHECK_NOTNULL(GPano::FromXmp(xmp));
  PrintPanoMetaData(gpano->GetPanoMetaData());

  // Optionally decode and save the right image.
  if (!FLAGS_output_image.empty()) {
    auto gimage = CHECK_NOTNULL(GImage::FromXmp(xmp));
    WriteStringToFileOrDie(gimage->GetData(), FLAGS_output_image);
  }

  // Optionally decode and save the audio (if it exists).
  if (!FLAGS_output_audio.empty()) {
    auto gaudio = GAudio::FromXmp(xmp);
    if (gaudio != nullptr) {
      WriteStringToFileOrDie(gaudio->GetData(), FLAGS_output_audio);
    } else {
      LOG(WARNING) << "Pano does not appear to have audio";
    }
  }

  return 0;
}
