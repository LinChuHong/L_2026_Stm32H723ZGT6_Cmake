
# Module name = folder name
get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Collect sources from custom and generated folders
file(GLOB_RECURSE SOURCE
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
)

file(GLOB_RECURSE EXFUNS
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/exfuns/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/exfuns/*.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${SOURCE}
    ${EXFUNS}
)


add_library(${MODULE_NAME}  ${ALL_SOURCES})

target_link_libraries(
    ${MODULE_NAME} 
    PRIVATE 
    stm32cubemx
    ${MYLIB_LINK_LIBS}
    NORFLASH
)
# Include directories
target_include_directories(
    ${MODULE_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/exfuns
)