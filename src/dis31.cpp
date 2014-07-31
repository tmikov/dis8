#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <stdio.h>

#include "compat.h"
#include "dis31.h"

char Strings[][6] =
{
  "ADD",  // 0
  "ADDC",  // 1
  "SUBB",  // 2
  "SJMP",  // 3
  "LJMP",  // 4
  "AJMP",  // 5
  "INC",  // 6
  "MOV",  // 7
  "DEC",  // 8
  "MUL",  // 9
  "DIV",  // 10
  "DA",  // 11
  "ANL",  // 12
  "ORL",  // 13
  "XRL",  // 14
  "CLR",  // 15
  "CPL",  // 16
  "RL",  // 17
  "RLC",  // 18
  "RR",  // 19
  "RRC",  // 20
  "SWAP",  // 21
  "MOVC",  // 22
  "MOVX",  // 23
  "PUSH",  // 24
  "POP",  // 25
  "XCH",  // 26
  "XCHD",  // 27
  "ACALL",  // 28
  "LCALL",  // 29
  "RET",  // 30
  "RETI",  // 31
  "JMP",  // 32
  "JZ",  // 33
  "JNZ",  // 34
  "JC",  // 35
  "JNC",  // 36
  "JB",  // 37
  "JNB",  // 38
  "JBC",  // 39
  "CJNE",  // 40
  "DJNZ",  // 41
  "NOP",  // 42
  "SETB"   // 43
};

static TIType Types[] =
{
  ITOther, // "ADD",  // 0
  ITOther, // "ADDC",  // 1
  ITOther, // "SUBB",  // 2
  ITJump,  // "SJMP",  // 3
  ITJump , // "LJMP",  // 4
  ITJump , // "AJMP",  // 5
  ITOther, // "INC",  // 6
  ITOther, // "MOV",  // 7
  ITOther, // "DEC",  // 8
  ITOther, // "MUL",  // 9
  ITOther, // "DIV",  // 10
  ITOther, // "DA",  // 11
  ITOther, // "ANL",  // 12
  ITOther, // "ORL",  // 13
  ITOther, // "XRL",  // 14
  ITOther, // "CLR",  // 15
  ITOther, // "CPL",  // 16
  ITOther, // "RL",  // 17
  ITOther, // "RLC",  // 18
  ITOther, // "RR",  // 19
  ITOther, // "RRC",  // 20
  ITOther, // "SWAP",  // 21
  ITOther, // "MOVC",  // 22
  ITOther, // "MOVX",  // 23
  ITOther, // "PUSH",  // 24
  ITOther, // "POP",  // 25
  ITOther, // "XCH",  // 26
  ITOther, // "XCHD",  // 27
  ITCall,  // "ACALL",  // 28
  ITCall,  // "LCALL",  // 29
  ITRet,   // "RET",  // 30
  ITRet,   // "RETI",  // 31
  ITJump,  // "JMP",  // 32
  ITCondJump, // "JZ",  // 33
  ITCondJump, // "JNZ",  // 34
  ITCondJump, // "JC",  // 35
  ITCondJump, // "JNC",  // 36
  ITCondJump, // "JB",  // 37
  ITCondJump, // "JNB",  // 38
  ITCondJump, // "JBC",  // 39
  ITCondJump, // "CJNE",  // 40
  ITCondJump, // "DJNZ",  // 41
  ITCondJump, // "NOP",  // 42
  ITCondJump  // "SETB"   // 43
};

#define CODE_MOV_RAM_RAM     0x85     // Special case, handled separately

