get_filename_component(BUTTON ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(BUTTON_SOURCE
    button/multi_button.c
    L_Buttons.cpp
)

add_library(${BUTTON}  ${BUTTON_SOURCE})

target_link_libraries(${BUTTON} PRIVATE
   stm32cubemx 
)

target_include_directories(${BUTTON} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
