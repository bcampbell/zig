// zigtypes.h
// provide basic datatypes, using ANSI C types if available.
//
#ifndef ZIGTYPES_H
#define ZIGTYPES_H

#include "config.h"

#ifdef HAVE_STDINT_H
	#include <stdint.h>
#else
	typedef signed char			int8_t;
	typedef signed short int	int16_t;
	typedef signed int			int32_t;
	typedef unsigned char		uint8_t;
	typedef unsigned short int	uint16_t;
	typedef unsigned int		uint32_t;
#endif


#endif // ZIGTYPES_H

