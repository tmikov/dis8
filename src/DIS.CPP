#include <string.h>
#include <stdio.h>
#include "sr.h"


//******** from SR.CPP
extern label * searchLabel(ADDRESS a);
extern char  * referLabel(label * l);

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
  {          ANone,  48,       Ret},	// 00  -  BRK
  {   AIndIndirect,  26,       Mem},	// 01  -  ORA
  {AUnknown},
  {AUnknown},
  {      AZeroPage,  63,       Mem},	// 04  -  TSB
  {      AZeroPage,  26,       Mem},	// 05  -  ORA
  {      AZeroPage,  28,       Mem},	// 06  -  ASL
  {      AZeroPage,  81,       Mem},	// 07  -  RMB0
  {          ANone,  14,     Other},	// 08  -  PHP
  {     AImmediate,  26,     Other},	// 09  -  ORA
  {          AAcc,  28,      Other},	// 0A  -  ASL
  {AUnknown},
  {          AFull,  63,       Mem},	// 0C  -  TSB
  {          AFull,  26,       Mem},	// 0D  -  ORA
  {          AFull,  28,       Mem},	// 0E  -  ASL
  {   AZeroPageRel,  65,   BitJump},	// 0F  -  BBR0
  {      ARelative,  41,  CondJump},	// 10  -  BPL
  {   AIndirectInd,  26,       Mem},	// 11  -  ORA
  {AUnknown},
  {AUnknown},
  {      AZeroPage,  64,       Mem},	// 14  -  TRB
  {  AIndZeroPageX,  26,       Mem},	// 15  -  ORA
  {  AIndZeroPageX,  28,       Mem},	// 16  -  ASL
  {      AZeroPage,  82,       Mem},	// 17  -  RMB1
  {          ANone,  50,     Other},	// 18  -  CLC
  {          AIndY,  26,       Mem},	// 19  -  ORA
  {           AAcc,  19,     Other},	// 1A  -  INC
  {AUnknown},
  {          AFull,  64,       Mem},	// 1C  -  TRB
  {          AIndX,  26,       Mem},	// 1D  -  ORA
  {          AIndX,  28,       Mem},	// 1E  -  ASL
  {   AZeroPageRel,  66,   BitJump},	// 1F  -  BBR1
  {          AFull,  45,      Call},	// 20  -  JSR
  {   AIndIndirect,  25,       Mem},	// 21  -  AND
  {AUnknown},
  {AUnknown},
  {      AZeroPage,  35,       Mem},	// 24  -  BIT
  {      AZeroPage,  25,       Mem},	// 25  -  AND
  {      AZeroPage,  30,       Mem},	// 26  -  ROL
  {      AZeroPage,  83,       Mem},	// 27  -  RMB2
  {          ANone,  16,     Other},	// 28  -  PLP
  {     AImmediate,  25,     Other},	// 29  -  AND
  {          AAcc,  30,      Other},	// 2A  -  ROL
  {AUnknown},
  {          AFull,  35,       Mem},	// 2C  -  BIT
  {          AFull,  25,       Mem},	// 2D  -  AND
  {          AFull,  30,       Mem},	// 2E  -  ROL
  {   AZeroPageRel,  67,   BitJump},	// 2F  -  BBR2
  {      ARelative,  39,  CondJump},	// 30  -  BMI
  {   AIndirectInd,  25,       Mem},	// 31  -  AND
  {AUnknown},
  {AUnknown},
  {  AIndZeroPageX,  35,       Mem},	// 34  -  BIT
  {  AIndZeroPageX,  25,       Mem},	// 35  -  AND
  {  AIndZeroPageX,  30,       Mem},	// 36  -  ROL
  {      AZeroPage,  84,       Mem},	// 37  -  RMB3
  {          ANone,  54,     Other},	// 38  -  SEC
  {          AIndY,  25,       Mem},	// 39  -  AND
  {           AAcc,  22,     Other},	// 3A  -  DEC
  {AUnknown},
  {          AIndX,  35,       Mem},	// 3C  -  BIT
  {          AIndX,  25,       Mem},	// 3D  -  AND
  {          AIndX,  30,       Mem},	// 3E  -  ROL
  {   AZeroPageRel,  68,   BitJump},	// 3F  -  BBR3
  {          ANone,  47,       Ret},	// 40  -  RTI
  {   AIndIndirect,  27,       Mem},	// 41  -  EOR
  {AUnknown},
  {AUnknown},
  {AUnknown},
  {      AZeroPage,  27,       Mem},	// 45  -  EOR
  {      AZeroPage,  29,       Mem},	// 46  -  LSR
  {      AZeroPage,  85,       Mem},	// 47  -  RMB4
  {          ANone,  13,     Other},	// 48  -  PHA
  {     AImmediate,  27,     Other},	// 49  -  EOR
  {          AAcc,  29,     Other},	// 4A  -  LSR
  {AUnknown},
  {          AFull,  44,      Jump},	// 4C  -  JMP
  {          AFull,  27,       Mem},	// 4D  -  EOR
  {          AFull,  29,       Mem},	// 4E  -  LSR
  {   AZeroPageRel,  69,   BitJump},	// 4F  -  BBR4
  {      ARelative,  42,  CondJump},	// 50  -  BVC
  {   AIndirectInd,  27,       Mem},	// 51  -  EOR
  {AUnknown},
  {AUnknown},
  {AUnknown},
  {  AIndZeroPageX,  27,       Mem},	// 55  -  EOR
  {  AIndZeroPageX,  29,       Mem},	// 56  -  LSR
  {      AZeroPage,  86,       Mem},	// 57  -  RMB5
  {          ANone,  52,     Other},	// 58  -  CLI
  {          AIndY,  27,       Mem},	// 59  -  EOR
  {          ANone,  58,     Other},	// 5A  -  PHY
  {AUnknown},
  {AUnknown},
  {          AIndX,  27,       Mem},	// 5D  -  EOR
  {          AIndX,  29,       Mem},	// 5E  -  LSR
  {   AZeroPageRel,  70,   BitJump},	// 5F  -  BBR5
  {          ANone,  46,       Ret},	// 60  -  RTS
  {   AIndIndirect,  17,       Mem},	// 61  -  ADC
  {AUnknown},
  {AUnknown},
  {      AZeroPage,  62,       Mem},	// 64  -  STZ
  {      AZeroPage,  17,       Mem},	// 65  -  ADC
  {      AZeroPage,  31,       Mem},	// 66  -  ROR
  {      AZeroPage,  87,       Mem},	// 67  -  RMB6
  {          ANone,  15,     Other},	// 68  -  PLA
  {     AImmediate,  17,     Other},	// 69  -  ADC
  {          AAcc,  31,     Other},	// 6A  -  ROR
  {AUnknown},
  {      AIndirect,  44,   MemJump},	// 6C  -  JMP
  {          AFull,  17,       Mem},	// 6D  -  ADC
  {          AFull,  31,       Mem},	// 6E  -  ROR
  {   AZeroPageRel,  71,   BitJump},	// 6F  -  BBR6
  {      ARelative,  43,  CondJump},	// 70  -  BVS
  {   AIndirectInd,  17,       Mem},	// 71  -  ADC
  {AUnknown},
  {AUnknown},
  {  AIndZeroPageX,  62,       Mem},	// 74  -  STZ
  {  AIndZeroPageX,  17,       Mem},	// 75  -  ADC
  {  AIndZeroPageX,  31,       Mem},	// 76  -  ROR
  {      AZeroPage,  88,       Mem},	// 77  -  RMB7
  {          ANone,  56,     Other},	// 78  -  SEI
  {          AIndY,  17,       Mem},	// 79  -  ADC
  {          ANone,  59,     Other},	// 7A  -  PLY
  {AUnknown},
  {          AIndX,  44,   MemJump},	// 7C  -  JMP
  {          AIndX,  17,       Mem},	// 7D  -  ADC
  {          AIndX,  31,       Mem},	// 7E  -  ROR
  {   AZeroPageRel,  72,   BitJump},	// 7F  -  BBR7
  {      ARelative,  57,      Jump},	// 80  -  BRA
  {   AIndIndirect,   4,       Mem},	// 81  -  STA
  {AUnknown},
  {AUnknown},
  {      AZeroPage,   6,       Mem},	// 84  -  STY
  {      AZeroPage,   4,       Mem},	// 85  -  STA
  {      AZeroPage,   5,       Mem},	// 86  -  STX
  {      AZeroPage,  89,       Mem},	// 87  -  SMB0
  {          ANone,  24,     Other},	// 88  -  DEY
  {     AImmediate,  35,     Other},	// 89  -  BIT
  {          ANone,  10,     Other},	// 8A  -  TXA
  {AUnknown},
  {          AFull,   6,       Mem},	// 8C  -  STY
  {          AFull,   4,       Mem},	// 8D  -  STA
  {          AFull,   5,       Mem},	// 8E  -  STX
  {   AZeroPageRel,  73,   BitJump},	// 8F  -  BBS0
  {      ARelative,  36,  CondJump},	// 90  -  BCC
  {   AIndirectInd,   4,       Mem},	// 91  -  STA
  {AUnknown},
  {AUnknown},
  {  AIndZeroPageX,   6,       Mem},	// 94  -  STY
  {  AIndZeroPageX,   4,       Mem},	// 95  -  STA
  {  AIndZeroPageY,   5,       Mem},	// 96  -  STX
  {      AZeroPage,  90,       Mem},	// 97  -  SMB1
  {          ANone,  12,     Other},	// 98  -  TYA
  {          AIndY,   4,       Mem},	// 99  -  STA
  {          ANone,  11,     Other},	// 9A  -  TXS
  {AUnknown},
  {          AFull,  62,       Mem},	// 9C  -  STZ
  {          AIndX,   4,       Mem},	// 9D  -  STA
  {          AIndX,  62,       Mem},	// 9E  -  STZ
  {   AZeroPageRel,  74,   BitJump},	// 9F  -  BBS1
  {     AImmediate,   3,     Other},	// A0  -  LDY
  {   AIndIndirect,   1,       Mem},	// A1  -  LDA
  {     AImmediate,   2,     Other},	// A2  -  LDX
  {AUnknown},
  {      AZeroPage,   3,       Mem},	// A4  -  LDY
  {      AZeroPage,   1,       Mem},	// A5  -  LDA
  {      AZeroPage,   2,       Mem},	// A6  -  LDX
  {      AZeroPage,  91,       Mem},	// A7  -  SMB2
  {          ANone,   8,     Other},	// A8  -  TAY
  {     AImmediate,   1,     Other},	// A9  -  LDA
  {          ANone,   7,     Other},	// AA  -  TAX
  {AUnknown},
  {          AFull,   3,       Mem},	// AC  -  LDY
  {          AFull,   1,       Mem},	// AD  -  LDA
  {          AFull,   2,       Mem},	// AE  -  LDX
  {   AZeroPageRel,  75,   BitJump},	// AF  -  BBS2
  {      ARelative,  37,  CondJump},	// B0  -  BCS
  {   AIndirectInd,   1,       Mem},	// B1  -  LDA
  {AUnknown},
  {AUnknown},
  {  AIndZeroPageX,   3,       Mem},	// B4  -  LDY
  {  AIndZeroPageX,   1,       Mem},	// B5  -  LDA
  {  AIndZeroPageY,   2,       Mem},	// B6  -  LDX
  {      AZeroPage,  92,       Mem},	// B7  -  SMB3
  {          ANone,  53,     Other},	// B8  -  CLV
  {          AIndY,   1,       Mem},	// B9  -  LDA
  {          ANone,   9,     Other},	// BA  -  TSX
  {AUnknown},
  {          AIndX,   3,       Mem},	// BC  -  LDY
  {          AIndX,   1,       Mem},	// BD  -  LDA
  {          AIndY,   2,       Mem},	// BE  -  LDX
  {   AZeroPageRel,  76,   BitJump},	// BF  -  BBS3
  {     AImmediate,  34,     Other},	// C0  -  CPY
  {   AIndIndirect,  32,       Mem},	// C1  -  CMP
  {AUnknown},
  {AUnknown},
  {      AZeroPage,  34,       Mem},	// C4  -  CPY
  {      AZeroPage,  32,       Mem},	// C5  -  CMP
  {      AZeroPage,  22,       Mem},	// C6  -  DEC
  {      AZeroPage,  93,       Mem},	// C7  -  SMB4
  {          ANone,  21,     Other},	// C8  -  INY
  {     AImmediate,  32,     Other},	// C9  -  CMP
  {          ANone,  23,     Other},	// CA  -  DEX
  {AUnknown},
  {          AFull,  34,       Mem},	// CC  -  CPY
  {          AFull,  32,       Mem},	// CD  -  CMP
  {          AFull,  22,       Mem},	// CE  -  DEC
  {   AZeroPageRel,  77,   BitJump},	// CF  -  BBS4
  {      ARelative,  40,  CondJump},	// D0  -  BNE
  {   AIndirectInd,  32,       Mem},	// D1  -  CMP
  {AUnknown},
  {AUnknown},
  {AUnknown},
  {  AIndZeroPageX,  32,       Mem},	// D5  -  CMP
  {  AIndZeroPageX,  22,       Mem},	// D6  -  DEC
  {      AZeroPage,  94,       Mem},	// D7  -  SMB5
  {          ANone,  51,     Other},	// D8  -  CLD
  {          AIndY,  32,       Mem},	// D9  -  CMP
  {          ANone,  60,     Other},	// DA  -  PHX
  {AUnknown},
  {AUnknown},
  {          AIndX,  32,       Mem},	// DD  -  CMP
  {          AIndX,  22,       Mem},	// DE  -  DEC
  {   AZeroPageRel,  78,   BitJump},	// DF  -  BBS5
  {     AImmediate,  33,     Other},	// E0  -  CPX
  {   AIndIndirect,  18,       Mem},	// E1  -  SBC
  {AUnknown},
  {AUnknown},
  {      AZeroPage,  33,       Mem},	// E4  -  CPX
  {      AZeroPage,  18,       Mem},	// E5  -  SBC
  {      AZeroPage,  19,       Mem},	// E6  -  INC
  {      AZeroPage,  95,       Mem},	// E7  -  SMB6
  {          ANone,  20,     Other},	// E8  -  INX
  {     AImmediate,  18,     Other},	// E9  -  SBC
  {          ANone,  49,     Other},	// EA  -  NOP
  {AUnknown},
  {          AFull,  33,       Mem},	// EC  -  CPX
  {          AFull,  18,       Mem},	// ED  -  SBC
  {          AFull,  19,       Mem},	// EE  -  INC
  {   AZeroPageRel,  79,   BitJump},	// EF  -  BBS6
  {      ARelative,  38,  CondJump},	// F0  -  BEQ
  {   AIndirectInd,  18,       Mem},	// F1  -  SBC
  {AUnknown},
  {AUnknown},
  {AUnknown},
  {  AIndZeroPageX,  18,       Mem},	// F5  -  SBC
  {  AIndZeroPageX,  19,       Mem},	// F6  -  INC
  {      AZeroPage,  96,       Mem},	// F7  -  SMB7
  {          ANone,  55,     Other},	// F8  -  SED
  {          AIndY,  18,       Mem},	// F9  -  SBC
  {          ANone,  61,     Other},	// FA  -  PLX
  {AUnknown},
  {AUnknown},
  {          AIndX,  18,       Mem},	// FD  -  SBC
  {          AIndX,  19,       Mem},	// FE  -  INC
  {   AZeroPageRel,  80,   BitJump}	// FF  -  BBS7
};


