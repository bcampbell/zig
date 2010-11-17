OSX notes
---------

SDL.framework in /Library/Frameworks/
SDL_mixer.framework /Library/Frameworks/

unpack libpng
cp scripts/makefile.darwin Makefile
edit makefile to set ZLIBLIB and ZLIBINC to /usr/lib and /usr/include
make
sudo make install


