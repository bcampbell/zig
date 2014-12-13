#ifndef OSX_GLUE_H
#define OSX_GLUE_H

// where we'll plonk per-user stuff (highscores, options etc)
extern bool osx_get_app_support_path( char * buffer, int size );

// bundle resource path
extern bool osx_get_resource_path( char * buffer, int size );

#endif // OSX_GLUE_H
