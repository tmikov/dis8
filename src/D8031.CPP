#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <io.h>
#include <ctype.h>
#include <conio.h>

#include "dis31.h"

BYTE get_instr ( unsigned i, char * buf )
{
  int step;
  step = DisassembleSingle( i );
  strcpy( buf, ResultBuffer );
  return step;
};

unsigned curaddr;
unsigned codesize;

void load_file ( void )
{
  char name[80];
  int   f;
  long  s;

  if (Code)
    farfree(Code);
  printf("Enter file name: ");
  scanf("%s", name);
  if ( (f = _open(name, 0)) == -1 )
  {
    printf(" *** Sorry, file not found. Do you know what DIR means?\n\n");
    return;
  };
  if ( (s = filelength( f )) - 1> 0xFFFF )
  {
    printf(" *** File is too long. Go cut it.\n\n");
    return;
  };
  if ( (Code =(BYTE huge *)farmalloc(s)) == NULL)
  {
    printf(" *** I'm afraid your PC doesn't have a single gigabyte of RAM.\n\n");
    return;
  };

  if (s <= 0xFF00)
  {
    if (_read( f, (void *)Code, s ) == -1)
      goto errExit;
  }
  else
  {
    if(_read(f,(void*)Code,0xFF00)==-1||_read(f,(void *)((BYTE huge *)Code+0xFF00),0xFF00)==-1)
    {
errExit:
      printf(" *** What's the matter with this fucking file? I can't read it!\n\n");
      return;
    };
  };

  _close( f );
  printf("Tell me the absolute address(hex): ");
  scanf("%X", &AbsAddress);
  printf("So far, so good...\n\n");
  codesize = s - 1;
};

void dis_screen ( void )
{
  unsigned lines, i, j, z;
  char     name[20];

  for ( i = curaddr, lines = 1; lines <= 20; ++lines)
  {
    printf("%04X:  ", AbsAddress + i );
    j = get_instr( i, name );
    for(z = 0; z < j; ++z)
      printf("%02X ", Code[i+z]);
    for(; z < 3; ++z)
      printf("   ");
    printf("\t%s\n", name);
    if ((long)i + j > codesize)
      break;
    i += j;
  };
  curaddr = i;
};


void dis_addr ( void )
{
  unsigned i, j, z, e;
  char     name[20];

  printf(" Enter start address (16): ");
  scanf("%X", &i);
  if (i < AbsAddress || i > codesize + AbsAddress )
  {
    printf(" *** It is outside the file, Smartass!\n\n");
    return;
  };
  printf(" Enter end address (16): ");
  scanf("%X", &e);
  if (e < AbsAddress || e > codesize + AbsAddress || e < i)
  {
    printf(" *** This address doesn't seem OK.\n\n");
    return;
  };

  i -= AbsAddress;
  e -= AbsAddress;

  while(1)
  {
    printf("%04X:  ", AbsAddress + i );
    j = get_instr( i, name );
    for(z = 0; z < j; ++z)
      printf("%02X ", Code[i+z]);
    for(; z < 3; ++z)
      printf("   ");
    printf("\t%s\n", name);
    if ((long)i + j > e )
      break;
    i += j;
  };
  if ( ((long)i + j) > codesize )
    curaddr = codesize;
  else
    curaddr = i + j;
};

void goto_address ( void )
{
  unsigned i;
  printf(" C'mon give me the new address (16): ");
  scanf("%X", &i);
  if (i < AbsAddress || i > codesize + AbsAddress - 1)
  {
    printf(" *** It is outside the file, Smartass!\n\n");
    return;
  };
  curaddr = i - AbsAddress;
};

void dump_screen ( void )
{
  BYTE buf[80], niz[20], c;
  unsigned bpos, i, lines, z;

  for(i = curaddr & 0xFFF0, lines = 0; lines < 20; ++lines)
  {
    sprintf(buf, "%04X: ", i + AbsAddress);
    bpos = 6;

    for (z = 0; z < 16; ++z)
    {
      if (i < curaddr)
      {
        buf[bpos++] = ' ';
        buf[bpos++] = ' ';
        buf[bpos++] = ' ';
        niz[z] = ' ';
      }
      else
      {
        c = Code[i];
        sprintf(buf + bpos, " %02X", c);
        bpos += 3;

        if ( c < 32 || c > 127)
          c = '.';
        niz[z] = c;
      };
      if (i == codesize)
      {
         for(++z; z < 16; ++z)
         {
           buf[bpos++] = ' ';
           buf[bpos++] = ' ';
           buf[bpos++] = ' ';
           niz[z] = ' ';
         };
         niz[16] = 0;
         buf[bpos] = 0;
         printf("%s  %s\n", buf, niz);
         curaddr = 0;
         return;
      }
      else
        ++i;
    };
    buf[bpos] = 0;
    niz[16] = 0;
    printf("%s  %s\n", buf, niz);
  };
  curaddr = i;
};

