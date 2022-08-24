cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0104 OLD)
project(json-validator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Widgets Gui ) 
ADD_DEFINITIONS(${QT_DEFINITIONS})
find_package(Qt5 REQUIRED COMPONENTS OpenGL Quick Core Gui Sql Charts)

find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})
link_libraries(${JSONCPP_LIBRARIES})

# Import installed valijson and link it to your executable
find_package(valijson REQUIRED)

include_directories(src src/resource src/forms src/headers src/sources)

set(HEADERS
    src/headers/mainwindow.h
    src/headers/aboutwindow.h
    src/headers/jsonValidator.h
)

set(SOURCE_json-validator
    src/sources/mainwindow.cpp
    src/sources/aboutwindow.cpp
    src/sources/main.cpp 
    src/sources/jsonValidator.cpp
)

qt5_wrap_cpp(HEADER_json-validator ${HEADERS})
qt5_wrap_ui(UI_json-validator src/forms/mainwindow.ui src/forms/aboutwindow.ui) 
qt5_add_resources(RESOURCES src/resource/resources.qrc)

add_executable(json-validator 
    ${SOURCE_json-validator} 
    ${HEADER_json-validator} 
    ${UI_json-validator} # ${RESOURCES}
)

target_link_libraries(json-validator 
    jsoncpp
    valijson
    Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Charts Qt5::OpenGL
) 

