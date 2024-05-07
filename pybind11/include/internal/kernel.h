#pragma once

#include <pocketpy.h>

namespace pybind11
{
    inline pkpy::VM* vm = nullptr;
    inline std::map<pkpy::PyObject*, int*>* _ref_counts_map = nullptr;

    inline void init(bool enable_os = true)
    {
        vm = ::new pkpy::VM(enable_os);
        _ref_counts_map = ::new std::map<pkpy::PyObject*, int*>();

        // use to keep alive PyObject, when the object is hold by C++ side.
        vm->heap._gc_marker_ex = [](pkpy::VM* vm)
        {
            for(auto iter = _ref_counts_map->begin(); iter != _ref_counts_map->end();)
            {
                auto ref_count = iter->second;
                if(*ref_count != 0)
                {
                    // if ref count is not zero, then mark it.
                    PK_OBJ_MARK(iter->first);
                    ++iter;
                }
                else
                {
                    // if ref count is zero, then delete it.
                    iter = _ref_counts_map->erase(iter);
                    ::delete ref_count;
                }
            }
        };
    }
}  // namespace pybind11
