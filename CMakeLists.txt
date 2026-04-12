cmake_minimum_required(VERSION 3.24)

# cmake -S . -B build
# cmake --build build -j$(nproc)

# pkill CakePlanner
# or
# killall CakePlanner

project(CakePlanner
    VERSION 1.4.0
    DESCRIPTION "Cake Planner Backend"
    HOMEPAGE_URL "https://zheng-bote.github.io/cake-planner-backend"
    LANGUAGES C CXX)

# --- IMPORTANT: Activate Qt MOC ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROG_LONGNAME "Crow Cake Planner Backend")
set(PROG_CREATED "2026")
set(PROG_AUTHOR "ZHENG Bote")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "net.hase-zheng")
set(PROG_LICENSE "MIT")

# Since we might use system Qt or Conan Qt, we should get version after find_package
# But with Conan, find_package(Qt6) happens later.
# I'll move this after find_package(Qt6).

add_subdirectory(configure)

# --- Dependencies ---

# Use Conan for packages found on ConanCenter
find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
find_package(spdlog REQUIRED)
find_package(argon2 REQUIRED)
find_package(Crow REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Sql Gui Network)

if(Qt6_FOUND)
    set(CMAKE_QTV "${Qt6_VERSION}")
else()
    set(CMAKE_QTV "Unknown")
endif()

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

include(FetchContent)

# Keep FetchContent for packages NOT on ConanCenter
FetchContent_Declare(
    SimpleMail
    GIT_REPOSITORY https://github.com/cutelyst/simple-mail.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(SimpleMail)

FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        v0.7.1
)
FetchContent_MakeAvailable(jwt-cpp)

# --- Sources & Headers ---

set(HEADERS
    include/database.hpp
    include/middleware/auth_middleware.hpp
    include/models/user_model.hpp
    include/models/event_model.hpp
    include/models/config_model.hpp
    include/controllers/user_controller.hpp
    include/controllers/auth_controller.hpp
    include/controllers/admin_controller.hpp
    include/controllers/event_controller.hpp
    include/controllers/system_controller.hpp
    include/utils/token_utils.hpp
    include/utils/password_utils.hpp
    include/utils/env_loader.hpp
    include/utils/seeder.hpp
    include/utils/totp_utils.hpp
    include/utils/image_processor.hpp
    include/utils/app_helpers.hpp
    include/services/smtp_service.hpp
    include/services/notification_service.hpp
    include/rz_config.hpp
)

set(SOURCES
    src/main.cpp
    src/database.cpp
    src/models/user_model.cpp
    src/models/event_model.cpp
    src/models/config_model.cpp
    src/controllers/user_controller.cpp
    src/controllers/auth_controller.cpp
    src/controllers/admin_controller.cpp
    src/controllers/event_controller.cpp
    src/controllers/system_controller.cpp
    src/utils/token_utils.cpp
    src/utils/password_utils.cpp
    src/utils/env_loader.cpp
    src/utils/seeder.cpp
    src/utils/totp_utils.cpp
    src/utils/image_processor.cpp
    src/services/smtp_service.cpp
    src/services/notification_service.cpp
)

add_executable(CakePlanner ${SOURCES} ${HEADERS})

target_include_directories(CakePlanner PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${simplemail_SOURCE_DIR}/src
)

target_compile_features(CakePlanner PUBLIC cxx_std_23)

target_link_libraries(CakePlanner PRIVATE
    Crow::Crow
    Qt6::Core
    Qt6::Sql
    Qt6::Gui
    Qt6::Network
    SimpleMail3Qt6
    argon2::argon2
    OpenSSL::SSL
    OpenSSL::Crypto
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    asio::asio
)

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(CakePlanner PRIVATE -O3)
endif()

# libs.txt Generierung (Timing-sicher via GENERATE)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/libs.txt"
     CONTENT "$<TARGET_PROPERTY:CakePlanner,LINK_LIBRARIES>")
