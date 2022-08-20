set(SOURCES src/SystemClock.cpp
            src/HT16K33ClockDisplay.cpp)
            
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS include
                    REQUIRES Events Peripherals)