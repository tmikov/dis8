/*
 Description
   Source for DIS8031 sourcer module

  History:
  -----------------------------------------------------------------------------
  09.01.1996 Cendy  Copied & corrected from dis65c02

*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include "sr.h"

#pragma warn -pia

//*** from MAIN.CPP *****************************
extern void     out(char * mess ...);
extern char     inFileName[];
extern ADDRESS  startAddress;
extern BOOL     strictJumps;

static BOOL unknownInstr;


segment * codeSegs;
unsigned  freeSeg;

ADDRESS * entries;
unsigned  freeEntry;

labelList codeLabels;
labelList iRamLabels;
labelList xRamLabels;
labelList bitRamLabels;

labelList * memories[4] =
{
  &codeLabels,
  &iRamLabels,
  &xRamLabels,
  &bitRamLabels
};

static char      startLabel[] = "START";

static char buf[32];
static char lineBuf[128];

//---------------------------------------------------------------------------
// Name           genLabSize
//
// Description    Returns a pointer to label size description
//---------------------------------------------------------------------------
static char * genLabSize ( label * l )
{
  int s = l->size;
  switch (s)
  {
    case LabByte:
      return "BYTE";
    case LabWord:
      return "WORD";
    case LabBitSize:
      return "BIT";
  }
  assert( 0 );
  return NULL;
};

//---------------------------------------------------------------------------
// Name           genLabel
//
// Description    returns a pointer to label name
//---------------------------------------------------------------------------
static char * genLabel ( label * l )
{
  static char Prefixes[][4][16] =
  {
    { "###_",    "###_",     "###_",     "###_" },
    { "###_",    "###_",     "###_",     "###_" },
    { "###_",    "###_",     "###_",     "bit_" },
    { "edata_",  "idata_",   "xdata_",   "###_" },
    { "data_",   "###_",     "###_",     "###_" },
    { "tab_",    "itab_",    "xtab_",    "###_" },
    { "jmptab_", "ijmptab_", "xjmptab_", "###_" },
    { "loc_",    "###_",     "###_",     "###_" },
    { "sub_",    "###_",     "###_",     "###_" }
  };

  if (l->name)
    return l->name;
  if (l->type == LabSecond)
    strcat( genLabel( l-1 ), "+1" );
  else
  {
    assert( l->type < NELEM( Prefixes ) && l->memory < NELEM( Prefixes[0] ) );
    ltoa( l->num, stpcpy( buf, Prefixes[l->type][l->memory] ), 10 );
  }
  return buf;
};



//---------------------------------------------------------------------------
// Name           useLabel
//
// Description    Generate the label name and mark it as generated
//---------------------------------------------------------------------------
char * useLabel ( label * l )
{
  l->generated = 1;
  return genLabel( l );
};

//---------------------------------------------------------------------------
// Name           referLabel
//
// Description    Reference label.
//---------------------------------------------------------------------------
char * referLabel ( label * l )
{
  l->referenced = 1;
  return genLabel( l );
};


//---------------------------------------------------------------------------
// Name           initAll
//
// Description    Allocate memory for all buffers
//---------------------------------------------------------------------------
void initAll ( void )
{
  codeSegs = new segment[MAXSEGS];
  entries  = new ADDRESS[MAXENTRIES];
  codeLabels.labels  = new label[MAXCODELABELS];
  codeLabels.maxLabels = MAXCODELABELS;
  xRamLabels.labels  = new label[MAXXRAMLABELS];
  xRamLabels.maxLabels = MAXXRAMLABELS;
  iRamLabels.labels  = new label[MAXIRAMLABELS];
  iRamLabels.maxLabels = MAXIRAMLABELS;
  bitRamLabels.labels  = new label[MAXBITRAMLABELS];
  bitRamLabels.maxLabels = MAXBITRAMLABELS;
  freeSeg =
  freeEntry =
  codeLabels.freeLabel =
  iRamLabels.freeLabel =
  xRamLabels.freeLabel =
  bitRamLabels.freeLabel = 0;
};

//---------------------------------------------------------------------------
// Name           pushLabel
//
// Description    Add a label to a label list
//---------------------------------------------------------------------------
unsigned pushLabel ( ADDRESS addr, labType t, labMemory m, labSize s,
                 char * name, int notParsed, int subNoRet )
{
  labelList * l = memories[m];
  unsigned res;

  if (l->freeLabel == l->maxLabels)
  {
    fprintf( stderr, "Out of label space\n" );
    abort();
  }

  res = l->freeLabel++;

  l->labels[res].referenced =
  l->labels[res].generated  = 0;
  l->labels[res].notParsed  = notParsed;
  l->labels[res].name       = name;
  l->labels[res].addr       = addr;
  l->labels[res].type       = t;
  l->labels[res].subNoRet   = subNoRet;
  l->labels[res].memory     = m;
  l->labels[res].size     = s;

  return res;
};

//---------------------------------------------------------------------------
// Name           findLabel
//
// Description    Find a label for an address in the label list.
// NOTE:          Returns number starting from 1 (not index).
//                Returns 0 if label not found.
//---------------------------------------------------------------------------
int findLabel ( labelList * l, ADDRESS addr )
{
  for(int i = 0; i < l->freeLabel; ++i)
    if (addr == l->labels[i].addr)
      return i+1;
  return 0;
};

//---------------------------------------------------------------------------
// Name           addLabName
//
// Description    If the label is not already defined define it with
//                pushLabel and return 0.
//                Assign _name_ to label's name.
//                Returns the number of the label or 0 if the label
//                has not beed defined.
//---------------------------------------------------------------------------
int addLabName ( ADDRESS addr, labType t, labMemory m, labSize s, char * name,
                 int notParsed, int subNoRet )
{
  labelList * l = memories[m];
  int num;
  if ((num = findLabel( l, addr )) == 0)
  {
    pushLabel( addr, t, m, s, name, notParsed, subNoRet );
    return 0;
  };
// Check if one label is referenced in different ways
// and determine its type
  --num;   // Convert to index

  if (t > l->labels[num].type )
    l->labels[num].type = t;
  if (s > l->labels[num].size)
    l->labels[num].size = s;
  if (name && !l->labels[num].name)
    l->labels[num].name = name;

  if (subNoRet)
    l->labels[num].subNoRet = subNoRet;

  return num+1;
};

//---------------------------------------------------------------------------
// Name           addLabel
//
// Description    Add a parsed label.
//---------------------------------------------------------------------------
int addLabel ( ADDRESS addr, labType t, labMemory m, labSize s )
{
  return addLabName( addr, t, m, s, NULL, PARSED, 0 );
};


//---------------------------------------------------------------------------
// Name           addSegment
//
// Description    Add a segment to segment list.
//---------------------------------------------------------------------------
void addSegment ( ADDRESS from, ADDRESS to )
{
  if (freeSeg == MAXSEGS)
  {
    fprintf( stderr, "Out of segment space %u\n", freeSeg );
    abort();
  }

  codeSegs[freeSeg].from = from;
  codeSegs[freeSeg++].to = to;
  addLabel( from, LabSegment, LabCodeMemory, LabByte );
};

//---------------------------------------------------------------------------
// Name           inSegment
//
// Description    Returns TRUE if a label lies inside any segment.
//---------------------------------------------------------------------------
BOOL inSegment ( ADDRESS addr )
{
  for(int i = 0; i < freeSeg; ++i)
    if (addr >= codeSegs[i].from && addr <= codeSegs[i].to)
      return TRUE;
  return FALSE;
};

//---------------------------------------------------------------------------
// Name           pushEntry
//
// Description    Adds an entry to be processed to the entry list
//---------------------------------------------------------------------------
void pushEntry ( ADDRESS addr )
{
  if (freeEntry == MAXENTRIES)
  {
    fprintf( stderr, "Out of entry space\n" );
    abort();
  }
  entries[freeEntry++] = addr;
};

//---------------------------------------------------------------------------
// Name           popEntry
//
// Description    Removes the least pushed entry from the entry list
//---------------------------------------------------------------------------
ADDRESS popEntry ( void )
{
  return entries[--freeEntry];
};

//---------------------------------------------------------------------------
// Name           findEntry
//
// Description    Returns TRUE if entry with this address exists in teh entry
//                list
//---------------------------------------------------------------------------
BOOL findEntry ( ADDRESS addr )
{
  for(int i = 0; i < freeEntry; ++i)
    if (addr == entries[i])
      return TRUE;
  return FALSE;
};

//---------------------------------------------------------------------------
// Name           addEntry
//
// Description    If entry with this address doesn't exist => add the entry
//---------------------------------------------------------------------------
void addEntry ( ADDRESS addr )
{
  if (addr >= AbsAddress && addr <= EndAddress)
    if (!findEntry(addr))
      pushEntry(addr);
};

//---------------------------------------------------------------------------
// Name           newEntry
//
// Description    Adds a label, and if the label has not been parsed adds
//                it to the entry list.
//                If we have strictJumps on doesn't add the entry if it falls
//                outside AbsAddress-EndAddress
//
//		  Returns the number of the label if it has already been
//		  defined or 0
//---------------------------------------------------------------------------
int newEntry ( ADDRESS addr, labType t, labSize s )
{
  int i;

  if (strictJumps && (addr < AbsAddress || addr > EndAddress))
    return 0;
  if ((i = addLabel( addr, t, LabCodeMemory, s )) == 0 ||
      codeLabels.labels[i-1].notParsed)
     addEntry( addr );
  return i;
};

static void doOperand ( TAddrMode am, WORD operand )
{
  int labNum;

  switch ( am )
  {
    case AUnknown:
    case ANone:
    case ACFlag:
    case AAcc:
    case ADPTR:
    case AImmediate:
    case ARegister:
    case AIndexShort:
    case AIndexFull:
    case AIndexFullOffset:
    case AIndexRelOffset:
    case AJmpIndexFullOffset:
      break;

    case ABit:
    case ANotBit:
      addLabel( (ADDRESS)operand, LabBit, LabBitRAM, LabBitSize );
      break;

    case ARAMDirect:
      addLabel( (ADDRESS)operand, LabEData, LabIRAM, LabByte );
      break;

    case AJmpPage:
    case AJmpRelative:
    case AJmpExtended:
      if (labNum = newEntry( (ADDRESS)operand,
                             InstrType == ITCall ? LabSub : LabLoc,
                             LabByte ))
      {
        // The label exists...

	// If it is a "subroutine without return" => pretend the
	// instructrion was a jump
	if (memories[LabCodeMemory]->labels[labNum-1].subNoRet)
	  if (InstrType == ITCall)
	    InstrType = ITJump;
      }
      break;

    case AImmExtended:
      addLabel( (ADDRESS)operand, LabEData, LabXRAM, LabByte );
      break;

    default:
      assert( 0 );
  }
};


//---------------------------------------------------------------------------
// Name           srSegment
//
// Description    Processes a segment. Logs all labels referred.
//                Returns the address of the end of the segment.
//---------------------------------------------------------------------------
ADDRESS srSegment ( ADDRESS start )
{
  long addr = start;

  printf("Sourcing at: %04X\r", start);
  unknownInstr = FALSE;
  for( ; addr <= EndAddress; )
  {
    ADDRESS oldAddr = addr;
    addr += ParseSingle( addr );

    if (Op1AM != ANone)
    {
      doOperand( Op1AM, Operand1 );
      if (Op2AM != ANone)
      {
        doOperand( Op2AM, Operand2 );
        if (Op3AM != ANone)
          doOperand( Op3AM, Operand3 );
      }
    }

    switch (InstrType)
    {
      case ITUnknown:
        unknownInstr = TRUE;
        return addr - 2;

      case ITJump:
        if (strictJumps && (Operand1 < AbsAddress || Operand1 > EndAddress))
          return oldAddr;
      case ITRet:
        return addr - 1;

      case ITCall:
        if (strictJumps && (Operand1 < AbsAddress || Operand1 > EndAddress))
          return oldAddr;
        break;
    }
  }
  // We can come here only if addr > EndAddress
  return EndAddress;
};

//---------------------------------------------------------------------------
// Name           processEntries
//
// Description    Process all entries that have been pushed.
//                While this happens new entries can be pushed.
//---------------------------------------------------------------------------
static void near processEntries(void)
{
  while (freeEntry)
  {
    ADDRESS start;
    if (!inSegment(start = popEntry()))
      addSegment(start, srSegment(start));
  };
};

//---------------------------------------------------------------------------
// Name           pass1
//
// Description
//---------------------------------------------------------------------------
void pass1 ( ADDRESS start )
{
  pushLabel( start, LabLoc, LabCodeMemory, LabByte, startLabel, PARSED, 0 );
  addSegment(start, srSegment(start));
  processEntries();
};

//---------------------------------------------------------------------------
// Name           pass1SDF
//
// Description
//---------------------------------------------------------------------------
void pass1SDF ( void )
{
  for (int i = 0; i < freeSeg; ++i)
  {
    ADDRESS to;

    for(to = codeSegs[i].from;
        (to = srSegment(to)) < codeSegs[i].to && !unknownInstr;
        ++to);
    codeSegs[i].to = MAX(to, codeSegs[i].to);
  };
  processEntries();
};


//---------------------------------------------------------------------------
// Name           wordTableLen
//
// Description    Returns length of table in WORDS.
//---------------------------------------------------------------------------
static WORD near wordTableLen ( ADDRESS from, ADDRESS to )
{
  WORD len;
  if ((len = to - from + 1) < 2)
    len = 0;
  else
  if (EVEN(len))
    len /= 2;
  else
    len = 1;
  return len;
};


//---------------------------------------------------------------------------
// Name           fixATable
//
// Description    Adds all targets that are in a jump table to the
//                label & entry list.
//                Returns TRUE if at least one new label has been found
//---------------------------------------------------------------------------
static BOOL near fixATable ( label * l, ADDRESS to )
{
  WORD len;
  BOOL  added = FALSE;

  len = wordTableLen( l->addr, to );

  for( WORD * p = (WORD *)(Code + l->addr - AbsAddress); len; --len, ++p )
  {
/*
    if (!addLabel( _rotl( (ADDRESS)*p, 8 ), LabLoc, LabCodeMemory, LabByte ))
    {
      added = TRUE;
      addEntry( _rotl( (ADDRESS)*p, 8 ) );
    };
*/
    if (!addLabel( _rotl( (ADDRESS)*p, 8 ), LabLoc, LabCodeMemory, LabByte ))
    {
      added = TRUE;
      addEntry( _rotl( (ADDRESS)*p, 8 ) );
    };
  };
  return added;
};

