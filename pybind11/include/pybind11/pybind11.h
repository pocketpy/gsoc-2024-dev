#pragma once

#include "internal/class.h"

namespace pkbind {

namespace literals {
inline arg operator""_a (const char* c, size_t) { return arg(c); }
}  // namespace literals

/// a RAII class to initialize and finalize python interpreter
class scoped_interpreter {
public:
    scoped_interpreter(int object_pool_size = 1024) { initialize(object_pool_size); }

    ~scoped_interpreter() { finalize(); }
};

}  // namespace pkbind
