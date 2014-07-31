#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __TURBOC__
#include <alloc.h>
#include <io.h>
#include <conio.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <ctype.h>

#include "compat.h"

typedef unsigned char uchar;

typedef struct
  {
    uchar  addr, inum;
  } idesc;

char inames[][5] =
{
  "???",
  "LDA",
  "LDX",
  "LDY",
  "STA",
  "STX",
  "STY",
  "TAX",
  "TAY",
  "TSX",
  "TXA",
  "TXS",
  "TYA",
  "PHA",
  "PHP",
  "PLA",
  "PLP",
  "ADC",
  "SBC",
  "INC",
  "INX",
  "INY",
  "DEC",
  "DEX",
  "DEY",
  "AND",
  "ORA",
  "EOR",
  "ASL",
  "LSR",
  "ROL",
  "ROR",
  "CMP",
  "CPX",
  "CPY",
  "BIT",
  "BCC",
  "BCS",
  "BEQ",
  "BMI",
  "BNE",
  "BPL",
  "BVC",
  "BVS",
  "JMP",
  "JSR",
  "RTS",
  "RTI",
  "BRK",
  "NOP",
  "CLC",
  "CLD",
  "CLI",
  "CLV",
  "SEC",
  "SED",
  "SEI",
  "BRA",
  "PHY",
  "PLY",
  "PHX",
  "PLX",
  "STZ",
  "TSB",
  "TRB",
  "BBR0",
  "BBR1",
  "BBR2",
  "BBR3",
  "BBR4",
  "BBR5",
  "BBR6",
  "BBR7",
  "BBS0",
  "BBS1",
  "BBS2",
  "BBS3",
  "BBS4",
  "BBS5",
  "BBS6",
  "BBS7",
  "RMB0",
  "RMB1",
  "RMB2",
  "RMB3",
  "RMB4",
  "RMB5",
  "RMB6",
  "RMB7",
  "SMB0",
  "SMB1",
  "SMB2",
  "SMB3",
  "SMB4",
  "SMB5",
  "SMB6",
  "SMB7"
};

