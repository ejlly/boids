set(GLM_VERSION "@GLM_VERSION@")

@PACKAGE_INIT@

set_and_check(GLM_INCLUDE_DIRS "@PACKAGE_CMAKE_INSTALL_INCLUDEDIR@")

if (NOT CMAKE_VERSION VERSION_LESS "3.0")
    include("${CMAKE_CURRENT_LIST_DIR}/glmTargets.cmake")
endif()
