cmake_minimum_required(VERSION 3.22)

set(AI_GENERATED_LIB_NAME AIGeneratedLib)

add_library(${AI_GENERATED_LIB_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/uart2_ring_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esp32_uart_binary_parser.c
)

target_include_directories(${AI_GENERATED_LIB_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${AI_GENERATED_LIB_NAME}
    PRIVATE
    stm32cubemx
)

set(AI_GENERATED_LINK_LIBS ${AI_GENERATED_LIB_NAME} PARENT_SCOPE)
