/**
 * @file EspI2CBus.cpp
 * @author Zach Hannum
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "EspI2CBus.hpp"

namespace I2C {
EspI2CBus::EspI2CBus(const i2c_config_t& aConf, const uint8_t aDeviceAddress,
                     const i2c_port_t aPortNum)
    : mPortNum(aPortNum), mDeviceAddress(aDeviceAddress) {
  i2c_param_config(aPortNum, &aConf);
  i2c_driver_install(aPortNum, aConf.mode, 0, 0, 0);
}

EspI2CBus::~EspI2CBus() { i2c_driver_delete(mPortNum); }

bool EspI2CBus::Write(const std::vector<uint8_t>& aData) {
  if (aData.empty()) return false;

  return i2c_master_write_to_device(mPortNum, mDeviceAddress, aData.data(),
                                    aData.size(), I2C_MASTER_TIMEOUT) == ESP_OK;
}

bool EspI2CBus::Read(std::vector<uint8_t>& aBuf, size_t aNum) {
  aBuf.clear();
  aBuf.reserve(aNum);

  return i2c_master_read_from_device(mPortNum, mDeviceAddress, aBuf.data(),
                                     aNum, I2C_MASTER_TIMEOUT);
}

}  // namespace I2C