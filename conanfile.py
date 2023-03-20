from conans import ConanFile  # , CMake
#from conan.tools.cmake import CMakeDeps
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps


class b0berman(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "glbinding/3.1.0",
        "opengl/system",
        "glfw/3.3.8",
        "nlohmann_json/3.11.2",
        "lyra/1.6.1",
        "spdlog/1.11.0",
        "freeimage/3.18.0",
        "glm/0.9.9.8",
        "boost/1.81.0"
    ]

    generators = "CMakeDeps"
    default_options = {}

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        cmake = CMakeDeps(self)
        cmake.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        #cmake = CMake(self)
        # cmake.build()
        pass

        #cmake = CMake(self)
        # cmake.configure()
        # cmake.build()
