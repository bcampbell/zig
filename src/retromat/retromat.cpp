#include "retromat.h"
#include "sidstyle.h"

#include "Stk.h"
using namespace stk;

void SetRetromatParams( int samplerate, std::string const& rawwavepath )
{
	Stk::setSampleRate( samplerate );
	Stk::setRawwavePath( rawwavepath );
}



void GenerateLaser( std::vector<float>& out )
{
	SIDStyle::Config sidconf =
	{
		0.2f,						// duration
		SINE, 440.0f, 10.0f,		// src: type, startf, endf
		SQUARE, 0.0f, 0.0f, 0.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.01f, 0.7f, 0.1f,	// adsr
		0.0f, 0.95f,				// filter: startpole, endpole
		0.3f						// gain
	};

	SIDStyle sid( sidconf );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}


void GenerateBaiterAlert( std::vector<float>& out )
{
	SIDStyle::Config sidconf =
	{
		1.0f,						// duration, gain
		TRIANGLE, 440.0f, 2500.0f,		// src: type, startf, endf
		SINE, 5.0f, 100.0f, 1000.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.0001f, 1.0f, 0.1f,	// adsr
		0.95f, -0.95f,				// filter: startpole, endpole
		0.3f						// gain
	};

	SIDStyle sid( sidconf );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}



void GeneratePlayerToast( std::vector<float>& out )
{
	SIDStyle::Config sidconf =
	{
		1.0f,						// duration, gain
		SQUARE, 1.0f, 50.0f,		// src: type, startf, endf
		SINE, 500.0f, 1.0f, 1000.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.0001f, 1.0f, 0.1f,	// adsr
		-0.9f, 0.9f,				// filter: startpole, endpole
		0.3f						// gain
	};

	SIDStyle sid( sidconf );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}

void GenerateLevelIntro( std::vector<float>& out )
{
	SIDStyle::Config sidconf =
	{
		2.0f,						// duration, gain
		SQUARE, 8000.0f, -1000.0f,		// src: type, startf, endf
		SAWTOOTH, 2000.0f, 1.0f, 1000.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.2f, 0.7f, 1.0f,	// adsr
		0.9f, 0.9f,				// filter: startpole, endpole
		1.0f						// gain
	};

	SIDStyle sid( sidconf );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}

void GenerateBigExplosion( std::vector<float>& out )
{
	SIDStyle::Config sidconf =
	{
		2.0f,						// duration, gain
		SQUARE, 100.0f, 10.0f,		// src: type, startf, endf
		SAWTOOTH, 12000.0f, 10.0f, 1000.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.2f, 0.7f, 1.0f,	// adsr
		0.9f, 0.9f,				// filter: startpole, endpole
		1.0f						// gain
	};

	SIDStyle sid( sidconf );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}

void GenerateGameOver( std::vector<float>& out )
{
	SIDStyle::Config sidconf2 =
	{
		3.0f,						// duration, gain
		SQUARE, 80.0f, 1.0f,		// src: type, startf, endf
		SQUARE, 5.0f, 0.0f, 20.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.2f, 0.7f, 0.1f,	// adsr
		0.9f, -0.9f,				// filter: startpole, endpole
		1.0f						// gain
	};

	SIDStyle sid( sidconf2 );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}




void GenerateDullBlast( std::vector<float>& out )
{
	SIDStyle::Config sidconf =
	{
		0.15f,						// duration
		SQUARE, 200.0f, 1.0f,			// src: type, startf, endf
		SINE, 	10000.0f, 10.0f, 10.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.01f, 0.7f, 0.05f,	// adsr
		0.95f, 0.95f,				// filter: startpole, endpole
		0.3f						// gain
	};

	SIDStyle sid( sidconf );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}


void GenerateWibblePop( std::vector<float>& out )
{
	SIDStyle::Config sidconf =
	{
		0.7f,						// duration
		TRIANGLE, 220.0f, 2840.0f,			// src: type, startf, endf
		SQUARE, 20.0f, 1.0f, 440.0f,	// mod: type, startf, endf, ampl
		0.1f, 0.1f, 1.0f, 0.3f,	// adsr
		0.95f, -0.95f,				// filter: startpole, endpole
		0.7f						// gain
	};

	SIDStyle sid( sidconf );
	while( !sid.IsDone() )
		out.push_back( sid.Tick() );
}



