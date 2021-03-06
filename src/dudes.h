

#ifndef DUDES_H
#define DUDES_H

#include <list>
#include <vector>
#include <deque>
#include "agent.h"
#include "colours.h"
#include "soundmgr.h"
#include "snow.h"
#include "texture.h"

class Player;
class Bullet;
class ColourCycle;
class Texture;
class Dude;



typedef void (*DudeCreatorFn)( std::list<Dude*>& );

// Given the name of a dude type, GetDudeCreator() returns a function which
// will create a new instance of the dude when invoked.
DudeCreatorFn GetDudeCreator( std::string const& dudetype );



// Dude class just adds some flags and helper functionality to the basic
// Agent class.

class Dude : public Agent
{
public:
	enum
	{
		flagCanHitPlayer=1,
		flagCanHitBullet=2,
		flagIndestructible=4,	//
		flagLocksLevel=8		// can't end level until this dude dies
	};
	int Flags() const;
	void SetFlags( int flags );

	// respawn on level resume (optional override)
	virtual void Respawn();
	// collision response (optional overrides)
	virtual void OnHitPlayer( Player& player );
	virtual void OnHitBullet( Bullet& bullet );
protected:
	Dude( bool worldspace=false );

	// a standard death sequence - explosion, point assignment etc...
	void StandardDeath( Player& causeofdeath, int points, int fragcount=20 );
	void BigArseDeath( Player& causeofdeath, int points );

    // TODO: kill these!
	void StandardDeath( Bullet& b, int points, int fragcount=20 );
	void BigArseDeath( Bullet& b, int points );


private:
	int m_Flags;
};


typedef std::list< Dude* > DudeList;

inline int Dude::Flags() const
	{ return m_Flags; }

inline void Dude::SetFlags( int flags )
	{ m_Flags=flags; }


//---------------------------------------------------------------------------


class Baiter : public Dude
{
public:
	Baiter();
	Baiter( vec2 const& pos );
	virtual void Draw() { StaticDraw(); };
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
    static void StaticDraw();
private:
	vec2 m_Spd;
};



class Obstacle : public Dude
{
public:
	Obstacle();
	Obstacle( vec2 const& pos );
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Obstacle() ); }
	static void StaticDraw();
};

class Grunt : public Dude
{
public:
	Grunt();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Grunt() ); }
    static void StaticDraw(float playerProximty);
};


class Wiggler : public Dude
{
public:
	Wiggler();
	Wiggler( vec2 const& pos );
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void StaticDraw();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Wiggler() ); }
private:
	float m_fuz;
};

class Flanker : public Dude
{
public:
	Flanker();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Flanker() ); }
    static void StaticDraw(bool flanking);
private:
	enum {flank,charge} m_Mode;
	float m_ChargeDist;
	float m_ChargeSpd;
	float m_FlankSpd;
};

class WallHugger : public Dude
{
public:
	WallHugger();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void StaticDraw( float flash );
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new WallHugger() ); }
private:
    void Orient();

	int		m_Life;
    float   m_Flash;
    float   m_FireTimer;
	float	m_Angle;
	float	m_AngularSpd;
	float	m_MaxSpd;
	float	m_AngularAccel;
};


class Rock : public Dude
{
public:
	Rock();
	Rock( vec2 const& pos, int size );
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Rock() ); }
private:
	void CommonInit();
	float m_TurnRate;
	vec2 m_Spd;
	int m_Size;
	typedef std::vector<vec2> VertexList;
	VertexList m_VertexList;
};


class UFO : public Dude
{
public:
	UFO();
	virtual ~UFO();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new UFO() ); }

	static int NumInstances()
		{ return s_NumInstances; }
private:
	vec2 m_Spd;
	int m_Timer;

	static int s_NumInstances;
	static int s_Warble;
};


class UFOBullet : public Dude
{
public:
	UFOBullet( vec2 const& pos, vec2 const& spd );
	virtual void Draw();
	virtual void Tick();
	virtual void Respawn();
private:
	UFOBullet();
	vec2 m_Spd;
	int m_Timer;
};


class SnowflakeTexture : public Texture
{
public:
	virtual void UploadToGL();
private:
	UniqueSnowFlake m_Flake;
};


class Snowflake : public Dude
{
public:
	Snowflake();
	Snowflake( vec2 const& pos, vec2 const& vel );
	virtual ~Snowflake();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Snowflake() ); }
    static void StaticDraw(float size, Texture* texture);
private:
	static const float s_MinSpd;
	static const float s_MaxSpd;

	void CommonInit();

	Texture* m_Texture;
	float m_Size;
	float m_RotSpd;
	vec2 m_Vel;
};


class Missile : public Dude
{
public:
	Missile(vec2 const& pos, float heading);
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
private:
	int m_Timer;
};


