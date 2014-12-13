# we've got our own module to find STK
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${zig_SOURCE_DIR})

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)
#find_package(SDL2 REQUIRED)
#find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(STK REQUIRED)


if(APPLE)
    #  find_library(CORE_FOUNDATION NAMES CoreFoundation)
    find_library(COCOA NAMES Cocoa)
    set(PLATFORM_LIBS ${COCOA})
    set(PLATFORM_SRCS osx_glue.mm)
endif()


#set( DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/games/zig )
#configure_file( config.h.in config.h )

include_directories(${OPENGL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS} ${STK_INCLUDE_DIR})

# actually probably a few unused .cpp files we should zap, but hey.
file(GLOB SRCS *.cpp)
set( SRCS ${SRCS} ${PLATFORM_SRCS} retromat/retromat.cpp retromat/sidstyle.cpp )

add_executable(zig ${SRCS})
target_link_libraries( zig ${OPENGL_LIBRARY} ${ZLIB_LIBRARY} ${PNG_LIBRARY} ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${STK_LIBRARY} ${PLATFORM_LIBS})


#if( UNIX )
    # TODO: don't want this on OSX if we're building a bundle...
    #install( TARGETS zig RUNTIME DESTINATION bin )
    #file( GLOB DATAFILES data/* )
    #install( FILES ${DATAFILES} DESTINATION ${DATA_PATH} )
#endif( UNIX )

