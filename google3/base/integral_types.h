#ifndef BASE_INTEGRAL_TYPES_H_
#define BASE_INTEGRAL_TYPES_H_

#include "base/integral_types.h"

namespace google {

inline void InitGoogleBase(const char* argv0) {
  // The gflags shim in //third_party/xmpmeta/google3/gflags/gflags.h
  // already calls InitGoogle, which gets base/integral_types.h initialized.
}

}  // namespace google

#endif  // BASE_INTEGRAL_TYPES_H_
