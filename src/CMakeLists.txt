##
# Main targets
##
add_library( VC4CC SHARED "")
add_executable( VC4C main.cpp concepts.h)

set(VC4C_LIBRARY_NAME VC4CC)
set(VC4C_PROGRAM_NAME VC4C)

target_compile_definitions(${VC4C_LIBRARY_NAME} PUBLIC VC4C_VERSION="${PROJECT_VERSION}")
target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE VC4C_VERSION="${PROJECT_VERSION}")
if(VC4CL_STDLIB_DIR)
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE VC4CL_STDLIB_FOLDER="${VC4CL_STDLIB_DIR}")
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE VC4CL_STDLIB_FOLDER="${VC4CL_STDLIB_DIR}")
endif()

# append usage of C++ to compiler flags, also optimize for speed and enable all warnings
set(VC4C_ENABLED_WARNINGS -Wall -Wextra -Wold-style-cast -Wnon-virtual-dtor -Wnull-dereference -Wno-unused-parameter -Wno-missing-field-initializers -Wno-write-strings -Werror=return-type -Werror=unused-result -Werror=shift-count-overflow -Werror=missing-field-initializers -Werror=reorder)
# Enable additional warnings, if available
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	SET(VC4C_ENABLED_WARNINGS ${VC4C_ENABLED_WARNINGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-shadow -Wno-padded -Wno-shadow-field-in-constructor -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-documentation -Wno-unused-command-line-argument -Wno-unused-member-function -Wno-gnu-zero-variadic-macro-arguments -Wno-covered-switch-default -Wno-switch-enum -Wno-return-std-move-in-c++11 -Wswitch -Werror=return-stack-address)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	SET(VC4C_ENABLED_WARNINGS ${VC4C_ENABLED_WARNINGS} -Wlogical-op -Wmisleading-indentation -Wduplicated-cond -Wdouble-promotion -fdelete-null-pointer-checks -Wuninitialized -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=format -Wsuggest-override -Wconversion -Wzero-as-null-pointer-constant -Wno-psabi -Werror=return-local-addr -Werror=uninitialized)
endif()
target_compile_options(${VC4C_LIBRARY_NAME} PRIVATE ${VC4C_ENABLED_WARNINGS})
target_compile_options(${VC4C_PROGRAM_NAME} PRIVATE ${VC4C_ENABLED_WARNINGS})

if(BUILD_DEBUG)
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE DEBUG_MODE=1)
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE DEBUG_MODE=1)
endif(BUILD_DEBUG)
if(CLANG_FOUND)
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE CLANG_PATH="${CLANG_FOUND}")
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE CLANG_PATH="${CLANG_FOUND}")
endif(CLANG_FOUND)
if(LLVM_LINK_FOUND)
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE LLVM_LINK_PATH="${LLVM_LINK_FOUND}")
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE LLVM_LINK_PATH="${LLVM_LINK_FOUND}")
endif()
if(SPIRV_CLANG_FOUND)
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE SPIRV_CLANG_PATH="${SPIRV_CLANG_FOUND}")
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE SPIRV_CLANG_PATH="${SPIRV_CLANG_FOUND}")
endif(SPIRV_CLANG_FOUND)
if(ADVANCED_OPENCL_FEATURES)
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE USE_CLANG_OPENCL=1)
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE USE_CLANG_OPENCL=1)
endif(ADVANCED_OPENCL_FEATURES)
if(ENABLE_COVERAGE)
	target_compile_options(${VC4C_LIBRARY_NAME} PRIVATE -fprofile-arcs -ftest-coverage --coverage)
	target_compile_options(${VC4C_PROGRAM_NAME} PRIVATE -fprofile-arcs -ftest-coverage --coverage)
	target_link_libraries(${VC4C_LIBRARY_NAME} gcov "-fprofile-arcs -ftest-coverage")
	target_link_libraries(${VC4C_PROGRAM_NAME} gcov "-fprofile-arcs -ftest-coverage")
endif(ENABLE_COVERAGE)
if(OPT_FOUND)
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE OPT_PATH="${OPT_FOUND}")
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE OPT_PATH="${OPT_FOUND}")
endif()

# Add all sources
include(sources.list)
include(analysis/sources.list)
include(asm/sources.list)
include(intermediate/sources.list)
include(intrinsics/sources.list)
include(llvm/sources.list)
include(normalization/sources.list)
include(optimization/sources.list)
include(periphery/sources.list)
include(precompilation/sources.list)
include(spirv/sources.list)
include(tools/sources.list)