typedef int (*aptr)( ADDRESS i, char * buf );

int addr_err (  ADDRESS i, char * buf );
int addrImmediate (  ADDRESS i, char * buf );
int addrFull ( ADDRESS i, char * buf );
int addrZeroPage ( ADDRESS i, char * buf );
int addrNone ( ADDRESS i, char * buf );
int addrRelative ( ADDRESS i, char * buf );
int addrIndirect ( ADDRESS i, char * buf );
int addrIndZeroPageX ( ADDRESS i, char * buf );
int addrIndZeroPageY ( ADDRESS i, char * buf );
int addrIndX ( ADDRESS i, char * buf );
int addrIndY ( ADDRESS i, char * buf );
int addrIndIndirect ( ADDRESS i, char * buf );
int addrIndirectInd ( ADDRESS i, char * buf );
int addrAcc ( ADDRESS i, char * buf );
int addrZeroPageRel ( ADDRESS i, char * buf );


aptr addr_funcs[15] =
{
  addr_err,
  addrImmediate,
  addrFull,
  addrZeroPage,
  addrNone,
  addrRelative,
  addrIndirect,
  addrIndZeroPageX,
  addrIndZeroPageY,
  addrIndX,
  addrIndY,
  addrIndIndirect,
  addrIndirectInd,
  addrAcc,
  addrZeroPageRel
};

