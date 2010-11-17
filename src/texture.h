

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL_opengl.h>


class Colour;
struct SDL_Surface;

class Texture
{
public:
	Texture()					{}
	virtual ~Texture()			{}

	virtual void UploadToGL() = 0;
	void SetID( GLuint id )			{ m_TextureID = id; }
	GLuint ID() const			{ return m_TextureID; }
private:
	GLuint m_TextureID;
};


class FileTexture : public Texture
{
public:
	FileTexture( const char* filename );
	virtual ~FileTexture();

	virtual void UploadToGL();
private:
	SDL_Surface* m_Image;
};


class CalculatedTexture : public Texture
{
public:
	CalculatedTexture( int pixw, int pixh );
	virtual void UploadToGL();
	virtual void Calc( float x, float y, Colour& out ) {}	//= 0;
private:
	int m_Width;
	int m_Height;
};




#endif // TEXTURE_H


