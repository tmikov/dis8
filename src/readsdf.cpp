/*
 Description
   Source for reading SDF file data

  History:
  -----------------------------------------------------------------------------
  09.01.1996 Cendy   Adapted old source

*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "compat.h"
#include "sr.h"

#pragma warn -pia

//******** from MAIN.CPP
extern char     * sdfFileName;
extern ADDRESS  startAddress;


extern void error(const char * mess...);

//******** from SR.CPP
extern labelList codeLabels;
extern labelList iRamLabels;
extern labelList xRamLabels;
extern void addSegment ( ADDRESS from, ADDRESS to );
extern int  addLabName ( ADDRESS a, labType t, labMemory m, labSize s,
                         char * name, int notParsed, int subNoRet );

static FILE * inf;
int    lineLen;
static char * line;
static char   lineBuf[256];
static int  curLine;


static inline void delInLine(int n)
{
  line    += n;
  lineLen -= n;
};

static void skipBlanks(void)
{
  while (isspace(*line) || *line == '-' || *line == ',' || *line == ':' ||
         *line == '.')
  {
    ++line;
    --lineLen;
  };
};

static int readLine ( void )
{
  do
  {
    printf("%d\r", curLine);
    ++curLine;
    if (!fgets(lineBuf, sizeof(lineBuf), inf))
      return 0;
    lineLen = strlen(line = lineBuf);  //strupr
    skipBlanks();
  }
  while (line[0] == 0 || line[0] == '*');
  return 1;
};

static int match ( const char * m )
{
  int l;
  skipBlanks();
  if (strncmpi(line, m, l = strlen(m)) == 0)
  {
    delInLine(l);
    return 1;
  };
  return 0;
};

static int matchSeq(const char * first ...)
{
  char * oldLine;
  va_list ap;
  va_start(ap, first);

  oldLine = line;
  do
  {
    if (!match(first))
    {
      line = oldLine;
      return 0;
    };
  }
  while (first = va_arg(ap, const char *));

  va_end(ap);
  return 1;
};

static WORD readWord(void)
{
  char buf[20];
  int  i;

  skipBlanks();
  for(i = 0;
      isdigit(line[i]) || line[i] >= 'A' && line[i] <= 'F'
      || line[i] >= 'a' && line[i] <= 'f';
      buf[i] = line[i], ++i);
  buf[i] = 0;
  if (!i)
    error("Hexadecimal digit expected in line %d", curLine);
  delInLine(i);
  return strtol(buf, 0, 16);
};

int getToken ( char * buf )
{
  int  len = 0;
  skipBlanks();
  while (isalnum(*line) || *line == '_')
  {
    *buf++ = *line;
    ++len;
    delInLine(1);
  };
  *buf = 0;
  return len;
};

void readSDF ( void )
{
  int subNoRet;
  labType      curSection = LabData;
  labMemory    curMemory  = LabIRAM;
  curLine      = 1;
  startAddress =
  AbsAddress   = 0;

  if (!(inf = fopen(sdfFileName, "rt")))
    error("Can't open %s", sdfFileName);
  while (readLine())
  {
    subNoRet = 0;

    if (matchSeq("FILE", "SIZE", NULL))
      FileLength = readWord();
    else
    if (matchSeq("ENTRY", "POINT", NULL))
      startAddress = readWord();
    else
    if (matchSeq("ABSOLUTE", "ADDRESS", NULL))
      AbsAddress = readWord();
    else
    if (matchSeq("CODE", "SEGMENT", "AT", NULL))
    {
      ADDRESS from, to;
      from = readWord();
      to = readWord();
      addSegment(from, to);
    }
    else
    if (match("SUBROUTINES"))
    {
      curSection = LabSub;
      curMemory = LabCodeMemory;
    }
    else
    if (match("LOCATIONS"))
    {
      curSection = LabLoc;
      curMemory = LabCodeMemory;
    }
    else
    if (matchSeq("CODE", "DATA", NULL))
    {
      curSection = LabData;
      curMemory = LabCodeMemory;
    }
    else
    if (matchSeq("CODE", "EQUATES", NULL))
    {
      curSection = LabEData;
      curMemory = LabCodeMemory;
    }
    else
    if (matchSeq("CODE", "TABLES", NULL))
    {
      curSection = LabTab;
      curMemory = LabCodeMemory;
    }
    else
    if (matchSeq("CODE", "JUMP", "TABLES", NULL))
    {
      curSection = LabJmpTab;
      curMemory = LabCodeMemory;
    }
    else
    if (matchSeq("IRAM", "EQUATES", NULL))
    {
      curSection = LabEData;
      curMemory = LabIRAM;
    }
    else
    if (matchSeq("BIT", "EQUATES", NULL))
    {
      curSection = LabBit;
      curMemory = LabBitRAM;
    }
    else
    if (matchSeq("IRAM", "TABLES", NULL))
    {
      curSection = LabTab;
      curMemory = LabIRAM;
    }
    else
    if (matchSeq("IRAM", "JUMP", "TABLES", NULL))
    {
      curSection = LabJmpTab;
      curMemory = LabIRAM;
    }
    else
    if (matchSeq("XRAM", "EQUATES", NULL))
    {
      curSection = LabEData;
      curMemory = LabXRAM;
    }
    else
    if (matchSeq("XRAM", "TABLES", NULL))
    {
      curSection = LabTab;
      curMemory = LabXRAM;
    }
    else
    if (matchSeq("XRAM", "JUMP", "TABLES", NULL))
    {
      curSection = LabJmpTab;
      curMemory = LabXRAM;
    }
    else //Label
    {
      char * name = NULL;
      char   buf[33];
      if (match("'"))
      {
        int len = getToken(buf);
        match("'");
        strcpy(name = new char[len+1], buf);
      }
      else
        getToken(buf); // Skip label name
      ADDRESS addr = readWord();
      labSize s = LabByte;
      if (match("WORD"))
        s = LabWord;
      else
      if (match("BIT"))
        s = LabBitSize;
      else
      if (match( "NORET" ))
        subNoRet = 1;

      if (curSection == LabJmpTab)
        s = LabWord;
      else
      if (curSection == LabBit)
        s = LabBitSize;
      addLabName( addr, curSection, curMemory, s, name, NOTPARSED, subNoRet );
    };
  };
  fclose(inf);
};