char    tb[20];

BYTE    huge * code;
ADDRESS absAddress, endAddress;
long    fileLen;

iType   instrType;
int     instruction;
int     indexedAddr;
WORD    operand, operand1;

char * word_to_str ( WORD w )
{
  sprintf(tb, "$%04X", w);
  return tb;
};

inline WORD getWord( ADDRESS i )
{
  i -= absAddress;
  return code[i] + ( (WORD)code[i+1] << 8);
};

inline BYTE getByte( ADDRESS i )
{
  return code[i - absAddress];
};


char * gen_word( ADDRESS i )
{
  return word_to_str( operand = getWord(i) );
};

char * gen_byte( ADDRESS i )
{
  sprintf(tb, "$%02X", operand = getByte(i));
  return tb;
};

char * genOperand ( ADDRESS i )
{
  label * l;

  if (l = searchLabel(i))
  {
    return referLabel(l);
  };
  return word_to_str(i);
};

#pragma argsused

int addr_err ( ADDRESS i, char * buf )
{
  return 1;
};

int addrImmediate ( ADDRESS i, char * buf )
{
  strcat( strcat(buf, "\t#"), gen_byte( i ) );
  return 2;
};

int addrFull ( ADDRESS i, char * buf )
{
  strcat(strcat(buf, "\t"), genOperand( getWord( i ) ) );
  return 3;
};

