cmake_minimum_required(VERSION 3.10)

PROJECT ("pca")

include (MacroAddFileDependencies)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (WIN32)
    if (MSVC)
        # Standard:          /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR
        # Reference: https://msdn.microsoft.com/en-us/library/fwkeyyhe.aspx
        # WIN32 --> Win32 console application
        # Wx  --> Warning level
        # Zi  --> Produces a program database (PDB) for complete debug information
        # EHsc --> Exception handling model (catches C++ exceptions only)
        # GR(-) --> Enables / Disables Run-Time Type Information
        # MP  --> Multi-processes compilation
        # openmp --> Enables openmp
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj")
    endif (MSVC)
endif (WIN32)

add_definitions(-D_WIN32_WINNT=0x600)
###########################################
# - variables -----------------------------

set(ENABLE_EXAMPLE_PROJECT ON) #CACHE BOOL "Make example project"
set(PREBUILT_DIR "${CMAKE_SOURCE_DIR}/prebuilt" CACHE PATH "Prebuilt path")
set(ENABLE_CUDA "TRUE" CACHE BOOL "Enable CUDA build")
set(CUDA_VERSION 10.2 CACHE STRING "Make example project")

###########################################
# - check cuda ----------------------------

if(ENABLE_CUDA)
    find_package(CUDA ${CUDA_VERSION})
endif()

if( NOT CUDA_FOUND )
    set(ENABLE_CUDA "FALSE" CACHE BOOL "Enable CUDA build")
endif()

if (ENABLE_CUDA)
    include(CheckLanguage)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        enable_language(CUDA)
        set(CUDA_ARCHITECTURES "Auto" CACHE STRING "\"Auto\" detects local machine GPU compute arch at runtime, \"Common\" and \"All\" cover common and entire subsets of architectures, \"Names\" is a list of architectures to enable by name, \"Numbers\" is a list of compute capabilities (version number) to enable")
        set_property(CACHE CUDA_ARCHITECTURES PROPERTY STRINGS "Auto" "Common" "All" "Kepler Maxwell Kepler+Tegra Maxwell+Tegra Pascal" "5.0 7.5")
        cuda_select_nvcc_arch_flags(CUDA_ARCH_FLAGS ${CUDA_ARCHITECTURES})
        if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.17")
            if(BUILD_SHARED_LIBS)
                set(CMAKE_CUDA_RUNTIME_LIBRARY "Shared")
            else()
                set(CMAKE_CUDA_RUNTIME_LIBRARY "Static")
            endif()
        endif()
    else()
        set(ENABLE_CUDA "FALSE" CACHE BOOL "Enable CUDA build" FORCE)
        message(SEND_ERROR "CMake cannot locate a CUDA compiler")
    endif()
endif()

if(CMAKE_CUDA_COMPILER AND ENABLE_CUDA)
  set(CUDA_ARCHITECTURES "Auto" CACHE STRING "\"Auto\" detects local machine GPU compute arch at runtime, \"Common\" and \"All\" cover common and entire subsets of architectures, \"Names\" is a list of architectures to enable by name, \"Numbers\" is a list of compute capabilities (version number) to enable")
  set_property(CACHE CUDA_ARCHITECTURES PROPERTY STRINGS "Auto" "Common" "All" "Kepler Maxwell Kepler+Tegra Maxwell+Tegra Pascal" "5.0 7.5")
  enable_language(CUDA)
  if(CMAKE_CUDA_COMPILER_VERSION VERSION_LESS "9.0")
    message(STATUS "Unsupported CUDA version, please upgrade to CUDA 9+. Disabling CUDA support")
    set(ENABLE_CUDA "FALSE" CACHE BOOL "Enable CUDA support" FORCE)
  else()
    find_package(CUDA REQUIRED)
    cuda_select_nvcc_arch_flags(CUDA_ARCH_FLAGS ${CUDA_ARCHITECTURES})
    message(STATUS "Building with CUDA flags: " "${CUDA_ARCH_FLAGS}")
    if (NOT "arch=compute_70,code=sm_70" IN_LIST CUDA_ARCH_FLAGS AND NOT "arch=compute_72,code=sm_72" IN_LIST CUDA_ARCH_FLAGS AND NOT "arch=compute_75,code=sm_75" IN_LIST CUDA_ARCH_FLAGS AND NOT "arch=compute_80,code=sm_80" IN_LIST CUDA_ARCH_FLAGS)
      set(ENABLE_CUDNN_HALF "FALSE" CACHE BOOL "Enable CUDNN Half precision" FORCE)
      message(STATUS "Your setup does not supports half precision (it requires CC >= 7.0)")
    else()
      message(STATUS "Your setup supports half precision (it requires CC >= 7.0)")
    endif()
  endif()
  if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.17")
    if(BUILD_SHARED_LIBS)
      set(CMAKE_CUDA_RUNTIME_LIBRARY "Shared")
    else()
      set(CMAKE_CUDA_RUNTIME_LIBRARY "Static")
    endif()
  endif()
  string (REPLACE ";" " " CUDA_ARCH_FLAGS_SPACE_SEPARATED "${CUDA_ARCH_FLAGS}")
  set(CMAKE_CUDA_FLAGS "${CUDA_ARCH_FLAGS_SPACE_SEPARATED} ${CUDA_HOST_COMPILER_FLAGS} ${CMAKE_CUDA_FLAGS}")
  message(STATUS "CMAKE_CUDA_FLAGS: ${CMAKE_CUDA_FLAGS}")