void dump_addr ( void )
{
  BYTE buf[80], niz[20], c;
  unsigned bpos, i, z, s, e;

  printf(" Enter start address (16): ");
  scanf("%X", &s);
  if (s < AbsAddress || s > codesize + AbsAddress )
  {
    printf(" *** It is outside the file, Smartass!\n\n");
    return;
  };
  printf(" А ▒ега к░айни┐ (16): ");
  scanf("%X", &e);
  if (e < AbsAddress || e > codesize + AbsAddress || e < s)
  {
    printf(" *** Are you trying to fuck me up, HUH?.\n\n");
    return;
  };

  s -= AbsAddress;
  e -= AbsAddress;
  i = s & 0xFFF0;
  while(1)
  {
    sprintf(buf, "%04X: ", i + AbsAddress);
    bpos = 6;

    for (z = 0; z < 16; ++z)
    {
      if (i < s)
      {
        buf[bpos++] = ' ';
        buf[bpos++] = ' ';
        buf[bpos++] = ' ';
        niz[z] = ' ';
      }
      else
      {
        c = Code[i];
        sprintf(buf + bpos, " %02X", c);
        bpos += 3;

        if ( c < 32 || c > 127)
          c = '.';
        niz[z] = c;
      };
      if (i == e)
      {
         for(++z; z < 16; ++z)
         {
           buf[bpos++] = ' ';
           buf[bpos++] = ' ';
           buf[bpos++] = ' ';
           niz[z] = ' ';
         };
         niz[16] = 0;
         buf[bpos] = 0;
         printf("%s  %s\n", buf, niz);
         if ( e == codesize )
           curaddr = 0;
         else
           curaddr = e;
         return;
      }
      else
        ++i;
    };
    buf[bpos] = 0;
    niz[16] = 0;
    printf("%s  %s\n", buf, niz);
  };
};



void main_loop ( void )
{
  static char * shits[5] =
  {
    "Fuck YOU!",
    "Go kill yourself!",
    "Is that you, LZKK?",
    "You'd better call someone smarted than you.",
    "It is obvious computers are too complicated for you, Asshole!"
  };
  int key;

  while (1)
  {
    printf(">");
    key = toupper( getche() );
    printf("\n");
    if (isspace(key))
      continue;
    switch ( key )
    {
      case 'U':
        if (!Code)
          printf("*** You haven't loaded a file, Shithead!\n\n");
        else
          dis_screen();
        break;

      case 'A':
        if (!Code)
          printf("*** You haven't loaded a file, Shithead!\n\n");
        else
          dis_addr();
        break;

      case 'G':
        if (!Code)
          printf("*** You haven't loaded a file, Shithead!\n\n");
        else
          goto_address();
        break;

      case 'D':
        if (!Code)
          printf("*** You haven't loaded a file, Shithead!\n\n");
        else
          dump_screen();
        break;

      case 'S':
        if (!Code)
          printf("*** You haven't loaded a file, Shithead!\n\n");
        else
          dump_addr();
        break;

      case 'L':
        load_file();
        break;

      case 'H':
        puts("Help hah? Here's your help, stupid...");
        puts("  H - Only for fools");
        puts("  Q - End. Press this immediately!");
        puts("  U - Disassemble a screen");
        puts("  А - Disassemble from address to address");
        puts("  G - Go to address");
        puts("  D - Dupm a screen");
        puts("  S - Dump from address to address");
        puts("  L - Load bin file\n");
        break;

      case 'Q':
        if (Code)
          farfree( Code );
        printf(" It was high time. Have mercy, don't run me!\n\n");
        return;

      default:
        printf("*** Unrecognized command!!! (I knew it...)***\n");
        printf("*** %s\n\n", shits[random(5)]);
        break;
    };
  };
};

int main(void)
{
  printf("***** 8031 Intercative disassambler *****\n");
  printf("  Press 'h' for help \n\n\n");

  Code = NULL;
  curaddr = 0;

  main_loop();
  return TRUE;
};

