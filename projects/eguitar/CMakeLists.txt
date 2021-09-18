project(eguitar)

file(GLOB EGUITAR_SRC "./*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(${PROJECT_NAME} ${EGUITAR_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE "./")
target_link_libraries(${PROJECT_NAME} PUBLIC stk)