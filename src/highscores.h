

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <string>
#include <assert.h>

class HighScores
{
public:
	// ctor loads scores
	HighScores();

	// dtor saves scores
	~HighScores();

	// access the scores
	int NumScores() const;
	int Score( int idx ) const;
	std::string Name( int idx ) const;

	// returns index of place in score table, or -1 if the score isn't high
	// enough to make it onto the table.
	int Submit( int score, int level );

	void SetName( int idx, std::string const& name );

	// max number of characters in name
	enum {MAX_NAME_SIZE=16};

	// merge in another highscore file
	void Merge( std::string const& filename );

private:
	bool Load();
	void Save();

	
	enum { TABLE_SIZE=10 };
	struct Entry
	{
		int Score;
        int Level;
		std::string Name;
	};

	Entry m_Scores[ TABLE_SIZE ];
};







inline int HighScores::NumScores() const
{
	return TABLE_SIZE;
}

inline int HighScores::Score( int idx ) const
{
	assert( idx >=0 && idx < TABLE_SIZE );
	return m_Scores[idx].Score;
}

inline std::string HighScores::Name( int idx ) const
{
	assert( idx >=0 && idx < TABLE_SIZE );
	return m_Scores[idx].Name;
}


inline void HighScores::SetName( int idx, std::string const& name )
{
	assert( idx >=0 && idx < TABLE_SIZE );
	m_Scores[idx].Name = name;	
}

#endif	// HIGHSCORES_H



