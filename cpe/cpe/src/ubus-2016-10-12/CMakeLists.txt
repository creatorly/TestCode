cmake_minimum_required(VERSION 2.6)
PROJECT(ubus C)

MESSAGE(STATUS "===== ubus project source dir: " ${PROJECT_SOURCE_DIR})

SET(LIBS_PATH $ENV{CPE_OPENWRT_LIBS})
MESSAGE(STATUS "cpe libs: " ${LIBS_PATH})
SET(CMAKE_INSTALL_PREFIX ${LIBS_PATH})
#SET(EXECUTABLE_OUTPUT_PATH ${LIBS_PATH}/bin/)
#SET(LIBRARY_OUTPUT_PATH ${LIBS_PATH}/lib/)

SET(NDK_STANDLONE_TOOLCHAIN /Android/toolchains/OpenWrt-SDK-15.05-ramips-mt7628_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2)
SET(CMAKE_C_COMPILER mipsel-openwrt-linux-gcc)
SET(CMAKE_CXX_COMPILER mipsel-openwrt-linux-g++)
SET(CMAKE_FIND_ROOT_PATH ${NDK_STANDLONE_TOOLCHAIN})

ADD_DEFINITIONS(-O0 -Wall -Wno-unused-function --std=gnu99 -g3 -fPIC -Wmissing-declarations --sysroot=${NDK_STANDLONE_TOOLCHAIN})

INCLUDE_DIRECTORIES(${LIBS_PATH}/include)
INCLUDE_DIRECTORIES(${LIBS_PATH}/include/json-c)
INCLUDE_DIRECTORIES(${LIBS_PATH}/include/libubox)
LINK_DIRECTORIES(${LIBS_PATH}/lib)

OPTION(BUILD_LUA "build Lua plugin" ON)
OPTION(BUILD_EXAMPLES "build examples" ON)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(UBUS_UNIX_SOCKET "/etc/cpe/ubus.sock")
SET(UBUS_MAX_MSGLEN 1048576)

ADD_DEFINITIONS( -DUBUS_UNIX_SOCKET="${UBUS_UNIX_SOCKET}")
ADD_DEFINITIONS( -DUBUS_MAX_MSGLEN=${UBUS_MAX_MSGLEN})

IF(APPLE)
  INCLUDE_DIRECTORIES(/opt/local/include)
  LINK_DIRECTORIES(/opt/local/lib)
ENDIF()

find_library(time eoutime ${CMAKE_INSTALL_PREFIX}/lib)
find_library(cpeLog eouCpeLog ${CMAKE_INSTALL_PREFIX}/lib)

IF(BUILD_STATIC)
  	FIND_LIBRARY(ubox_library ubox.a ${CMAKE_INSTALL_PREFIX}/lib)
  	FIND_LIBRARY(blob_library blobmsg_json.a ${CMAKE_INSTALL_PREFIX}/lib)
ELSE(BUILD_STATIC)
	FIND_LIBRARY(ubox_library ubox ${CMAKE_INSTALL_PREFIX}/lib)
	FIND_LIBRARY(blob_library blobmsg_json ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF(BUILD_STATIC)

FIND_PATH(ubox_include_dir libubox/usock.h)
INCLUDE_DIRECTORIES(${ubox_include_dir})

ADD_LIBRARY(ubus SHARED libubus.c libubus-io.c libubus-obj.c libubus-sub.c libubus-req.c libubus-acl.c)
TARGET_LINK_LIBRARIES(ubus ${ubox_library} ${time})

find_library(json json-c ${CMAKE_INSTALL_PREFIX}/lib)
IF(NOT json)
	MESSAGE(FATAL_ERROR "===== ubus not found json")
ENDIF(NOT json)

ADD_EXECUTABLE(ubusd ubusd.c ubusd_id.c ubusd_obj.c ubusd_proto.c ubusd_event.c ubusd_acl.c ubusd_monitor.c)
TARGET_LINK_LIBRARIES(ubusd ${ubox_library} ${blob_library} ${json} ${time} ${cpeLog} -lm -lpthread)

ADD_EXECUTABLE(cli cli.c)
SET_TARGET_PROPERTIES(cli PROPERTIES OUTPUT_NAME ubus)
TARGET_LINK_LIBRARIES(cli ubus ${ubox_library} ${blob_library} ${json} ${time} ${cpeLog} -lm -lpthread)

#ADD_SUBDIRECTORY(lua)
ADD_SUBDIRECTORY(examples)

INSTALL(TARGETS ubus cli
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
INSTALL(TARGETS ubusd
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/sbin
)

INSTALL(FILES ubusmsg.h ubus_common.h libubus.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
