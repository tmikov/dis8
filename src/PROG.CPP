#include <stdio.h>
#include "sr.h"

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

struct idesc1
{
  int addr;
  byte inum;
  iType it;
};

idesc1 coms1[256] =
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

char * types[] =
{
  "Unknown",
  "CondJump",
  "BitJump",
  "Jump",
  "MemJump",
  "Call",
  "Mem",
  "Ret",
  "Other"
};

char * addr[] =
{
  "AUnknown",
  "AImmediate",
  "AFull",
  "AZeroPage",
  "ANone",
  "ARelative",
  "AIndirect",
  "AIndZeroPageX",
  "AIndZeroPageY",
  "AIndX",
  "AIndY",
  "AIndIndirect",
  "AIndirectInd",
  "AAcc",
  "AZeroPageRel"
};

void main(void)
{
  idesc * coms = (idesc *)coms1;
  for (int i = 0; i < 256; ++i)
  {
    if (coms[i].addr == AImmediate || coms[i].addr == AAcc)
      coms[i].it = Other;
    else
    if (coms[i].addr == AZeroPage || coms[i].addr == AIndZeroPageX ||
        coms[i].addr == AIndZeroPageY || coms[i].addr == AIndY ||
        coms[i].addr == AIndIndirect || coms[i].addr == AIndirectInd)
      coms[i].it = Mem;
    else
    if (coms[i].addr == AZeroPageRel)
      coms[i].it = BitJump;
    else
    if (coms[i].addr == ARelative)
      coms[i].it = CondJump;
    else
    if (coms[i].addr == AIndirect)
      coms[i].it = MemJump;
    else
    if (coms[i].addr)
    {
      int t;
      printf("%03d - %s\t%s\t: ", i, inames[coms[i].inum], addr[coms[i].addr]);
      scanf("%d", &t);
      coms[i].it = (iType)t;
    };
  };
  FILE * f;

  f = fopen("tab.cpp", "wt");
  fputs("idesc coms[256] =\n", f);
  fputs("{\n", f);
  for (i = 0; i < 256; ++i)
  {
    if (coms[i].addr)
      fprintf(f, "  {%15s,%4d,%10s},\t// %02X  -  %s\n", addr[coms[i].addr],
              coms[i].inum, types[coms[i].it], i, inames[coms[i].inum]);
    else
      fprintf(f, "  {%s},\n", addr[AUnknown]);
  };
  fputs("}\n", f);
  fclose(f);
};