TIDesc IDesc[256] =
{
  {  42, ANone, ANone, ANone }, // 0
  {   5, AJmpPage, ANone, ANone }, // 1
  {   4, AJmpExtended, ANone, ANone }, // 2
  {  19, AAcc, ANone, ANone }, // 3
  {   6, AAcc, ANone, ANone }, // 4
  {   6, ARAMDirect, ANone, ANone }, // 5
  {   6, AIndexShort, ANone, ANone }, // 6
  {   6, AIndexShort, ANone, ANone }, // 7
  {   6, ARegister, ANone, ANone }, // 8
  {   6, ARegister, ANone, ANone }, // 9
  {   6, ARegister, ANone, ANone }, // 10
  {   6, ARegister, ANone, ANone }, // 11
  {   6, ARegister, ANone, ANone }, // 12
  {   6, ARegister, ANone, ANone }, // 13
  {   6, ARegister, ANone, ANone }, // 14
  {   6, ARegister, ANone, ANone }, // 15
  {  39, ABit, AJmpRelative, ANone }, // 16
  {  28, AJmpPage, ANone, ANone }, // 17
  {  29, AJmpExtended, ANone, ANone }, // 18
  {  20, AAcc, ANone, ANone }, // 19
  {   8, AAcc, ANone, ANone }, // 20
  {   8, ARAMDirect, ANone, ANone }, // 21
  {   8, AIndexShort, ANone, ANone }, // 22
  {   8, AIndexShort, ANone, ANone }, // 23
  {   8, ARegister, ANone, ANone }, // 24
  {   8, ARegister, ANone, ANone }, // 25
  {   8, ARegister, ANone, ANone }, // 26
  {   8, ARegister, ANone, ANone }, // 27
  {   8, ARegister, ANone, ANone }, // 28
  {   8, ARegister, ANone, ANone }, // 29
  {   8, ARegister, ANone, ANone }, // 30
  {   8, ARegister, ANone, ANone }, // 31
  {  37, ABit, AJmpRelative, ANone }, // 32
  {   5, AJmpPage, ANone, ANone }, // 33
  {  30, ANone, ANone, ANone }, // 34
  {  17, AAcc, ANone, ANone }, // 35
  {   0, AAcc, AImmediate, ANone }, // 36
  {   0, AAcc, ARAMDirect, ANone }, // 37
  {   0, AAcc, AIndexShort, ANone }, // 38
  {   0, AAcc, AIndexShort, ANone }, // 39
  {   0, AAcc, ARegister, ANone }, // 40
  {   0, AAcc, ARegister, ANone }, // 41
  {   0, AAcc, ARegister, ANone }, // 42
  {   0, AAcc, ARegister, ANone }, // 43
  {   0, AAcc, ARegister, ANone }, // 44
  {   0, AAcc, ARegister, ANone }, // 45
  {   0, AAcc, ARegister, ANone }, // 46
  {   0, AAcc, ARegister, ANone }, // 47
  {  38, ABit, AJmpRelative, ANone }, // 48
  {  28, AJmpPage, ANone, ANone }, // 49
  {  31, ANone, ANone, ANone }, // 50
  {  18, AAcc, ANone, ANone }, // 51
  {   1, AAcc, AImmediate, ANone }, // 52
  {   1, AAcc, ARAMDirect, ANone }, // 53
  {   1, AAcc, AIndexShort, ANone }, // 54
  {   1, AAcc, AIndexShort, ANone }, // 55
  {   1, AAcc, ARegister, ANone }, // 56
  {   1, AAcc, ARegister, ANone }, // 57
  {   1, AAcc, ARegister, ANone }, // 58
  {   1, AAcc, ARegister, ANone }, // 59
  {   1, AAcc, ARegister, ANone }, // 60
  {   1, AAcc, ARegister, ANone }, // 61
  {   1, AAcc, ARegister, ANone }, // 62
  {   1, AAcc, ARegister, ANone }, // 63
  {  35, AJmpRelative, ANone, ANone }, // 64
  {   5, AJmpPage, ANone, ANone }, // 65
  {  13, ARAMDirect, AAcc, ANone }, // 66
  {  13, ARAMDirect, AImmediate, ANone }, // 67
  {  13, AAcc, AImmediate, ANone }, // 68
  {  13, AAcc, ARAMDirect, ANone }, // 69
  {  13, AAcc, AIndexShort, ANone }, // 70
  {  13, AAcc, AIndexShort, ANone }, // 71
  {  13, AAcc, ARegister, ANone }, // 72
  {  13, AAcc, ARegister, ANone }, // 73
  {  13, AAcc, ARegister, ANone }, // 74
  {  13, AAcc, ARegister, ANone }, // 75
  {  13, AAcc, ARegister, ANone }, // 76
  {  13, AAcc, ARegister, ANone }, // 77
  {  13, AAcc, ARegister, ANone }, // 78
  {  13, AAcc, ARegister, ANone }, // 79
  {  36, AJmpRelative, ANone, ANone }, // 80
  {  28, AJmpPage, ANone, ANone }, // 81
  {  12, ARAMDirect, AAcc, ANone }, // 82
  {  12, ARAMDirect, AImmediate, ANone }, // 83
  {  12, AAcc, AImmediate, ANone }, // 84
  {  12, AAcc, ARAMDirect, ANone }, // 85
  {  12, AAcc, AIndexShort, ANone }, // 86
  {  12, AAcc, AIndexShort, ANone }, // 87
  {  12, AAcc, ARegister, ANone }, // 88
  {  12, AAcc, ARegister, ANone }, // 89
  {  12, AAcc, ARegister, ANone }, // 90
  {  12, AAcc, ARegister, ANone }, // 91
  {  12, AAcc, ARegister, ANone }, // 92
  {  12, AAcc, ARegister, ANone }, // 93
  {  12, AAcc, ARegister, ANone }, // 94
  {  12, AAcc, ARegister, ANone }, // 95
  {  33, AJmpRelative, ANone, ANone }, // 96
  {   5, AJmpPage, ANone, ANone }, // 97
  {  14, ARAMDirect, AAcc, ANone }, // 98
  {  14, ARAMDirect, AImmediate, ANone }, // 99
  {  14, AAcc, AImmediate, ANone }, // 100
  {  14, AAcc, ARAMDirect, ANone }, // 101
  {  14, AAcc, AIndexShort, ANone }, // 102
  {  14, AAcc, AIndexShort, ANone }, // 103
  {  14, AAcc, ARegister, ANone }, // 104
  {  14, AAcc, ARegister, ANone }, // 105
  {  14, AAcc, ARegister, ANone }, // 106
  {  14, AAcc, ARegister, ANone }, // 107
  {  14, AAcc, ARegister, ANone }, // 108
  {  14, AAcc, ARegister, ANone }, // 109
  {  14, AAcc, ARegister, ANone }, // 110
  {  14, AAcc, ARegister, ANone }, // 111
  {  34, AJmpRelative, ANone, ANone }, // 112
  {  28, AJmpPage, ANone, ANone }, // 113
  {  13, ACFlag, ABit, ANone }, // 114
  {  32, AJmpIndexFullOffset, ANone, ANone }, // 115
  {   7, AAcc, AImmediate, ANone }, // 116
  {   7, ARAMDirect, AImmediate, ANone }, // 117
  {   7, AIndexShort, AImmediate, ANone }, // 118
  {   7, AIndexShort, AImmediate, ANone }, // 119
  {   7, ARegister, AImmediate, ANone }, // 120
  {   7, ARegister, AImmediate, ANone }, // 121
  {   7, ARegister, AImmediate, ANone }, // 122
  {   7, ARegister, AImmediate, ANone }, // 123
  {   7, ARegister, AImmediate, ANone }, // 124
  {   7, ARegister, AImmediate, ANone }, // 125
  {   7, ARegister, AImmediate, ANone }, // 126
  {   7, ARegister, AImmediate, ANone }, // 127
  {   3, AJmpRelative, ANone, ANone }, // 128
  {   5, AJmpPage, ANone, ANone }, // 129
  {  12, ACFlag, ABit, ANone }, // 130
  {  22, AAcc, AIndexRelOffset, ANone }, // 131
  {  10, ANone, ANone, ANone }, // 132
  {   7, ARAMDirect, ARAMDirect, ANone }, // 133
  {   7, ARAMDirect, AIndexShort, ANone }, // 134
  {   7, ARAMDirect, AIndexShort, ANone }, // 135
  {   7, ARAMDirect, ARegister, ANone }, // 136
  {   7, ARAMDirect, ARegister, ANone }, // 137
  {   7, ARAMDirect, ARegister, ANone }, // 138
  {   7, ARAMDirect, ARegister, ANone }, // 139
  {   7, ARAMDirect, ARegister, ANone }, // 140
  {   7, ARAMDirect, ARegister, ANone }, // 141
  {   7, ARAMDirect, ARegister, ANone }, // 142
  {   7, ARAMDirect, ARegister, ANone }, // 143
  {   7, ADPTR, AImmExtended, ANone }, // 144
  {  28, AJmpPage, ANone, ANone }, // 145
  {   7, ABit, ACFlag, ANone }, // 146
  {  22, AAcc, AIndexFullOffset, ANone }, // 147
  {   2, AAcc, AImmediate, ANone }, // 148
  {   2, AAcc, ARAMDirect, ANone }, // 149
  {   2, AAcc, AIndexShort, ANone }, // 150
  {   2, AAcc, AIndexShort, ANone }, // 151
  {   2, AAcc, ARegister, ANone }, // 152
  {   2, AAcc, ARegister, ANone }, // 153
  {   2, AAcc, ARegister, ANone }, // 154
  {   2, AAcc, ARegister, ANone }, // 155
  {   2, AAcc, ARegister, ANone }, // 156
  {   2, AAcc, ARegister, ANone }, // 157
  {   2, AAcc, ARegister, ANone }, // 158
  {   2, AAcc, ARegister, ANone }, // 159
  {  13, ACFlag, ANotBit, ANone }, // 160
  {   5, AJmpPage, ANone, ANone }, // 161
  {   7, ACFlag, ABit, ANone }, // 162
  {   6, ADPTR, ANone, ANone }, // 163
  {   9, ANone, ANone, ANone }, // 164
  {   0, AUnknown, AUnknown, AUnknown }, // 165
  {   7, AIndexShort, ARAMDirect, ANone }, // 166
  {   7, AIndexShort, ARAMDirect, ANone }, // 167
  {   7, ARegister, ARAMDirect, ANone }, // 168
  {   7, ARegister, ARAMDirect, ANone }, // 169
  {   7, ARegister, ARAMDirect, ANone }, // 170
  {   7, ARegister, ARAMDirect, ANone }, // 171
  {   7, ARegister, ARAMDirect, ANone }, // 172
  {   7, ARegister, ARAMDirect, ANone }, // 173
  {   7, ARegister, ARAMDirect, ANone }, // 174
  {   7, ARegister, ARAMDirect, ANone }, // 175
  {  12, ACFlag, ANotBit, ANone }, // 176
  {  28, AJmpPage, ANone, ANone }, // 177
  {  16, ABit, ANone, ANone }, // 178
  {  16, ACFlag, ANone, ANone }, // 179
  {  40, AAcc, AImmediate, AJmpRelative }, // 180
  {  40, AAcc, ARAMDirect, AJmpRelative }, // 181
  {  40, AIndexShort, AImmediate, AJmpRelative }, // 182
  {  40, AIndexShort, AImmediate, AJmpRelative }, // 183
  {  40, ARegister, AImmediate, AJmpRelative }, // 184
  {  40, ARegister, AImmediate, AJmpRelative }, // 185
  {  40, ARegister, AImmediate, AJmpRelative }, // 186
  {  40, ARegister, AImmediate, AJmpRelative }, // 187
  {  40, ARegister, AImmediate, AJmpRelative }, // 188
  {  40, ARegister, AImmediate, AJmpRelative }, // 189
  {  40, ARegister, AImmediate, AJmpRelative }, // 190
  {  40, ARegister, AImmediate, AJmpRelative }, // 191
  {  24, ARAMDirect, ANone, ANone }, // 192
  {   5, AJmpPage, ANone, ANone }, // 193
  {  15, ABit, ANone, ANone }, // 194
  {  15, ACFlag, ANone, ANone }, // 195
  {  21, AAcc, ANone, ANone }, // 196
  {  26, AAcc, ARAMDirect, ANone }, // 197
  {  26, AAcc, AIndexShort, ANone }, // 198
  {  26, AAcc, AIndexShort, ANone }, // 199
  {  26, AAcc, ARegister, ANone }, // 200
  {  26, AAcc, ARegister, ANone }, // 201
  {  26, AAcc, ARegister, ANone }, // 202
  {  26, AAcc, ARegister, ANone }, // 203
  {  26, AAcc, ARegister, ANone }, // 204
  {  26, AAcc, ARegister, ANone }, // 205
  {  26, AAcc, ARegister, ANone }, // 206
  {  26, AAcc, ARegister, ANone }, // 207
  {  25, ARAMDirect, ANone, ANone }, // 208
  {  28, AJmpPage, ANone, ANone }, // 209
  {  43, ABit, ANone, ANone }, // 210
  {  43, ACFlag, ANone, ANone }, // 211
  {  11, AAcc, ANone, ANone }, // 212
  {  41, ARAMDirect, AJmpRelative, ANone }, // 213
  {  27, AAcc, AIndexShort, ANone }, // 214
  {  27, AAcc, AIndexShort, ANone }, // 215
  {  41, ARegister, AJmpRelative, ANone }, // 216
  {  41, ARegister, AJmpRelative, ANone }, // 217
  {  41, ARegister, AJmpRelative, ANone }, // 218
  {  41, ARegister, AJmpRelative, ANone }, // 219
  {  41, ARegister, AJmpRelative, ANone }, // 220
  {  41, ARegister, AJmpRelative, ANone }, // 221
  {  41, ARegister, AJmpRelative, ANone }, // 222
  {  41, ARegister, AJmpRelative, ANone }, // 223
  {  23, AAcc, AIndexFull, ANone }, // 224
  {   5, AJmpPage, ANone, ANone }, // 225
  {  23, AAcc, AIndexShort, ANone }, // 226
  {  23, AAcc, AIndexShort, ANone }, // 227
  {  15, AAcc, ANone, ANone }, // 228
  {   7, AAcc, ARAMDirect, ANone }, // 229
  {   7, AAcc, AIndexShort, ANone }, // 230
  {   7, AAcc, AIndexShort, ANone }, // 231
  {   7, AAcc, ARegister, ANone }, // 232
  {   7, AAcc, ARegister, ANone }, // 233
  {   7, AAcc, ARegister, ANone }, // 234
  {   7, AAcc, ARegister, ANone }, // 235
  {   7, AAcc, ARegister, ANone }, // 236
  {   7, AAcc, ARegister, ANone }, // 237
  {   7, AAcc, ARegister, ANone }, // 238
  {   7, AAcc, ARegister, ANone }, // 239
  {  23, AIndexFull, AAcc, ANone }, // 240
  {  28, AJmpPage, ANone, ANone }, // 241
  {  23, AIndexShort, AAcc, ANone }, // 242
  {  23, AIndexShort, AAcc, ANone }, // 243
  {  16, AAcc, ANone, ANone }, // 244
  {   7, ARAMDirect, AAcc, ANone }, // 245
  {   7, AIndexShort, AAcc, ANone }, // 246
  {   7, AIndexShort, AAcc, ANone }, // 247
  {   7, ARegister, AAcc, ANone }, // 248
  {   7, ARegister, AAcc, ANone }, // 249
  {   7, ARegister, AAcc, ANone }, // 250
  {   7, ARegister, AAcc, ANone }, // 251
  {   7, ARegister, AAcc, ANone }, // 252
  {   7, ARegister, AAcc, ANone }, // 253
  {   7, ARegister, AAcc, ANone }, // 254
  {   7, ARegister, AAcc, ANone }  // 255
};


