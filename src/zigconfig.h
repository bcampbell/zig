#ifndef ZIGCONFIG_H
#define ZIGCONFIG_H


struct ZigConfig
{
	// user settings
	int width;
	int height;
	int depth;
	bool fullscreen;
	bool nosound;

	int soundfreq;
	int soundbuffer;
	bool sounddebug;

	// dev settings
	bool nomultitexture;
	bool flatout;	// run as fast as possible! (for debugging)

	// defaults:
	ZigConfig() :
		width(800),
		height(600),
		depth(32),
		fullscreen(true),
		nosound(false),
		soundfreq( 22050 ),	// MIX_DEFAULT_FREQUENCY
		soundbuffer( 1024 ),
		sounddebug( false ),
		nomultitexture(false),
		flatout(false)
	{}

	void Init( int ac, char* av[] );

	// just save user settings, not dev ones
	void Save() const;
};




#endif // ZIGCONFIG_H