//---------------------------------------------------------------------------
// Name           cmpLabel
//
// Description    Function for comparing two labels.
//                Used by qsort & bsearch
//---------------------------------------------------------------------------
static int cmpLabels (const void * a, const void * b)
{
  long s = (long)((label *)a)->addr - (long)((label *)b)->addr;
  return s < 0 ? -1 : (s == 0 ? 0 : 1);
};

//---------------------------------------------------------------------------
// Name           searchLabel
//
// Description    Searches a label by address. Returns pointer to it.
//                Supposes the labels have been sorted.
//---------------------------------------------------------------------------
static label * searchLabel ( labelList * l, ADDRESS a )
{
  label * p;
  label key;
  key.addr = a;
  if (p = (label *)bsearch( &key, l->labels, l->freeLabel, sizeof(l->labels[0]),
                            cmpLabels))
  {
    // Go back to the first label on that address
    while (p > l->labels && p[-1].addr == a)
      --p;
  }
  return p;
};

//---------------------------------------------------------------------------
// Name           sortLabels
//
// Description    Sorts a label table by address
//---------------------------------------------------------------------------
static void sortLabels ( labelList * l )
{
  if (l->freeLabel)
  {
    printf("\rSorting labels...\n");
    qsort( l->labels, l->freeLabel, sizeof(l->labels[0]), cmpLabels);
  };
};

