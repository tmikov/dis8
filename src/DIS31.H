/*
 Description
   Definitions for 8031 disassembler: addressing modes, etc.

  History:
  -----------------------------------------------------------------------------
  08.01.1996 Cendy  Started

*/

typedef unsigned char BYTE;
typedef signed char   SBYTE;
typedef unsigned int  WORD;
typedef int           BOOL;
typedef WORD          ADDRESS;

#define TRUE          1
#define FALSE         0

#define NELEM( x )   (sizeof( x ) / sizeof( (x)[0] ))

// This enum defines the addressing modes of one operand of the instruction
// For example:  mov A, @R0 has the following AddrModes:
//    AAcc, AIndexShort
//
enum TAddrMode
{
  AUnknown,
  ABit,               // bit memory
  ANotBit,
  ACFlag,             // the carry bit
  ANone,              // RET, RETI
  AAcc,               // e.g. RL A
  ADPTR,              // e.g. mov DPTR,#100, inc DPTR
  AImmediate,
  ARegister,
  ARAMDirect,         // in chip RAM
  AIndexShort,        // @R0
  AIndexFull,         // movx A,@DPTR
  AIndexFullOffset,   // movc A,@A+DPTR
  AIndexRelOffset,    // for movc A,@A+PC

  AJmpIndexFullOffset,// jmp @A+DPTR
  AJmpPage,           // ajmp, acall
  AJmpRelative,       // jcond, sjmp
  AJmpExtended,       // ljmp, lcall

  AImmExtended        // mov dptr,#
};

struct TIDesc
{
  BYTE String;
  TAddrMode AM1, AM2, AM3;
};

enum TIType
{
  ITUnknown,
  ITCondJump,
  ITJump,
  ITCall,
  ITRet,
  ITOther
};

extern char ResultBuffer[];
int DisassembleSingle ( ADDRESS aPC );
int ParseSingle ( ADDRESS aPC );

extern ADDRESS AbsAddress, EndAddress;
extern BYTE huge * Code;
extern long   FileLength;
extern TAddrMode Op1AM, Op2AM, Op3AM;
extern WORD   Operand1, Operand2, Operand3;
extern int    InstrCode;
extern TIType InstrType;
extern char   Strings[][6];

#define INSTR_MUL 9
#define INSTR_DIV 10
