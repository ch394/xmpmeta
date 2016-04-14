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

#include "xmpmeta/md5.h"

#include <sstream>
#include <string>

#include "gtest/gtest.h"

namespace xmpmeta {
namespace {

const int kExpectedHashLength = 32;

TEST(MD5, MD5Hash) {
  std::ostringstream data_stream;
  // Make a very large payload.
  for (int i = 0; i < 100000; i++) {
    for (int j = 0xff; j >= 0; j--) {
      data_stream.put(static_cast<char>(j));
    }
  }

  const string data = data_stream.str();
  string expected("e0fd444298d27b86d6bd865780dd71e0");
  string value =  MD5Hash(data);
  ASSERT_EQ(expected, value);
  ASSERT_EQ(kExpectedHashLength, value.length());
}

TEST(MD5, MD5HashNullString) {
  std::ostringstream data_stream;
  // Make a null payload.
  for (int i = 0; i < 100; i++) {
    data_stream.put(static_cast<char>(0));
  }

  const string data = data_stream.str();
  string expected("6d0bb00954ceb7fbee436bb55a8397a9");
  string value =  MD5Hash(data);
  ASSERT_EQ(expected, value);
  ASSERT_EQ(kExpectedHashLength, value.length());
}

}  // namespace
}  // namespace xmpmeta
