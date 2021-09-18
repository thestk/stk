project(demo)

file(GLOB DEMO_SRC "./*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(stk-demo "demo.cpp" "utilities.cpp")
target_include_directories(stk-demo PRIVATE "./")
target_link_libraries(stk-demo PUBLIC stk)