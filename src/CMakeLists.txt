add_definitions(
    -D_WIN32_WINNT=0x0600
)


add_library(pca STATIC
    pca.cpp
    pca.hpp
)

set(pca_LIBRARIES
    CONAN_PKG::eigen
)

list(APPEND pca_LIBRARIES
    ${OpenCV_LIBS}   
)

message(${pca_LIBRARIES})
message(${pca_LIBRARIES})
message(${pca_LIBRARIES})
message(${pca_LIBRARIES})

###################################################################################################
# - version ---------------------------------------------------------------------------------------

###################################################################################################
# - library targets -------------------------------------------------------------------------------

if(NOT WIN32 AND NOT APPLE)
    target_link_libraries(pca -Wl, --start-group ${pca_LIBRARIES} -Wl, --end-group)
else()
    target_link_libraries(pca ${pca_LIBRARIES})
endif()

get_target_property(pca_LIBRARIES pca LINK_LIBRARIES)
message(STATUS ${pca_LIBRARIES})