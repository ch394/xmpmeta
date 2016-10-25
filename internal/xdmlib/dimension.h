// Copyright 2016 The XMPMeta Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef XMPMETA_XDM_DIMENSION_H_
#define XMPMETA_XDM_DIMENSION_H_

namespace xmpmeta {
namespace xdm {

// A struct that contains the width and height of a size or the x and y
// coordinates of a point.
struct Dimension {
  Dimension(int w, int h) : width(w), height(h) {}
  int width;
  int height;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_DIMENSION_H_
