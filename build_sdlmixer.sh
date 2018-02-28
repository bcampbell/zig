#/bin/bash
set -e

# grab and build sdl_mixer

SITE=~/site32
if [ ! -d "$SITE" ]; then
    echo "$SITE doesn't exist"
    exit 1
fi

cd $SITE

tarball=SDL2_mixer-2.0.2.zip
if [ ! -f $tarball ]; then
    wget https://www.libsdl.org/projects/SDL_mixer/release/$tarball
fi
unzip $tarball
cd SDL2_mixer-2.0.2

./configure \
    --prefix $SITE \
    --disable-music-cmd \
    --disable-music-wave \
    --disable-music-mod \
    --disable-music-mod-modplug \
    --disable-music-mod-modplug-shared \
    --disable-music-mod-mikmod \
    --disable-music-mod-mikmod-shared \
    --disable-music-midi \
    --disable-music-midi-timidity \
    --disable-music-midi-native \
    --disable-music-midi-fluidsynth \
    --disable-music-midi-fluidsynth-shared \
    --disable-music-ogg \
    --disable-music-ogg-tremor \
    --disable-music-ogg-shared \
    --disable-music-flac \
    --disable-music-flac-shared \
    --disable-music-mp3 \
    --disable-music-mp3-smpeg \
    --disable-music-mp3-smpeg-shared \
    --disable-smpegtest \
    --disable-music-mp3-mad-gpl \
    --disable-music-mp3-mad-gpl-dithering \
    --disable-music-mp3-mpg123 \
    --disable-music-mp3-mpg123-shared

make
make install

export PKG_CONFIG_PATH=$SITE/lib/pkgconfig:$PKG_CONFIG_PATH