else()
  set(ENABLE_CUDA "FALSE" CACHE BOOL "Enable CUDA support" FORCE)
endif()

###########################################
# - build type ----------------------------
# Set a default build type if none was specified
IF(NOT MSVC)
  set(DEFAULT_BUILD_TYPE "Release")

  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' since none specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()
endif()

###########################################
# - compiler options ----------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
ADD_DEFINITIONS(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

add_definitions(-DWIN32_LEAN_AND_MEAN )

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

###########################################
# - Windows crt library option ------------

foreach(flag_var
    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
endforeach(flag_var)

foreach(flag_var
    CMAKE_CUDA_FLAGS CMAKE_CUDA_FLAGS_DEBUG CMAKE_CUDA_FLAGS_RELEASE
    CMAKE_CUDA_FLAGS_MINSIZEREL CMAKE_CUDA_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "-MD" "-MT" ${flag_var} "${${flag_var}}")
endforeach(flag_var)

###########################################
# - Conan Setting -------------------------

include (CheckTypeSize)
CHECK_TYPE_SIZE("void*"  CMAKE_SIZEOF_VOID_P)

include(conan.cmake)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  configure_file(conanfile.py "${CMAKE_CURRENT_SOURCE_DIR}/_conanfile86.py" @ONLY)
  conan_cmake_run(CONANFILE "_conanfile86.py"
                BASIC_SETUP NO_OUTPUT_DIRS CMAKE_TARGETS
                SETTINGS arch=x86
                BUILD missing)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  configure_file(conanfile.py "${CMAKE_CURRENT_SOURCE_DIR}/_conanfile64.py" @ONLY)
  conan_cmake_run(CONANFILE "_conanfile64.py"
                BASIC_SETUP NO_OUTPUT_DIRS CMAKE_TARGETS
                SETTINGS arch=x86_64
                BUILD missing)
endif()

conan_global_flags()

###########################################
# - prebuilt paths setting ----------------
# set include path

find_package( OpenCV REQUIRED )

if( OpenCV_FOUND )
    message(STATUS "OpenCV IncludeDir ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV Libs ${OpenCV_LIBS}")
endif()

file(GLOB PREBUILT_INCLUDE_DIR
    "${PREBUILT_DIR}/include/*/"
    "${PREBUILT_DIR}/*/include/"
    "${PREBUILT_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${PREBUILT_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CUDA_INCLUDE_DIRS}
)

# set library path
file(GLOB PREBUILT_STATIC_LIBRARY_DIR
    "${PREBUILT_DIR}/lib/*/${CMAKE_BUILD_TYPE}"
    "${PREBUILT_DIR}/lib/*/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}"
    "${PREBUILT_DIR}/*/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/lib/${CMAKE_BUILD_TYPE}"
)
file(GLOB PREBUILT_SHARED_LIBRARY_DIR
    "${PREBUILT_DIR}/lib/*/${CMAKE_BUILD_TYPE}"
    "${PREBUILT_DIR}/bin/*/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}"
    "${PREBUILT_DIR}/*/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/bin/${CMAKE_BUILD_TYPE}"
)

link_directories(
    ${PREBUILT_STATIC_LIBRARY_DIR}
    ${PREBUILT_SHARED_LIBRARY_DIR}
)

###########################################
# - subdirectory --------------------------

add_subdirectory(src)
add_subdirectory(example)
add_dependencies(example ${CMAKE_PROJECT_NAME})

