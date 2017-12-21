
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
			m_Scores[i].Level = 5 + (TABLE_SIZE - i);
			m_Scores[i].Name = "AAA";
			s -= 1000;
		}
	}
}

HighScores::~HighScores()
{
	Save();
}




bool HighScores::Load()
{
    std::string filename = g_DataPath->ResolveForRead("scores");
    if(filename.empty()) {
        return false;   // not found
    }
	std::ifstream in( filename.c_str() );

	if( !in.good() )
		return false;

    std::string buf;
    std::getline( in, buf );
    if( buf.compare(0,4,"zig1") != 0)
    {
        return false;
    }

    int idx = 0;
    while(idx<TABLE_SIZE && in.good())
    {
        std::getline( in, buf );
        if (!in.good())
        {
            break;
        }
        std::vector<std::string> parts = Split(buf,'\t');
        if( parts.size()!=3)
        {
            continue;
        }
		m_Scores[idx].Score = atoi(parts[0].c_str());
        m_Scores[idx].Level = atoi(parts[1].c_str());
		m_Scores[idx].Name = parts[2];
        ++idx;
    }

	return true;
}



void HighScores::Save()
{
    std::string filename = g_DataPath->ResolveForWrite("scores");
    if(filename.empty()) {
        return;   // no good
    }
	std::ofstream out( filename.c_str() );
	if( !out.good() )
		return;
	int i;
    out << "zig1\n";
	for( i=0; i<TABLE_SIZE; ++i )
    {
		out << m_Scores[i].Score << "\t" << m_Scores[i].Level << "\t" << m_Scores[i].Name << '\n';
    }
}


int HighScores::Submit( int score, int level )
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
		m_Scores[pos].Level = level;
		m_Scores[pos].Name = "";
	}

	return pos;
}




