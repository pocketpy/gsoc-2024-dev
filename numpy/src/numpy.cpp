#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <numpy.hpp>
#include <typeinfo>

namespace py = pybind11;
using float64 = pkpy::float64;

// Function to parse attributes
int parseAttr(const py::object &obj) {
    if (py::isinstance<py::none>(obj)) {
        return INT_MAX;
    } else if (py::isinstance<py::int_>(obj)) {
        return obj.cast<int>();
    } else {
        throw std::runtime_error("Unsupported type");
    }
};

class ndarray_base {
public:
    virtual ~ndarray_base() = default;

    virtual int ndim() const = 0;
    virtual int size() const = 0;
    virtual std::string dtype() const = 0;
    virtual std::vector<int> shape() const = 0;
    virtual bool all() const = 0;
    virtual bool any() const = 0;  
    virtual py::object sum() const = 0;
    virtual ndarray_base* sum_axis(int axis) const = 0;
    virtual ndarray_base* sum_axes(py::tuple axes) const = 0;
    virtual py::object prod() const = 0;
    virtual ndarray_base* prod_axis(int axis) const = 0;
    virtual ndarray_base* prod_axes(py::tuple axes) const = 0;
    virtual py::object min() const = 0;
    virtual ndarray_base* min_axis(int axis) const = 0;
    virtual ndarray_base* min_axes(py::tuple axes) const = 0;
    virtual py::object max() const = 0;
    virtual ndarray_base* max_axis(int axis) const = 0;
    virtual ndarray_base* max_axes(py::tuple axes) const = 0;
    virtual py::object mean() const = 0;
    virtual ndarray_base* mean_axis(int axis) const = 0;
    virtual ndarray_base* mean_axes(py::tuple axes) const = 0;
    virtual py::object std() const = 0;
    virtual ndarray_base* std_axis(int axis) const = 0;
    virtual ndarray_base* std_axes(py::tuple axes) const = 0;
    virtual py::object var() const = 0;
    virtual ndarray_base* var_axis(int axis) const = 0;
    virtual ndarray_base* var_axes(py::tuple axes) const = 0;
    virtual py::object argmin() const = 0;
    virtual ndarray_base* argmin_axis(int axis) const = 0;
    virtual py::object argmax() const = 0;
    virtual ndarray_base* argmax_axis(int axis) const = 0;
    virtual ndarray_base* argsort() const = 0;
    virtual ndarray_base* argsort_axis(int axis) const = 0;
    virtual void sort() = 0;
    virtual void sort_axis(int axis) = 0;
    virtual ndarray_base* reshape(const std::vector<int>& shape) const = 0;
    virtual ndarray_base* squeeze() const = 0;
    virtual ndarray_base* squeeze_axis(int axis) const = 0;
    virtual ndarray_base* transpose() const = 0;
    virtual ndarray_base* transpose_tuple(py::tuple permutations) const = 0;
    virtual ndarray_base* transpose_args(py::args args) const = 0;
    virtual ndarray_base* repeat(int repeats, int axis) const = 0;
    virtual ndarray_base* repeat_axis(const std::vector<size_t>& repeats, int axis) const = 0;
    virtual ndarray_base* flatten() const = 0;
    virtual ndarray_base* copy() const = 0;
    virtual ndarray_base* astype(const std::string& dtype) const = 0;
    virtual bool eq(const ndarray_base& other) const = 0;
    virtual ndarray_base* add(const ndarray_base& other) const = 0;
    virtual ndarray_base* add_int(int other) const = 0;
    virtual ndarray_base* add_float(float64 other) const = 0;
    virtual ndarray_base* sub(const ndarray_base& other) const = 0;
    virtual ndarray_base* sub_int(int other) const = 0;
    virtual ndarray_base* sub_float(float64 other) const = 0;
    virtual ndarray_base* rsub_int(int other) const = 0;
    virtual ndarray_base* rsub_float(float64 other) const = 0;
    virtual ndarray_base* mul(const ndarray_base& other) const = 0;
    virtual ndarray_base* mul_int(int other) const = 0;
    virtual ndarray_base* mul_float(float64 other) const = 0;
    virtual ndarray_base* div(const ndarray_base& other) const = 0;
    virtual ndarray_base* div_int(int other) const = 0;
    virtual ndarray_base* div_float(float64 other) const = 0;
    virtual ndarray_base* rdiv_int(int other) const = 0;
    virtual ndarray_base* rdiv_float(float64 other) const = 0;
    virtual ndarray_base* pow(const ndarray_base& other) const = 0;
    virtual ndarray_base* pow_int(int other) const = 0;
    virtual ndarray_base* pow_float(float64 other) const = 0;
    virtual ndarray_base* rpow_int(int other) const = 0;
    virtual ndarray_base* rpow_float(float64 other) const = 0;
    virtual ndarray_base* matmul(const ndarray_base& other) const = 0;
    virtual py::object get_item_int(int index) const = 0;
    virtual py::object get_item_tuple(py::tuple indices) const = 0;
    virtual ndarray_base* get_item_vector(const std::vector<int>& indices) const = 0;
    virtual ndarray_base* get_item_slice(py::slice slice) const = 0;
    virtual void set_item_int(int index, int value) = 0;
    virtual void set_item_index_int(int index, const std::vector<int>& value) = 0;
    virtual void set_item_float(int index, float64 value) = 0;
    virtual void set_item_index_float(int index, const std::vector<float64>& value) = 0;
    virtual void set_item_tuple_int(py::tuple args, int value) = 0;
    virtual void set_item_tuple_float(py::tuple args, float64 value) = 0;
    virtual void set_item_vector_int1(const std::vector<int>& indices, int value) = 0;
    virtual void set_item_vector_int2(const std::vector<int>& indices, const std::vector<int>& value) = 0;
    virtual void set_item_vector_float1(const std::vector<int>& indices, float64 value) = 0;
    virtual void set_item_vector_float2(const std::vector<int>& indices, const std::vector<float64>& value) = 0;
    virtual void set_item_slice_int1(py::slice slice, int value) = 0;
    virtual void set_item_slice_int2(py::slice slice, const std::vector<int>& value) = 0;
    virtual void set_item_slice_float1(py::slice slice, float64 value) = 0;
    virtual void set_item_slice_float2(py::slice slice, const std::vector<float64>& value) = 0;
    virtual int len() const = 0;
    virtual std::string to_string() const = 0;
};

