get_filename_component(LITTLEFS ${CMAKE_CURRENT_SOURCE_DIR} NAME)


set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/lfs_util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/lfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/L_port_littlefs.c
)

add_library( ${LITTLEFS} ${SOURCE} )

target_link_libraries(${LITTLEFS} PRIVATE
    stm32cubemx
    ${MYLIB_LINK_LIBS}
    NORFLASH
)

target_include_directories(${LITTLEFS} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)