class HeavyThing : public Dude
{
public:
	HeavyThing();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new HeavyThing() ); }
private:
	vec2 m_Spd;
};


class Blip : public Dude
{
public:
        Blip();
        virtual void Draw();
        virtual void Tick();
        virtual void OnHitBullet( Bullet& bullet );
        virtual void Respawn();
private:
        float m_fuz;
        int m_Tick;
};

class EvilHoverDude : public Dude
{
public:
	EvilHoverDude();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
private:
	
};

class Amoeba : public Dude
{
public:
	Amoeba();
	Amoeba( vec2 const& pos, vec2 const& spd, float radius );
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Amoeba() ); }
private:
	void CommonInit( float radius );
	
	struct AmoebaVert
	{
		vec2 Heading;
		float Phase;
	};
	
	std::vector< AmoebaVert > m_Verts;
	float m_Timer;
	vec2 m_Spd;

	static const float InitialRadius;
};


class Swarmer : public Dude
{
public:
	Swarmer();
	Swarmer( vec2 const& pos, float heading );
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Swarmer() ); }
    static void StaticDraw();
private:
    float   m_MoveTimer;
	vec2    m_Vel;

};

class ZipperMat : public Dude
{
public:
	ZipperMat();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new ZipperMat() ); }
private:
	static const int HitPoints;
	static const int NumShots;
	static const float FireTime;
	static const float ChargeTime;
	
	vec2	m_Spd;
	int		m_Life;
	float	m_Flash;
	float	m_Inflation;
	float	m_Timer;
	enum {charging,firing} m_State;
    int     m_FireCnt;
};



class Tank : public Dude
{
public:
	Tank();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Tank() ); }
    static void StaticDraw(float flash);
private:
	int m_Life;
	int m_FireTimer;
	float m_Flash;
};



class Pacman : public Dude
{
public:
	Pacman();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Pacman() ); }
private:
	void	NewDirection();
	float	m_MouthAnim;
	int		m_MoveTimer;
	vec2	m_Velocity;
};

class Invader : public Dude
{
public:
	Invader();
	virtual ~Invader();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Invader() ); }
	static void Callibrate();
private:
	int m_Type;
	int m_Timer;
	int	m_AnimToggle;

	static int s_CurrentCount;
	static int s_LevelStartCount;
};


class Snake : public Dude
{
public:
	Snake( Snake* prev );
	virtual ~Snake();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes );
private:
	int m_HitPoints;
	float m_Glow;
	Snake* m_PrevSeg;	// previous segment of snake (null for head)
	Snake* m_NextSeg;	// next segement of snake (hull for tail)

	enum { HISTORY_COUNT=8 };
	struct HistoryEntry
	{
		vec2 pos;
		float heading;
	};
	std::deque< HistoryEntry > m_History;

};

class Bomber : public Dude
{
public:
	Bomber();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Bomber() ); }
    static void StaticDraw(float flash, float pop);
private:
	static const float s_DropInterval;
	static const float s_QuietDuration;
	static const int s_DropNum;
	vec2	m_Spd;
	float	m_Timer;
	int		m_Life;
	float	m_Flash;
    float   m_Pop;
    int     m_DropCnt;
    enum {quiet,dropping} m_State;
};

class Bomb : public Dude
{
public:
	Bomb( vec2 const& pos );
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
private:
	static const float s_FuseTime;
	static const float s_ExplosionTime;
	static const float s_ExplosionR0;
	static const float s_ExplosionR1;

	Bomb();					// disable default ctor
	Bomb( Bomb const& );	// disable copy ctor
	float	m_Cyc;
	float	m_Timer;
	bool	m_Exploding;
};


class Agitator : public Dude
{
public:
	Agitator();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Agitator() ); }

    static void StaticDraw(float agitation);
    static void DrawShape( vec2 const& pos, float r );
private:
	static const float s_RadiusMin;
	static const float s_RadiusMax;
	static const float s_MaxJitter;
	static const float s_MaxSpd;
	static const float s_HitDamage;
	
	vec2	m_Vel;
	float	m_Agitation;
	float	m_Flash;
};



class Divider : public Dude
{
public:
	Divider();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void StaticDraw();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Divider() ); }
private:
	vec2 m_Spd;
	float m_RotSpd;
	float m_Timer;
	float m_HappyOrbit;
};



class Puffer : public Dude
{
public:
	Puffer();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Puffer() ); }
    static void StaticDraw(float r, float time);
    static const float s_MinArea;
    static const float s_MaxArea;
private:

    float   m_Area;
    float   m_RespiteTimer;
	vec2	m_Vel;
    float   m_Accel;
    float   m_MoveTimer;

    void UpdateRadius();
};

#endif	// DUDES_H


