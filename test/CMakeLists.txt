add_executable(xrootd-tcp-gtest main.cpp)
add_dependencies(xrootd-tcp-gtest gtest)
include_directories("${PROJECT_SOURCE_DIR}/vendor/gtest/googletest/include")

target_link_libraries(xrootd-tcp-gtest XrdTCPStats-5 "${CMAKE_BINARY_DIR}/external/gtest/src/gtest-build/lib/libgtest.a" -lpthread)

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/test/xrootd-tcp-gtest
  )