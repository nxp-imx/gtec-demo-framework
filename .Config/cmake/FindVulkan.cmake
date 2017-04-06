# Find Vulkan
#
# Vulkan_INCLUDE_DIRS
# Vulkan_LIBRARY
# Vulkan_FOUND

if (WIN32)
    find_path(Vulkan_INCLUDE_DIRS NAMES vulkan/vulkan.h HINTS
        "$ENV{Vulkan_SDK}/Include"
        "$ENV{VK_SDK_PATH}/Include")
    if (CMAKE_CL_64)
        find_library(Vulkan_LIBRARY NAMES vulkan-1 HINTS
            "$ENV{Vulkan_SDK}/Bin"
            "$ENV{VK_SDK_PATH}/Bin")
    else()
        find_library(Vulkan_LIBRARY NAMES vulkan-1 HINTS
            "$ENV{Vulkan_SDK}/Bin32"
            "$ENV{VK_SDK_PATH}/Bin32")
    endif()
else()
    find_path(Vulkan_INCLUDE_DIRS NAMES vulkan/vulkan.h HINTS
        "$ENV{Vulkan_SDK}/include")
    find_library(Vulkan_LIBRARY NAMES vulkan HINTS
        "$ENV{Vulkan_SDK}/lib")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vulkan DEFAULT_MSG Vulkan_LIBRARY Vulkan_INCLUDE_DIRS)

mark_as_advanced(Vulkan_INCLUDE_DIRS Vulkan_LIBRARY)
