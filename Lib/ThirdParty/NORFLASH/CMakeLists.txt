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
target_link_libraries(${MODULE_NAME} PRIVATE 
    stm32cubemx
    ${MYLIB_LINK_LIBS}
    lvgl
    L_GUI_Guider
    QSPI
    FATFS
)

# Allow this module to include headers from ENTIRE project
target_include_directories(${MODULE_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