idesc coms[256] =
{
  {  4, 48 },
  { 11, 26 },
  {0, 0},
  {0, 0},
  {  3, 63 },  /* 04 */
  {  3, 26 },
  {  3, 28 },
  {  3, 81 },
  {  4, 14 },
  {  1, 26 },
  {  4, 28 },
  {0, 0},
  {  2, 63 },  /* 0C */
  {  2, 26 },
  {  2, 28 },
  { 14, 65 },
  {  5, 41 },
  { 12, 26 },
  {0, 0},
  {0, 0},
  {  3, 64 }, /* 14 */
  {  7, 26 },
  {  7, 28 },
  {  3, 82 },
  {  4, 50 },
  { 10, 26 },
  { 13, 19},  /* 1A */
  {0, 0},
  {  2, 64 }, /* 1C */
  {  9, 26 },
  {  9, 28 },
  { 14, 66 },
  {  2, 45 },
  { 11, 25 },
  {0, 0},
  {0, 0},
  {  3, 35 },
  {  3, 25 },
  {  3, 30 },
  {  3, 83 },
  {  4, 16 },
  {  1, 25 },
  {  4, 30 },
  {0, 0},
  {  2, 35 },
  {  2, 25 },
  {  2, 30 },
  { 14, 67 },
  {  5, 39 },
  { 12, 25 },
  {0, 0},
  {0, 0},
  {  7, 35 },  /* 34 */
  {  7, 25 },
  {  7, 30 },
  {  3, 84 },
  {  4, 54 },
  { 10, 25 },
  { 13, 22 }, /* 3A */
  {0, 0},
  {  9, 35 }, /* 3C */
  {  9, 25 },
  {  9, 30 },
  { 14, 68 },
  {  4, 47 },
  { 11, 27 },
  {0, 0},
  {0, 0},
  {0, 0},
  {  3, 27 },
  {  3, 29 },
  {  3, 85 },
  {  4, 13 },
  {  1, 27 },
  {  4, 29 },
  {0, 0},
  {  2, 44 },
  {  2, 27 },
  {  2, 29 },
  { 14, 69 },
  {  5, 42 },
  { 12, 27 },
  {0, 0},
  {0, 0},
  {0, 0},
  {  7, 27 },
  {  7, 29 },
  {  3, 86 },
  {  4, 52 },
  { 10, 27 },
  {  4, 58 },  /* 5a */
  {0, 0},
  {0, 0},
  {  9, 27 },
  {  9, 29 },
  { 14, 70 },
  {  4, 46 },
  { 11, 17 },
  {0, 0},
  {0, 0},
  {  3, 62 },  /* 64 */
  {  3, 17 },
  {  3, 31 },
  {  3, 87 },
  {  4, 15 },
  {  1, 17 },
  {  4, 31 },
  {0, 0},
  {  6, 44 },
  {  2, 17 },
  {  2, 31 },
  { 14, 71 },
  {  5, 43 },
  { 12, 17 },
  {0, 0},
  {0, 0},
  {  7, 62 }, /* 74 */
  {  7, 17 },
  {  7, 31 },
  {  3, 88 },
  {  4, 56 },
  { 10, 17 },
  {  4, 59 }, /* 7A */
  {0, 0},
  {  9, 44 }, /* 7C */
  {  9, 17 },
  {  9, 31 },
  { 14, 72 },
  {  5, 57 }, /* 80 */
  { 11,  4 },
  {0, 0},
  {0, 0},
  {  3,  6 },
  {  3,  4 },
  {  3,  5 },
  {  3, 89 },
  {  4, 24 },
  {  1, 35 }, /* 89 */
  {  4, 10 },
  {0, 0},
  {  2,  6 },
  {  2,  4 },
  {  2,  5 },
  { 14, 73 },
  {  5, 36 },
  { 12,  4 },
  {0, 0},
  {0, 0},
  {  7,  6 },
  {  7,  4 },
  {  8,  5 },
  {  3, 90 },
  {  4, 12 },
  { 10,  4 },
  {  4, 11 },
  {0, 0},
  {  2, 62 }, /* 9C */
  {  9,  4 },
  {  9, 62 }, /* 9E */
  { 14, 74 },
  {  1,  3 },
  { 11,  1 },
  {  1,  2 },
  {0, 0},
  {  3,  3 },
  {  3,  1 },
  {  3,  2 },
  {  3, 91 },
  {  4,  8 },
  {  1,  1 },
  {  4,  7 },
  {0, 0},
  {  2,  3 },
  {  2,  1 },
  {  2,  2 },
  { 14, 75 },
  {  5, 37 },
  { 12,  1 },
  {0, 0},
  {0, 0},
  {  7,  3 },
  {  7,  1 },
  {  8,  2 },
  {  3, 92 },
  {  4, 53 },
  { 10,  1 },
  {  4,  9 },
  {0, 0},
  {  9,  3 },
  {  9,  1 },
  { 10,  2 },
  { 14, 76 },
  {  1, 34 },
  { 11, 32 },
  {0, 0},
  {0, 0},
  {  3, 34 },
  {  3, 32 },
  {  3, 22 },
  {  3, 93 },
  {  4, 21 },
  {  1, 32 },
  {  4, 23 },
  {0, 0},
  {  2, 34 },
  {  2, 32 },
  {  2, 22 },
  { 14, 77 },
  {  5, 40 },
  { 12, 32 },
  {0, 0},
  {0, 0},
  {0, 0},
  {  7, 32 },
  {  7, 22 },
  {  3, 94 },
  {  4, 51 },
  { 10, 32 },
  {  4, 60 },    /* DA */
  {0, 0},
  {0, 0},
  {  9, 32 },
  {  9, 22 },
  { 14, 78 },
  {  1, 33 },
  { 11, 18 },
  {0, 0},
  {0, 0},
  {  3, 33 },
  {  3, 18 },
  {  3, 19 },
  {  3, 95 },
  {  4, 20 },
  {  1, 18 },
  {  4, 49 },
  {0, 0},
  {  2, 33 },
  {  2, 18 },
  {  2, 19 },
  { 14, 79 },
  {  5, 38 },
  { 12, 18 },
  {0, 0},
  {0, 0},
  {0, 0},
  {  7, 18 },
  {  7, 19 },
  {  3, 96 },
  {  4, 55 },
  { 10, 18 },
  {  4, 61 }, /* FA */
  {0, 0},
  {0, 0},
  {  9, 18 },
  {  9, 19 },
  { 14, 80 }
};

