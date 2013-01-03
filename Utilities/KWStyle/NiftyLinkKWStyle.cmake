#/*================================================================================
#
#  NiftyLink:  A software library to facilitate communication over OpenIGTLink. 
#  
#  Copyright (c) University College London (UCL). All rights reserved.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  
#
#  See LICENSE.txt in the top level directory for details. 
#
#=================================================================================*/

OPTION(NIFTYLINK_USE_KWSTYLE "Enable the use of KWStyle for checking coding style." OFF)
IF(NIFTYLINK_USE_KWSTYLE)

# Set the required KWStyle version
SET(KWSTYLE_REQ_MAJOR 1)
SET(KWSTYLE_REQ_MINOR 0)
SET(KWSTYLE_REQ_PATCH 1)

OPTION(KWSTYLE_USE_VIM_FORMAT "Set KWStyle to generate errors with a VIM-compatible format." OFF)
OPTION(KWSTYLE_USE_MSVC_FORMAT "Set KWStyle to generate errors with a VisualStudio-compatible format." OFF)

FIND_PROGRAM(KWSTYLE_EXECUTABLE
NAMES KWStyle 
PATHS
/usr/local/bin
)

IF(KWSTYLE_EXECUTABLE)
  
  EXECUTE_PROCESS(
  COMMAND ${KWSTYLE_EXECUTABLE} -version
  OUTPUT_VARIABLE KWSTYLE_VERSION_TEXT
  ) 

string(STRIP ${KWSTYLE_VERSION_TEXT} KWSTYLE_VERSION_TEXT)

IF(KWSTYLE_VERSION_TEXT STREQUAL "Version: Not defined")
  MESSAGE("This project requires a newer version of KWStyle. Please upgrade the KWStyle executable.")
ELSE(${KWSTYLE_VERSION_TEXT} STREQUAL "Version: Not defined")

  string(LENGTH ${KWSTYLE_VERSION_TEXT} KWSTYLE_VERSION_LENGTH)
  math(EXPR KWSTYLE_VERSION_FINAL_LENGTH "${KWSTYLE_VERSION_LENGTH}-9")
  string(SUBSTRING ${KWSTYLE_VERSION_TEXT} 9 ${KWSTYLE_VERSION_FINAL_LENGTH} KWSTYLE_VERSION)

  # now parse the parts of the user given version string into variables
  STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+" "\\1" KWSTYLE_MAJOR_VERSION "${KWSTYLE_VERSION}")
  STRING(REGEX REPLACE "^[0-9]+\\.([0-9])+\\.[0-9]+" "\\1" KWSTYLE_MINOR_VERSION "${KWSTYLE_VERSION}")
  STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" KWSTYLE_PATCH_VERSION "${KWSTYLE_VERSION}")

  MATH(EXPR KWSTYLE_REQ_VERSION "${KWSTYLE_REQ_MAJOR}*10000 + ${KWSTYLE_REQ_MINOR}*100 + ${KWSTYLE_REQ_PATCH}")
  MATH(EXPR KWSTYLE_LONG_VERSION "${KWSTYLE_MAJOR_VERSION}*10000 + ${KWSTYLE_MINOR_VERSION}*100 + ${KWSTYLE_PATCH_VERSION}")
  
  # Set the minimum require version for batchmake
  IF(KWSTYLE_LONG_VERSION LESS KWSTYLE_REQ_VERSION)
    MESSAGE(FATAL_ERROR "This project requires a newer version of KWStyle. Please upgrade the KWStyle executable.")
  ELSE(KWSTYLE_LONG_VERSION LESS KWSTYLE_REQ_VERSION)
    SET(KWSTYLE_FOUND 1)
  ENDIF(KWSTYLE_LONG_VERSION LESS KWSTYLE_REQ_VERSION)
ENDIF(KWSTYLE_VERSION_TEXT STREQUAL "Version: Not defined")

IF(KWSTYLE_FOUND)
#
#  Define file names
#
SET(KWSTYLE_CONFIGURATION_FILE 
  ${PROJECT_BINARY_DIR}/Utilities/KWStyle/NiftyLinkKWS.xml)

SET(KWSTYLE_NIFTYLINK_FILES_LIST
  ${PROJECT_BINARY_DIR}/Utilities/KWStyle/NiftyLinkFiles.txt)

SET(KWSTYLE_NIFTYLINK_OVERWRITE_FILE
  ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/NiftyLinkOverwrite.txt )

#
# Configure the files
#
CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/NiftyLinkFiles.txt.in
  ${KWSTYLE_NIFTYLINK_FILES_LIST})

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/NiftyLinkKWS.xml.in
  ${KWSTYLE_CONFIGURATION_FILE})


#
#  Define formatting for error messages
#
SET(KWSTYLE_EDITOR_FORMAT " ")
SET(KWSTYLE_EDITOR_FORMAT "")

IF(${CMAKE_CXX_COMPILER} MATCHES "cl.exe$")
  SET(KWSTYLE_USE_MSVC_FORMAT 1)
ENDIF(${CMAKE_CXX_COMPILER} MATCHES "cl.exe$")

IF(${CMAKE_C_COMPILER} MATCHES "g[cx][cx]$")
  SET(KWSTYLE_USE_VIM_FORMAT 1)
ENDIF(${CMAKE_C_COMPILER} MATCHES "g[cx][cx]$")

IF(KWSTYLE_USE_VIM_FORMAT)
  SET(KWSTYLE_EDITOR_FORMAT -vim)
ENDIF(KWSTYLE_USE_VIM_FORMAT)

IF(KWSTYLE_USE_MSVC_FORMAT)
  SET(KWSTYLE_EDITOR_FORMAT -msvc)
ENDIF(KWSTYLE_USE_MSVC_FORMAT)

SET(KWSTYLE_ARGUMENTS_CODE
  -xml ${KWSTYLE_CONFIGURATION_FILE} -v -D ${KWSTYLE_NIFTYLINK_FILES_LIST}
  -o ${KWSTYLE_NIFTYLINK_OVERWRITE_FILE} ${KWSTYLE_EDITOR_FORMAT}
  )

ADD_CUSTOM_COMMAND(
  OUTPUT  ${NIFTYLINK_BINARY_DIR}/KWStyleCodeReport.txt
  COMMAND ${KWSTYLE_EXECUTABLE}
  ARGS    ${KWSTYLE_ARGUMENTS_CODE}
  COMMENT "Coding Style Checker"
  )

ADD_CUSTOM_TARGET(StyleCheckCode DEPENDS ${NIFTYLINK_BINARY_DIR}/KWStyleCodeReport.txt)

ADD_TEST(KWStyleCodeTest   ${KWSTYLE_EXECUTABLE} ${KWSTYLE_ARGUMENTS_CODE})

ENDIF(KWSTYLE_FOUND)
ENDIF(KWSTYLE_EXECUTABLE)
ENDIF(NIFTYLINK_USE_KWSTYLE)
