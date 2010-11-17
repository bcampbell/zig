

#ifndef LEVELDEF_H
#define LEVELDEF_H

#include <string>
#include <vector>
#include <fstream>

class LevelDef;
class LevelParser;


//-------------------------------
// level definition class
class LevelDef
{
public:
	LevelDef() :
		m_UFOProbability(0.0f),
		m_ArenaRadius(300.0f),
		m_BaiterStart(16.0f),
		m_BaiterInterval(8.0f),
		m_Retro(false)
	{}
		
	friend class LevelParser;

	struct SpawnEntry
	{
		std::string dudetype;
		int	quantity;
	};

	int	NumSpawnEntries() const;
	SpawnEntry const& GetSpawnEntry( int idx ) const;

	// assorted level settings
	float m_UFOProbability;
	float m_ArenaRadius;
	float m_BaiterStart;	// time until 1st baiter
	float m_BaiterInterval;	// time between baiters
	bool m_Retro;
protected:
	void AddSpawnEntry( std::string const& type, int quant );

private:
	std::vector< SpawnEntry > m_SpawnEntries;
};



inline int LevelDef::NumSpawnEntries() const
{
	return (int)m_SpawnEntries.size();
}

inline LevelDef::SpawnEntry const& LevelDef::GetSpawnEntry( int idx ) const
{
	return m_SpawnEntries[idx];
}

inline void LevelDef::AddSpawnEntry( std::string const& type, int quant )
{
	m_SpawnEntries.push_back( SpawnEntry() );
	m_SpawnEntries.back().dudetype=type;
	m_SpawnEntries.back().quantity=quant;
}


//-------------------------------
// for reading level definitions from a file.

class LevelParser
{
public:
	LevelParser( std::string const& filename,
		std::vector<LevelDef>& defs );
private:
	std::string const&	m_Filename;
	int					m_LineNum;
	std::ifstream		m_Stream;
	std::vector< LevelDef >&	m_Defs;

	void SplitLine( std::string const& line,
		std::vector<std::string>& args );
	void Process( std::vector<std::string> const& args );
	void BailOut( const char* reason );
};



#endif // LEVELDEF_H