typedef uchar (*aptr)( unsigned i, char * buf );

uchar addr_err (  unsigned i, char * buf );
uchar addr_imm (  unsigned i, char * buf );
uchar addr_full ( unsigned i, char * buf );
uchar addr_zp ( unsigned i, char * buf );
uchar addr_no ( unsigned i, char * buf );
uchar addr_rel ( unsigned i, char * buf );
uchar addr_nep ( unsigned i, char * buf );
uchar addr_indzpx ( unsigned i, char * buf );
uchar addr_indzpy ( unsigned i, char * buf );
uchar addr_indx ( unsigned i, char * buf );
uchar addr_indy ( unsigned i, char * buf );
uchar addr_indnep ( unsigned i, char * buf );
uchar addr_nepind ( unsigned i, char * buf );
uchar addr_acc ( unsigned i, char * buf );
uchar addr_zprel ( unsigned i, char * buf );


aptr addr_funcs[15] =
{
  addr_err,
  addr_imm,
  addr_full,
  addr_zp,
  addr_no,
  addr_rel,
  addr_nep,
  addr_indzpx,
  addr_indzpy,
  addr_indx,
  addr_indy,
  addr_indnep,
  addr_nepind,
  addr_acc,
  addr_zprel
};

char tb[20];
uchar  huge * code;
unsigned cstart;

char * make_word ( unsigned w )
{
  sprintf(tb, "%04X", w);
  return tb;
};

char * getword( unsigned i )
{
  return make_word( code[i] + ( (unsigned)code[i+1] << 8) );
};

char * getbyte( unsigned i )
{
  sprintf(tb, "%02X", code[i]);
  return tb;
};


uchar addr_err ( unsigned i, char * buf )
{
  return 1;
};

uchar addr_imm ( unsigned i, char * buf )
{
  strcat( strcat(buf, "\t#$"), getbyte( i ) );
  return 2;
};

uchar addr_full ( unsigned i, char * buf )
{
  strcat(strcat(buf, "\t$"), getword( i ) );
  return 3;
};

uchar addr_zp ( unsigned i, char * buf )
{
  strcat( strcat(buf, "\t$"), getbyte( i ) );
  return 2;
};

uchar addr_no ( unsigned i, char * buf )
{
  return 1;
};


uchar addr_rel ( unsigned i, char * buf )
{
  strcat( strcat(buf, "\t$"), make_word(i + cstart + 1 + (signed char)code[i]) );
  return 2;
};

uchar addr_nep ( unsigned i, char * buf )
{
  strcat( strcat( strcat(buf, "\t($"), getword( i ) ), ")" );
  return 3;
};

uchar addr_indzpx ( unsigned i, char * buf )
{
  strcat( strcat( strcat( buf, "\t$"), getbyte( i ) ), ",X" );
  return 2;
};

uchar addr_indzpy ( unsigned i, char * buf )
{
  strcat( strcat( strcat( buf, "\t$"), getbyte( i ) ), ",Y" );
  return 2;
};

uchar addr_indx ( unsigned i, char * buf )
{
  strcat( strcat( strcat( buf, "\t$"), getword( i ) ), ",X" );
  return 3;
};

uchar addr_indy ( unsigned i, char * buf )
{
  strcat( strcat( strcat( buf, "\t$"), getword( i ) ), ",Y" );
  return 3;
};

uchar addr_indnep ( unsigned i, char * buf )
{
  strcat( strcat( strcat( buf, "\t($"), getbyte( i ) ), ",X)" );
  return 2;
};

uchar addr_nepind ( unsigned i, char * buf )
{
  strcat( strcat( strcat( buf, "\t($"), getbyte( i ) ), "),Y" );
  return 2;
};

uchar addr_acc ( unsigned i, char * buf )
{
  strcat( buf, "\tA" );
  return 1;
};

uchar addr_zprel ( unsigned i, char * buf )
{
  strcat( buf, "\t$" );
  strcat( buf, getbyte(i) );
  strcat( buf, ",$" );
  strcat( buf, make_word(i + cstart + 2 + (signed char)code[i+1]) );
  return 3;
};



