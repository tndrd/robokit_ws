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

#include <cstring>
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

  hw_commands_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
  hw_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());

  std::string port = info_.hardware_parameters["port"];
  long baudrate    = stol(info_.hardware_parameters["baud"]);
  int timeout      = stoi(info_.hardware_parameters["timeout"]);

  try
  {
    serial_.open(port, baudrate, timeout);
  }
  catch(serial::IOException& exc)
  {
    RCLCPP_FATAL(rclcpp::get_logger("RobokitHardware"), "Serial port open failed: %s", exc.what());
    return hardware_interface::return_type::ERROR;
  }

  std::cout << "Configuring..." << std::endl;

  for (const hardware_interface::ComponentInfo & joint : info_.joints)
  {
        
    int id   = 1;//std::stoi(joint.parameters.at("id"));
    int sio  = 1;//std::stoi(joint.parameters.at("sio"));

    std::cout << "Found joint " << joint.name << " (" << id << "@" << sio << ")" << std::endl;
    
    Rcb4BaseClass::ServoData servo;
    servo.Id   = id;
    servo.Sio  = sio;
    servo.Data = rad2encoder(0);
    
    servos_.push_back(servo);
  }

  std::cout << "HWI configured" << std::endl;

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
    std::cout << "Configured position state interface for joint " << info_.joints[i].name << std::endl;
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
    std::cout << "Configured position command interface for joint " << info_.joints[i].name << std::endl;
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
  return hardware_interface::return_type::OK;
}

hardware_interface::return_type RobokitHardware::write()
{
  for (size_t i = 0; i < servos_.size(); i++)
  {
    servos_[i].Data = rad2encoder(hw_commands_[i]);
    dump_servo(i);
  }

  serial_.setServoPos(servos_.data(), servos_.size(), 1);
  return hardware_interface::return_type::OK;
}

int RobokitHardware::rad2encoder(const double rad)
{
  return rad * 1698 + 7500;
}

double RobokitHardware::encoder2rad(const int enc)
{
  return (double(enc) - 7500.0) / 1698;
}

void RobokitHardware::dump_servo(const size_t index)
{ 

  Rcb4BaseClass::ServoData servo = servos_[index];

  int    enc = servo.Data;
  double rad = encoder2rad(enc);
  double deg = rad * (180 / 3.14);

  printf("%s (%d@%d): { deg: %lf | rad: %lf | enc: %d }\n", info_.joints[index].name.c_str(),
    servo.Id, servo.Sio, deg, rad, enc); 
}

}

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(
  robokit_hardware::RobokitHardware, hardware_interface::SystemInterface)