int addrZeroPage ( ADDRESS i, char * buf )
{
  strcat( strcat(buf, "\t"), genOperand( getByte( i ) ) );
  return 2;
};


#pragma argsused

int addrNone ( ADDRESS i, char * buf )
{
  return 1;
};


int addrRelative ( ADDRESS i, char * buf )
{
  strcat( strcat(buf, "\t"), genOperand(i + 1 + (SBYTE)getByte(i)) );
  return 2;
};

int addrIndirect ( ADDRESS i, char * buf )
{
  strcat( strcat( strcat(buf, "\t("), genOperand(getWord( i )) ), ")" );
  return 3;
};

int addrIndZeroPageX ( ADDRESS i, char * buf )
{
  strcat( strcat( strcat( buf, "\t"), genOperand(getByte( i )) ), ",X" );
  return 2;
};

int addrIndZeroPageY ( ADDRESS i, char * buf )
{
  strcat( strcat( strcat( buf, "\t"), genOperand(getByte( i )) ), ",Y" );
  return 2;
};

int addrIndX ( ADDRESS i, char * buf )
{
  strcat( strcat( strcat( buf, "\t"), genOperand(getWord( i )) ), ",X" );
  return 3;
};

int addrIndY ( ADDRESS i, char * buf )
{
  strcat( strcat( strcat( buf, "\t"), genOperand(getWord( i )) ), ",Y" );
  return 3;
};

