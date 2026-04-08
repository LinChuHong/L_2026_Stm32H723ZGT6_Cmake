cmake_minimum_required(VERSION 3.22)
enable_language(C CXX)

# ---------------------------------------------------------------------
# Automatically scan Hardware/* folders and create a library for each
# ---------------------------------------------------------------------

file(GLOB CHILD_DIRS RELATIVE  
    ${CMAKE_CURRENT_SOURCE_DIR}/MyLib
    ${CMAKE_CURRENT_SOURCE_DIR}/MyLib/*
)

set(MYLIB_LINK_LIBS "")

foreach(DIR ${CHILD_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/MyLib/${DIR}")
        set(LIB_NAME "${DIR}Lib")



        file(GLOB_RECURSE LIB_SOURCES
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/MyLib/${DIR}/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/MyLib/${DIR}/*.cpp
        )

        # Collect all source files in this directory
        if(LIB_SOURCES)
          
            message(STATUS "Creating user library: ${LIB_NAME}")

            # Create an OBJECT library like CubeMX style
            add_library( ${LIB_NAME}  ${LIB_SOURCES})
            # target_sources(${LIB_NAME} INTERFACE ${LIB_SOURCES})

            target_include_directories(${LIB_NAME} 
                PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/MyLib/${DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/MyLib

            )

            # Link with CubeMX settings
            target_link_libraries(${LIB_NAME} PRIVATE 
                stm32cubemx
                lvgl
                L_GUI_Guider
                NORFLASH
                FATFS
                littlefs
                L_button
            )
            





            # Add this lib to global list so parent can link later
            list(APPEND MYLIB_LINK_LIBS ${LIB_NAME})
        else()
            message(STATUS "Skipping ${LIB_NAME} (no source files) ")
        endif()

    endif()
endforeach()

# Export the library list back to the parent scope
set(MYLIB_LINK_LIBS ${MYLIB_LINK_LIBS} PARENT_SCOPE)

message(STATUS "mylib: auto-discovered libraries: ${MYLIB_LINK_LIBS}")