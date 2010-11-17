

#ifndef BACKGROUND_H
#define BACKGROUND_H


class Texture;
class Image;
class vec2;

class Background
{
public:
	Background()			{}
	virtual ~Background()	{}
	virtual void Draw( vec2 const& pos, float orient ) = 0;
};



class OriginalBackground : public Background
{
public:
	OriginalBackground();
	~OriginalBackground();
	void Draw( vec2 const& pos, float orient );

private:
	Texture*	m_Texture;
	Texture*	m_TextureToo;	// for multitexture
};



class RetroBackground : public Background
{
public:
	RetroBackground();
	~RetroBackground();
	void Draw( vec2 const& pos, float orient );

private:
	Texture*	m_ForeTexture;
	Texture*	m_BackTexture;
};


#endif	// BACKGROUND_H