//---------------------------------------------------------------------------
// Name           numberLabels
//
// Description    Assign numbers to all labels in a table
//---------------------------------------------------------------------------
static void numberLabels ( labelList * list, ADDRESS fromAddr, ADDRESS toAddr )
{
  unsigned locCount, subCount, dataCount, edataCount,
           tabCount, jtabCount, bitCount;

  label    * l;
  int      i;

  bitCount =
  locCount =
  subCount =
  tabCount =
  jtabCount =
  dataCount =
  edataCount = 1;
  for (l = list->labels, i = 0; i < list->freeLabel; ++i, ++l )
  {
    if ((l->addr < fromAddr || l->addr > toAddr) &&
        l->type == LabData)
      l->type = LabEData;

    if (l->size == LabWord && i != list->freeLabel-1 && l[1].addr == l->addr + 1)
      l[1].type = LabSecond;

    switch (l->type)
    {
      case LabSegment:
      case LabSecond:
        break;

      case LabLoc:
        l->num = locCount++;
        break;
      case LabSub:
        l->num = subCount++;
        break;
      case LabTab:
        l->num = tabCount++;
        break;
      case LabJmpTab:
        l->num = jtabCount++;
        break;
      case LabData:
        l->num = dataCount++;
        break;
      case LabEData:
        l->num = edataCount++;
        break;

      case LabBit:
        l->num = bitCount++;
        break;

      default:
        assert( 0 );
    };
  };
};

