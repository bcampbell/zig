#include "gen.h"
//#include "retromat.h"
#include "sidstyle.h"

void Loopum(std::vector<float>& buf, float fadeTime)
{
    int len = fadeTime * g_RFreq;

    assert(len < (int)buf.size());

    int i;
    for(i=0;i<len; ++i)
    {
        int dest = i;
        int src = (buf.size()-len) + i;
        float t = (float)i/(float)len;

        buf[dest] = buf[dest]*t + buf[src]*(1-t);
    }


    buf.resize(buf.size()-len);

}



void GenerateLaser( std::vector<float>& out )
{
/*
	SIDStyle::Config sidconf =
	{
		4.0f,						// duration
		SINE, 440.0f, 440.0f,		// src: type, startf, endf
		SQUARE, 0.0f, 0.0f, 0.0f,	// mod: type, startf, endf, ampl
		1.0f, 1.0f, 0.5f, 1.0f,	// adsr
		0.0f, 0.0f,				// filter: startpole, endpole
		1.0f						// gain
	};
    */
	SIDStyle::Config sidconf =
	{
		0.2f,						// duration
		SINE, 440.0f, 10.0f,		// src: type, startf, endf
		SQUARE, 0.0f, 0.0f, 0.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.01f, 0.7f, 0.1f,	// adsr
		0.0f, 0.95f,				// filter: startpole, endpole
		0.1f						// gain
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
    /*
	SIDStyle::Config sidconf =
	{
		2.0f,						// duration, gain
		PULSE, 100.0f, 10.0f,		// src: type, startf, endf
		SAWTOOTH, 12000.0f, 10.0f, 1000.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.2f, 0.7f, 1.0f,	// adsr
		0.9f, 0.9f,				// filter: startpole, endpole
		1.0f						// gain
	};
    */
	SIDStyle::Config sidconf =
	{
		2.0f,						// duration, gain
		SQUARE, 5000.0f, 10.0f,		// src: type, startf, endf
		SAWTOOTH, 420.0f, 1.0f, 20000.0f,	// mod: type, startf, endf, ampl
		0.0001f, 0.2f, 0.7f, 1.0f,	// adsr
		0.9f, 0.6f,				// filter: startpole, endpole
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




// looping, electric blast
void GenerateElectric( std::vector<float>& out )
{
    SineOsc lfo1(33.0f);
    SawtoothOsc lfo2(1.0f);
    SawtoothOsc lfo3(13.0f);


    SawtoothOsc w1;

    SawtoothOsc o;
    NoiseOsc noise;


    SquareOsc high;


    OnePole filt;
    float t; 
    for (t=0.0f; t<2.5f; t+=g_RStep)
    {
        float f1 = lfo1.tick();
        float f2 = lfo2.tick();
        float f3 = lfo3.tick();

        filt.setCutoff(60 + f2*f3*59 , g_RFreq);
        o.setFrequency(1000.0f + 999.0f*f1*f1*f1);
        float v = o.tick();



        w1.setFrequency(10.0f + t*t*1.0f );
        v += (w1.tick()*0.2);
        v = filt.tick(v);

        high.setFrequency( 5000 - t*t*t*50.0f );
        v += high.tick() * 0.005f;



		out.push_back( v );
    }


    Loopum(out,0.5f);

}

void GenerateChargeUp( std::vector<float>& out )
{
    TriangleOsc wave;

    SineOsc lfo(1000.1f);


    float t; 
    const float dur=3.0f;
    OnePole filt;
    for (t=0.0f; t<dur; t+=g_RStep)
    {
        //float f = t/dur;
        filt.setCutoff(800.0f + lfo.tick()*500, g_RFreq);
        
        wave.setFrequency(8000.0f + t*t*400.0f);
        float v = wave.tick()*0.2f;
        v = filt.tick(v);
        out.push_back(v);
    }
    Loopum(out,1.5f);
}

void GenerateThrust( std::vector<float>& out )
{
    NoiseOsc noise;

    SquareOsc w;
    SineOsc lfo1(220.0f);
    SineOsc lfo2(13.0f);

    OnePole filt;
    float t;
    for (t=0.0f; t<2.0f; t+=g_RStep)
    {
        float f1 = lfo1.tick();
        float f2 = lfo2.tick();
        filt.setCutoff(80.0f + f1*30.0f, g_RFreq);

        w.setFrequency(6080.0f + f2*39.0f);
        
        float v = 0.2f*w.tick();
        v += noise.tick()*0.5f;
        v = filt.tick(v);


        v *= 0.5f;
		out.push_back( v );
    }
}