char ResultBuffer[256];
static BOOL PrevOperand;   // TRUE if we have added an operand to the string buffer
static int  OpOffset;  // Offset from first instruction byte of the next
                       // operand to be processed
ADDRESS AbsAddress, EndAddress; // Address of start of code
BYTE huge * Code;
long FileLength;

TAddrMode Op1AM, Op2AM, Op3AM;
WORD Operand1, Operand2, Operand3;
int    InstrCode;
TIType InstrType;

static char * AddToResult ( const char * s )
{
  return strcat( ResultBuffer, s );
};

static char * AddChToResult ( char c )
{
  char * s;
  s = strchr( ResultBuffer, 0 );
  s[0] = c;
  s[1] = 0;

  return ResultBuffer;
};

static void CheckPrevOperand ( void )
{
  AddChToResult( PrevOperand ? ',' : '\t' );
  PrevOperand = TRUE;
};


static char * AddByteStr ( BYTE b )
{
  sprintf( strchr( ResultBuffer, 0 ), "%02X", b );
  return ResultBuffer;
};

static char * AddWordStr ( WORD w )
{
  sprintf( strchr( ResultBuffer, 0 ), "%04X", w );
  return ResultBuffer;
};

static WORD PageProcess ( const BYTE * code, ADDRESS aPC )
{
  ++OpOffset;
  return (((WORD)code[0] << 3) & 0x0700) | code[OpOffset-1] |
         ((aPC + 2) & 0xF800);
};

