cmake_minimum_required(VERSION 2.8)

project(KMSCube)

include(FindPkgConfig)

pkg_search_module(DRM REQUIRED libdrm)
include_directories(${DRM_INCLUDE_DIRS})
add_executable(kmsshooter main.c esTransform.c)
target_link_libraries(kmsshooter ${DRM_LIBRARIES} GLESv2 EGL gbm m)
