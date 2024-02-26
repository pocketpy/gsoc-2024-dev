#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// glm is a header only, fast and useful math library for C++: https://github.com/g-truc/glm
// It's used in a lot of graphics related projects, including Cave Engine. 
// Binding it to python is interesting because it envolves a lot of math 
// operators. I'm making it optional to this file with this macro, so you
// can test it even without glm. I'd recommend including it later on.
#define WITH_GLM_BINDINGS
#define GLM_ENABLE_EXPERIMENTAL

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace py = pybind11;
using namespace py::literals;

/*===========================================================================//
// 					 C++ CODE USED FOR THE PYBIND11 BINDINGS				 //
//===========================================================================*/


void ExampleFunction(){}
int SumIntegers(int x, int y) { return x + y; }
float ArgsWithDefaultValues(float x, float y=1.f) {return x * y;}

void OverloadingExample(int x)   { std::cout << " - OverloadingExample - int: " << x << "\n";   }
void OverloadingExample(float x) { std::cout << " - OverloadingExample - float: " << x << "\n"; }
void OverloadingExample(bool x)  { std::cout << " - OverloadingExample - bool: " << x << "\n";  }

enum ExampleEnum {
	FIRST_VALUE = 0,
	SECOND_VALUE,
	THIRD_VALUE
};

struct BasicStruct {
	BasicStruct(const std::string& _name) : name(_name) {}
	
	std::string name;
	
	void PrintName(){ std::cout << " - Printing basic struct name: " << name << "\n";}
};

BasicStruct* ReturnByReferenceTest(){
	static BasicStruct basicStruct("Owned by C++");
	return &basicStruct;
}


class BasicClass {
public:
	BasicClass(){}
	virtual ~BasicClass(){}
	
	virtual void DoSomethingPolymorphic() { std::cout << " - BasicClass is doing something polymorphic!"; }
	
	int width;
	int height;
};

class ChildClassA : public BasicClass {
public:
	ChildClassA(){}
	virtual ~ChildClassA(){}
};

class ChildClassB : public BasicClass {
public:
	ChildClassB(){}
	virtual ~ChildClassB(){}
	
	virtual void DoSomethingPolymorphic() override { std::cout << " - BasicClass is doing something polymorphic!"; }
};

class ComplicatedClass {
public:
	ComplicatedClass(){}
	virtual ~ComplicatedClass(){
		for (auto child : children){
			delete child;
		}
	}

	int GetValue() const { return value; }
	void SetValue(int v) { value = v; }
	
	ComplicatedClass* NewChild() { 
		children.push_back(new ComplicatedClass()); 
		return children.back();
	}	
		
	std::vector<ComplicatedClass*> children;
	std::unordered_map<std::string, int> tags;
	
	// Python dict inside the class:
	py::dict properties;
		
	// This method is about accessing some python stuff from C++
	void InspectProperties(){
		// This code snippet is inspired by a Cave method of our PythonScript class
		// called "SyncAllOverrides(const std::string& className, py::dict& overrides)",
		// that given a python class, inspects it and sync all its props into the overrides
		// dictionary. 
		// I'm not doing the same thing here, in this example method I'm just randomply 
		// inspecting the variables from the properties field, but I tried to use the same 
		// pybind11 methods I used in the mentioned cave method.
		
		std::cout << "[Inspecting ComplicatedClass Properties]\n";
		
		// Checking if a py::dict contains a key:
		if (properties.contains("test")) {
			std::cout << " - Oh, the properties contains a test key, nice!\n";
		}
		
		try {
			auto callable = py::eval("callable"); // I use this to see if a given python prop is callable
			
			py::list nonCallableElements;
			for (auto it : properties) { 
				if (!callable(it.second).cast<bool>()){
					nonCallableElements.append(it.first); 
				}
				else {
					std::cout << " - Key \"" << it.first.cast<std::string>() << "\" contains a callable element.\n";
					
					// Calling it to see what's going on:
					py::dict locals;
					locals["callableElement"] = it.second;
					// Using eval...
					auto res = py::eval("callableElement()", py::globals(), locals);
				}
			}
			
			for (auto key : nonCallableElements) {
				auto value = properties.attr("get")(key);
				
				if (value.is_none()) {
					// This was not supposed to happen since the key should be in the properties...
					std::cout << " - Somethind went wrong trying to get the key \"" << key.cast<std::string>() << "\"...\n";
				}
				else {
					std::string msg = "Element found: " + key.cast<std::string>();
					
					// Another way to build a pybind11 dict:
					auto locals = py::dict("msg"_a = py::cast(msg), "value"_a= value);
					py::exec("print(msg, \"=\", value)", py::globals(), locals);
				}
			}
		}
		catch (py::error_already_set e) {
			std::cerr << "Python error when printing Properties: " << e.what() << "\n";
		}
	}
protected:
	int value;
};