uchar get_instr ( unsigned i, char * buf )
{
  strcpy( buf, inames[coms[code[i]].inum] );
  return (*addr_funcs[coms[code[i]].addr])(i+1, buf);
};

unsigned curaddr;
unsigned codesize;

void load_file ( void )
{
  char name[80];
  int   f;
  long  s;

  if (code)
    free(code);
  printf("Я ╖│кни име▓о на ┤айла: ");
  scanf("%s", name);
  if ( (f = open(name, O_RDONLY)) == -1 )
  {
    printf(" *** Sorry, мой ╖овек. Т│й ┤айл╖е го нема...\n\n");
    return;
  };
  if ( (s = filelength( f )) - 1> 0xFFFF )
  {
    printf(" *** Е-е-е, много ▓и е ▓л║▒▓ ┤айл║▓.\n\n");
    return;
  };
  if ( (code = farmalloc(s)) == NULL)
  {
    printf(" *** Тази ░або▓а не ▒▓ава ▒ ▓олкова малко паме▓.\n\n");
    return;
  };

  if (s <= 0xFF00)
  {
    if (read( f, code, s ) == -1)
      goto errExit;
  }
  else
  {
    if(read(f,code,0xFF00)==-1||read(f,(uchar huge *)code+0xFF00,0xFF00)==-1)
    {
errExit:
      printf(" *** Файл║▓ май не ▓и е на░ед. Не мога да го п░о╖е▓а.\n\n");
      return;
    };
  };

  close( f );
  printf("Я ▒ега ми кажи, кой е аб▒ол╛▓ни┐▓ ад░е▒ (╕е▒▓най▒е▓и╖ен): ");
  scanf("%X", &cstart);
  printf("До▓│к доб░е...\n\n");
  codesize = s - 1;
};

