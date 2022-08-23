set(SOURCES main.cpp 
            app_server.cpp)
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ".")