static ADDRESS near findTableEnd ( int i )
{
  ADDRESS to = EndAddress;

  for ( int k = i+1; k < codeLabels.freeLabel &&
                     codeLabels.labels[k].addr < EndAddress; ++k )
  {
    if (codeLabels.labels[k].type != LabSecond)
    {
      to = codeLabels.labels[k].addr - 1;
      break;
    };
  };
  return to;
};



static void near sortAllLabels ( void )
{
  sortLabels( &codeLabels );
  sortLabels( &iRamLabels );
  sortLabels( &xRamLabels );
  sortLabels( &bitRamLabels );
  numberLabels( &codeLabels, AbsAddress, EndAddress );
  numberLabels( &iRamLabels, 1, 0 );
  numberLabels( &xRamLabels, 1, 0 );
  numberLabels( &bitRamLabels, 1, 0 );
};

//---------------------------------------------------------------------------
// Name           fixJumpTables
//
// Description    Processes the jump tables.
//---------------------------------------------------------------------------
void fixJumpTables ( void )
{
  label * l;
  int     i;
  BOOL    added = FALSE;

  sortAllLabels();
  for(i = 0, l = codeLabels.labels; i < codeLabels.freeLabel; ++i, ++l)
  {
    if (l->type == LabJmpTab && l->addr >= AbsAddress && l->addr <= EndAddress)
    {
      if (fixATable(l, findTableEnd(i) ))
        added = TRUE;
    };
  };
  if (added)
  {
    processEntries();
    sortAllLabels();
  };
};

