cmake_minimum_required(VERSION 3.15)

project(itkSphereGenerator)

find_package(ITK REQUIRED)

include(${ITK_USE_FILE})

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cxx)
