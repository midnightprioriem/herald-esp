/**
 * @file HT16K33ClockDisplay.cpp
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "HT16K33ClockDisplay.hpp"

#include "esp_log.h"

static auto constexpr TAG = "HT16K33";

namespace Clocks {
HT16K33ClockDisplay::HT16K33ClockDisplay(I2C::I2CBus& aDisplayBus)
    : mDisplayBus(aDisplayBus) {
  /* TODO add error handling here, throw exception if communication with HT16K33
   * fails */

  mDisplayBus.Write({Command::OscillatorOn});  // Clock oscillator on
  mDisplayBus.Write({Command::DisplayOn});     // Clock display on
  mDisplayBus.Write(
      {Command::DisplayBrightness & 0xF});  // Clock brightness full
  ClearDisplay();
}

void HT16K33ClockDisplay::SetTime(const time_t aTime) {
  const auto tm = localtime(&aTime);
  auto hours = tm->tm_hour % 12;  // TODO read preference for 12 or 24 hours
  auto mins = tm->tm_min;

  SetDigit(DispPos::Two, hours % 10);
  hours /= 10;
  if (hours == 0) {
    SetCharacter(DispPos::One, SpecialChar::ClearChar);
  } else {
    SetDigit(DispPos::One, hours);
  }
  SetDigit(DispPos::Four, mins % 10);
  mins /= 10;
  SetDigit(DispPos::Three, mins);
  SetCharacter(DispPos::Colon, SpecialChar::ColonChar);
}

void HT16K33ClockDisplay::ClearDisplay() {
  SetCharacter(DispPos::One, SpecialChar::ClearChar);
  SetCharacter(DispPos::Two, SpecialChar::ClearChar);
  SetCharacter(DispPos::Colon, SpecialChar::ClearChar);
  SetCharacter(DispPos::Three, SpecialChar::ClearChar);
  SetCharacter(DispPos::Four, SpecialChar::ClearChar);
}

void HT16K33ClockDisplay::SetCharacter(DispPos aPos, uint8_t aCharacter) {
  if (mCache[aPos / 2] == aCharacter) {
    return;
  }
  ESP_LOGI(TAG, "Writing 0x%X to position 0x%X", aCharacter, aPos);
  mDisplayBus.Write({aPos, aCharacter});
  mCache[aPos / 2] = aCharacter;
}

void HT16K33ClockDisplay::SetDigit(DispPos aPos, uint8_t aNum) {
  auto charData = NumericChar[aNum];
  if (aPos == DispPos::Colon || aNum > sizeof(NumericChar) ||
      mCache[aPos] == charData) {
    return;
  }
  SetCharacter(aPos, charData);
}
}  // namespace Clocks