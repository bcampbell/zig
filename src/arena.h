#ifndef ARENA_H
#define ARENA_H

class Texture;

class Arena
{
public:
	Arena( float initialradius );
	~Arena();
	void Tick();
	void Draw();

	float Radius() const
		{ return m_Radius; }

	void SetRadius( float r )
		{ m_Radius = r; }
private:
	Arena();
	enum { SEGMENTS=32*3 };
	float m_Radius;

	Texture* m_Glow;
};


#endif // ARENA_H


