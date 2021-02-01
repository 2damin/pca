cmake_minimum_required(VERSION 3.10)

project("example")

if(WIN32)
    include_directories(
        ${PREBUILT_INCLUDE_DIR}
        ${OpenCV_INCLUDE_DIRS}
    )
    link_directories(
        ${PREBUILT_SHARED_LIBRARY_DIR}
        ${PREBUILT_STATIC_LIBRARY_DIR}
    )
else()
    include_directories(
        ${PREBUILT_INCLUDE_DIR}
        ${OpenCV_INCLUDE_DIRS}
    )
    link_directories(
        ${PREBUILT_SHARED_LIBRARY_DIR}
        ${PREBUILT_STATIC_LIBRARY_DIR}
    )
endif()

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/main.cpp
)

##########################################################################
#-- library targets ------------------------------------------------------

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        pca
    )
else()
    target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        pca
    )
endif()
