

#ifndef WEAPONS_H
#define WEAPONS_H

class Player;

class Weapon
{
public:
	Weapon( Player& owner )	: m_Owner(owner)		{}
	virtual ~Weapon() 								{}
	virtual void Tick( bool firebutton )			{}
	virtual void Draw()								{}

	virtual bool Expired()							{ return false; }

	Player& Owner()									{ return m_Owner; }
private:
	Player&	m_Owner;
};



class SingleLaser : public Weapon
{
public:
	SingleLaser( Player& owner );
	virtual void Tick( bool firebutton );
	virtual bool Expired()	{ return false; }
private:
	int m_FireTimer;
};


class DoubleLaser : public Weapon
{
public:
	DoubleLaser( Player& owner );
	virtual void Tick( bool firebutton );
	virtual bool Expired();
private:
	int m_FireTimer;
	int m_RemainingTicks;
};


class TripleLaser : public Weapon
{
public:
	TripleLaser( Player& owner );
	virtual void Tick( bool firebutton );
	virtual bool Expired();
private:
	int m_FireTimer;
	int m_RemainingTicks;
};



class SuperNashwanPower : public Weapon
{
public:
	SuperNashwanPower( Player& owner );
	virtual void Tick( bool firebutton );
	virtual void Draw();
	virtual bool Expired();
private:
	int m_FatTimer;
	int m_FrontTimer;
	int m_RearTimer;
	int m_RemainingTicks;
};


#endif // WEAPONS_H

