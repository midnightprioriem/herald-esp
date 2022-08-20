/**
 * @file I2CBus.h
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <cstdint>
#include <vector>

namespace I2C {
class I2CBus {
 public:
  /**
   * @brief Master write a stream of data over the I2C bus
   *
   * @param aData Vector containing data to be written
   * @return true success
   * @return false error
   */
  virtual bool Write(const std::vector<uint8_t>& aData) = 0;

  /**
   * @brief Master read a stream of data into a buffer from the I2C bus
   *
   * @param aNum Number of bytes to read
   * @param aBuf Vector buffer to read the data in to
   * @return true success
   * @return false error
   */
  virtual bool Read(std::vector<uint8_t>& aBuf, size_t aNum) = 0;
};
}  // namespace I2C

#endif  // I2C_BUS_H