//---------------------------------------------------------------------------
// Name           ProcessOperand
//
// Description    Processes a single operand.
//                code points to the first byte of the instruction.
//                OpOffset contains offset of next explicit operand to be
//                processed.
//                addrMode is the addressation mode of the operand
//                Adds the appropriate string value to ResultBuffer
//
//      On exit corrects OpOffset to point to the next operand (if necessarry)
//---------------------------------------------------------------------------
static void ProcessOperand ( const BYTE * code,  TAddrMode addrMode,
                             ADDRESS aPC )
{
  if (addrMode == ANone)
    return;

  CheckPrevOperand();
  switch ( addrMode )
  {
    case ACFlag:
      AddChToResult( 'C' );
      break;

    case ADPTR:
      AddToResult( "DPTR" );
      break;

    case AImmediate:
      AddChToResult( '#' );
      AddByteStr( code[OpOffset] );
      ++OpOffset;
      break;

    case ANotBit:
      AddChToResult( '/' );
    case ARAMDirect:
    case ABit:
      AddByteStr( code[OpOffset] );
      ++OpOffset;
      break;

    case AAcc:
      AddChToResult( 'A' );
      break;

    case ARegister:
      AddChToResult( 'R' );
      AddChToResult( (code[0] & 7) + '0' );
      break;

    case AIndexShort:
      AddToResult( code[0] & 1 ? "@R1" : "@R0" );
      break;

    case AIndexFull:
      AddToResult( "@DPTR" );
      break;

    case AJmpIndexFullOffset:
    case AIndexFullOffset:
      AddToResult( "@A+DPTR" );
      break;

    case AIndexRelOffset:
      AddToResult( "@A+PC" );
      break;

    case AJmpPage:
      AddWordStr( PageProcess( code, aPC ) );
      break;

    case AImmExtended:
    case AJmpExtended:
      AddWordStr( (code[OpOffset] << 8) + code[OpOffset + 1] );
      OpOffset += 2;
      break;

    case AJmpRelative:
      AddWordStr( aPC + OpOffset + 1 + ((SBYTE)code[OpOffset]) );
      ++OpOffset;
      break;

    default:
      assert( 0 );
  }
};

