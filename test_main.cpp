#include <internal/class.h>

namespace py = pybind11;
using namespace pybind11;

struct Point
{
    int x, y;

    void print(int z) { std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl; }
};

int main()
{
    py::init();
    py::extras_info<2> info;

    try
    {
        auto __main__ = py::module_::import("__main__");

        class_<Point> point(__main__, "Point");

        bind(point.ptr(), "print", &Point::print, info);
        // vm->bind_property()
        py::object p = py::cast(Point{1, 3});
        p.attr("print")(1);
    }
    catch(pkpy::Exception& e)
    {
        std::cerr << e.msg << std::endl;
    }

    return 0;
}