# Add all dependencies
target_link_libraries(${VC4C_PROGRAM_NAME} ${VC4C_LIBRARY_NAME} ${SYSROOT_LIBRARY_FLAGS})

# cpplog
add_dependencies(${VC4C_LIBRARY_NAME} cpplog-project-build)
add_library(cpplog-static STATIC IMPORTED)
ExternalProject_Get_Property(cpplog-project BINARY_DIR)
set_target_properties(cpplog-static PROPERTIES IMPORTED_LOCATION "${BINARY_DIR}/libcpplog-static.a")
target_link_libraries(${VC4C_LIBRARY_NAME} cpplog-static)
target_include_directories(${VC4C_LIBRARY_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/cpplog/include>)
target_compile_definitions(${VC4C_LIBRARY_NAME} PUBLIC CPPLOG_NAMESPACE=logging CPPLOG_CUSTOM_LOGGER=true)

# threading library
if(MULTI_THREADED)
	target_link_libraries(${VC4C_LIBRARY_NAME} ${CMAKE_THREAD_LIBS_INIT})
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE MULTI_THREADED=1)
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE MULTI_THREADED=1)
	# For dlopen, dlsym
	target_link_libraries(${VC4C_LIBRARY_NAME} dl)
endif(MULTI_THREADED)

# SPIR-V Tools
if(VC4C_ENABLE_SPIRV_FRONTEND)
	add_dependencies(${VC4C_LIBRARY_NAME} spirv-tools-project-build)
	ExternalProject_Get_Property(spirv-tools-project BINARY_DIR)

	set(spirv-tools "-Wl,--whole-archive ${BINARY_DIR}/source/libSPIRV-Tools.a -Wl,--no-whole-archive")
	target_link_libraries(${VC4C_LIBRARY_NAME} "${spirv-tools}")

	set(spirv-tools-opt "-Wl,--whole-archive ${BINARY_DIR}/source/opt/libSPIRV-Tools-opt.a -Wl,--no-whole-archive")
	target_link_libraries(${VC4C_LIBRARY_NAME} "${spirv-tools-opt}")

	add_library(spirv-tools-link STATIC IMPORTED)
	set_target_properties(spirv-tools-link PROPERTIES IMPORTED_LOCATION "${BINARY_DIR}/source/link/libSPIRV-Tools-link.a")
	target_link_libraries(${VC4C_LIBRARY_NAME} spirv-tools-link)

	target_include_directories(${VC4C_LIBRARY_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/lib/spirv-headers/include")
	target_include_directories(${VC4C_LIBRARY_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/lib/spirv-tools/include")
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE SPIRV_LLVM_SPIRV_PATH="${SPIRV_LLVM_SPIR_FOUND}" SPIRV_FRONTEND=1)
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE SPIRV_LLVM_SPIRV_PATH="${SPIRV_LLVM_SPIR_FOUND}" SPIRV_FRONTEND=1)
endif(VC4C_ENABLE_SPIRV_FRONTEND)

# LLVM library
if(VC4C_ENABLE_LLVM_LIB_FRONTEND)
	#This works with libLLVM.so as well as the single static component libraries
	string(STRIP "-L ${LLVM_LIBS_PATH} ${LLVM_LIB_NAMES} ${LLVM_SYSTEM_LIB_NAMES}" llvm)
	target_link_libraries(${VC4C_LIBRARY_NAME} "${llvm}")

	# adds require macros as well as include-paths
	# SYSTEM here suppresses (at least on some compilers/systems) all warnings from the LLVM header file
	target_include_directories(${VC4C_LIBRARY_NAME} SYSTEM PRIVATE "${LLVM_INCLUDE_PATH}")
	# TODO LLVM_LIB_FLAGS results in a lot of warnings about redefining _GNU_SOURCE (at least for Clang)
	target_compile_options(${VC4C_LIBRARY_NAME} PRIVATE ${LLVM_LIB_FLAGS})
	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE USE_LLVM_LIBRARY=1 LLVM_LIBRARY_VERSION=${LLVM_LIBRARY_VERSION})
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE USE_LLVM_LIBRARY=1 LLVM_LIBRARY_VERSION=${LLVM_LIBRARY_VERSION})
endif(VC4C_ENABLE_LLVM_LIB_FRONTEND)

