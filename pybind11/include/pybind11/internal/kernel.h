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

class handle;

/// hold the object temporarily
template <int N>
struct reg_t {
    py_Ref value;

    void operator= (py_Ref ref) & { py_setreg(N, ref); }

    operator py_Ref () & {
        assert(value && "register is not initialized");
        return value;
    }

    void operator= (handle value) &;

    operator handle () &;

    // pkpy provide user 8 registers.
    // 8th register is used for object pool, so N is limited to [0, 7).
    static_assert(N >= 0 && N <= 6, "N must be in [0, 7)");
};

struct retv_t {
    py_Ref value;

    void operator= (py_Ref ref) & { py_assign(value, ref); }

    operator py_Ref () & {
        assert(value && "return value is not initialized");
        return value;
    }

    void operator= (handle value) &;

    operator handle () &;
};

/// hold the object long time.
struct object_pool {
    inline static int cache = -1;
    inline static py_Ref pool = nullptr;
    inline static std::vector<int>* indices_ = nullptr;

    struct object_ref {
        py_Ref data;
        int index;
    };

    static void initialize(int size) noexcept {
        // use 8th register.
        pool = py_getreg(7);
        py_newtuple(pool, size);
        indices_ = new std::vector<int>(size, 0);
    }

    static void finalize() noexcept { delete indices_; }

    /// alloc an object from pool, note that the object is uninitialized.
    static object_ref alloc() {
        auto& indices = *indices_;
        if(cache != -1) {
            auto index = cache;
            cache = -1;
            indices[index] = 1;
            return {py_tuple_getitem(pool, index), index};
        }

        for(int i = 0; i < indices.size(); ++i) {
            if(indices[i] == 0) {
                indices[i] = 1;
                return {py_tuple_getitem(pool, i), i};
            }
        }

        throw std::runtime_error("object pool is full");
    }

    /// alloc an object from pool, the object is initialized with ref.
    static object_ref realloc(py_Ref ref) {
        auto result = alloc();
        py_assign(result.data, ref);
        return result;
    }

    static void inc_ref(object_ref ref) {
        if(ref.data == py_tuple_getitem(pool, ref.index)) {
            auto& indices = *indices_;
            indices[ref.index] += 1;
        } else {
            throw std::runtime_error("object_ref is invalid");
        }
    }

    static void dec_ref(object_ref ref) {
        if(ref.data == py_tuple_getitem(pool, ref.index)) {
            auto& indices = *indices_;
            indices[ref.index] -= 1;
            assert(indices[ref.index] >= 0 && "ref count is negative");
            if(indices[ref.index] == 0) { cache = ref.index; }
        } else {
            throw std::runtime_error("object_ref is invalid");
        }
    }
};

struct action {
    using function = void (*)();
    inline static std::vector<function>* starts = nullptr;
    inline static std::vector<function>* ends = nullptr;

    static void initialize() noexcept {
        if(starts) {
            for(auto func: *starts) {
                func();
            }
            delete starts;
        }
    }

    static void finalize() noexcept {
        if(ends) {
            for(auto func: *ends) {
                func();
            }
            delete ends;
        }
    }

    // register a function to be called at the start of the vm.
    static void register_start(function func) {
        if(!starts) { starts = new std::vector<function>(); }
        starts->push_back(func);
    }

    // register a function to be called at the end of the vm.
    static void register_end(function func) {
        if(!ends) { ends = new std::vector<function>(); }
        ends->push_back(func);
    }
};

template <int N>
inline reg_t<N> reg;

inline retv_t retv;

/// initialize the vm.
inline void initialize(int object_pool_size = 1024) {
    py_initialize();
    reg<0> = {py_getreg(0)};
    reg<1> = {py_getreg(1)};
    reg<2> = {py_getreg(2)};
    reg<3> = {py_getreg(3)};
    reg<4> = {py_getreg(4)};
    reg<5> = {py_getreg(5)};
    reg<6> = {py_getreg(6)};
    retv = {py_retval()};
    object_pool::initialize(object_pool_size);
    action::initialize();
}

/// finalize the vm.
inline void finalize() {
    action::finalize();
    object_pool::finalize();
    py_finalize();
}

}  // namespace pkbind
