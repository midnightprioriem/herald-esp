/**
 * @file EspI2CBus.h
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ESP_I2C_BUS_H
#define ESP_I2C_BUS_H

#include "I2CBus.hpp"
#include "driver/i2c.h"

namespace I2C {
class EspI2CBus : public I2CBus {
  static auto constexpr I2C_MASTER_TIMEOUT = 1000 / portTICK_PERIOD_MS;

 public:
  EspI2CBus(const i2c_config_t& aConf, uint8_t aDeviceAddress,
            i2c_port_t aPortNum);
  ~EspI2CBus();

  /**
   * @brief Master write a stream of data over the I2C bus
   *
   * @param aData Vector containing data to be written
   * @return true success
   * @return false error
   */
  bool Write(const std::vector<uint8_t>& aData) final;

  /**
   * @brief Master read a stream of data into a buffer from the I2C bus
   *
   * @param aNum Number of bytes to read
   * @param aBuf Vector buffer to read the data in to
   * @return true success
   * @return false error
   */
  bool Read(std::vector<uint8_t>& aBuf, size_t aNum) final;

 private:
  const i2c_port_t mPortNum;
  const uint8_t mDeviceAddress;
};
}  // namespace I2C

#endif