template<typename T>
class ndarray : public ndarray_base {
public:
    pkpy::numpy::ndarray<T> data;
    // Constructors
    ndarray() = default;
    ndarray(const int value) : data(value) {}
    ndarray(const float64 value) : data(value) {}
    ndarray(const pkpy::numpy::ndarray<T>& _arr) : data(_arr) {}
    ndarray(const std::vector<T>& init_list) : data(pkpy::numpy::adapt<T>(init_list)) {}
    ndarray(const std::vector<std::vector<T>>& init_list) : data(pkpy::numpy::adapt<T>(init_list)) {}
    ndarray(const std::vector<std::vector<std::vector<T>>>& init_list) : data(pkpy::numpy::adapt<T>(init_list)) {}
    ndarray(const std::vector<std::vector<std::vector<std::vector<T>>>>& init_list) : data(pkpy::numpy::adapt<T>(init_list)) {}
    ndarray(const std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>& init_list) : data(pkpy::numpy::adapt<T>(init_list)) {}

    // Properties
    int ndim() const override { return data.ndim(); }
    int size() const override { return data.size(); }
    std::string dtype() const override { return data.dtype(); }
    std::vector<int> shape() const override { return data.shape(); }

    // Boolean Functions
    bool all() const override { return data.all(); }
    bool any() const override { return data.any(); }

