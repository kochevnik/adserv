cmake_minimum_required(VERSION 2.6)
project(ADSERV C)
set(PACKAGE "adserv")
set(VERSION "0.1")
set(SYSCONF_INSTALL_DIR etc CACHE STRING "Install location for configuration files.")
set(BIN_INSTALL_DIR bin CACHE STRING "Install location for binaries.")
set(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")

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
		message(FATAL_ERROR "Unable to find lib${_LIB}!")
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
		message(FATAL_ERROR "Unable to find lib${_LIB}!")
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
		message(FATAL_ERROR "Unable to find lib${_LIB}!")
	endif (NOT ${_LIB}_FOUND)
	set(LIBMEMCACHED_LIBRARIES ${LIBMEMCACHED_LIBRARIES} ${${_LIB}_FOUND})
endforeach(_LIB)
message(STATUS "LIBMEMCACHED_LIBRARIES = '${LIBMEMCACHED_LIBRARIES}'")



#
# take care of glib2 libs
#
#foreach (_LIB glib-2.0 gthread-2.0 gobject-2.0)
foreach (_LIB glib-2.0)
	find_library(${_LIB}_FOUND ${_LIB})
	if (NOT ${_LIB}_FOUND)
		message(FATAL_ERROR "Unable to find lib${_LIB}!")
	endif (NOT ${_LIB}_FOUND)
	set(GLIB2_LIBRARIES ${GLIB2_LIBRARIES} ${${_LIB}_FOUND})
	get_filename_component(_GLIB2_LIBDIR ${${_LIB}_FOUND} PATH CACHE)
endforeach(_LIB)
message(STATUS "GLIB2_LIBRARIES = '${GLIB2_LIBRARIES}'")
#
# ...and includes (search the glibconfig.h include dir under the same root where the library is found)
#
find_path(GLIB2_INTERNAL_INCLUDE_DIR glibconfig.h PATH_SUFFIXES "glib-2.0/include" PATHS ${_GLIB2_LIBDIR})
if (NOT GLIB2_INTERNAL_INCLUDE_DIR)
	message(FATAL_ERROR "Unable to find glibconfig.h!")
endif (NOT GLIB2_INTERNAL_INCLUDE_DIR)
find_path(GLIB2_INCLUDE_DIR glib.h PATH_SUFFIXES "glib-2.0")
if (NOT GLIB2_INCLUDE_DIR)
	message(FATAL_ERROR "Unable to find glib2 include files!")
endif (NOT GLIB2_INCLUDE_DIR)
set(GLIB2_INCLUDE_DIR ${GLIB2_INCLUDE_DIR} ${GLIB2_INTERNAL_INCLUDE_DIR})
message(STATUS "GLIB2_INCLUDE_DIR = '${GLIB2_INCLUDE_DIR}'")
include_directories(${GLIB2_INCLUDE_DIR})



add_executable(adserv
	adserv.c
	adserv_db.c
	adserv_cfg.c
)
target_link_libraries(adserv
	${LIBEVENT_LIBRARIES}
	${LIBPQ_LIBRARIES}
	${LIBMEMCACHED_LIBRARIES}
	${GLIB2_LIBRARIES}
)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/adserv DESTINATION ${BIN_INSTALL_DIR})
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/dbtool.py DESTINATION ${BIN_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/adserv.conf DESTINATION ${SYSCONF_INSTALL_DIR})

