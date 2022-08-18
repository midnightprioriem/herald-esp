set(SOURCES src/EventQueue.cpp
            src/EventDispatcher.cpp)
            
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS include)