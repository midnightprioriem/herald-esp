/**
 * @file HT16K33ClockDisplay.hpp
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef HT16K33_CLOCK_DISPLAY_H
#define HT16K33_CLOCK_DISPLAY_H

#include <time.h>

#include "ClockDisplay.hpp"
#include "I2CBus.hpp"

namespace Clocks {

class HT16K33ClockDisplay : public ClockDisplay {
  enum Command : uint8_t {
    OscillatorOn = 0x21,
    OscillatorOf = 0x20,
    DisplayOn = 0x81,
    DisplayOff = 0x80,
    BlinkOn5Hz = 0x87,
    BlinkOn1Hz = 0x85,
    BlinkOn2Hz = 0x83,
    BlinkOff = 0x81,
    DisplayBrightness = 0xE0, /* & with lower 8 bits to set brightness */
  };

  enum DispPos : uint8_t {
    One = 0x00,
    Two = 0x02,
    Colon = 0x04,
    Three = 0x06,
    Four = 0x08,
    Num = 5,
  };

  static constexpr uint8_t DigitPos[] = {
      DispPos::One,
      DispPos::Two,
      DispPos::Three,
      DispPos::Four,
  };

  static constexpr uint8_t NumericChar[] = {
      0x3F,  // 0
      0x06,  // 1
      0x5B,  // 2
      0x4F,  // 3
      0x66,  // 4
      0x6D,  // 5
      0x7D,  // 6
      0x07,  // 7
      0x7F,  // 8
      0x6F,  // 9
  };

  enum SpecialChar : uint8_t {
    ClearChar = 0x00,
    MinusChar = 0x40,
    ColonChar = 0x02,
  };

 public:
  HT16K33ClockDisplay(I2C::I2CBus& aDisplayBus);
  ~HT16K33ClockDisplay() = default;
  /**
   * @brief Set the Clock Display time
   *
   * @param aTime the time to set the clock to
   */
  void SetTime(time_t aTime) final;

  /**
   * @brief Clear the display
   *
   */
  void ClearDisplay() final;

 private:
  void SetCharacter(DispPos aPos, uint8_t aCharacter);
  void SetDigit(DispPos aPos, uint8_t aNum);
  I2C::I2CBus& mDisplayBus;
  uint8_t mCache[DispPos::Num]{0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};
}  // namespace Clocks

#endif