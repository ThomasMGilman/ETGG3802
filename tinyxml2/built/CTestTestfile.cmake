# CMake generated Testfile for 
# Source directory: D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2
# Build directory: D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xmltest "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/Debug/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  WORKING_DIRECTORY "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/Debug" _BACKTRACE_TRIPLES "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;104;add_test;D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xmltest "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/Release/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  WORKING_DIRECTORY "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/Release" _BACKTRACE_TRIPLES "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;104;add_test;D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xmltest "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/MinSizeRel/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  WORKING_DIRECTORY "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/MinSizeRel" _BACKTRACE_TRIPLES "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;104;add_test;D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xmltest "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/RelWithDebInfo/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  WORKING_DIRECTORY "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/built/RelWithDebInfo" _BACKTRACE_TRIPLES "D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;104;add_test;D:/SchoolAndProjects/Programs/ETGG 3802_01 Realtime Interactive Prog 2/tinyxml2/CMakeLists.txt;0;")
else()
  add_test(xmltest NOT_AVAILABLE)
endif()
