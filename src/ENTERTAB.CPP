#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>

#include "dis31.h"

#define NELEM( x )  (sizeof( x ) / sizeof( (x)[0] ))

#include "entertab.inc"

int FindString ( char * str )
{
  for( int i = 0; i < NumStrings; ++i )
    if (strcmp( Strings[i], str ) == 0)
      return i;
  return -1;
};

int AddString ( char * str )
{
  int i;
  if ((i = FindString( str )) != -1)
    return i;
  strcpy( Strings[NumStrings], str );
  return NumStrings++;
};

int EnterString ( void )
{
  char buf[20]/*, buf1[10]*/;

//  do
//  {
    printf( "Enter instruction name:" );
    gets( buf );
//    printf( "Is that correct? " );
//    gets( buf1 );
//  }
//  while (buf1[0] != 'y' && buf1[0] != 'Y' );
  strupr( buf );
  if (strcmp( buf, "QUIT" ) == 0)
    return -1;
  return AddString( buf );
};

static char * AddrModes[] =
{
  "AUnknown",
  "ABit",
  "ANotBit",
  "ACFlag",
  "ANone",
  "AAcc",
  "ADPTR",
  "AImmediate",
  "ARegister",
  "ARAMDirect",
  "AIndexShort",
  "AIndexFull",
  "AIndexFullOffset",
  "AIndexRelOffset",

  "AJmpIndexFullOffset",
  "AJmpPage",
  "AJmpRelative",
  "AJmpExtended",

  "AImmExtended"
};


TAddrMode EnterAddrMode ( int opNum )
{

  char buf[80];
  int i;

  for(;;)
  {
    printf( "Enter addr mode for operand %d:", opNum );
    gets( buf );
    if (buf[0] == '?')
    {
      for( i = 0; i < NELEM( AddrModes ); ++i )
        puts( AddrModes[i] );
      continue;
    }

    for( i = 0; i < NELEM( AddrModes ); ++i )
      if (stricmp( buf, AddrModes[i] ) == 0)
        return (TAddrMode)i;
    puts( "Invalid addressing mode." );
  }
};

void EnterCode ( char * code )
{
  char buf[80];
  for(;;)
  {
    printf( "Enter binary code:" );
    gets( buf );
    strupr( buf );
    if (strlen( buf ) != 8)
      continue;
    for( int i = 0; i < 8; ++i )
      if (strchr( "01X", buf[i] ) == NULL)
        i = 10;
    if (i == 8)
    {
      strcpy( code, buf );
      break;
    }
  }
};

BOOL Map ( char * code, BYTE index )
{
  for( ;*code; index <<= 1, ++code )
    if (*code != 'X' && *code - '0' != ((index & 0x80) >> 7))
      return FALSE;
  return TRUE;
};

BOOL EnterInstruction ( void )
{
  TIDesc iDesc;
  char   code[9];
  int i;

  iDesc.AM1 = iDesc.AM2 = iDesc.AM3 = ANone;
  if ((i = EnterString()) == -1)
    return FALSE;
  iDesc.String = i;
  if ((iDesc.AM1 = EnterAddrMode( 1 )) != ANone)
    if ((iDesc.AM2 = EnterAddrMode( 2 )) != ANone)
      iDesc.AM3 = EnterAddrMode( 3 );
  EnterCode( code );
  for( i = 0; i < 256; ++i )
    if (Map( code, i ))
    {
      if (IDesc[i].AM1 != AUnknown)
      {
        printf( "Error in %d\n", i );
        exit( -1 );
      }
      IDesc[i] = iDesc;
    }
  return TRUE;
};

void gen ( void )
{
  int i;
  FILE * f;
  f = fopen( "gen.inc", "wt" );
  fprintf( f, "char Strings[100][6] =\n{\n" );
  for( i = 0; i < NumStrings; ++i )
    fprintf( f, "  \"%s\"%c  // %d\n", Strings[i], i == NumStrings - 1 ? ' ' : ',',
            i );
  fputs( "};\n\n", f );
  fprintf( f, "int NumStrings = %d;\n\n", NumStrings );

  fprintf( f, "TIDesc IDesc[256] =\n{\n" );
  for( i = 0; i < 256; ++i )
  {
    fprintf( f, "  { %3d, %s, %s, %s }%c // %d\n",
             IDesc[i].String,
             AddrModes[IDesc[i].AM1],
             AddrModes[IDesc[i].AM2],
             AddrModes[IDesc[i].AM3],
             i == 255 ? ' ' : ',',
             i );
  }
  fputs( "};\n\n", f );

  fclose( f );
};

void main ( void )
{
  clrscr();
  while (EnterInstruction());
  gen();
};