int addrIndIndirect ( ADDRESS i, char * buf )
{
  strcat( strcat( strcat( buf, "\t("), genOperand(getByte( i )) ), ",X)" );
  return 2;
};

int addrIndirectInd ( ADDRESS i, char * buf )
{
  strcat( strcat( strcat( buf, "\t("), genOperand(getByte( i )) ), "),Y" );
  return 2;
};

#pragma argsused

int addrAcc ( ADDRESS i, char * buf )
{
  strcat( buf, "\tA" );
  return 1;
};

int addrZeroPageRel ( ADDRESS i, char * buf )
{
  strcat( buf, "\t" );
  strcat( buf, genOperand(getByte(i)) );
  strcat( buf, "," );
  strcat( buf, genOperand(i + 2 + (SBYTE)getByte(i+1)) );
  return 3;
};

int get_instr ( ADDRESS i, char * buf )
{
  strcpy( buf, inames[coms[instruction = getByte(i)].inum] );
  int res = (*addr_funcs[coms[instruction].addr])(i+1, buf);
  if (coms[instruction].it == MemJump)
    strcat(buf, "\t;!1");
  return res;
};

//****************************************************
int addr_byte1 ( ADDRESS i );
int addr_word1 ( ADDRESS i );
int addr_rel1 ( ADDRESS i );
int addr_zprel1 ( ADDRESS i );
int addr_no1 ( ADDRESS i );
int addr_err1 ( ADDRESS i );

typedef int (*aptr1)( ADDRESS i );


aptr1 addr_funcs_pass1[15] =
{
  addr_err1,
  addr_byte1,
  addr_word1,
  addr_byte1,
  addr_no1,
  addr_rel1,
  addr_word1,
  addr_byte1,
  addr_byte1,
  addr_word1,
  addr_word1,
  addr_byte1,
  addr_byte1,
  addr_no1,
  addr_zprel1
};

#pragma argsused

int addr_no1 ( ADDRESS i )
{
  return 1;
};

#pragma argsused

int addr_err1 ( ADDRESS i )
{
  return 1;
};


int addr_byte1 ( ADDRESS i )
{
  operand = getByte(i);
  return 2;
};

int addr_word1 ( ADDRESS i )
{
  operand = getWord(i);
  return 3;
};

int addr_rel1 ( ADDRESS i )
{
  operand = i + 1 + (SBYTE)getByte(i);
  return 2;
};

int addr_zprel1 ( ADDRESS i )
{
  operand1 = getByte(i);
  operand  = i + 2 + (SBYTE)getByte(i+1);
  return 3;
};


int get_instr_pass1(ADDRESS i)
{
  instruction = getByte(i++);
  instrType   = coms[instruction].it;
  indexedAddr = coms[instruction].addr >= AIndZeroPageX &&
                coms[instruction].addr <= AIndirectInd;
  return (*addr_funcs_pass1[coms[instruction].addr])(i);
};