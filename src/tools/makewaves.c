#define LENGTH 256

#include <math.h>
#ifndef M_PI
#define M_PI (3.141592653589793)
#endif


#include <stdio.h>

static void Write( const char *filename, double* src );

int main( int argc, char* argv[] )
{
	double d[LENGTH];
	int i;

	// sine
	for( i=0; i<LENGTH; ++i )
	{
		double t = (double)i/(double)LENGTH;
		d[i] = sin( 2*M_PI * t );
	}
	Write( "sine.raw", d );

	// triangle
	for( i=0; i<LENGTH; ++i )
	{
		double t = (double)i/(double)LENGTH;

		if( t<0.25 )
			d[i] = t*4.0;
		else if( t<0.75 )
			d[i] = 1.0 - (t-0.25)*4.0;
		else // t<1.0
			d[i] = (t-0.75)*4.0 - 1.0;
	}
	Write( "triangle.raw", d );
	
	// square
	for( i=0; i<LENGTH; ++i )
	{
		double t = (double)i/(double)LENGTH;
		if( t<0.5 )
			d[i] = 1.0;
		else
			d[i] = -1.0;
	}
	Write( "square.raw", d );

	// sawtooth
	for( i=0; i<LENGTH; ++i )
	{
		double t = (double)i/(double)LENGTH;
		if( t<0.5 )
			d[i] = t*2.0;
		else
			d[i] = (t-0.5)*2.0 - 1.0;
	}
	Write( "sawtooth.raw", d );

	return 0;
}


static void Write( const char *filename, double* src )
{
	FILE *fp;
	int i;
	unsigned char outbuf[LENGTH*2];

	printf("%s...", filename );

	unsigned char* out = outbuf;
	for( i=0; i<LENGTH; ++i )
	{
		short int s = (short int)(32767.0 * src[i]);

		// output file is always be bigendian!
		*out++ = (s >> 8) & 0xFF;
		*out++ = s & 0xFF;
	}

	fp = fopen( filename, "wb" );
	fwrite( outbuf, 2, LENGTH, fp );
	fclose( fp );

	printf("done\n");
}



