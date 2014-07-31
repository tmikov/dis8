#ifndef _COMPAT_H
#define _COMPAT_H

#include <stddef.h>

#if !__MSDOS__
#define near
#define huge
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TURBOC__
long filelength ( int fd );

char * strupr ( char * s );
char * strlwr ( char * s );

int stricmp ( const char * s1, const char * s2 );
int strnicmp ( const char * s1, const char * s2, size_t maxlen);
#define strncmpi( s1, s2, maxlen) strnicmp( s1, s2, maxlen )

char * ltoa ( long value, char * string, int radix );
#define itoa( value, string, radix ) ltoa( (int)(value), string, radix )

unsigned short rotl16 ( unsigned short val, int count );
unsigned short rotr16 ( unsigned short val, int count );

#define setmem(dest,length,value) memset(dest, (char)(value), length)

#define farmalloc(ptr)  malloc(ptr)
#define farfree(ptr)    free(ptr)

#else
#define rotl16(val,count) _rotl( val, count )
#define rotr16(val,count) _rotr( val, count )
#endif


#ifdef __cplusplus
}
#endif

#endif // _COMPAT_H
