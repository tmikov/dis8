#include "compat.h"
#include <stdio.h>
#ifdef __TURBOC__
#include <dir.h>
#include <io.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "sr.h"

//********* from SR.CPP  *****************
extern void pass1(ADDRESS start);
extern void pass1SDF(void);
extern void fixJumpTables ( void );
extern void pass2(void);
extern void genDef(void);
extern void initAll(void);

//********* from READSDF.CPP *************
extern void readSDF ( void );

char errNoMem[] = "Out of memory";

int  haveSDF;

const char * inFileName,
     * outFileName,
     * defFileName,
     * sdfFileName;

FILE    * outFile;
ADDRESS startAddress;
BOOL    strictJumps;


void out(const char * mess ...)
{
  va_list ap;
  va_start(ap, mess);
  vfprintf(outFile, mess, ap);
  va_end(ap);
};


void error(const char * mess ...)
{
  va_list ap;
  va_start(ap, mess);

  printf("** Error: ");
  vprintf(mess, ap);
  printf("\n");

  va_end(ap);
  exit(-1);
};

static void near getFileName ( char * name )
{
#if __MSDOS__
  static char tmp[MAXPATH];
  char drv[MAXDRIVE],
       dir[MAXDIR],
       file[MAXFILE],
       ext[MAXEXT];

  fnsplit(strupr(name), drv, dir, file, ext);
  if (strcmp(ext, ".SDF") == 0)
  {
    haveSDF = 1;
    fnmerge(tmp, drv, dir, file, ".BIN");
    inFileName = strdup(tmp);
    fnmerge(tmp, drv, dir, file, ".SDF");
    sdfFileName = strdup(tmp);
  }
  else
  {
    haveSDF = 0;
    fnmerge(tmp, drv, dir, file, *ext ? ext : ".BIN");
    inFileName = strdup(tmp);
  };
  fnmerge(tmp, drv, dir, file, ".SRC");
  outFileName = strdup(tmp);
  fnmerge(tmp, drv, dir, file, ".DEF");
  defFileName = strdup(tmp);
#else
  size_t len = strlen( name );
  const char * ext = strrchr( name, '.' );
  char * tmp;
  if (ext && strcmp(ext, ".sdf")==0)
  {
    haveSDF = 1;
    tmp = (char *)malloc(ext - name + 4 + 1);
    memcpy( tmp, name, ext - name );
    strcpy( tmp+(ext - name), ".bin" );
    inFileName = tmp;
    sdfFileName = strdup(name);
  }
  else
  {
    haveSDF = 0;
    if (ext)
      inFileName = name;
    else
    {
      tmp = (char *)malloc(len+4+1);
      memcpy( tmp, name, len );
      strcpy( tmp+len, ".bin" );
      inFileName = tmp;
    }
  }
#endif
};

int freadLong(BYTE huge * buf, long len, FILE * f)
{
  while (len > 60000)
  {
    if (fread((void *)buf, 1, 60000, f) != 60000)
      return 0;
    buf += 60000;
    len -= 60000;
  };
  if (len)
    if (fread((void *)buf, 1, len, f) != len)
      return 0;
  return 1;
};

static void near readFile ( void )
{
  FILE * f;

  if (!(f = fopen(inFileName, "rb")))
    error("Can't open %s", inFileName);
  if (( FileLength = filelength(fileno(f)) ) > 0x10000)
    error("Input file is too long");
  if (!(Code = new BYTE[FileLength]))
    error(errNoMem);
  if (!freadLong(Code, FileLength, f))
    error("Can't read %s", inFileName);
  fclose(f);
};

int myScanf(const char * format ...)
{
  char buf[256];
  va_list ap;
  va_start(ap, format);

  gets(buf);
  int res = vsscanf(buf, format, ap);

  va_end(ap);
  return res;
};

void readInput(void)
{
  do
  {
    printf("Enter start ADDRESS(16)    : ");
    clearerr(stdin);
  }
  while (myScanf("%X", &startAddress) != 1);
  do
  {
    printf("Enter absolute ADDRESS(16) : ");
    clearerr(stdin);
  }
  while (myScanf("%X", &AbsAddress) != 1);
};

void readOptions ( void )
{
  char buf[80];
  int i;

  do
  {
    strictJumps = FALSE;
    printf( "Enter options (s):" );
    gets( buf );
    strupr( buf );
    for( i = 0; buf[i]; ++i )
    {
      if (buf[i] == 'S')
        strictJumps = TRUE;
      else
      {
        printf( "*** Invalid option '%c'\n", buf[i] );
        break;
      }
    }
  }
  while (buf[i] != 0);

};

int main(int argc, char ** argv)
{
  puts("DIS31 for 8031. Version 0.5. Copyright (c) Tse-Tso & Stan-Jo, 1996-1997");
  if (argc == 1)
  {
    puts("Syntax:  DIS31 filename[.BIN]");
    return 0;
  };
  getFileName(argv[1]);
  initAll();
  readFile();
  readOptions();

  if (haveSDF)
  {
    puts("Reading .SDF file...");
    readSDF();
    EndAddress = AbsAddress + FileLength - 1;
    pass1SDF();
  }
  else
  {
    readInput();
    EndAddress = AbsAddress + FileLength - 1;
    pass1(startAddress);
  };

  fixJumpTables();

  if (!(outFile = fopen(outFileName, "wt")))
    error("Can't create %s", outFileName);
  pass2();
  fclose(outFile);
  if (!(outFile = fopen(defFileName, "wt")))
    error("Can't create %s", defFileName);
  puts("Writing .DEF file...");
  genDef();
  fclose(outFile);
  puts("Done.");
  return 0;
};