#ifdef WITH_GLM_BINDINGS
// Code snippets taken from Cave Engine

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace math {
	float MapRange(float value, float fromMin, float fromMax, float toMin, float toMax) {
		const float aSpan = fromMax - fromMin; const float bSpan = toMax - toMin;
		return toMin + ((value - fromMin) / aSpan) * bSpan;
	}
}

void RegisterGlm(py::module_& m) {
	// This is some snippets of the exact same code that Cave Engine uses
	// to register the glm math classes and methods. Since most of the bindings
	// are very similar for vec2, vec3, vec4, ivec2, etc, I only kept some of them.
	
	py::module math = m.def_submodule("math");

	py::class_<glm::mat3>(m, "Matrix3")
		.def(py::init<>());

	py::class_<glm::mat4>(m, "Matrix4")
		.def(py::init<>());

	py::class_<glm::ivec3>(m, "IntVector3")
		.def(py::init<>())
		.def(py::init<int>())
		.def(py::init<const glm::ivec3&>())
		.def(py::init<int, int, int>())

		.def("__repr__", [](const glm::ivec3& v) {
			return "cave.IntVector3(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
			})

		.def("copy", [](const glm::ivec3& self) { return glm::ivec3(self); })

		.def_readwrite("x", &glm::ivec3::x)
		.def_readwrite("y", &glm::ivec3::y)
		.def_readwrite("z", &glm::ivec3::z)

		.def_readwrite("r", &glm::ivec3::x)
		.def_readwrite("g", &glm::ivec3::y)
		.def_readwrite("b", &glm::ivec3::z)

		.def(py::self == py::self)
		.def(py::self + py::self)
		.def(py::self += py::self)
		.def(py::self - py::self)
		.def(py::self -= py::self)
		.def(py::self *= int())
		.def(py::self /= int())
		.def(int() * py::self)
		.def(py::self * int())
		.def(py::self * py::self)
		.def(py::self *= py::self)
		.def(py::self / int());

	py::class_<glm::vec3>(m, "Vector3")
		.def(py::init<>())
		.def(py::init<float>())
		.def(py::init<const glm::vec3&>())
		.def(py::init<float, float, float>())

		.def("__repr__", [](const glm::vec3& v) {
			return "cave.Vector3(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
			})

		.def("copy", [](const glm::vec3& self) { return glm::vec3(self); })

		.def_readwrite("x", &glm::vec3::x)
		.def_readwrite("y", &glm::vec3::y)
		.def_readwrite("z", &glm::vec3::z)

		.def_readwrite("r", &glm::vec3::x)
		.def_readwrite("g", &glm::vec3::y)
		.def_readwrite("b", &glm::vec3::z)

		.def("length", [](const glm::vec3& v) { return glm::length(v); })
		.def("__neg__", [](const glm::vec3& v) {return -v; })

		.def("dot", [](const glm::vec3& self, const glm::vec3& other) {return glm::dot(self, other); })
		.def("project", [](const glm::vec3& self, const glm::vec3& other) {return glm::proj(self, other); })

		.def(py::self == py::self)
		.def(py::self + py::self)
		.def(py::self += py::self)
		.def(py::self - py::self)
		.def(py::self -= py::self)
		.def(py::self *= float())
		.def(py::self /= float())
		.def(float() * py::self)
		.def(py::self * float())
		.def(py::self * py::self)
		.def(py::self *= py::self)
		.def(py::self / float());

	math.def("clamp", &glm::clamp<float>);
	math.def("min", &glm::min<float>);
	math.def("max", &glm::max<float>);

	math.def("sin", [](float value) {return glm::sin(value); });
	math.def("cos", [](float value) {return glm::cos(value); });
	math.def("tan", [](float value) {return glm::tan(value); });

	math.def("asin", [](float value) {return glm::asin(value); });
	math.def("acos", [](float value) {return glm::acos(value); });
	math.def("atan", [](float value) {return glm::atan(value); });

	math.def("abs",   [](float value) {return glm::abs(value);   });
	math.def("ceil",  [](float value) {return glm::ceil(value);  });
	math.def("floor", [](float value) {return glm::floor(value); });

	math.def("pow", [](float x, float y) {return glm::pow(x, y); });
	
	// This one is not in GLM, it's a cave function, but with interesting args.
	math.def("mapRange", &math::MapRange,
		py::arg("value"), 
		py::arg("fromMin"), py::arg("fromMax"), 
		py::arg("toMin"), py::arg("toMax")
	);

	// Dot product for vec3, in cave, we also have this for vec2 and vec4.
	math.def("dot", &glm::dot<3, float, glm::packed_highp>);

	math.def("inverse", &glm::inverse<float, glm::packed_highp>);
	math.def("inverse", &glm::inverse<4, 4, glm::f32, glm::packed_highp>);

	// Projects a vector into the other.
	math.def("project", &glm::proj<glm::vec3>, py::arg("vec"), py::arg("other"));
}

