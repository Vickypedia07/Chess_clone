# CMake generated Testfile for 
# Source directory: E:/Chess_clone
# Build directory: E:/Chess_clone/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test("tests" "E:/Chess_clone/build/tests.exe")
set_tests_properties("tests" PROPERTIES  _BACKTRACE_TRIPLES "E:/Chess_clone/CMakeLists.txt;24;add_test;E:/Chess_clone/CMakeLists.txt;0;")
subdirs("_deps/catch2-build")
