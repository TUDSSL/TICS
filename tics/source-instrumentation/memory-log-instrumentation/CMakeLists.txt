cmake_minimum_required(VERSION 2.8.8)
project(clang-memory-log-instrumentation)

find_package(LLVM REQUIRED CONFIG)


message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

SET(BIN_NAME "memlog")

# Now build our tools
add_executable("${BIN_NAME}" memlog.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)
message(STATUS "LLVM_LIBS: ${llvm_libs}")


target_link_libraries(${BIN_NAME}
    LLVM
    clangAST
	clangASTMatchers
	clangAnalysis
	clangBasic
	clangDriver
	clangEdit
	clangFrontend
	clangFrontendTool
	clangLex
	clangParse
	clangSema
	clangEdit
	clangRewrite
	clangRewriteFrontend
	clangStaticAnalyzerFrontend
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangCrossTU
	clangIndex
	clangSerialization
	clangToolingCore
	clangTooling
	clangFormat
  )

target_link_libraries(${BIN_NAME} ${llvm_libs})

execute_process(COMMAND bash "-c" "llvm-config --cxxflags" OUTPUT_VARIABLE compile_flags OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Compile flags: ${compile_flags}")

execute_process(COMMAND bash "-c" "llvm-config --ldflags --libs --system-libs" OUTPUT_VARIABLE ld_flags OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "LD flags: ${ld_flags}")


set_property(TARGET ${BIN_NAME} APPEND PROPERTY COMPILE_FLAGS "-std=c++11 ${compile_flags}")

