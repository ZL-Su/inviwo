#--------------------------------------------------------------------
# Create python module
set(HEADER_FILES
    include/inviwopy/inviwopy.h
    include/inviwopy/pyglmtypes.h
    include/inviwopy/pyport.h
    include/inviwopy/pyprocessors.h
    include/inviwopy/pyproperties.h
    include/inviwopy/vectoridentifierwrapper.h
    include/inviwopy/pybuffer.h
    include/inviwopy/pydataformat.h
    include/inviwopy/pydatamapper.h
    include/inviwopy/pyimage.h
    include/inviwopy/pyinviwoapplication.h
    include/inviwopy/pyinviwomodule.h
    include/inviwopy/pymesh.h
    include/inviwopy/pynetwork.h
    include/inviwopy/pypickingmapper.h
    include/inviwopy/pypropertyowner.h
    include/inviwopy/pytfprimitiveset.h
    include/inviwopy/pyvolume.h
)
ivw_group("Header Files" BASE ${CMAKE_CURRENT_SOURCE_DIR}/include/inviwopy ${HEADER_FILES})

set(SOURCE_FILES
    src/inviwopy.cpp
    src/pybuffer.cpp
    src/pydataformat.cpp
    src/pydatamapper.cpp
    src/pyglmtypes.cpp
    src/pyimage.cpp
    src/pyinviwoapplication.cpp
    src/pyinviwomodule.cpp
    src/pymesh.cpp
    src/pynetwork.cpp
    src/pypickingmapper.cpp
    src/pyport.cpp
    src/pyprocessors.cpp
    src/pyproperties.cpp
    src/pypropertyowner.cpp
    src/pytfprimitiveset.cpp
    src/pyvolume.cpp
    src/vectoridentifierwrapper.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

ivw_add_py_wrapper(inviwopy ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(inviwopy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

ivw_default_install_comp_targets(python inviwopy)
