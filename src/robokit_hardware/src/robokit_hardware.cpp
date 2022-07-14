// Copyright 2020 ros2_control Development Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <robokit_hardware/robokit_hardware.hpp>

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/macros.hpp>

namespace robokit_hardware
{
hardware_interface::return_type RobokitHardware::configure(
  const hardware_interface::HardwareInfo & info)
{
  if (configure_default(info) != hardware_interface::return_type::OK)
  {
    return hardware_interface::return_type::ERROR;
  }

  std::string port = info_.hardware_parameters["serial_port"];
  long baudrate    = stol(info_.hardware_parameters["baudrate"]);
  int timeout      = stoi(info_.hardware_parameters["timeout"]);

  serial_.open(port, baudrate, timeout);


  for (const hardware_interface::ComponentInfo & joint : info_.joints)
  {
    std::cout << "Found joint " << joint.name << std::endl;
  }

  hw_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
  hw_commands_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
  status_ = hardware_interface::status::CONFIGURED;
  return hardware_interface::return_type::OK;
}

std::vector<hardware_interface::StateInterface>
RobokitHardware::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;
  for (uint i = 0; i < info_.joints.size(); i++)
  {
    state_interfaces.emplace_back(hardware_interface::StateInterface(
      info_.joints[i].name, hardware_interface::HW_IF_POSITION, &hw_states_[i]));
  }

  return state_interfaces;
}

std::vector<hardware_interface::CommandInterface>
RobokitHardware::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;
  for (uint i = 0; i < info_.joints.size(); i++)
  {
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
      info_.joints[i].name, hardware_interface::HW_IF_POSITION, &hw_commands_[i]));
  }

  return command_interfaces;
}

hardware_interface::return_type RobokitHardware::start()
{
  RCLCPP_INFO(rclcpp::get_logger("RobokitHardware"), "HWI started");
  status_ = hardware_interface::status::STARTED;
  return hardware_interface::return_type::OK;
}

hardware_interface::return_type RobokitHardware::stop()
{
  RCLCPP_INFO(rclcpp::get_logger("RobokitHardware"), "HWI stopped");
  status_ = hardware_interface::status::STOPPED;
  return hardware_interface::return_type::OK;
}

hardware_interface::return_type RobokitHardware::read()
{
  RCLCPP_INFO(rclcpp::get_logger("RobokitHardware"), "Reading...");
  return hardware_interface::return_type::OK;
}

hardware_interface::return_type RobokitHardware::write()
{
  RCLCPP_INFO(rclcpp::get_logger("RobokitHardware"), "Writing...");
  return hardware_interface::return_type::OK;
}

}

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(
  robokit_hardware::RobokitHardware, hardware_interface::SystemInterface)
