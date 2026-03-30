# Module name = folder name
get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Collect all source files
file(GLOB_RECURSE MODULE_SOURCES
    CONFIGURE_DEPENDS
    *.c
    *.cpp
    *.h
)
# Create library
add_library(${MODULE_NAME} ${MODULE_SOURCES})
target_link_libraries(${MODULE_NAME} PUBLIC stm32cubemx
    
)

# Allow this module to include headers from ENTIRE project
target_include_directories(${MODULE_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Lib/ThirdParty
    ${CMAKE_SOURCE_DIR}/Lib/MyLib
)