static char * normalStr ( char * s, int len )
{
  char * e;
  len -= (e = strchr( s, 0 )) - s;
  if (len > 0)
    setmem( e, len, ' ' );
  *(e += len) = 0;
  return e;
};

static char * GenHex ( char * buf, WORD value, labSize s )
{
  if (s != LabWord)
    sprintf( buf, (value & 0xF0) > 0x90 ? "%03Xh" : "%02Xh", value );
  else
    sprintf( buf, (value & 0xF000) > 0x9000 ? "%05Xh" : "%04Xh", value );
  return buf;
};

//---------------------------------------------------------------------------
// Name           outEQU
//
// Description    Prints an EQU definition for a label
//---------------------------------------------------------------------------
void outEQU ( label * l )
{
  if (l->type != LabSecond)
  {
    strcpy( normalStr( strcpy( lineBuf, useLabel( l ) ), 14 ), "\tequ\t" );
    out( "%s%s\t;%s\n", lineBuf, GenHex( buf, l->addr, l->size ),
         l->size == LabByte ? "" : genLabSize( l ) );
  }
};


void printExternalLabels ( labelList * l, ADDRESS from, ADDRESS to, BOOL all )
{
  int i;

  if (all)
  {
    for( i = 0; i < l->freeLabel; ++i )
      outEQU( l->labels+i );
  }
  else
  {
    for( i = 0; i < l->freeLabel && l->labels[i].addr < from; ++i)
      outEQU( l->labels+i );

    for(i = l->freeLabel - 1; i >= 0 && l->labels[i].addr > to; --i)
      outEQU( l->labels+i );
  }
};

