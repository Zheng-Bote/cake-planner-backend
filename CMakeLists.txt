cmake_minimum_required(VERSION 3.24)

# cmake -S . -B build
# cmake --build build -j$(nproc)

# Projektname und Sprache
project(CakePlanner
    VERSION 0.1.0
    DESCRIPTION "Cake Planner Backend"
    HOMEPAGE_URL "https://github.com/Zheng-Bote/cake-planner-backend"
    LANGUAGES C CXX)

# C++23 Standard erzwingen
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt6 finden
find_package(Qt6 REQUIRED COMPONENTS Core Sql)
# OpenSSL finden
find_package(OpenSSL REQUIRED)

# --- Configure (für rz_config.hpp) ---
set(EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROG_LONGNAME "Crow Cake Planner Backend")
set(PROG_CREATED "2025")
set(PROG_AUTHOR "ZHENG Bote")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "net.hase-zheng")

if(Qt6_FOUND)
    set(CMAKE_QTV "${Qt6_VERSION}")
else()
    set(CMAKE_QTV "Unknown")
endif()

# Generiert include/rz_config.hpp im Binary-Tree
add_subdirectory(configure) 

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-30-2
)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include)

FetchContent_Declare(
    dotenv-cpp
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(dotenv-cpp)

# Argon2id
FetchContent_Declare(
    Argon2
    GIT_REPOSITORY https://github.com/P-H-C/phc-winner-argon2.git
    GIT_TAG        20190702 # Commit ID oder Tag für Stabilität
)
FetchContent_MakeAvailable(Argon2)

# Da das Repo kein CMake hat, definieren wir die Library manuell:
# Wir nutzen die "opt" (optimierte) Implementierung für x86/x64
set(ARGON2_SRC
    ${argon2_SOURCE_DIR}/src/argon2.c
    ${argon2_SOURCE_DIR}/src/core.c
    ${argon2_SOURCE_DIR}/src/blake2/blake2b.c
    ${argon2_SOURCE_DIR}/src/thread.c
    ${argon2_SOURCE_DIR}/src/encoding.c
    ${argon2_SOURCE_DIR}/src/opt.c 
)

add_library(argon2_lib STATIC ${ARGON2_SRC})
target_include_directories(argon2_lib PUBLIC ${argon2_SOURCE_DIR}/include)
# Threads sind notwendig für Argon2 Parallelismus
find_package(Threads REQUIRED)
target_link_libraries(argon2_lib PRIVATE Threads::Threads)

# --- Dependency: jwt-cpp ---
FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(jwt-cpp)

# Crow Framework
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG master
)
FetchContent_MakeAvailable(Crow)



# Quelldateien definieren (inklusive MVC Struktur)
set(SOURCES
    src/main.cpp
    src/database.cpp
    src/models/user_model.cpp
    src/controllers/user_controller.cpp
    src/controllers/auth_controller.cpp 
    src/utils/token_utils.cpp           
    src/utils/password_utils.cpp
    src/utils/env_loader.cpp
    src/utils/seeder.cpp
)

# Executable erstellen
add_executable(CakePlanner ${SOURCES})

# Include Directories
# WICHTIG: CMAKE_BINARY_DIR hinzufügen, damit rz_config.hpp gefunden wird
target_include_directories(CakePlanner PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include 
    ${ASIO_INCLUDE_DIR}
)

target_compile_features(CakePlanner PUBLIC cxx_std_23)

# Bibliotheken linken
target_link_libraries(CakePlanner PRIVATE
    Crow::Crow
    Qt6::Core
    Qt6::Sql
    argon2_lib
    OpenSSL::SSL 
    OpenSSL::Crypto
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    dotenv
)

# Optimierungen
if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(CakePlanner PRIVATE -O3)
endif()