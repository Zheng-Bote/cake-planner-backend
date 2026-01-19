cmake_minimum_required(VERSION 3.24)

# cmake -S . -B build
# cmake --build build -j$(nproc)

# pkill CakePlanner
# or
# killall CakePlanner

project(CakePlanner
    VERSION 0.12.0
    DESCRIPTION "Cake Planner Backend"
    HOMEPAGE_URL "https://zheng-bote.github.io/cake-planner-backend"
    LANGUAGES C CXX)

# --- IMPORTANT: Activate Qt MOC ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Gui)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROG_LONGNAME "Crow Cake Planner Backend")
set(PROG_CREATED "2026")
set(PROG_AUTHOR "ZHENG Bote")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "net.hase-zheng")
set(PROG_LICENSE "MIT")

if(Qt6_FOUND)
    set(CMAKE_QTV "${Qt6_VERSION}")
else()
    set(CMAKE_QTV "Unknown")
endif()

add_subdirectory(configure)

include(FetchContent)

# --- Dependencies ---

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

# --- SPDLOG ---
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    dotenv-cpp
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(dotenv-cpp)

FetchContent_Declare(
    Argon2
    GIT_REPOSITORY https://github.com/P-H-C/phc-winner-argon2.git
    GIT_TAG        20190702
)
FetchContent_MakeAvailable(Argon2)

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
target_link_libraries(argon2_lib PRIVATE Threads::Threads)

FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(jwt-cpp)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG master
)
FetchContent_MakeAvailable(Crow)

FetchContent_Declare(
    SimpleMail
    GIT_REPOSITORY https://github.com/cutelyst/simple-mail.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(SimpleMail)

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
    ${ASIO_INCLUDE_DIR}
    ${simplemail_SOURCE_DIR}/src
)

target_compile_features(CakePlanner PUBLIC cxx_std_23)

target_link_libraries(CakePlanner PRIVATE
    Crow::Crow
    Qt6::Core
    Qt6::Sql
    Qt6::Gui
    SimpleMail3Qt6
    argon2_lib
    OpenSSL::SSL
    OpenSSL::Crypto
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    dotenv
    spdlog::spdlog
)

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(CakePlanner PRIVATE -O3)
endif()
