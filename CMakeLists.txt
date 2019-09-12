cmake_minimum_required(VERSION 3.1)

project(cnc-vision LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Qml Multimedia Charts REQUIRED)
#find_package(ZeroMQ REQUIRED)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE sources *.cpp)

add_executable(${PROJECT_NAME}
        ${sources}
        "quick.qrc"
        )

include_directories(${OpenCV_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWidhDebInfo>>:QY_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS} PRIVATE Qt5::Core Qt5::Quick Qt5::Qml Qt5::Multimedia Qt5::Charts)