#endif // WITH_GL_BINDINGS

/*===========================================================================//
// 							 PYBIND11 PROCEDURES							 //
//===========================================================================*/


void RegisterFunctions(py::module& m){
	// Basic function registry:
	m.def("exampleFunction", &ExampleFunction);
	m.def("sumIntegers", &SumIntegers);
	m.def("argsWithDefaultValues", &ArgsWithDefaultValues, py::arg("x"), py::arg("y")=1.f);
	
	// Function overloadings:
	m.def("overloadingExample", (void (*)(int))&OverloadingExample);
	m.def("overloadingExample", (void (*)(float))&OverloadingExample);
	m.def("overloadingExample", (void (*)(bool))&OverloadingExample);
	
	// Lambdas:
	m.def("lambdaFunction", [](int x) { std::cout << " - lambda called with x = " << x << "\n";});
}

void RegisterEnumerations(py::module_& m) {
	py::module subm = m.def_submodule("subm");

	py::enum_<ExampleEnum>(subm, "ExampleEnum")
		.value("FIRST_VALUE",  ExampleEnum::FIRST_VALUE)
		.value("SECOND_VALUE", ExampleEnum::SECOND_VALUE)
		.value("THIRD_VALUE",  ExampleEnum::THIRD_VALUE)
		.export_values();
}


void RegisterBasicStruct(py::module& m){
	// Registering a basic structure class:
	py::class_<BasicStruct>(m, "BasicStruct")
		.def(py::init<const std::string&>())
		.def_readwrite("name", &BasicStruct::name);
		
	// Extra function:
	// This is important: if the return policy is by reference, python should NOT garbage collect and delete it!
	m.def("returnByReferenceTest", &ReturnByReferenceTest, py::return_value_policy::reference);
}

void RegisterBasicClasses(py::module& m){
	// Registering the base class:
	py::class_<BasicClass>(m, "BasicClass")
		// Constructors:
		.def(py::init<>())
		
		// Methods:
		.def("doSomethingPolymorphic", &BasicClass::DoSomethingPolymorphic)
		
		// Variables:
		.def_readwrite("width", &BasicClass::width)
		.def_readwrite("height", &BasicClass::height);
		
	// Now the child classes. They should have all the defs from the Basic Class + the extra ones!
	py::class_<ChildClassA, BasicClass>(m, "ChildClassA")
		.def(py::init<>());
		
	py::class_<ChildClassB, BasicClass>(m, "ChildClassB")
		.def(py::init<>())
		.def("doSomethingPolymorphic", &BasicClass::DoSomethingPolymorphic);
}

void RegisterComplicatedClass(py::module& m){
	// This class is all about "extra" stuff:
	py::class_<ComplicatedClass>(m, "ComplicatedClass")
		.def(py::init<>())
		
		// This is a variable, but using the class's getter and setter:
		.def_property("value", &ComplicatedClass::GetValue, &ComplicatedClass::SetValue)
		// IMPORTANT: The def_property may be dealing with a simple variable type, but may
		// also be dealing with a complicated one such as another class. Pybind11 will allow
		// you to specify py::return_value_policy::reference here as well so the get value
		// won't be freed by python. It's important to test that as well!
		
		.def("newChild", &ComplicatedClass::NewChild, py::return_value_policy::reference)
		.def_readwrite("children", &ComplicatedClass::children)
		
		.def_readwrite("tags", &ComplicatedClass::tags)
		.def_readwrite("properties", &ComplicatedClass::properties)
		
		.def("inspectProperties", &ComplicatedClass::InspectProperties);
}


PYBIND11_EMBEDDED_MODULE(module, m) {
	RegisterEnumerations(m);
	RegisterFunctions(m);
	RegisterBasicStruct(m);
	RegisterBasicClasses(m);
	RegisterComplicatedClass(m);
	
#ifdef WITH_GLM_BINDINGS
	RegisterGlm(m);
#endif
}

/*===========================================================================//
// 								 MAIN ENTRYPOINT							 //
//===========================================================================*/

int main(int argc, char** argv){	
	py::scoped_interpreter guard{};
	
	py::module module = py::module::import("module");
	
	// TODO: Test the bindings here...
	
	return 0;
}