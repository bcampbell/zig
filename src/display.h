#ifndef DISPLAY_H
#define DISPLAY_H

struct SDL_Surface;
struct ZigConfig;

#include <string>
#include <vector>
#include <list>


class Texture;

struct Res
{
	Res( int width, int height ) : w(width),h(height) {}
	bool operator==( Res const& other ) const
		{ return other.w==w && other.h==h; }
	bool operator<( Res const& other ) const
		{ return w<other.w || h<other.h; }
	int w; int h;
};


class Display
{
public:
	Display( Res res, int depth, bool fullscreen );
	virtual ~Display();

	void ChangeSettings( Res res, int depth, bool fullscreen );

	std::vector<Res> const& Resolutions() const
		{ return m_Resolutions; }
	int MatchRes( Res const& r ) const;

	Res CurrentRes() const;
	int CurrentDepth() const;
	bool IsFullscreen() const;

	void AddTexture( Texture* t );
	void RemoveTexture( Texture* t);	// OK to pass in null

	void TakeScreenshot();
private:
	void OpenMode( Res res, int depth, bool fullscreen );
	void CalcAvailableResolutions();
	SDL_Surface* m_Screen;
	std::vector<Res> m_Resolutions;

	std::list< Texture* > m_Textures;
};


#endif // DISPLAY_H

