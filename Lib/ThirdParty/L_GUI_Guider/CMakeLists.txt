
# Module name = folder name
get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Collect sources from custom and generated folders
file(GLOB_RECURSE CUSTOM_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/custom/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/custom/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom/*.h
)

file(GLOB_RECURSE GENERATED_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.h
)

# Combine all sources
set(ALL_SOURCES
    ${CUSTOM_SOURCES}
    ${GENERATED_SOURCES}
)


add_library(${MODULE_NAME}  ${ALL_SOURCES})

target_link_libraries(${MODULE_NAME} PUBLIC stm32cubemx
    NORFLASH
    lvgl
)
# Include directories
target_include_directories(${MODULE_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/Lib/ThirdParty/NORFLASH
    ${CMAKE_SOURCE_DIR}/Lib/ThirdParty/lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/custom
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)