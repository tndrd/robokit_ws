cmake_minimum_required(VERSION 3.5)
project(robokit_hardware)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -fpic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(serial REQUIRED)

add_library(rcb4lib lib/RCB4lib/rcb4lib.cpp)
ament_target_dependencies(rcb4lib serial)

add_library(
  ${PROJECT_NAME}
  SHARED
  src/robokit_hardware.cpp
)

target_link_libraries(${PROJECT_NAME} rcb4lib)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  include
  lib/RCB4lib
)

ament_target_dependencies(
  ${PROJECT_NAME}
  hardware_interface
  pluginlib
  rclcpp
  serial
)

pluginlib_export_plugin_description_file(hardware_interface robokit_hardware.xml)

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION lib
)
install(
  DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(
  include
)
ament_export_libraries(
  ${PROJECT_NAME}
  rcb4lib
)
ament_export_dependencies(
  hardware_interface
  pluginlib
  rclcpp
  serial
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()