    // Aggregation Functions
    py::object sum() const override {
        if constexpr (std::is_same_v<T, int>) {
            return py::int_(data.sum());
        } else if constexpr (std::is_same_v<T, float64>) {
            return py::float_(data.sum());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* sum_axis(int axis) const override { return new ndarray<T>(data.sum(axis)); }
    ndarray_base* sum_axes(py::tuple axes) const override {
        std::vector<int> axes_;
        for (auto item : axes) axes_.push_back(py::cast<int>(item));
        return new ndarray<T>(data.sum(axes_));
    }
    py::object prod() const override {
        if constexpr (std::is_same_v<T, int>) {
            return py::int_(data.prod());
        } else if constexpr (std::is_same_v<T, float64>) {
            return py::float_(data.prod());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* prod_axis(int axis) const override { return new ndarray<T>(data.prod(axis)); }
    ndarray_base* prod_axes(py::tuple axes) const override {
        std::vector<int> axes_;
        for (auto item : axes) axes_.push_back(py::cast<int>(item));
        return new ndarray<T>(data.prod(axes_));
    }
    py::object min() const override {
        if constexpr (std::is_same_v<T, int>) {
            return py::int_(data.min());
        } else if constexpr (std::is_same_v<T, float64>) {
            return py::float_(data.min());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* min_axis(int axis) const override { return new ndarray<T>(data.min(axis)); }
    ndarray_base* min_axes(py::tuple axes) const override {
        std::vector<int> axes_;
        for (auto item : axes) axes_.push_back(py::cast<int>(item));
        return new ndarray<T>(data.min(axes_));
    }
    py::object max() const override {
        if constexpr (std::is_same_v<T, int>) {
            return py::int_(data.max());
        } else if constexpr (std::is_same_v<T, float64>) {
            return py::float_(data.max());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* max_axis(int axis) const override { return new ndarray<T>(data.max(axis)); }
    ndarray_base* max_axes(py::tuple axes) const override {
        std::vector<int> axes_;
        for (auto item : axes) axes_.push_back(py::cast<int>(item));
        return new ndarray<T>(data.max(axes_));
    }
    py::object mean() const override {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float64>) {
            return py::float_(data.mean());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* mean_axis(int axis) const override { return new ndarray<T>(data.mean(axis)); }
    ndarray_base* mean_axes(py::tuple axes) const override {
        std::vector<int> axes_;
        for (auto item : axes) axes_.push_back(py::cast<int>(item));
        return new ndarray<T>(data.mean(axes_));
    }
    py::object std() const override {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float64>) {
            return py::float_(data.std());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* std_axis(int axis) const override { return new ndarray<T>(data.std(axis)); }
    ndarray_base* std_axes(py::tuple axes) const override {
        std::vector<int> axes_;
        for (auto item : axes) axes_.push_back(py::cast<int>(item));
        return new ndarray<T>(data.std(axes_));
    }
    py::object var() const override {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float64>) {
            return py::float_(data.var());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* var_axis(int axis) const override { return new ndarray<T>(data.var(axis)); }
    ndarray_base* var_axes(py::tuple axes) const override {
        std::vector<int> axes_;
        for (auto item : axes) axes_.push_back(py::cast<int>(item));
        return new ndarray<T>(data.var(axes_));
    }

    py::object argmin() const override {
        if constexpr (std::is_same_v<T, int>) {
            return py::int_(data.argmin());
        } else if constexpr (std::is_same_v<T, float64>) {
            return py::float_(data.argmin());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* argmin_axis(int axis) const override { return new ndarray<T>(data.argmin(axis)); }
    py::object argmax() const override {
        if constexpr (std::is_same_v<T, int>) {
            return py::int_(data.argmax());
        } else if constexpr (std::is_same_v<T, float64>) {
            return py::float_(data.argmax());
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }
    ndarray_base* argmax_axis(int axis) const override { return new ndarray<T>(data.argmax(axis)); }
    ndarray_base* argsort() const override { return new ndarray<T>(data.argsort()); }
    ndarray_base* argsort_axis(int axis) const override { return new ndarray<T>(data.argsort(axis)); }
    void sort() override { data = data.sort(); }
    void sort_axis(int axis) override { data = data.sort(axis); }
    ndarray_base* reshape(const std::vector<int>& shape) const override { return new ndarray<T>(data.reshape(shape)); }
    ndarray_base* squeeze() const override { return new ndarray<T>(data.squeeze()); }
    ndarray_base* squeeze_axis(int axis) const override { return new ndarray<T>(data.squeeze(axis)); }
    ndarray_base* transpose() const override { return new ndarray<T>(data.transpose()); }
    ndarray_base* transpose_tuple(py::tuple permutations) const override {  
        std::vector<int> perm;
        for (auto item : permutations) perm.push_back(py::cast<int>(item));
        return new ndarray<T>(data.transpose(perm));
    }
    ndarray_base* transpose_args(py::args args) const override {
        std::vector<int> perm;
        for (auto item : args) perm.push_back(py::cast<int>(item));
        return new ndarray<T>(data.transpose(perm));
    }
    ndarray_base* repeat(int repeats, int axis) const override { return new ndarray<T>(data.repeat(repeats, axis)); }
    ndarray_base* repeat_axis(const std::vector<size_t>& repeats, int axis) const override { return new ndarray<T>(data.repeat(repeats, axis)); }
    ndarray_base* flatten() const override { return new ndarray<T>(data.flatten()); }
    ndarray_base* copy() const override { return new ndarray<T>(data.copy()); }
    ndarray_base* astype(const std::string& dtype) const override {
        if (dtype == "int") {
            return new ndarray<int>(data.template astype<int>());
        } else if (dtype == "float64") {
            return new ndarray<float64>(data.template astype<float64>());
        } else {
            throw std::invalid_argument("Invalid dtype");
        }
    }

    // Dunder Methods
    bool eq(const ndarray_base& other) const override {
        if constexpr(std::is_same_v<T, int>) {
            if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* int == float */
                return data == p->data;
            } else if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* int == int */
                return data == p->data;
            }
        } else if constexpr(std::is_same_v<T, float64>) {
            if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* float == int */
                return data == p->data;
            } else if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* float == float */
                return data == p->data;
            }
        }

        const ndarray<T>& other_ = dynamic_cast<const ndarray<T>&>(other);
        return data == other_.data;
    }
    ndarray_base* add(const ndarray_base& other) const override {
        if constexpr(std::is_same_v<T, int>) {
            if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* int + float */
                return new ndarray<float64>(data + p->data);
            } else if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* int + int */
                return new ndarray<int>(data + p->data);
            }
        } else if constexpr(std::is_same_v<T, float64>) {
            if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* float + int */
                return new ndarray<float64>(data + p->data);
            } else if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* float + float */
                return new ndarray<float64>(data + p->data);
            }
        }  

        const ndarray<T>& other_ = dynamic_cast<const ndarray<T>&>(other);
        return new ndarray<T>(data + other_.data);
    }
    ndarray_base* add_int(int other) const override {
        return new ndarray<T>(data + other);
    }
    ndarray_base* add_float(float64 other) const override {
        return new ndarray<float64>(data + other);
    }
    ndarray_base* sub(const ndarray_base& other) const override {
        if constexpr(std::is_same_v<T, int>) {
            if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* int - float */
                return new ndarray<float64>(data - p->data);
            } else if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* int - int */
                return new ndarray<int>(data - p->data);
            }
        } else if constexpr(std::is_same_v<T, float64>) {
            if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* float - int */
                return new ndarray<float64>(data - p->data);
            } else if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* float - float */
                return new ndarray<float64>(data - p->data);
            }
        }  

        const ndarray<T>& other_ = dynamic_cast<const ndarray<T>&>(other);
        return new ndarray<T>(data - other_.data);
    }
    ndarray_base* sub_int(int other) const override {
        return new ndarray<T>(data - other);
    }
    ndarray_base* sub_float(float64 other) const override {
        return new ndarray<float64>(data - other);
    }
    ndarray_base* rsub_int(int other) const override {
        return new ndarray<T>(other - data);
    }
    ndarray_base* rsub_float(float64 other) const override {
        return new ndarray<float64>(other - data);
    }
    ndarray_base* mul(const ndarray_base& other) const override {
        if constexpr(std::is_same_v<T, int>) {
            if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* int * float */
                return new ndarray<float64>(data * p->data);
            } else if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* int * int */
                return new ndarray<int>(data * p->data);
            }
        } else if constexpr(std::is_same_v<T, float64>) {
            if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* float * int */
                return new ndarray<float64>(data * p->data);
            } else if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* float * float */
                return new ndarray<float64>(data * p->data);
            }
        }  

        const ndarray<T>& other_ = dynamic_cast<const ndarray<T>&>(other);
        return new ndarray<T>(data * other_.data);
    }
    ndarray_base* mul_int(int other) const override {
        return new ndarray<T>(data * other);
    }
    ndarray_base* mul_float(float64 other) const override {
        return new ndarray<float64>(data * other);
    }
    ndarray_base* div(const ndarray_base& other) const override {
        if constexpr(std::is_same_v<T, int>) {
            if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* int / float */
                return new ndarray<float64>(data / p->data);
            } else if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* int / int */
                return new ndarray<int>(data / p->data);
            }
        } else if constexpr(std::is_same_v<T, float64>) {
            if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* float / int */
                return new ndarray<float64>(data / p->data);
            } else if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* float / float */
                return new ndarray<float64>(data / p->data);
            }
        }  

        const ndarray<T>& other_ = dynamic_cast<const ndarray<T>&>(other);
        return new ndarray<T>(data / other_.data);
    }
    ndarray_base* div_int(int other) const override {
        return new ndarray<float64>(data / other);
    }
    ndarray_base* div_float(float64 other) const override {
        return new ndarray<float64>(data / other);
    }
    ndarray_base* rdiv_int(int other) const override {
        return new ndarray<float64>(other / data);
    }
    ndarray_base* rdiv_float(float64 other) const override {
        return new ndarray<float64>(other / data);
    }
    ndarray_base* pow(const ndarray_base& other) const override {
        if constexpr(std::is_same_v<T, int>) {
            if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* int ** float */
                return new ndarray<float64>(data.pow(p->data));
            } else if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* int ** int */
                return new ndarray<int>(data.pow(p->data));
            }
        } else if constexpr(std::is_same_v<T, float64>) {
            if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* float ** int */
                return new ndarray<float64>(data.pow(p->data));
            } else if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* float ** float */
                return new ndarray<float64>(data.pow(p->data));
            }
        }  

        const ndarray<T>& other_ = dynamic_cast<const ndarray<T>&>(other);
        return new ndarray<T>(data.pow(other_.data));
    }
    ndarray_base* pow_int(int other) const override {
        return new ndarray<float64>(data.pow(other));
    }
    ndarray_base* pow_float(float64 other) const override {
        return new ndarray<float64>(data.pow(other));
    }
    ndarray_base* rpow_int(int other) const override {
        return new ndarray<float64>(pkpy::numpy::pow(other, data));
    }
    ndarray_base* rpow_float(float64 other) const override {
        return new ndarray<float64>(pkpy::numpy::pow(other, data));
    }
    ndarray_base* matmul(const ndarray_base& other) const override {
        if constexpr(std::is_same_v<T, int>){
            if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* int @ float */
                return new ndarray<float64>(data.matmul(p->data));
            } else if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* int @ int */
                return new ndarray<int>(data.matmul(p->data));
            }
        } else if constexpr(std::is_same_v<T, float64>){
            if(auto p = dynamic_cast<const ndarray<int>*>(&other)){ /* float @ int */
                return new ndarray<float64>(data.matmul(p->data));
            } else if(auto p = dynamic_cast<const ndarray<float64>*>(&other)){ /* float @ float */
                return new ndarray<float64>(data.matmul(p->data));
            }
        }

        const ndarray<T>& other_ = dynamic_cast<const ndarray<T>&>(other);
        return new ndarray<T>(data.matmul(other_.data));
    }
    int len() const override { return data.shape()[0]; }
    py::object get_item_int(int index) const override {
        if (data.ndim() == 1) {
            if constexpr(std::is_same_v<T, int>) {
                return py::int_(data(index));
            } else if constexpr(std::is_same_v<T, float64>) {
                return py::float_(data(index));
            }
        } else {
            return py::cast(ndarray<T>(data[index]));
        }
    }
    py::object get_item_tuple(py::tuple args) const override {
        pkpy::numpy::ndarray<T> store = data;
        std::vector<int> indices;
        for(auto item: args) {
            indices.push_back(py::cast<int>(item));
        }
        for(int i = 0; i < indices.size() - 1; i++) {
            pkpy::numpy::ndarray<T> temp = store[indices[i]];
            store = temp;
        }

        if(store.ndim() == 1) {
            if constexpr(std::is_same_v<T, int>) {
                return py::int_(store(indices[indices.size() - 1]));
            } else if constexpr(std::is_same_v<T, float64>) {
                return py::float_(store(indices[indices.size() - 1]));
            }
        } else {
            return py::cast(ndarray<T>(store[indices[indices.size() - 1]]));
        }
    }
    ndarray_base* get_item_vector(const std::vector<int>& indices) const override { return new ndarray<T>(data[indices]); }
    ndarray_base* get_item_slice(py::slice slice) const override {
        int start = parseAttr(getattr(slice, "start"));
        int stop = parseAttr(getattr(slice, "stop"));
        int step = parseAttr(getattr(slice, "step"));

        if(step == INT_MAX) step = 1;
        if(step >0) {
            if(start == INT_MAX) start = 0;
            if(stop == INT_MAX) stop = data.shape()[0];
        } else if(step < 0) {
            if(start == INT_MAX) start = data.shape()[0] - 1;
            if(stop == INT_MAX) stop = -(1 + data.shape()[0]);
        }

        return new ndarray<T>(data[std::make_tuple(start, stop, step)]);
    }
    void set_item_int(int index, int value) override {
        if constexpr(std::is_same_v<T, int>) {
            data.set_item(index, pkpy::numpy::adapt<int>(std::vector{value}));
        } else if constexpr(std::is_same_v<T, float64>) {
            data.set_item(index, (pkpy::numpy::adapt<int>(std::vector{value})).astype<float64>());
        }
    }
    void set_item_index_int(int index, const std::vector<int>& value) override {
        if constexpr(std::is_same_v<T, int>) {
            data.set_item(index, pkpy::numpy::adapt<int>(value));
        } else if constexpr(std::is_same_v<T, float64>) {
            data.set_item(index, (pkpy::numpy::adapt<int>(value)).astype<float64>());
        }
    }
    void set_item_float(int index, float64 value) override {
        if constexpr(std::is_same_v<T, float64>) {
            data.set_item(index, pkpy::numpy::adapt<float64>(std::vector{value}));
        } else if constexpr(std::is_same_v<T, int>) {
            data.set_item(index, (pkpy::numpy::adapt<float64>(std::vector{value})).astype<int>());
        }
    }
    void set_item_index_float(int index, const std::vector<float64>& value) override {
        if constexpr(std::is_same_v<T, float64>) {
            data.set_item(index, pkpy::numpy::adapt<float64>(value));
        } else if constexpr(std::is_same_v<T, int>) {
            data.set_item(index, (pkpy::numpy::adapt<float64>(value)).astype<int>());
        }
    }
    void set_item_tuple_int(py::tuple args, int value) override {
        std::vector<int> indices;
        for (auto item : args) {
            indices.push_back(py::cast<int>(item));
        }
        if(indices.size() == 1) {
            int index = indices[0];
            if constexpr(std::is_same_v<T, int>) {
                data.set_item(index, pkpy::numpy::adapt<int>(std::vector{value}));
            } else if constexpr(std::is_same_v<T, float64>) {
                data.set_item(index, (pkpy::numpy::adapt<int>(std::vector{value})).astype<float64>());
            }
        }
        else if(indices.size() == 2 && indices.size() <= data.ndim()) data.set_item_2d(indices[0], indices[1], value);
        else if(indices.size() == 3 && indices.size() <= data.ndim()) data.set_item_3d(indices[0], indices[1], indices[2], value);
        else if(indices.size() == 4 && indices.size() <= data.ndim()) data.set_item_4d(indices[0], indices[1], indices[2], indices[3], value);
        else if(indices.size() == 5 && indices.size() <= data.ndim()) data.set_item_5d(indices[0], indices[1], indices[2], indices[3], indices[4], value);
    }
    void set_item_tuple_float(py::tuple args, float64 value) override {
        std::vector<int> indices;
        for (auto item : args) {
            indices.push_back(py::cast<int>(item));
        }
        if(indices.size() == 1) {
            int index = indices[0];
            if constexpr(std::is_same_v<T, float64>) {
                data.set_item(index, pkpy::numpy::adapt<float64>(std::vector{value}));
            } else if constexpr(std::is_same_v<T, int>) {
                data.set_item(index, (pkpy::numpy::adapt<float64>(std::vector{value})).astype<int>());
            }
        }
        else if(indices.size() == 2 && indices.size() <= data.ndim()) data.set_item_2d(indices[0], indices[1], value);
        else if(indices.size() == 3 && indices.size() <= data.ndim()) data.set_item_3d(indices[0], indices[1], indices[2], value);
        else if(indices.size() == 4 && indices.size() <= data.ndim()) data.set_item_4d(indices[0], indices[1], indices[2], indices[3], value);
        else if(indices.size() == 5 && indices.size() <= data.ndim()) data.set_item_5d(indices[0], indices[1], indices[2], indices[3], indices[4], value);
    }
    void set_item_vector_int1(const std::vector<int>& indices, int value) override {
        if constexpr(std::is_same_v<T, int>) {
            data.set_item(indices, pkpy::numpy::adapt<int>(std::vector{value}));
        } else if constexpr(std::is_same_v<T, float64>) {
            data.set_item(indices, (pkpy::numpy::adapt<int>(std::vector{value})).astype<float64>());
        }
    }
    void set_item_vector_int2(const std::vector<int>& indices, const std::vector<int>& value) override {
        if constexpr(std::is_same_v<T, int>) {
            data.set_item(indices, pkpy::numpy::adapt<int>(value));
        } else if constexpr(std::is_same_v<T, float64>) {
            data.set_item(indices, (pkpy::numpy::adapt<int>(value)).astype<float64>());
        }
    }
    void set_item_vector_float1(const std::vector<int>& indices, float64 value) override {
        if constexpr(std::is_same_v<T, float64>) {
            data.set_item(indices, pkpy::numpy::adapt<float64>(std::vector{value}));
        } else if constexpr(std::is_same_v<T, int>) {
            data.set_item(indices, (pkpy::numpy::adapt<float64>(std::vector{value})).astype<int>());
        }
    }
    void set_item_vector_float2(const std::vector<int>& indices, const std::vector<float64>& value) override {
        if constexpr(std::is_same_v<T, float64>) {
            data.set_item(indices, pkpy::numpy::adapt<float64>(value));
        } else if constexpr(std::is_same_v<T, int>) {
            data.set_item(indices, (pkpy::numpy::adapt<float64>(value)).astype<int>());
        }
    }
    void set_item_slice_int1(py::slice slice, int value) override {
        int start = parseAttr(getattr(slice, "start"));
        int stop = parseAttr(getattr(slice, "stop"));
        int step = parseAttr(getattr(slice, "step"));

        if(step == INT_MAX) step = 1;
        if(step >0) {
            if(start == INT_MAX) start = 0;
            if(stop == INT_MAX) stop = data.shape()[0];
        } else if(step < 0) {
            if(start == INT_MAX) start = data.shape()[0] - 1;
            if(stop == INT_MAX) stop = -(1 + data.shape()[0]);
        }
        if constexpr(std::is_same_v<T, int>) {
            data.set_item(std::make_tuple(start, stop, step), pkpy::numpy::adapt<int>(std::vector{value}));
        } else if constexpr(std::is_same_v<T, float64>) {
            data.set_item(std::make_tuple(start, stop, step), (pkpy::numpy::adapt<int>(std::vector{value})).astype<float64>());
        }
    }
    void set_item_slice_int2(py::slice slice, const std::vector<int>& value) override {
        int start = parseAttr(getattr(slice, "start"));
        int stop = parseAttr(getattr(slice, "stop"));
        int step = parseAttr(getattr(slice, "step"));

        if(step == INT_MAX) step = 1;
        if(step >0) {
            if(start == INT_MAX) start = 0;
            if(stop == INT_MAX) stop = data.shape()[0];
        } else if(step < 0) {
            if(start == INT_MAX) start = data.shape()[0] - 1;
            if(stop == INT_MAX) stop = -(1 + data.shape()[0]);
        }
        if constexpr(std::is_same_v<T, int>) {
            data.set_item(std::make_tuple(start, stop, step), pkpy::numpy::adapt<int>(value));
        } else if constexpr(std::is_same_v<T, float64>) {
            data.set_item(std::make_tuple(start, stop, step), (pkpy::numpy::adapt<int>(value)).astype<float64>());
        }
    }
    void set_item_slice_float1(py::slice slice, float64 value) override {
        int start = parseAttr(getattr(slice, "start"));
        int stop = parseAttr(getattr(slice, "stop"));
        int step = parseAttr(getattr(slice, "step"));

        if(step == INT_MAX) step = 1;
        if(step >0) {
            if(start == INT_MAX) start = 0;
            if(stop == INT_MAX) stop = data.shape()[0];
        } else if(step < 0) {
            if(start == INT_MAX) start = data.shape()[0] - 1;
            if(stop == INT_MAX) stop = -(1 + data.shape()[0]);
        }
        if constexpr(std::is_same_v<T, float64>) {
            data.set_item(std::make_tuple(start, stop, step), pkpy::numpy::adapt<float64>(std::vector{value}));
        } else if constexpr(std::is_same_v<T, int>) {
            data.set_item(std::make_tuple(start, stop, step), (pkpy::numpy::adapt<float64>(std::vector{value})).astype<int>());
        }
    }
    void set_item_slice_float2(py::slice slice, const std::vector<float64>& value) override {
        int start = parseAttr(getattr(slice, "start"));
        int stop = parseAttr(getattr(slice, "stop"));
        int step = parseAttr(getattr(slice, "step"));

        if(step == INT_MAX) step = 1;
        if(step >0) {
            if(start == INT_MAX) start = 0;
            if(stop == INT_MAX) stop = data.shape()[0];
        } else if(step < 0) {
            if(start == INT_MAX) start = data.shape()[0] - 1;
            if(stop == INT_MAX) stop = -(1 + data.shape()[0]);
        }
        if constexpr(std::is_same_v<T, float64>) {
            data.set_item(std::make_tuple(start, stop, step), pkpy::numpy::adapt<float64>(value));
        } else if constexpr(std::is_same_v<T, int>) {
            data.set_item(std::make_tuple(start, stop, step), (pkpy::numpy::adapt<float64>(value)).astype<int>());
        }
    }
    std::string to_string() const override {
        std::ostringstream os;
        os << data;
        return os.str();
    }
};

