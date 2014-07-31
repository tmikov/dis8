static BOOL PrevOperand;
static WORD LastAddress;
static BOOL HaveLastAddress;
static BOOL LastAddrByte;

static void SaveLastAddress ( WORD operand, BOOL isByte )
{
  LastAddress = operand;
  HaveLastAddress = TRUE;
  LastAddrByte = isByte;
};

static void AddToResult ( const char * s )
{
  strcat( ResultBuffer, s );
};

static void AddChToResult ( char c )
{
  char * s;
  (s = strchr( ResultBuffer, 0 ))[0] = c;
  s[1] = 0;
};

static void CheckOperand ( void )
{
  AddChToResult( PrevOperand ? ',' : '\t' );
  PrevOperand = TRUE;
};


static void AddLabelReference ( labelList * list, ADDRESS addr )
{
  label * l;
  if (l = searchLabel( list, addr ))
    AddToResult( referLabel(l) );
  else
    ltoa( addr, strchr( ResultBuffer, 0 ), 10 );
};

static void GenOperand ( TAddrMode am, WORD operand )
{
  if (am == ANone || am == AUnknown)
    return;

  CheckOperand();

  switch ( am )
  {
    case ACFlag:
      AddChToResult( 'C' );
      break;

    case AAcc:
      AddChToResult( 'A' );
      break;

    case AIndexFull:
      AddChToResult( '@' );
    case ADPTR:
      AddToResult( "DPTR" );
      break;

    case AIndexShort:
      AddChToResult( '@' );
    case ARegister:
      AddChToResult( 'R' );
      AddChToResult( operand + '0' );
      break;

    case AIndexRelOffset:
      AddToResult( "@A+PC" );
      break;

    case AIndexFullOffset:
    case AJmpIndexFullOffset:
      AddToResult( "@A+DPTR" );
      break;

    case AImmediate:
      AddChToResult( '#' );
      itoa( (BYTE)operand, strchr( ResultBuffer, 0 ), 10 );
      SaveLastAddress( operand, TRUE );
      break;

    case ARAMDirect:
      AddLabelReference( &iRamLabels, operand );
      SaveLastAddress( operand, TRUE );
      break;

    case ANotBit:
      AddChToResult( '/' );
    case ABit:
      AddLabelReference( &bitRamLabels, operand );
      SaveLastAddress( operand, TRUE );
      break;

    case AJmpPage:
    case AJmpRelative:
    case AJmpExtended:
      AddLabelReference( &codeLabels, operand );
      SaveLastAddress( operand, FALSE );
      break;

    case AImmExtended:
      AddChToResult( '#' );
      AddLabelReference( &xRamLabels, operand );
      SaveLastAddress( operand, FALSE );
      break;

    default:
      assert( 0 );
  }
};

static int GenSingle ( ADDRESS aPC )
{
  int len = ParseSingle( aPC );

  if (InstrCode == -1)
  {
    strcpy( ResultBuffer, "???" );
    return 1;
  }
  strlwr( strcpy( ResultBuffer, Strings[InstrCode] ) );

  PrevOperand = FALSE;
  HaveLastAddress = FALSE;

  if (Op1AM != ANone)
  {
    GenOperand( Op1AM, Operand1 );
    if (Op2AM != ANone)
    {
      GenOperand( Op2AM, Operand2 );
      if (Op3AM != ANone)
        GenOperand( Op3AM, Operand3 );
    }
  }
  else    // check for mul & div to add "AB"
    if (InstrCode == INSTR_MUL || InstrCode == INSTR_DIV)
      AddToResult( "\tAB" );
  if (HaveLastAddress)
    sprintf( strchr( ResultBuffer, 0 ),
             LastAddrByte ? "\t\t\t; (%02xh)" : "\t\t\t; (%04xh)",
             LastAddress );
  return len;
};

