set(CMAKE_INCLUDE_CURRENT_DIR ON)

# we've got our own module to find STK
set(CMAKE_MODULE_PATH ${zig_SOURCE_DIR})

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(STK REQUIRED)

if( UNIX )
    # don't want this on OSX if we're building a bundle...
    set( DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/games/zig )
else()
    # just load data from "./data"
    set( DATA_PATH data )
endif( UNIX)

configure_file( config.h.in config.h )

include_directories(${OPENGL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR} ${STK_INCLUDE_DIR})

# actually probably a few unused .cpp files we should zap, but hey.
file(GLOB SRCS *.cpp)
set( SRCS ${SRCS} retromat/retromat.cpp retromat/sidstyle.cpp )

add_executable(zig ${SRCS})
target_link_libraries( zig ${OPENGL_LIBRARY} ${ZLIB_LIBRARY} ${PNG_LIBRARY} ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${STK_LIBRARY})

if( UNIX )
    # TODO: don't want this on OSX if we're building a bundle...
    install( TARGETS zig RUNTIME DESTINATION bin )
    file( GLOB DATAFILES data/* )
    install( FILES ${DATAFILES} DESTINATION ${DATA_PATH} )
endif( UNIX )