class Random {
public:
    static py::object rand() {
        return py::float_(pkpy::numpy::random::rand<float64>());
    }
    static ndarray_base* rand_shape(py::args args) {
        std::vector<int> shape;
        for (auto item : args) shape.push_back(py::cast<int>(item));
        return new ndarray<float64>(pkpy::numpy::random::rand<float64>(shape));
    }

    static py::object randn() {
        return py::float_(pkpy::numpy::random::randn<float64>());
    }
    static ndarray_base* randn_shape(py::args args) {
        std::vector<int> shape;
        for (auto item : args) shape.push_back(py::cast<int>(item));
        return new ndarray<float64>(pkpy::numpy::random::randn<float64>(shape));
    }

    static py::object randint(int low, int high) {
        return py::int_(pkpy::numpy::random::randint<int>(low, high));
    }
    static ndarray_base* randint_shape(int low, int high, const std::vector<int>& shape) {
        return new ndarray<int>(pkpy::numpy::random::randint<int>(low, high, shape));
    }

};

using ndarray_int = ndarray<int>;
using ndarray_float = ndarray<float64>;

PYBIND11_MODULE(numpy_bindings, m) {
    m.doc() = "Python bindings for pkpy::numpy::ndarray using pybind11";

    py::class_<ndarray_base>(m, "ndarray")
        .def("ndim", &ndarray_base::ndim)
        .def("size", &ndarray_base::size)
        .def("dtype", &ndarray_base::dtype)
        .def("shape", &ndarray_base::shape)
        .def("all", &ndarray_base::all)
        .def("any", &ndarray_base::any)
        .def("sum", &ndarray_base::sum)
        .def("sum", &ndarray_base::sum_axis)
        .def("sum", &ndarray_base::sum_axes)
        .def("prod", &ndarray_base::prod)
        .def("prod", &ndarray_base::prod_axis)
        .def("prod", &ndarray_base::prod_axes)
        .def("min", &ndarray_base::min)
        .def("min", &ndarray_base::min_axis)
        .def("min", &ndarray_base::min_axes)
        .def("max", &ndarray_base::max)
        .def("max", &ndarray_base::max_axis)
        .def("max", &ndarray_base::max_axes)
        .def("mean", &ndarray_base::mean)
        .def("mean", &ndarray_base::mean_axis)
        .def("mean", &ndarray_base::mean_axes)
        .def("std", &ndarray_base::std)
        .def("std", &ndarray_base::std_axis)
        .def("std", &ndarray_base::std_axes)
        .def("var", &ndarray_base::var)
        .def("var", &ndarray_base::var_axis)
        .def("var", &ndarray_base::var_axes)
        .def("argmin", &ndarray_base::argmin)
        .def("argmin", &ndarray_base::argmin_axis)
        .def("argmax", &ndarray_base::argmax)
        .def("argmax", &ndarray_base::argmax_axis)
        .def("argsort", &ndarray_base::argsort)
        .def("argsort", &ndarray_base::argsort_axis)
        .def("sort", &ndarray_base::sort)
        .def("sort", &ndarray_base::sort_axis)
        .def("reshape", &ndarray_base::reshape )
        .def("squeeze", &ndarray_base::squeeze)
        .def("squeeze", &ndarray_base::squeeze_axis)
        .def("transpose", &ndarray_base::transpose)
        .def("transpose", &ndarray_base::transpose_tuple)
        .def("transpose", &ndarray_base::transpose_args)
        .def("repeat", &ndarray_base::repeat)
        .def("repeat", &ndarray_base::repeat_axis)
        .def("flatten", &ndarray_base::flatten)
        .def("copy", &ndarray_base::copy)
        .def("astype", &ndarray_base::astype)
        .def("__eq__", &ndarray_base::eq)
        .def("__add__", &ndarray_base::add)
        .def("__add__", &ndarray_base::add_int)
        .def("__add__", &ndarray_base::add_float)
        .def("__radd__", &ndarray_base::add_int)
        .def("__radd__", &ndarray_base::add_float)
        .def("__sub__", &ndarray_base::sub)
        .def("__sub__", &ndarray_base::sub_int)
        .def("__sub__", &ndarray_base::sub_float)
        .def("__rsub__", &ndarray_base::rsub_int)
        .def("__rsub__", &ndarray_base::rsub_float)
        .def("__mul__", &ndarray_base::mul)
        .def("__mul__", &ndarray_base::mul_int)
        .def("__mul__", &ndarray_base::mul_float)
        .def("__rmul__", &ndarray_base::mul_int)
        .def("__rmul__", &ndarray_base::mul_float)
        .def("__truediv__", &ndarray_base::div)
        .def("__truediv__", &ndarray_base::div_int)
        .def("__truediv__", &ndarray_base::div_float)
        .def("__rtruediv__", &ndarray_base::rdiv_int)
        .def("__rtruediv__", &ndarray_base::rdiv_float)
        .def("__pow__", &ndarray_base::pow)
        .def("__pow__", &ndarray_base::pow_int)
        .def("__pow__", &ndarray_base::pow_float)
        .def("__rpow__", &ndarray_base::rpow_int)
        .def("__rpow__", &ndarray_base::rpow_float)
        .def("__matmul__", &ndarray_base::matmul)
        .def("__len__", &ndarray_base::len)
        .def("__getitem__", &ndarray_base::get_item_int)
        .def("__getitem__", &ndarray_base::get_item_tuple)
        .def("__getitem__", &ndarray_base::get_item_vector)
        .def("__getitem__", &ndarray_base::get_item_slice)
        .def("__setitem__", &ndarray_base::set_item_int)
        .def("__setitem__", &ndarray_base::set_item_index_int)
        .def("__setitem__", &ndarray_base::set_item_float)
        .def("__setitem__", &ndarray_base::set_item_index_float)
        .def("__setitem__", &ndarray_base::set_item_tuple_int)
        .def("__setitem__", &ndarray_base::set_item_tuple_float)
        .def("__setitem__", &ndarray_base::set_item_vector_int1)
        .def("__setitem__", &ndarray_base::set_item_vector_int2)
        .def("__setitem__", &ndarray_base::set_item_vector_float1)
        .def("__setitem__", &ndarray_base::set_item_vector_float2)
        .def("__setitem__", &ndarray_base::set_item_slice_int1)
        .def("__setitem__", &ndarray_base::set_item_slice_int2)
        .def("__setitem__", &ndarray_base::set_item_slice_float1)
        .def("__setitem__", &ndarray_base::set_item_slice_float2)

        .def("__str__", [](const ndarray_base& self) {
            std::ostringstream os;
            os << self.to_string();
            return os.str();
        })
        .def("__repr__", [](const ndarray_base& self) {
            std::ostringstream os;
            os << "ndarray(" << std::endl << self.to_string() << std::endl << ")";
            return os.str();
        });

    py::class_<ndarray<int>, ndarray_base>(m, "ndarray_int")
        .def(py::init<>())
        .def(py::init<int>())
        .def(py::init<const std::vector<int>&>())
        .def(py::init<const std::vector<std::vector<int>>&>())
        .def(py::init<const std::vector<std::vector<std::vector<int>>>&>())
        .def(py::init<const std::vector<std::vector<std::vector<std::vector<int>>>>&>())
        .def(py::init<const std::vector<std::vector<std::vector<std::vector<std::vector<int>>>>>&>())
        .def("__and__", [](ndarray<int>&v, ndarray<int>&w){
            return ndarray<int>(v.data & w.data);
        })
        .def("__and__", [](ndarray<int>&v, int w){
            return ndarray<int>(v.data & w);
        })
        .def("__rand__", [](ndarray<int>&v, int w){
            return ndarray<int>(w & v.data);
        })
        .def("__or__", [](ndarray<int>&v, ndarray<int>&w){
            return ndarray<int>(v.data | w.data);
        })
        .def("__or__", [](ndarray<int>&v, int w){
            return ndarray<int>(v.data | w);
        })
        .def("__ror__", [](ndarray<int>&v, int w){
            return ndarray<int>(w | v.data);
        })
        .def("__xor__", [](ndarray<int>&v, ndarray<int>&w){
            return ndarray<int>(v.data ^ w.data);
        })
        .def("__xor__", [](ndarray<int>&v, int w){
            return ndarray<int>(v.data ^ w);
        })
        .def("__rxor__", [](ndarray<int>&v, int w){
            return ndarray<int>(w ^ v.data);
        })
        .def("__invert__", [](ndarray<int>&v){
            return ndarray<int>(~v.data);
        });

    py::class_<ndarray<float64>, ndarray_base>(m, "ndarray_float")
        .def(py::init<>())
        .def(py::init<float64>())
        .def(py::init<const std::vector<float64>&>())
        .def(py::init<const std::vector<std::vector<float64>>&>())
        .def(py::init<const std::vector<std::vector<std::vector<float64>>>&>())
        .def(py::init<const std::vector<std::vector<std::vector<std::vector<float64>>>>&>())
        .def(py::init<const std::vector<std::vector<std::vector<std::vector<std::vector<float64>>>>>&>());

    py::class_<Random>(m, "random")
        .def_static("rand", &Random::rand)
        .def_static("rand_shape", &Random::rand_shape)
        .def_static("randn", &Random::randn)
        .def_static("randn_shape", &Random::randn_shape)
        .def_static("randint", &Random::randint)
        .def_static("randint_shape", &Random::randint_shape);

    m.def("array", [](int value) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(value));
    });
    m.def("array", [](const std::vector<int>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(values));
    });
    m.def("array", [](const std::vector<std::vector<int>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(values));
    });
    m.def("array", [](const std::vector<std::vector<std::vector<int>>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(values));
    });
    m.def("array", [](const std::vector<std::vector<std::vector<std::vector<int>>>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(values));
    });
    m.def("array", [](const std::vector<std::vector<std::vector<std::vector<std::vector<int>>>>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(values));
    });

    m.def("array", [](float64 value) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(value));
    });
    m.def("array", [](const std::vector<float64>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(values));
    });
    m.def("array", [](const std::vector<std::vector<float64>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(values));
    });
    m.def("array", [](const std::vector<std::vector<std::vector<float64>>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(values));
    });
    m.def("array", [](const std::vector<std::vector<std::vector<std::vector<float64>>>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(values));
    });
    m.def("array", [](const std::vector<std::vector<std::vector<std::vector<std::vector<float64>>>>>& values) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(values));
    });

    // Array Creation Functions
    m.def("ones", [](const std::vector<int>& shape) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::ones<float64>(shape)));
    });
    m.def("zeros", [](const std::vector<int>& shape) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::zeros<float64>(shape)));
    });
    m.def("full", [](const std::vector<int>& shape, int value) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(pkpy::numpy::full<int>(shape, value)));
    });
    m.def("full", [](const std::vector<int>& shape, float64 value) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::full<float64>(shape, value)));
    });
    m.def("identity", [](int n) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::identity<float64>(n)));
    });
    m.def("arange", [](int stop) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(pkpy::numpy::arange<int>(0, stop)));
    });
    m.def("arange", [](int start, int stop) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(pkpy::numpy::arange<int>(start, stop)));
    });
    m.def("arange", [](int start, int stop, int step) {
        return std::unique_ptr<ndarray_base>(new ndarray_int(pkpy::numpy::arange<int>(start, stop, step)));
    });
    m.def("arange", [](float64 stop) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::arange<float64>(0, stop)));
    });
    m.def("arange", [](float64 start, float64 stop) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::arange<float64>(start, stop)));
    });
    m.def("arange", [](float64 start, float64 stop, float64 step) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::arange<float64>(start, stop, step)));
    });
    m.def("linspace", [](float64 start, float64 stop, int num, bool endpoint) {
        return std::unique_ptr<ndarray_base>(new ndarray_float(pkpy::numpy::linspace(start, stop, num, endpoint)));
    }, py::arg("start"), py::arg("stop"), py::arg("num") = 50, py::arg("endpoint") = true);

    // Trigonometric Functions
    m.def("sin", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::sin(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::sin(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("cos", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::cos(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::cos(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("tan", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::tan(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::tan(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("arcsin", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::arcsin(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::arcsin(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("arccos", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::arccos(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::arccos(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("arctan", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::arctan(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::arctan(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });

    // Exponential and Logarithmic Functions
    m.def("exp", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::exp(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::exp(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("log", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::log(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::log(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("log2", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::log2(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::log2(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("log10", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::log10(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::log10(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });

    // Miscellaneous Functions
    m.def("round", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<int>(pkpy::numpy::round(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::round(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("floor", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<int>(pkpy::numpy::floor(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::floor(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("ceil", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<int>(pkpy::numpy::ceil(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::ceil(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("abs", [](const ndarray_base& arr) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<int>(pkpy::numpy::abs(p->data)));
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr)){
            return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::abs(p->data)));
        }
        throw std::invalid_argument("Invalid dtype");
    });
    m.def("concatenate", [](const ndarray_base& arr1, const ndarray_base& arr2, int axis) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr1)){
            if(auto q = dynamic_cast<const ndarray<int>*>(&arr2)){
                return std::unique_ptr<ndarray_base>(new ndarray<int>(pkpy::numpy::concatenate(p->data, q->data, axis)));
            } else if(auto q = dynamic_cast<const ndarray<float64>*>(&arr2)){
                return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::concatenate(p->data, q->data, axis)));
            }
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr1)){
            if(auto q = dynamic_cast<const ndarray<int>*>(&arr2)){
                return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::concatenate(p->data, q->data, axis)));
            } else if(auto q = dynamic_cast<const ndarray<float64>*>(&arr2)){
                return std::unique_ptr<ndarray_base>(new ndarray<float64>(pkpy::numpy::concatenate(p->data, q->data, axis)));
            }
        }
        throw std::invalid_argument("Invalid dtype");
    }, py::arg("arr1"), py::arg("arr2"), py::arg("axis") = 0);

    // Constants
    m.attr("pi") = pkpy::numpy::pi;
    m.attr("inf") = pkpy::numpy::inf;

    // Testing Functions
    m.def("allclose", [](const ndarray_base& arr1, const ndarray_base& arr2, float64 rtol, float64 atol) {
        if(auto p = dynamic_cast<const ndarray<int>*>(&arr1)){
            if(auto q = dynamic_cast<const ndarray<int>*>(&arr2)){
                return pkpy::numpy::allclose(p->data, q->data, rtol, atol);
            } else if(auto q = dynamic_cast<const ndarray<float64>*>(&arr2)){
                return pkpy::numpy::allclose(p->data, q->data, rtol, atol);
            }
        } else if(auto p = dynamic_cast<const ndarray<float64>*>(&arr1)){
            if(auto q = dynamic_cast<const ndarray<int>*>(&arr2)){
                return pkpy::numpy::allclose(p->data, q->data, rtol, atol);
            } else if(auto q = dynamic_cast<const ndarray<float64>*>(&arr2)){
                return pkpy::numpy::allclose(p->data, q->data, rtol, atol);
            }
        }
        throw std::invalid_argument("Invalid dtype");
    }, py::arg("arr1"), py::arg("arr2"), py::arg("rtol") = 1e-5, py::arg("atol") = 1e-8);
}
