#include "dis31.h"

#define PARSED     0
#define NOTPARSED  1

enum labType  //Ordered by priority
{
  LabSegment, // Marks the beginning of a segment
  LabSecond,  // If this is the second BYTE of the prev WORD label
  LabBit,
  LabEData,
  LabData,
  LabTab,
  LabJmpTab,
  LabLoc,
  LabSub
};

enum labSize
{
  LabBitSize,
  LabByte,
  LabWord
};

enum labMemory
{
  LabCodeMemory,
  LabIRAM,
  LabXRAM,
  LabBitRAM
};

struct label
{
  ADDRESS  addr;
  labType   type : 5;
  int       subNoRet : 1;  // TRUE if the label is a subroutine without ret
  labSize   size : 3;
  labMemory memory : 3;
  int      referenced : 1;
  int      generated  : 1;
  int      notParsed  : 1;
  unsigned num;
  WORD     len;
  char     * name;
};

struct segment
{
  ADDRESS from, to;
};

struct labelList
{
  label   * labels;
  unsigned freeLabel;
};

#define MAXSEGS    1000
#define MAXENTRIES 2000
#define MAXCODELABELS  (0xFF00 / sizeof(label))
#define MAXXRAMLABELS  MAXCODELABELS
#define MAXIRAMLABELS  1024
#define MAXBITRAMLABELS 256

#define MAX(a, b)  ((a)>(b) ? (a) : (b))
#define MIN(a, b)  ((a)<(b) ? (a) : (b))
#define ODD(x)     ((x) & 1)
#define EVEN(x)    (!ODD(x))
