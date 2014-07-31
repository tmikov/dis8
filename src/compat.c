#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "compat.h"

long filelength ( int fd )
{
  off_t cur, len;
  if ((cur = lseek( fd, 0, SEEK_CUR )) == (off_t)-1)
    return -1;
  if ((len = lseek( fd, 0, SEEK_END )) == (off_t)-1)
    return -1;
  if (lseek( fd, cur, SEEK_SET ) == (off_t)-1)
    return -1;
  long res = (long)len;
  if (res < 0)
  {
    errno = EOVERFLOW;
    return -1;
  }
  return res;
}

char * strupr ( char * s )
{
  char * res = s;
  for ( ; *s; ++s )
    *s = toupper( *s );
  return res;
}

char * strlwr ( char * s )
{
  char * res = s;
  for ( ; *s; ++s )
    *s = tolower( *s );
  return res;
}

int strnicmp ( const char * s1, const char * s2, size_t maxlen )
{
  if (maxlen == 0)
    return 0;
  for( ; --maxlen && *s1 && *s2; ++s1, ++s2 )
  {
    int d;
    if ( (d = toupper(*s1) - toupper(*s2)) != 0)
      return d;
  }
  return toupper(*s1) - toupper(*s2);
}

char * ltoa ( long value, char * string, int radix )
{
  char buf[80], *p = buf+sizeof(buf);
  char sign = 0;
  unsigned long uval;

  if (radix < 2 || radix > 36)
  {
    errno = EINVAL;
    return NULL;
  }

  if (value < 0)
  {
    sign = '-';
    uval = -value;
  }
  else
    uval = value;

  *--p = 0;

  do
  {
    *--p = "0123456789abcdefghijklmnopqrstuvwxyz"[uval % radix];
    uval /= radix;
  }
  while (uval != 0);

  if (sign)
    *--p = sign;

  return memcpy( string, p, buf+sizeof(buf) - p );
}

unsigned short rotl16 ( unsigned short val, int count )
{
  count &= 15;
  return (val << count) | (val >> (16 - count));
}

unsigned short rotr16 ( unsigned short val, int count )
{
  count &= 15;
  return (val >> count) | (val << (16 - count));
}

#if 0
#include <stdio.h>
#include <limits.h>
int main ( void )
{
  char buf[32];

  printf( "%s\n", ltoa( 100, buf, 10 ) );
  printf( "%s\n", ltoa( 100, buf, 16 ) );
  printf( "%s\n", ltoa( -100, buf, 10 ) );
  printf( "%s\n", ltoa( -100, buf, 16 ) );
  printf( "%s\n", ltoa( 100, buf, 2 ) );
  printf( "%s\n", ltoa( 100, buf, 8 ) );
  printf( "%s\n", ltoa( 100, buf, 3 ) );
  printf( "%s\n", ltoa( 100, buf, 20 ) );
  printf( "%s\n", ltoa( LONG_MAX, buf, 10 ) );
  printf( "%s\n", ltoa( LONG_MIN, buf, 10 ) );
  printf( "%s\n", ltoa( LONG_MAX, buf, 16 ) );
  printf( "%s\n", ltoa( LONG_MIN, buf, 16 ) );
  printf( "%s\n", ltoa( LONG_MAX, buf, 2 ) );
  printf( "%s\n", ltoa( LONG_MIN, buf, 2 ) );
  printf( "%s\n", ltoa( 0, buf, 10 ) );
  return 0;
}
#endif