//---------------------------------------------------------------------------
// Name           cmpSeg
//
// Description    A function used to compare two segment addresses with
//                qsort.
//---------------------------------------------------------------------------
static int cmpSegs (const void * a, const void * b)
{
  long s = (long)((segment *)a)->from - (long)((segment *)b)->from;
  return s < 0 ? -1 : (s == 0 ? 0 : 1);
};

//---------------------------------------------------------------------------
// Name           sortSegments
//
// Description    Sort segments by addresses and merge neighbours
//---------------------------------------------------------------------------
static void sortSegments ( void )
{
  qsort( codeSegs, freeSeg, sizeof(codeSegs[0]), cmpSegs );

  int cur, i;
  for (cur = 0, i = 1; i < freeSeg; ++i)
  {
    if (codeSegs[i].from <= (long)codeSegs[cur].to + 1)
      codeSegs[cur].to = MAX(codeSegs[cur].to, codeSegs[i].to);
    else
    {
      if (++cur != i)
        codeSegs[cur] = codeSegs[i];
    };
  };
  freeSeg = cur+1;
};

static int curLabel;

//---------------------------------------------------------------------------
// Name           checkString
//
// Description     Checks for an ASCII of length string. Returns length
//                 of string.
//---------------------------------------------------------------------------
static int checkString ( BYTE * c, char * buf, int len )
{
  for (int i = 0;i < len && c[i] >= ' ' && c[i] <= 'z'; *buf++ = c[i++]);
  *buf = 0;
  return i > 3 ? i : 0;
};

//---------------------------------------------------------------------------
// Name           checkDup
//
// Description    Checks for suplicate values
//---------------------------------------------------------------------------
static int checkDup ( BYTE * c, int len )
{
  for (int i = 0; i < len && c[i] == *c; ++i);
  return i > 63 ? i : 0;
};



//---------------------------------------------------------------------------
// Name           genDump
//
// Description    Generates data dump
//---------------------------------------------------------------------------
static void genDump ( ADDRESS from, ADDRESS to )
{
  char buf[40];
  BYTE huge * c = Code;

  c += from - AbsAddress;

  ADDRESS len, b, i;

  for (i = 0, len = (to - from + 1); len;)
  {
    if (b = checkString((BYTE *)c, buf, MIN(len, 32)))
    {
      if (i)
        out("\n");
      out("\tBYTE\t'%s'\n", buf);
      c += b;
      len -= b;
      i = 0;
    }
    else
    if (b = checkDup((BYTE *)c, len))
    {
      if (i)
        out("\n");
      out("\tBYTE\t%u DUP (%s)\n", b, GenHex( buf, *c, LabByte ) );
      c += b;
      len -= b;
      i = 0;
    }
    else
    {
      if (i == 0)
        out("\tBYTE\t%s", GenHex( buf, *c++, LabByte ));
      else
        out(", %s", GenHex( buf, *c++, LabByte ));
      --len;
      ++i;
      if (len)
      {
        if (i == 7)
        {
          out("\n");
          i = 0;
        };
      }
      else
        out("\n");
    };
  };
};

