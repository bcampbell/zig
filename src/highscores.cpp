
#include "highscores.h"
#include "paths.h"
#include "util.h"
#include "zig.h"

#include <cstdio>
#include <fstream>

HighScores::HighScores()
{
	if( !Load() )
	{
		int i;
		int s=10000;
		for( i=0; i<TABLE_SIZE; ++i )
		{
			m_Scores[i].Score = s;
			m_Scores[i].Name = "ABCD";
			s -= 1000;
		}
	}
}

HighScores::~HighScores()
{
	Save();
}

void HighScores::Merge( std::string const& filename )
{
	std::ifstream in( filename.c_str() );

	if( !in.good() )
		return;			// throw?

	int i;
	for( i=0; i<TABLE_SIZE; ++i )
	{
		std::string name;
		int score;
		in >> score;
		in.get();	// skip space
		std::getline( in, name );

		int idx = Submit( score );
		if( idx != -1 )
		{
			SetName( idx, name );
		}
	}

	return;
}




bool HighScores::Load()
{
    std::string filename = g_DataPath->ResolveForRead("highscores");
    if(filename.empty()) {
        return false;   // not found
    }
	std::ifstream in( filename.c_str() );

	if( !in.good() )
		return false;

	int i;
	for( i=0; i<TABLE_SIZE; ++i )
	{
		in >> m_Scores[i].Score;
		in.get();							// skip space
		std::getline( in, m_Scores[i].Name );	// rest of line is name
	}

	return true;
}



void HighScores::Save()
{
    std::string filename = g_DataPath->ResolveForWrite("highscores");
    if(filename.empty()) {
        return;   // no good
    }
	std::ofstream out( filename.c_str() );
	if( !out.good() )
		return;
	int i;
	for( i=0; i<TABLE_SIZE; ++i )
		out << m_Scores[i].Score << " " << m_Scores[i].Name << '\n';
}


int HighScores::Submit( int score )
{
	int pos = -1;
	int i;
	for( i=TABLE_SIZE-1; i>=0; --i )
	{
		if( score > m_Scores[i].Score )
			pos=i;
	}

	if( pos != -1 )
	{
		// insert new score into table
		for( i=TABLE_SIZE-1; i>pos; --i )
			m_Scores[i] = m_Scores[i-1];
		m_Scores[pos].Score = score;
		m_Scores[pos].Name = "";
	}

	return pos;
}




