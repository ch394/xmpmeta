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

#ifndef XMPMETA_PANO_META_DATA_H_
#define XMPMETA_PANO_META_DATA_H_

namespace xmpmeta {

// A struct the contains all the meta data needed to describe the geometry
// of a panorama. The fields in the struct are in one-to-one correspondence
// with https://developers.google.com/photo-sphere/metadata
struct PanoMetaData {
  // The dimensions of the full mosaic image that the result image is
  // cropped from.
  // Note that these dimensions currently need to have a 2:1 aspect ratio
  // as the result is rendered as an equi-rectangular panorama.
  int full_width = 0;
  int full_height = 0;

  // The bounds of the result image within the full equi-rectangular 2:1 pano.
  int cropped_width = 0;
  int cropped_height = 0;
  int cropped_left = 0;
  int cropped_top = 0;

  // The initial heading when looking at the panorama, e.g. the center of the
  // cropped region, in degrees.
  int initial_heading_degrees = 0;

  // Returns a PanoMetaData that is a copy of this one, except with pixel
  // coordinates multiplied by the given scale factor.
  PanoMetaData Scale(double s) const;
};

inline PanoMetaData PanoMetaData::Scale(double s) const {
  PanoMetaData scaled = *this;
  scaled.full_width = static_cast<int>(s * full_width);
  scaled.full_height = static_cast<int>(s * full_height);
  scaled.cropped_width = static_cast<int>(s * cropped_width);
  scaled.cropped_height = static_cast<int>(s * cropped_height);
  scaled.cropped_left = static_cast<int>(s * cropped_left);
  scaled.cropped_top = static_cast<int>(s * cropped_top);
  return scaled;
}

}  // namespace xmpmeta

#endif  // XMPMETA_PANO_META_DATA_H_
