#pragma once

#include <array>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <typeindex>
#include <stdexcept>
#include <unordered_map>

#include "pocketpy.h"
#include "type_traits.h"

namespace pkbind {

inline void initialize();

inline void finalize();

}  // namespace pkbind
