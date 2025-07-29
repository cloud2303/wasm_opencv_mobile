cmake_minimum_required(VERSION 3.31)
project(wasm_test)

set(CMAKE_CXX_STANDARD 26)
add_executable(wasm_test main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    target_compile_options(wasm_test PRIVATE -O3 -flto)
    target_link_options(wasm_test PRIVATE -O3 -flto)
endif ()
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv-mobile-4.11.0-webassembly/basic/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)
target_link_libraries(wasm_test PRIVATE ${OpenCV_LIBS})
find_package(WebP CONFIG REQUIRED)
target_link_libraries(wasm_test PRIVATE WebP::webp WebP::webpdecoder WebP::webpdemux)

target_link_options(wasm_test PRIVATE
        -sINITIAL_MEMORY=32MB
        -sALLOW_MEMORY_GROWTH=1
        -sMODULARIZE=1
        -sEXPORT_NAME=createModule
        -sEXPORTED_FUNCTIONS=["_malloc","_free"]
        -sEXPORTED_RUNTIME_METHODS=["cwrap","HEAPU8"]
        --bind
)