if(VERIFY_OUTPUT)
	add_dependencies(${VC4C_LIBRARY_NAME} vc4asm-project-build)
	add_library(vc4asm STATIC IMPORTED)
	ExternalProject_Get_Property(vc4asm-project BINARY_DIR)
	set_target_properties(vc4asm PROPERTIES IMPORTED_LOCATION "${BINARY_DIR}/libvc4asm.a")
	target_link_libraries(${VC4C_LIBRARY_NAME} vc4asm)

	target_compile_definitions(${VC4C_LIBRARY_NAME} PRIVATE VERIFIER_HEADER="${PROJECT_SOURCE_DIR}/lib/vc4asm/src/Validator.h")
	target_compile_definitions(${VC4C_PROGRAM_NAME} PRIVATE VERIFIER_HEADER="${PROJECT_SOURCE_DIR}/lib/vc4asm/src/Validator.h")
endif()

# Download mpark/variant library as dependency
add_dependencies(${VC4C_LIBRARY_NAME} variant-project-build)
target_include_directories(${VC4C_LIBRARY_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/lib/variant/include")
target_include_directories(${VC4C_PROGRAM_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/lib/variant/include")

if(VC4CL_STDLIB_DIR)
	# Pre-compile VC4CL standard library files if development headers available and output files do not yet exist
	add_custom_command(TARGET ${VC4C_PROGRAM_NAME} POST_BUILD
	    COMMAND             if \[\[ ! -e ${VC4CL_STDLIB_DIR}/VC4CLStdLib.bc || ! -e ${VC4CL_STDLIB_DIR}/VC4CLStdLib.h.pch \]\]; then $<TARGET_FILE:${VC4C_PROGRAM_NAME}> --quiet --precompile-stdlib -o ${VC4CL_STDLIB_DIR}/ ${VC4CL_STDLIB_DIR}/VC4CLStdLib.h && echo \"VC4CL standard library precompiled into ${VC4CL_STDLIB_DIR}\" \; fi
	)
endif(VC4CL_STDLIB_DIR)

# "For shared libraries VERSION and SOVERSION can be used to specify the build version and API version respectively."
set_target_properties(
	${VC4C_LIBRARY_NAME} PROPERTIES
	# This corresponds to the project/library-version
	VERSION "${PROJECT_VERSION}"
	# This corresponds to the API-version (e.g. OpenCL 1.2)
	SOVERSION "1.2"
)

find_program(
  CLANG_FORMAT_PATH
  NAMES clang-format clang-format-3.8 clang-format-3.9 clang-format-4.0
  HINTS "/usr/bin" "/usr/local/bin"
)

if(CLANG_FORMAT_PATH)
  message (STATUS "found clang-format: ${CLANG_FORMAT_PATH}")
  get_target_property(VC4CC_SRCS ${VC4C_LIBRARY_NAME} SOURCES)
  get_target_property(VC4C_SRCS ${VC4C_PROGRAM_NAME} SOURCES)
  add_custom_target(
    clang-format
    COMMAND ${CLANG_FORMAT_PATH}
      -i ${VC4CC_SRCS} ${VC4C_SRCS}
    WORKING_DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}
  )
else()
  message (WARNING "clang-format not found: strongly recommend to use it before commit!")
endif()

##
# Installation targets
##
# Adds the public headers to the target, so they are exported
target_include_directories(${VC4C_LIBRARY_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>  $<INSTALL_INTERFACE:include/vc4cc>)
# Creates the install target for the library and the compiler
install(TARGETS ${VC4C_LIBRARY_NAME} EXPORT VC4CC-targets LIBRARY DESTINATION lib)
install(TARGETS ${VC4C_PROGRAM_NAME} EXPORT VC4C-targets RUNTIME DESTINATION bin)
# Creates the export target (to be used by CMake to find the INSTALLED library)
install(EXPORT VC4CC-targets DESTINATION share/vc4cc)
# Creates the install target for the headers
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION include/vc4cc FILES_MATCHING PATTERN "*.h")
# Exports the target (to be used by CMake to find the SOURCE library)
export(TARGETS ${VC4C_LIBRARY_NAME} FILE vc4cc-exports.cmake)
# Adds custom uninstall command
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "cmake_uninstall.cmake")