//---------------------------------------------------------------------------
// Name           DisassembleSingle
//
// Description    Disassembles a signle instruction at address aPC.
//                Returns instruction size.
//                The disassembled instruction string is stored in
//                ResultBuffer.
//
// 03.03.1996 Cendy  "mov x,y" was disasembled as "mov y,x"
//---------------------------------------------------------------------------
int DisassembleSingle ( ADDRESS aPC )
{
  const TIDesc * iDesc;
  const BYTE   * code;

  code = (BYTE *)(Code + aPC - AbsAddress);
  iDesc = IDesc + *code;

  OpOffset = 1;
  PrevOperand = FALSE;

  if (iDesc->AM1 == AUnknown)
    strcpy( ResultBuffer, "???" );
  else
  {
    strcpy( ResultBuffer, Strings[iDesc->String] );
//
// 03.03.1996 Cendy
// Patch(bug fix) for mov ram,ram: (The operands must be swapped)
//
    if (*code == CODE_MOV_RAM_RAM)
    {
      OpOffset = 2;
      ProcessOperand( code, iDesc->AM2, aPC );
      OpOffset = 1;
      ProcessOperand( code, iDesc->AM1, aPC );
      OpOffset = 3;
    }
    else
    if (iDesc->AM1 != ANone)
    {
      ProcessOperand( code, iDesc->AM1, aPC );
      if (iDesc->AM2 != ANone)
      {
        ProcessOperand( code, iDesc->AM2, aPC );
        if (iDesc->AM3 != ANone)
          ProcessOperand( code, iDesc->AM3, aPC );
      }
    }
  }
  return OpOffset;
};

