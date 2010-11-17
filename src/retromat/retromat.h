#ifndef RETROMAT_H
#define RETROMAT_H


#include <vector>
#include <string>

// must be called before generating effects. can be called multiple times.
void SetRetromatParams( int samplerate, std::string const& rawwavepath );

void GenerateLaser( std::vector<float>& out );
void GenerateBaiterAlert( std::vector<float>& out );
void GeneratePlayerToast( std::vector<float>& out );
void GenerateLevelIntro( std::vector<float>& out );
void GenerateBigExplosion( std::vector<float>& out );
void GenerateGameOver( std::vector<float>& out );
void GenerateDullBlast( std::vector<float>& out );
void GenerateWibblePop( std::vector<float>& out );

typedef void (*RetromatFn)( std::vector<float>& out );

#endif // RETROMAT_H

