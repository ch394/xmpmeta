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
//
////////////////////////////////////////////////////////////////////////////////
//
// This file serves two purposes.
//
// 1. Translate the gflags includes used by the OSS version of Ceres
// so that it links into the google3 version.
//
// 2. Call InitGoogle when ParseCommandLineFlags is called. This is
// needed because while google3 binaries call InitGoogle and that call
// initializes the logging and command line handling amongst other
// things, the open source versions of gflags and glog are distributed
// separately and require separate initialization. By hijacking this
// function, and calling InitGoogle, we can compile all the example
// code that ships with Ceres without any modifications. This
// modification will have no impact on google3 binaries using Ceres,
// as they will never call google::ParseCommandLineFlags.

#ifndef GFLAGS_GFLAGS_H_
#define GFLAGS_GFLAGS_H_

#include "base/init_google.h"
#include "base/commandlineflags.h"

namespace google {

inline void ParseCommandLineFlags(int* argc,
                                  char*** argv,
                                  const bool remove_flags) {
  InitGoogle(**argv, argc, argv, remove_flags);
}

}  // namespace google

#endif  // GFLAGS_GFLAGS_H_