static void near genWordDump ( ADDRESS from, ADDRESS to, int printLocs)
{
  WORD huge * c;
  WORD len = (to - from) / 2 + 1;
  label * l;

  c = (WORD huge *)(Code + from - AbsAddress);
  for(int i = 0; len; ++c)
  {
    --len;
    if (i == 0)
      out("\tWORD\t");
    else
      out(", ");
    if (printLocs && (l = searchLabel( &codeLabels, _rotl( (ADDRESS)*c, 8 )) ))
      out(referLabel(l));
    else
      out("%s", GenHex( buf, *c, LabWord ));
    ++i;
    if (len == 0)
      out("\n");
    else
    {
      if (i == 4)
      {
        i = 0;
        out("\n");
      };
    }
  };
};



//---------------------------------------------------------------------------
// Name           genTable
//
// Description    Generates table
//---------------------------------------------------------------------------
static ADDRESS near genTable ( int t )
{
  ADDRESS to = findTableEnd(t);

  if (codeLabels.labels[t].size == LabByte)
  {
    genDump(codeLabels.labels[t].addr, to);
  }
  else
  {
    WORD len;

    if (len = wordTableLen(codeLabels.labels[t].addr, to))
    {
      genWordDump( codeLabels.labels[t].addr,
                   codeLabels.labels[t].addr + (len-1)*2,
                   codeLabels.labels[t].type == LabJmpTab);
    };

    genDump( codeLabels.labels[t].addr + len*2, to );
  };
  return to + 1;
};

static ADDRESS near genDataLabel ( int t )
{
  if (codeLabels.labels[t].addr == EndAddress ||
      codeLabels.labels[t].size == LabByte)
  {
    genDump( codeLabels.labels[t].addr, codeLabels.labels[t].addr );
    return codeLabels.labels[t].addr + 1;
  };

  genWordDump( codeLabels.labels[t].addr, codeLabels.labels[t].addr, 0 );
  return codeLabels.labels[t].addr + 2;
};



static void genData ( ADDRESS from, ADDRESS to )
{
  int first = 1;

  if (from >= to)
    return;
  --to;

  out("\n");

  for(;;++curLabel)
  {
    for (; curLabel < codeLabels.freeLabel &&
           codeLabels.labels[curLabel].addr < from; ++curLabel);
    if ( curLabel == codeLabels.freeLabel ||
         codeLabels.labels[curLabel].addr > to)
    {
      if (first)
        out(";!3 **** Data at %04X\n", from);
      genDump(from,to);
      break;
    };
    first = 0;
    if ( from < codeLabels.labels[curLabel].addr )
    {
      genDump(from, codeLabels.labels[curLabel].addr - 1);
      from = codeLabels.labels[curLabel].addr;
    };
    if (codeLabels.labels[curLabel].type != LabSecond)
    {
      out(useLabel(codeLabels.labels + curLabel));
      if (codeLabels.labels[curLabel].type == LabTab ||
          codeLabels.labels[curLabel].type == LabJmpTab)
        from = genTable(curLabel);
      else
      if (codeLabels.labels[curLabel].type == LabData)
        from = genDataLabel(curLabel);
    };
  };
  out("\n");
};


static void printLabel ( ADDRESS a )
{
  for(; curLabel < codeLabels.freeLabel &&
        codeLabels.labels[curLabel].addr < a; ++curLabel);
  if (curLabel < codeLabels.freeLabel && codeLabels.labels[curLabel].addr == a)
  {
    if (codeLabels.labels[curLabel].type == LabSegment)
    {
      out(";!2 **** Unreachable Code segment at %04X\n", a);
    }
    else
    if (codeLabels.labels[curLabel].type == LabSub)
    {
      out("\n;********************************************\n");
      out(";*             Subroutine at %04X           *\n", codeLabels.labels[curLabel].addr);
      out(";********************************************\n");
    }
    out( "%s:\n", useLabel(codeLabels.labels + curLabel++) );
  };
  out("\t");
};

#include "dis31gen.inc"


