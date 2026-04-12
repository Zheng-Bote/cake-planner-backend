from conan import ConanFile

class CakePlannerRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("nlohmann_json/3.11.3")
        self.requires("asio/1.30.2", override=True)
        self.requires("spdlog/1.12.0")
        self.requires("argon2/20190702")
        self.requires("crowcpp-crow/1.2.1")
        # Removed jwt-cpp to avoid Conan bringing in its own OpenSSL
