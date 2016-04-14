// Copyright 2010 Google Inc. All Rights Reserved.
// Author: jyrki@google.com (Jyrki Alakuijala)
// Refactored from contributions of various authors in strings/strutil.cc
//
// This file contains string processing functions related to
// uppercase, lowercase, etc.

#include "strings/case.h"

#include "strings/ascii_ctype.h"

namespace xmpmeta {

void LowerString(string* s) {
  string::iterator end = s->end();
  for (string::iterator i = s->begin(); i != end; ++i) {
    *i = ascii_tolower(*i);
  }
}

}  // namespace xmpmeta