//---------------------------------------------------------------------------
// Name           ParseOperand
//
// Description    Parses an operand and modifies OpOffset to point to the
//                offset of the next operand. Returns the value of the
//                operand if necessarry.
//---------------------------------------------------------------------------
static WORD ParseOperand ( const BYTE * code, TAddrMode addrMode,
                           ADDRESS aPC )
{
  switch ( addrMode )
  {
    case ANone:
    case AAcc:
    case ACFlag:
    case ADPTR:
    case AIndexFull:
    case AJmpIndexFullOffset:
    case AIndexFullOffset:
    case AIndexRelOffset:
      break;

    case AImmediate:
    case ARAMDirect:
    case ABit:
    case ANotBit:
      return code[OpOffset++];


    case ARegister:
      return code[0] & 7;

    case AIndexShort:
      return code[0] & 1;

    case AJmpPage:
      return PageProcess( code, aPC );

    case AImmExtended:
    case AJmpExtended:
      OpOffset += 2;
      return (code[OpOffset-2] << 8) + code[OpOffset - 1];

    case AJmpRelative:
      ++OpOffset;
      return aPC + OpOffset + ((SBYTE)code[OpOffset-1]);

    default:
      assert( 0 );
  }
  return 0;
};

//---------------------------------------------------------------------------
// Name           ParseSingle
//
// Description    Parses a single instruction at address aPC.
//                Returns the size of the instruction.
//                Sets InstrCode to the index of instruction in Strings
//                and Op1AM, Op2AM, Op3AM and Operand1, Operand2, Operand3.
//                Sets InstrCode to -1 if invalid instruction
//
// 03.03.1996 Cendy  "mov ram1,ram2" was disasembled as "mov ram2,ram1"
//---------------------------------------------------------------------------
int ParseSingle ( ADDRESS aPC )
{
  const TIDesc * iDesc;
  const BYTE   * code;

  code = (BYTE *)(Code + aPC - AbsAddress);
  iDesc = IDesc + *code;

  OpOffset = 1;

  Op1AM = Op2AM = Op3AM = ANone;
  Operand1 = Operand2 = Operand3 = 0;

  if (iDesc->AM1 == AUnknown)
  {
    InstrType = ITUnknown;
    InstrCode = -1;
    Op1AM = AUnknown;
  }
  else
  {
    InstrCode = iDesc->String;
    InstrType = Types[InstrCode];
    if ((Op1AM = iDesc->AM1) != ANone)
    {
      Operand1 = ParseOperand( code, Op1AM, aPC );
      if ((Op2AM = iDesc->AM2) != ANone)
      {
        Operand2 = ParseOperand( code, Op2AM, aPC );
        if ((Op3AM = iDesc->AM3) != ANone)
          Operand3 = ParseOperand( code, Op3AM, aPC );
      }
    }
  }
//
// 03.03.1996 Cendy
// Patch(bug fix) for mov ram,ram: (The operands must be swapped)
//
  if (*code == CODE_MOV_RAM_RAM)
  {
    WORD t = Operand1;
    Operand1 = Operand2;
    Operand2 = t;
  }
  return OpOffset;
};

