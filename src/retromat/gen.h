#ifndef RETROMAT_EFFECTS_H
#define RETROMAT_EFFECTS_H

#include <vector>

void GenerateLaser( std::vector<float>& out );
void GenerateBaiterAlert( std::vector<float>& out );
void GeneratePlayerToast( std::vector<float>& out );
void GenerateLevelIntro( std::vector<float>& out );
void GenerateBigExplosion( std::vector<float>& out );
void GenerateGameOver( std::vector<float>& out );
void GenerateDullBlast( std::vector<float>& out );
void GenerateWibblePop( std::vector<float>& out );

void GenerateElectric( std::vector<float>& out );



#endif // RETROMAT_EFFECTS_H