void dis_screen ( void )
{
  unsigned lines, i, j, z;
  char     name[20];

  for ( i = curaddr, lines = 1; lines <= 20; ++lines)
  {
    printf("%04X:  ", cstart + i );
    j = get_instr( i, name );
    for(z = 0; z < j; ++z)
      printf("%02X ", code[i+z]);
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

  printf(" Я ми дай на╖ални┐ ад░е▒ (16): ");
  scanf("%X", &i);
  if (i < cstart || i > codesize + cstart )
  {
    printf(" *** Ад░е▒╖е▓о е изв║н ┤айла.\n\n");
    return;
  };
  printf(" А ▒ега к░айни┐ (16): ");
  scanf("%X", &e);
  if (e < cstart || e > codesize + cstart || e < i)
  {
    printf(" *** Ад░е▒╖е▓о май не╣о не е на░ед.\n\n");
    return;
  };

  i -= cstart;
  e -= cstart;

  while(1)
  {
    printf("%04X:  ", cstart + i );
    j = get_instr( i, name );
    for(z = 0; z < j; ++z)
      printf("%02X ", code[i+z]);
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
  printf(" Хайде, давай нови┐ ад░е▒ (16): ");
  scanf("%X", &i);
  if (i < cstart || i > codesize + cstart - 1)
  {
    printf(" *** Ад░е▒╖е▓о е изв║н ┤айла.\n\n");
    return;
  };
  curaddr = i - cstart;
};

void dump_screen ( void )
{
  char buf[80], niz[20];
  uchar c;
  unsigned bpos, i, lines, z;

  for(i = curaddr & 0xFFF0, lines = 0; lines < 20; ++lines)
  {
    sprintf(buf, "%04X: ", i + cstart);
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
        c = code[i];
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
  char buf[80], niz[20];
  uchar c;
  unsigned bpos, i, z, s, e;

  printf(" Я ми дай на╖ални┐ ад░е▒ (16): ");
  scanf("%X", &s);
  if (s < cstart || s > codesize + cstart )
  {
    printf(" *** Ад░е▒╖е▓о е изв║н ┤айла.\n\n");
    return;
  };
  printf(" А ▒ега к░айни┐ (16): ");
  scanf("%X", &e);
  if (e < cstart || e > codesize + cstart || e < s)
  {
    printf(" *** Ад░е▒╖е▓о май не╣о не е на░ед.\n\n");
    return;
  };

  s -= cstart;
  e -= cstart;
  i = s & 0xFFF0;
  while(1)
  {
    sprintf(buf, "%04X: ", i + cstart);
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
        c = code[i];
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
    "Т│й да не ▓и е ЕЛКА, бе гал┤он !",
    "Що не иде╕ да ▒е за▒▓░ел┐╕ !!!",
    "Кога ╣е на│╖и╕ команди▓е, некад║░ник ?",
    "А бе, ▓и н┐ма╕ ли ▒и д░│га ░або▓а ?",
    "Я викни н┐кой по-│мен."
  };
  int key;

  while (1)
  {
    printf(">");
#if __MSDOS__
    key = getche();
    printf("\n");
#else
    key = getchar();
#endif
    key = toupper( key );
    if (isspace(key))
      continue;
    switch ( key )
    {
      case 'U':
        if (!code)
          printf("*** Как ┐ ▒м┐▓а╕ ▓ази ░або▓а, без да ▒и за░едил ┤айл?\n\n");
        else
          dis_screen();
        break;

      case 'A':
        if (!code)
          printf("*** Как ┐ ▒м┐▓а╕ ▓ази ░або▓а, без да ▒и за░едил ┤айл?\n\n");
        else
          dis_addr();
        break;

      case 'G':
        if (!code)
          printf("*** Как ┐ ▒м┐▓а╕ ▓ази ░або▓а, без да ▒и за░едил ┤айл?\n\n");
        else
          goto_address();
        break;

      case 'D':
        if (!code)
          printf("*** Как ┐ ▒м┐▓а╕ ▓ази ░або▓а, без да ▒и за░едил ┤айл?\n\n");
        else
          dump_screen();
        break;

      case 'S':
        if (!code)
          printf("*** Как ┐ ▒м┐▓а╕ ▓ази ░або▓а, без да ▒и за░едил ┤айл?\n\n");
        else
          dump_addr();
        break;

      case 'L':
        load_file();
        break;

      case 'H':
        puts("Помо╣, а? Е, доб░е. О▓ мен да мине.");
        puts("  H - Извежда помо╣");
        puts("  Q - К░ай на п░ог░ама▓а");
        puts("  U - Диза▒ембли░а един ек░ан ин▒▓░│к╢ии");
        puts("  A - Диза▒ембли░а о▓ ад░е▒ до ад░е▒");
        puts("  G - О▓ива на ад░е▒");
        puts("  D - Д║мпва един ек░ан");
        puts("  S - Д║мпва о▓ ад░е▒ до ад░е▒");
/*        puts("  F - Т║░▒и");*/
        puts("  L - За░еждане на ┤айл\n");
        break;

      case 'Q':
        if (code)
          free( code );
        printf(" О╡, най-по▒ле.\n\n");
        return;

      default:
        printf("*** Непозна▓а команда!!! ***\n");
        printf("*** %s\n\n", shits[rand()%5]);
        break;
    };
  };
};

int main(void)
{
  printf("***** 65C02 Ин▓е░ак▓ивен диза▒ембле░ *****\n");
  printf("***             Ве░▒и┐ 0.1             ***\n\n");
  printf("  На▓и▒не▓е 'H' за помо╣\n\n\n");

  code = NULL;
  curaddr = 0;

  main_loop();
  return 0;
};

/*
int main(void)
{
  unsigned i, j, z;
  char name[30];
  int f, s;


  printf("Enter file name : ");
  scanf("%s", name);
  if ((f = open( name, O_RDONLY )) == -1)
    return -1;

  if ((code = malloc(s = filelength(f)))==NULL)
    return -1;
  if (!read( f, code, s))
    return -1;

  printf("Enter absolute start address: ");
  scanf("%X", &cstart);
  i = 0;
  do
  {
    printf("%04X:  ", cstart + i );
    j = get_instr( i, name );
    for(z = 0; z < j; ++z)
      printf("%02X ", code[i+z]);
    for(; z < 3; ++z)
      printf("   ");
    i += j;
    printf("\t%s\n", name);
  }
  while ( i < s );
  close( f );
  return 0;
};*/