//---------------------------------------------------------------------------
// Name           genCode
//
// Description    Generates the source itself
//---------------------------------------------------------------------------
void genCode ( void )
{
  long from = AbsAddress;
  curLabel = 0;

  out("\n\torg\t%s\n\n", GenHex( buf, AbsAddress, LabWord ));
  for (int i = 0; i < freeSeg; ++i)
  {
    genData((ADDRESS)from, codeSegs[i].from);
    from = codeSegs[i].to+1;
    for(long addr = codeSegs[i].from; addr <= codeSegs[i].to;)
    {
      char  buf[50];
      printLabel((ADDRESS)addr);
      addr += GenSingle( (ADDRESS)addr );
      out( ResultBuffer );
      out( "\n" );
    };
  };
  genData(from, EndAddress + 1);
};


//---------------------------------------------------------------------------
// Name           pass2
//
// Description
//---------------------------------------------------------------------------
void pass2 ( void )
{
  printf("Generating source...\n");
  printExternalLabels( &iRamLabels, 0, 0, TRUE );
  printExternalLabels( &bitRamLabels, 0, 0, TRUE );
  printExternalLabels( &xRamLabels, 0, 0, TRUE );
  printExternalLabels( &codeLabels, AbsAddress, EndAddress, FALSE );

  sortSegments();
  genCode();
  out("\n\tEND\n\n\n");
};

void genLabDef ( label * l, BOOL genSize )
{
  if (l->name)
    out("'%-10s'        %04X", genLabel(l), l->addr);
  else
    out("%-12s        %04X", genLabel(l), l->addr);
  if (genSize)
    out( ", %s", genLabSize(l) );
  else
  if (l->subNoRet)
    out( ", NORET" );
    
  if (l->generated)
  {
    if (!l->referenced)
      out("  Generated but not used");
  }
  else
  {
    if (!l->referenced)
      out("  Not used");
    else
      out("  ! Referenced but not generated");
  };
  out("\n");
};

static void near genDefSection ( char * name, labelList * l, labType t,
                                 BOOL genSize )
{
  int i;
  out( name );
  for ( i = 0; i < l->freeLabel; ++i )
    if (l->labels[i].type == t)
      genLabDef( l->labels + i, genSize );
  out("\n");
};

void genDef(void)
{
  out("*********************************************\n");
  out("**  Definitions for %s\n", inFileName);
  out("**  Sourced with DeCODE Ver 0.1\n");
  out("*********************************************\n");

  out("  File size       : %04X\n", FileLength);
  out("  Entry point     : %04X\n", startAddress);
  out("  Absolute ADDRESS: %04X\n", AbsAddress);
  out("*********************************************\n\n");

  genDefSection("--------- Subroutines -----------------------\n",
                 &codeLabels, LabSub, FALSE );
  genDefSection("--------- Locations -----------------------\n",
                 &codeLabels, LabLoc, FALSE );
  genDefSection("--------- Code data -----------------------\n",
                 &codeLabels, LabData, TRUE );
  genDefSection("--------- Code equates -----------------------\n",
                 &codeLabels, LabEData, TRUE );
  genDefSection("--------- Code tables -----------------------\n",
                 &codeLabels, LabTab, TRUE );
  genDefSection("--------- Code jump tables -----------------------\n",
                 &codeLabels, LabJmpTab, TRUE );

  genDefSection("--------- IRAM equates -----------------------\n",
                 &iRamLabels, LabEData, TRUE );
  genDefSection("--------- Bit equates -----------------------\n",
                 &bitRamLabels, LabBit, TRUE );
  genDefSection("--------- IRAM tables -----------------------\n",
                 &iRamLabels, LabTab, TRUE );
  genDefSection("--------- IRam jump tables -----------------------\n",
                 &iRamLabels, LabJmpTab, TRUE );

  genDefSection("--------- XRAM equates -----------------------\n",
                 &xRamLabels, LabEData, TRUE );
  genDefSection("--------- XRAM tables -----------------------\n",
                 &xRamLabels, LabTab, TRUE );
  genDefSection("--------- XRam jump tables -----------------------\n",
                 &xRamLabels, LabJmpTab, TRUE );


  out("\n\n");
  for ( int i = 0; i < freeSeg; ++i )
    out("Code segment at %04X-%04X\n", codeSegs[i].from, codeSegs[i].to);
};

