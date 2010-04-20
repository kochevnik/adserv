cmake_minimum_required(VERSION 2.6)
project(ADSERV C)
set(PACKAGE "adserv")
set(VERSION "0.1")
set(SYSCONF_INSTALL_DIR /etc CACHE STRING "Install location for configuration files.")
set(BIN_INSTALL_DIR bin CACHE STRING "Install location for binaries.")
set(LIB_INSTALL_DIR lib CACHE STRING "Install location for libraries.")
set(DATA_INSTALL_DIR share CACHE STRING "Install location for data files.")
set(LOCALESDIR "${DATA_INSTALL_DIR}/locale")
set(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")

#add_definitions(
#	-DPACKAGE="${PACKAGE}"
#	-DVERSION="${VERSION}"
#	-D_GNU_SOURCE=1
#)
include_directories("${ADSERV_SOURCE_DIR}")



#
# take care of libevent includes
#
if (WITH_LIBEVENT_INCLUDE)
	find_path(LIBEVENT_INCLUDE_DIR event.h PATHS ${WITH_LIBEVENT_INCLUDE} NO_DEFAULT_PATH)
else (WITH_LIBEVENT_INCLUDE)
	find_path(LIBEVENT_INCLUDE_DIR event.h)
endif (WITH_LIBEVENT_INCLUDE)
if (NOT LIBEVENT_INCLUDE_DIR)
	message(FATAL_ERROR "Unable to find libevent include files!")
endif (NOT LIBEVENT_INCLUDE_DIR)
message(STATUS "LIBEVENT_INCLUDE_DIR = '${LIBEVENT_INCLUDE_DIR}'")
include_directories(${LIBEVENT_INCLUDE_DIR})
#
# ...and libs
#
foreach(_LIB event)
	if (WITH_LIBEVENT_LIB)
		find_library(${_LIB}_FOUND ${_LIB} PATHS ${WITH_LIBEVENT_LIB} NO_DEFAULT_PATH)
	else (WITH_LIBEVENT_LIB)
		find_library(${_LIB}_FOUND ${_LIB})
	endif (WITH_LIBEVENT_LIB)
	if (NOT ${_LIB}_FOUND)
		message(FATAL_ERROR "Unable to find ${_LIB}!")
	endif (NOT ${_LIB}_FOUND)
	set(LIBEVENT_LIBRARIES ${LIBEVENT_LIBRARIES} ${${_LIB}_FOUND})
endforeach(_LIB)
message(STATUS "LIBEVENT_LIBRARIES = '${LIBEVENT_LIBRARIES}'")



#
# take care of libpq includes
#
if (WITH_LIBPQ_INCLUDE)
	find_path(LIBPQ_INCLUDE_DIR libpq-fe.h PATHS ${WITH_LIBPQ_INCLUDE} NO_DEFAULT_PATH)
else (WITH_LIBPQ_INCLUDE)
	find_path(LIBPQ_INCLUDE_DIR libpq-fe.h)
endif (WITH_LIBPQ_INCLUDE)
if (NOT LIBPQ_INCLUDE_DIR)
	message(FATAL_ERROR "Unable to find libpq include files!")
endif (NOT LIBPQ_INCLUDE_DIR)
message(STATUS "LIBPQ_INCLUDE_DIR = '${LIBPQ_INCLUDE_DIR}'")
include_directories(${LIBPQ_INCLUDE_DIR})
#
# ...and libs
#
foreach(_LIB pq)
	if (WITH_LIBPQ_LIB)
		find_library(${_LIB}_FOUND ${_LIB} PATHS ${WITH_LIBPQ_LIB} NO_DEFAULT_PATH)
	else (WITH_LIBPQ_LIB)
		find_library(${_LIB}_FOUND ${_LIB})
	endif (WITH_LIBPQ_LIB)
	if (NOT ${_LIB}_FOUND)
		message(FATAL_ERROR "Unable to find ${_LIB}!")
	endif (NOT ${_LIB}_FOUND)
	set(LIBPQ_LIBRARIES ${LIBPQ_LIBRARIES} ${${_LIB}_FOUND})
endforeach(_LIB)
message(STATUS "LIBPQ_LIBRARIES = '${LIBPQ_LIBRARIES}'")



#
# take care of libmemcached includes
#
if (WITH_LIBMEMCACHED_INCLUDE)
	find_path(LIBMEMCACHED_INCLUDE_DIR "libmemcached/memcached.h" PATHS ${WITH_LIBMEMCACHED_INCLUDE} NO_DEFAULT_PATH)
else (WITH_LIBMEMCACHED_INCLUDE)
	find_path(LIBMEMCACHED_INCLUDE_DIR "libmemcached/memcached.h")
endif (WITH_LIBMEMCACHED_INCLUDE)
if (NOT LIBMEMCACHED_INCLUDE_DIR)
	message(FATAL_ERROR "Unable to find libmemcached include files!")
endif (NOT LIBMEMCACHED_INCLUDE_DIR)
message(STATUS "LIBMEMCACHED_INCLUDE_DIR = '${LIBMEMCACHED_INCLUDE_DIR}'")
include_directories(${LIBMEMCACHED_INCLUDE_DIR})
#
# ...and libs
#
foreach(_LIB memcached)
	if (WITH_LIBMEMCACHED_LIB)
		find_library(${_LIB}_FOUND ${_LIB} PATHS ${WITH_LIBMEMCACHED_LIB} NO_DEFAULT_PATH)
	else (WITH_LIBMEMCACHED_LIB)
		find_library(${_LIB}_FOUND ${_LIB})
	endif (WITH_LIBMEMCACHED_LIB)
	if (NOT ${_LIB}_FOUND)
		message(FATAL_ERROR "Unable to find ${_LIB}!")
	endif (NOT ${_LIB}_FOUND)
	set(LIBMEMCACHED_LIBRARIES ${LIBMEMCACHED_LIBRARIES} ${${_LIB}_FOUND})
endforeach(_LIB)
message(STATUS "LIBMEMCACHED_LIBRARIES = '${LIBMEMCACHED_LIBRARIES}'")



add_executable(adserv
	adserv.c
	adserv_db.c
)
target_link_libraries(adserv
	${LIBEVENT_LIBRARIES}
	${LIBPQ_LIBRARIES}
	${LIBMEMCACHED_LIBRARIES}
)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/adserv DESTINATION ${BIN_INSTALL_DIR})
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/dbtool.py DESTINATION ${BIN_INSTALL_DIR})

