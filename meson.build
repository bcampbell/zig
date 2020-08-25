project('zig', 'cpp')

srcs = ['src/agent.cpp',
    'src/agentmanager.cpp',
    'src/arena.cpp',
    'src/background.cpp',
    'src/beam.cpp',
    'src/boids.cpp',
    'src/bonuses.cpp',
    'src/boss.cpp',
    'src/bullets.cpp',
    'src/colours.cpp',
    'src/completionscreen.cpp',
    'src/controller.cpp',
    'src/display.cpp',
    'src/drawing.cpp',
    'src/dudegallery.cpp',
    'src/dudes.cpp',
    'src/edgepuller.cpp',
    'src/effects.cpp',
    'src/fatzapper.cpp',
    'src/gameover.cpp',
    'src/gamestate.cpp',
    'src/highscores.cpp',
    'src/highscorescreen.cpp',
    'src/humanoid.cpp',
    'src/icongen.cpp',
    'src/image.cpp',
    'src/level.cpp',
    'src/leveldef.cpp',
    'src/loadpng.cpp',
    'src/log.cpp',
    'src/mempool.cpp',
    'src/menusupport.cpp',
    'src/optionsmenu.cpp',
    'src/optionsscreen.cpp',
    'src/paths.cpp',
    'src/pausemenu.cpp',
    'src/perlinnoise.cpp',
    'src/player.cpp',
    'src/proceduraltextures.cpp',
    'src/scene.cpp',
    'src/sinusoidal_texture.cpp',
    'src/snowblower.cpp',
    'src/snow.cpp',
    'src/soundexplore.cpp',
    'src/soundmgr.cpp',
    'src/spiker.cpp',
    'src/texture.cpp',
    'src/titlescreen.cpp',
    'src/util.cpp',
    'src/vec2.cpp',
    'src/wav.cpp',
    'src/weapons.cpp',
    'src/wobbly.cpp',
    'src/zigconfig.cpp',
    'src/zig.cpp',
    'src/retromat/retromat.cpp',
    'src/retromat/sidstyle.cpp',
    'src/retromat/gen.cpp' ]

zdep = dependency('zlib')
pngdep = dependency('libpng')
gldep = dependency('GL')
sdldep = dependency('SDL2')
sdlmixerdep = dependency('SDL2_mixer')

# generate config.h
conf = configuration_data()
datadir = join_paths([ get_option('prefix'), get_option('datadir')])
conf.set_quoted('ZIG_DATA_DIR', datadir)
configure_file(output : 'config.h', configuration : conf)

executable('zig', srcs,
  dependencies : [zdep, pngdep, gldep, sdldep, sdlmixerdep],
  install : true)

# install everything inside 'data' at '$PREFIX/share/zig/'

datafiles = ['data/font.png', 'data/gamecontrollerdb.txt', 'data/invaders.png', 'data/levels']
install_data(datafiles, install_dir : 'share/zig')

