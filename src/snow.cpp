/*
 * Copyright (c) 2001
 * Thaddaeus Frogley
 * codemonkey_uk@hotmail.com
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Thaddaeus Frogley makes no representations about the
 * suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
*/

#include "snow.h"
#include <stdlib.h>
#include <memory.h>

#define WIDTH 32
#define DEPTH 32
#define BMP_WIDTH 15
#define BMP_DEPTH 15
#define BMP_SIZE BMP_WIDTH*BMP_DEPTH

inline int randm(int n)
{
	if (n) return rand()%n;
	else return 0;
}

const int UniqueSnowFlake::width() const
{ 
	return WIDTH;
}

const int UniqueSnowFlake::depth() const
{
	return DEPTH;
}

UniqueSnowFlake::UniqueSnowFlake() : 
	myImage( new byte[WIDTH*DEPTH*4] ),
	myBMP( new bool[BMP_SIZE] )
{
  int done=0;
  int x,y,i,n,dx,dy,dp;
  bool *p,*p2;

  memset(myImage,0,WIDTH*DEPTH*4*sizeof(byte));
  memset(myBMP,0,BMP_SIZE*sizeof(bool));

  //seed factal
  point(BMP_WIDTH-1,BMP_DEPTH-1);
  myBMP[(BMP_SIZE-1)]=true;

  //shooting particle algorithm with 8 way reflection
  n=0;
  do{    
    y=(BMP_WIDTH-1)-randm(n);
    if (randm(2)==1){
      x=y; y=0;
      dy=1;dx=0;dp=BMP_WIDTH;
    }
    else{
      x=0;
      dy=0;dx=1;dp=1;
    }
    p=myBMP+x+BMP_WIDTH*y;
    i=0;
    do{
     p2=p+dp;
      if (*p2){

		point(x+dx,y+dy);
		point(y+dy,x+dx);

		++*p;
		++*(myBMP+y+BMP_WIDTH*x);

		point(x,y);
		point(y,x);

		if (i==0) done = 1;

        break;
      }
      x=x+dx;y=y+dy;p=p2;
    }while(++i<(BMP_WIDTH-1));
    if (n<(BMP_WIDTH-1)) n++;
  }while(!done);	
}

UniqueSnowFlake::~UniqueSnowFlake()
{
	delete[] myImage;
	delete[] myBMP;
}

void UniqueSnowFlake::pixel(int x, int y, byte r, byte g, byte b, byte a)
{
	byte* p;
	p = myImage + (x + y*WIDTH)*4;
	*p++ = r;
	*p++ = g;
	*p++ = b;
	*p++ = a;
}

void UniqueSnowFlake::point(int x, int y)
{
  int px1,py1,px2,py2;

  px1 = x+(WIDTH/2-BMP_WIDTH);
  py1 = y+(DEPTH/2-BMP_DEPTH);
  px2 = (WIDTH-1)-x-(WIDTH/2-BMP_WIDTH);
  py2 = (DEPTH-1)-y-(DEPTH/2-BMP_DEPTH);;

  pixel( px1, py1, 255, 255, 255, 255 );
  pixel( px1, py2, 255, 255, 255, 255 );
  pixel( px2, py1, 255, 255, 255, 255 );
  pixel( px2, py2, 255, 255, 255, 255 );
}
