include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_LIST *.cpp)
file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/src/*/*.cpp)
list(REMOVE_ITEM SRC_LIST "/home/ubuntu/FudgeDB/FudgeDBCPP/src/general/main.cpp")

add_executable(
  ftest
  ${TEST_LIST}
  ${SRC_LIST}
)
message(STATUS "src_list for test" ${SRC_LIST})
target_include_directories(
  ftest
  PUBLIC ${PROJECT_SOURCE_DIR}/inc/sqlsrc
  PUBLIC ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(
  ftest
  ${PROJECT_SOURCE_DIR}/lib/libsqlparser.so
  gtest_main
  readline
)

include(GoogleTest)
gtest_discover_tests(ftest)