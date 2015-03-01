/* PrintTest.cpp copyright notice

  Copyright (c) 2015 Michael Jonker. All right reserved.

  "THE BEER-WARE LICENSE" (Revision 42++)  http://en.wikipedia.org/wiki/Beerware
  Michael Jonker <EBPISTC: 52.36040, 4.87001, NAP+5, -493899258> wrote this file.

  As long as you retain this notice you can can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation; either version 2.1 of the License, or (at your option)
  any later version.
  If we meet some day, and you think this stuff is worth it, you can buy me a beer in return.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  For a copy of the GNU Lesser General Public License write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
*/

#include "Arduino.h"

static unsigned long&  _asLong (const float& aFloat) { return (unsigned long&)  aFloat;} // interpret a float as long
static float& _asFloat(const unsigned long&  aLong)  { return (float&) aLong; } // interpret a long  as float

void printTest()
{
  unsigned int nc;
  unsigned int i;
  
  Serial.println(F("The new version of the Print class allows you to code things like:"));
  Serial.print(F("Serial.print(3.141592, PRECISION(4) & FIELDSIZE(3));  // "));
                  Serial.print(3.141592, PRECISION(4) & FIELDSIZE(3));                Serial.println(F(" ; note FIELDSIZE only affects the integer part of the number"));
  Serial.print(F("Serial.print(66, HEX); // "));
                  Serial.print(66, HEX);                                              Serial.println();
  Serial.print(F("Serial.print(22, RADIX(5) & FIELDSIZE(8) & ALIGNLEFT); // "));
                  Serial.print(22, RADIX(5) & FIELDSIZE(8) & ALIGNLEFT);              Serial.println();
  Serial.print(F("Serial.print(34, OCT & FIELDSIZE(8) & FORCESIGN & FILLZEROS); // "));
                  Serial.print(34, OCT & FIELDSIZE(8) & FORCESIGN & FILLZEROS);       Serial.println();
  Serial.print(F("Serial.print(54, 13); // "));
                  Serial.print(54, 13);                                               Serial.println(F(" ; backward compatible base argument"));
  Serial.print(F("Serial.print(3.141592, 3); // "));
                  Serial.print(3.141592, 3);                                          Serial.println(F(" ; backward compatible precision argument"));

  Serial.println(F("\nRadix test, looping from radix 0 .. 34 (0x22)"));
  IntegerFormat di = HEX & FIELDSIZE(2) & FILLZEROS;
  IntegerFormat ti = RADIX(0) & FIELDSIZE(4);
  Serial.print("0x"); Serial.print( 0, di); nc=Serial.print(42u, ti & RADIX( 0) & FIELDSIZE(8)); Serial.print(" nc="); Serial.println(nc); // will result in the default radix
  Serial.print("0x"); Serial.print( 1, di); nc=Serial.print(42u, ti & RADIX( 1) & FIELDSIZE(8)); Serial.print(" nc="); Serial.println(nc); // should become binary format?
  Serial.print("0x"); Serial.print( 2, di); nc=Serial.print(42u, ti & RADIX( 2) & FIELDSIZE(8)); Serial.print(" nc="); Serial.println(nc);
  ti = ti & FIELDSIZE(8);
  for(i=3; i<35; i++)
  {
    Serial.print("0x"); Serial.print( i, di); nc=Serial.print(42u, ti & RADIX( i) ); Serial.print(" nc="); Serial.println(nc);
  }
  Serial.println(F("note: Radix 33 (0x21)... wraps onto radix 1..."));

  Serial.println(F("\nPrecision test looping over precisison and complemented with Serial.printPattern((char*) \" - -.- - |\", 42-nc, nc%10)"));
  IntegerFormat df = DEC & FIELDSIZE(2);
  PrintFormat   tf = FIELDSIZE(8);
  for(i=3; i<32; i++)
  {
    Serial.print( i, df);
    nc =Serial.print(41.9876543210987654321098765432109876543210, tf & PRECISION( i) );
    nc+=Serial.print("  ");
    nc+=Serial.printPattern((char*) " - -.- - |", 42-nc, nc%10);
    Serial.print(" nc="); Serial.println(nc,FIELDSIZE(2));
  }

  Serial.println(F("\nundocumented printPattern feature:"));
  Serial.print(F("Serial.printPattern((char*) \" - -.- - |\\0surprise \", 60, 11); // "));
  Serial.printPattern((char*) " - -.- - |\0surprise :", 60, 11);
  Serial.println();

  Serial.print(F("\ntest print FlashStringHelper\nSerial.print(F(\"waar eens de boterbloemples bloeiden...\"));   // "));
  Serial.println(F("waar eens de boterbloemples bloeiden..."));

  Serial.println("\ndefault test:");
  Serial.print("Serial.print(0xB)    :"); Serial.println(0xB);
  Serial.print("Serial.print(12.345) :"); Serial.println(12.345);

  Serial.println("\nspecial float test");
  {
  nc=Serial.println("test positive float overflow");
  FloatFormat ff = FIELDSIZE(15) & PRECISION(2);
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff60u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff60u); nc +=Serial.print( float(0xffffff60u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff70u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff70u); nc +=Serial.print( float(0xffffff70u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff7fu, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff7fu); nc +=Serial.print( float(0xffffff7fu), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff80u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff80u); nc +=Serial.print( float(0xffffff80u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff81u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff81u); nc +=Serial.print( float(0xffffff81u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff90u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff90u); nc +=Serial.print( float(0xffffff90u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffffa0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffffa0u); nc +=Serial.print( float(0xffffffa0u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xfffffff0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xfffffff0u); nc +=Serial.print( float(0xfffffff0u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffffffu, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffffffu); nc +=Serial.print( float(0xffffffffu), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.println("test negative float overflow");
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff60u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff60u); nc +=Serial.print(-float(0xffffff60u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff70u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff70u); nc +=Serial.print(-float(0xffffff70u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff7fu, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff7fu); nc +=Serial.print(-float(0xffffff7fu), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff80u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff80u); nc +=Serial.print(-float(0xffffff80u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff81u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff81u); nc +=Serial.print(-float(0xffffff81u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffff90u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffff90u); nc +=Serial.print(-float(0xffffff90u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffffa0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffffa0u); nc +=Serial.print(-float(0xffffffa0u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xfffffff0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xfffffff0u); nc +=Serial.print(-float(0xfffffff0u), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print(      0xffffffffu, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(      0xffffffffu); nc +=Serial.print(-float(0xffffffffu), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.println("test positive/negative float nan & inf"); // see also http://en.wikipedia.org/wiki/IEEE_754-1985 and 
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0x7F800000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0x7F800000l, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0x7F800000), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0x7F800001, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0x7F800001l, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0x7F800001), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0x7FC00000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0x7FC00000l, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0x7FC00000), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0x7FFFFFFF, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0x7FFFFFFFl, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0x7FFFFFFF), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0xFF800000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0xFF800000l, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0xFF800000), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0xFF800001, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0xFF800001l, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0xFF800001), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0xFFC00000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0xFFC00000l, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0xFFC00000), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  nc=Serial.print("0x"); nc +=Serial.print((signed long)0xFFFFFFFF, HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print((signed long)0xFFFFFFFFl, FIELDSIZE(10)); nc +=Serial.print(_asFloat(0xFFFFFFFF), ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));

#ifdef development_test_negative_nan_conversion
  float negnan = _asFloat(0xFFFFFFFF);
  nc=Serial.println("test negative float nan conversion");
  nc=Serial.print("0x"); nc +=Serial.print(_asLong(negnan), HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(_asLong(negnan), FIELDSIZE(10)); nc +=Serial.print(negnan, ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
  if(_asLong(negnan)&0x80000000) { _asLong(negnan) = _asLong(negnan) & ~0x80000000; }
  nc=Serial.print("0x"); nc +=Serial.print(_asLong(negnan), HEX & FIELDSIZE(8) & FILLZEROS); nc +=Serial.print(" "); nc +=Serial.print(_asLong(negnan), FIELDSIZE(10)); nc +=Serial.print(negnan, ff); nc +=Serial.print(" | "); nc +=Serial.println(nc,FIELDSIZE(2));
#endif

  Serial.print("\nBeware:\n");
  Serial.print("print(              -1,   HEX/DEC) : "); Serial.print(              -1,   HEX & FIELDSIZE(8) & FILLZEROS); Serial.print("/"); Serial.println(               -1  );
  Serial.print("print(      0xFFFFFFFF,   HEX/DEC) : "); Serial.print(      0xFFFFFFFF,   HEX & FIELDSIZE(8) & FILLZEROS); Serial.print("/"); Serial.println(       0xFFFFFFFF  );
  Serial.print("print(      0xFFFFFFFFl,  HEX/DEC) : "); Serial.print(      0xFFFFFFFFl,  HEX & FIELDSIZE(8) & FILLZEROS); Serial.print("/"); Serial.println(       0xFFFFFFFFl );
  Serial.print("print((long)0xFFFFFFFF,   HEX/DEC) : "); Serial.print((long)0xFFFFFFFF,   HEX & FIELDSIZE(8) & FILLZEROS); Serial.print("/"); Serial.println( (long)0xFFFFFFFF  );
  Serial.print("The constants 0xFFFFFFFF and 0xFFFFFFFFl are interpreted as (unsigned long)\n");
  }
  Serial.println("\ntest FIELDSIZE(5)");
  {
  Serial.println(       0, DEC & FIELDSIZE(5));
  Serial.println(       1, DEC & FIELDSIZE(5));
  Serial.println(      12, DEC & FIELDSIZE(5));
  Serial.println(     123, DEC & FIELDSIZE(5));
  Serial.println(    1234, DEC & FIELDSIZE(5));
  Serial.println(   12345, DEC & FIELDSIZE(5));
  Serial.println(  123456, DEC & FIELDSIZE(5));
  Serial.println( 1234567, DEC & FIELDSIZE(5));
  Serial.println(-      1, DEC & FIELDSIZE(5));
  Serial.println(-     12, DEC & FIELDSIZE(5));
  Serial.println(-    123, DEC & FIELDSIZE(5));
  Serial.println(-   1234, DEC & FIELDSIZE(5));
  Serial.println(-  12345, DEC & FIELDSIZE(5));
  Serial.println(- 123456, DEC & FIELDSIZE(5));
  Serial.println(-1234567, DEC & FIELDSIZE(5));

  Serial.println("\ntest FIELDSIZE(5) & STRICTSIZE");

  Serial.println(       0, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(       1, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(      12, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(     123, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(    1234, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(   12345, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(  123456, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println( 1234567, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(-      1, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(-     12, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(-    123, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(-   1234, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(-  12345, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(- 123456, DEC & FIELDSIZE(5) & STRICTSIZE);
  Serial.println(-1234567, DEC & FIELDSIZE(5) & STRICTSIZE);

  Serial.println("\ntest FIELDSIZE(5) & STRICTSIZE & FORCESIGN");
  Serial.println(       0, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(       1, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(      12, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(     123, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(    1234, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(   12345, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(  123456, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println( 1234567, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(-      1, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(-     12, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(-    123, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(-   1234, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(-  12345, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(- 123456, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  Serial.println(-1234567, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);

  Serial.println("\ntest FIELDSIZE(1) & STRICTSIZE");
  Serial.println(       0, DEC & FIELDSIZE(1) & STRICTSIZE);
  Serial.println(       1, DEC & FIELDSIZE(1) & STRICTSIZE);
  Serial.println(      12, DEC & FIELDSIZE(1) & STRICTSIZE);
  Serial.println(     123, DEC & FIELDSIZE(1) & STRICTSIZE);
  Serial.println(-      1, DEC & FIELDSIZE(1) & STRICTSIZE);
  Serial.println(-     12, DEC & FIELDSIZE(1) & STRICTSIZE);
  Serial.println(-    123, DEC & FIELDSIZE(1) & STRICTSIZE);
  Serial.println("\ntest FIELDSIZE(1) & STRICTSIZE & FORCESIGN");
  Serial.println(       0, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  Serial.println(       1, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  Serial.println(      12, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  Serial.println(     123, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  Serial.println(-      1, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  Serial.println(-     12, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  Serial.println(-    123, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  }
  Serial.println("\ntest alignments");
  {
  Serial.print( 1234, DEC & FIELDSIZE(8) & FILLZEROS );                         Serial.println(F(" : FIELDSIZE(8) & FILLZEROS"));
  Serial.print( 1234, DEC & FIELDSIZE(8) );                                     Serial.println(F(" : FIELDSIZE(8)"));
  Serial.print( 1234, DEC & FIELDSIZE(8) & ALIGNLEFT );                         Serial.println(F(" : FIELDSIZE(8) & ALIGNLEFT"));
  Serial.print( 1234, DEC & FIELDSIZE(8) & ALIGNLEFT & FILLZEROS );             Serial.println(F(" : FIELDSIZE(8) & ALIGNLEFT & FILLZEROS"));
  Serial.print(-1234, DEC & FIELDSIZE(8) & FILLZEROS );                         Serial.println(F(" : FIELDSIZE(8) & FILLZEROS"));
  Serial.print(-1234, DEC & FIELDSIZE(8) );                                     Serial.println(F(" : FIELDSIZE(8)"));
  Serial.print(-1234, DEC & FIELDSIZE(8) & ALIGNLEFT );                         Serial.println(F(" : FIELDSIZE(8) & ALIGNLEFT"));
  Serial.print(-1234, DEC & FIELDSIZE(8) & ALIGNLEFT & FILLZEROS );             Serial.println(F(" : FIELDSIZE(8) & ALIGNLEFT & FILLZEROS"));
  Serial.println("\ntest alignments with forced sign");
  Serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN & FILLZEROS );             Serial.println(F(" : FIELDSIZE(8) & FORCESIGN & FILLZEROS"));
  Serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN );                         Serial.println(F(" : FIELDSIZE(8) & FORCESIGN"));
  Serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT );             Serial.println(F(" : FIELDSIZE(8) & FORCESIGN & ALIGNLEFT"));
  Serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT & FILLZEROS ); Serial.println(F(" : FIELDSIZE(8) & FORCESIGN & ALIGNLEFT & FILLZEROS"));
  Serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN & FILLZEROS );             Serial.println(F(" : FIELDSIZE(8) & FORCESIGN & FILLZEROS"));
  Serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN );                         Serial.println(F(" : FIELDSIZE(8) & FORCESIGN"));
  Serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT );             Serial.println(F(" : FIELDSIZE(8) & FORCESIGN & ALIGNLEFT"));
  Serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT & FILLZEROS ); Serial.println(F(" : FIELDSIZE(8) & FORCESIGN & ALIGNLEFT & FILLZEROS"));
  }
  Serial.println(F("notes:"));
  Serial.println(F("& FILLZER0 & ALLIGNLEFT produces dashes and not zeros, should we find a more appropriate name for FILLZEROS (FILLALTCHAR, NOSPACEFILL, ...)"));
  Serial.println(F("& FORCESIGN symbol is not subtracted from FIELDSIZE(), the same holds for PRECISION"));
  PrintFormat pf0 = FORCESIGN;

  Serial.println(F("\nIllegal format combinattions will lead to compiler errors:"));
  Serial.println(F("Serial.print(42, PRECISION(4));"));
  Serial.println(F("Serial.print(42., HEX);"));
  Serial.println(F("PrintFormat myFormat(HEX & PRECISION(4));"));

#ifdef testCompilationErrors
// The following code will produce compilation errors, as it involves impossible PrintFormat combinations or conversions
  PrintFormat pf1 = HEX;                                    // error : 'PrintFormat' is an inaccessible base of 'IntegerFormat'
  PrintFormat pf2 = PRECISION(4);                           // error : 'PrintFormat' is an inaccessible base of 'FloatFormat'
  PrintFormat pf3 = HEX & PRECISION(4);                     // error : conversion from 'invalid_PrintFormat_operation' to non-scalar type 'PrintFormat' requested
  PrintFormat pf4 = PRECISION(4) & HEX;                     // error : conversion from 'invalid_PrintFormat_operation' to non-scalar type 'PrintFormat' requested
  PrintFormat pf5 = PRECISION(4) & FIELDSIZE(4) & HEX;      // error : conversion from 'invalid_PrintFormat_operation' to non-scalar type 'PrintFormat' requested
  PrintFormat pf6 = PRECISION(4) & FIELDSIZE(4) & 42;       // error : conversion from 'invalid_PrintFormat_operation' to non-scalar type 'PrintFormat' requested

  int i0 = HEX & PRECISION(4);                              // error : cannot convert 'invalid_PrintFormat_operation' to 'int' in initialization
  int i1 = PRECISION(4) & HEX;                              // error : cannot convert 'invalid_PrintFormat_operation' to 'int' in initialization
  int i2 = PRECISION(4) & FIELDSIZE(4) & HEX;               // error : cannot convert 'invalid_PrintFormat_operation' to 'int' in initialization
  int i3 = PRECISION(4) & FIELDSIZE(4) & 42;                // error : conversion from 'invalid_PrintFormat_operation' to non-scalar type 'PrintFormat' requested

  Serial.print((unsigned)42, FIELDSIZE(4) & PRECISION(4) & FILLZEROS & FORCESIGN);  // error : 'PrintFormat' is an inaccessible base of 'FloatFormat'
  Serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & FILLZEROS & FORCESIGN);  // error : 'PrintFormat' is an inaccessible base of 'FloatFormat'

  Serial.print((unsigned)42, HEX & PRECISION(4) & FIELDSIZE(4) & FILLZEROS & FORCESIGN);      // error : ambiguous overload for 'operator&' (operand types are 'const IntegerFormat' and 'const FloatFormat')
  Serial.print((unsigned)42, PRECISION(4) & HEX & FIELDSIZE(4) & FILLZEROS & FORCESIGN);      // error : ambiguous overload for 'operator&' (operand types are 'const FloatFormat' and 'const IntegerFormat')
  Serial.print((unsigned)42, 42 & PRECISION(4) & FIELDSIZE(4) & HEX & FILLZEROS & FORCESIGN); // error : ambiguous overload for 'operator&' (operand types are 'int' and 'const FloatFormat')
  Serial.print((unsigned)42, PRECISION(4) & 42 & FIELDSIZE(4) & HEX & FILLZEROS & FORCESIGN); // error : ambiguous overload for 'operator&' (operand types are 'const FloatFormat' and 'int')
  Serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & HEX & 42 & FILLZEROS & FORCESIGN); // error : ambiguous overload for 'operator&' (operand types are 'FloatFormat' and 'const IntegerFormat')
  Serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & HEX & FILLZEROS & FORCESIGN);      // error : ambiguous overload for 'operator&' (operand types are 'FloatFormat' and 'const IntegerFormat')
  Serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & 42 & HEX & FILLZEROS & FORCESIGN); // error : ambiguous overload for 'operator&' (operand types are 'FloatFormat' and 'int')
  Serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & HEX );                             // error : ambiguous overload for 'operator&' (operand types are 'FloatFormat' and 'const IntegerFormat')
  Serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & 42 );                              // error : ambiguous overload for 'operator&' (operand types are 'FloatFormat' and 'int')

  Serial.print((unsigned)42, FIELDSIZE(4) & HEX & PRECISION(4) & FILLZEROS & FORCESIGN);  // error : ambiguous overload for 'operator&' (operand types are 'IntegerFormat' and 'const FloatFormat')
  Serial.print((unsigned)42, FIELDSIZE(4) & PRECISION(4) & HEX & FILLZEROS & FORCESIGN);  // error : ambiguous overload for 'operator&' (operand types are 'FloatFormat' and 'const IntegerFormat')
  Serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & FILLZEROS & FORCESIGN);        // error : call of overloaded 'print(unsigned int, FloatFormat)' is ambiguous

  // no match for operator
  Serial.print((unsigned)42,  FIELDSIZE(4) || HEX || FILLZEROS);
  Serial.print((unsigned)42,  FIELDSIZE(4) |  HEX |  FILLZEROS);
  Serial.print((unsigned)42,  FIELDSIZE(4) && HEX && FILLZEROS);
  Serial.print((unsigned)42,  FIELDSIZE(4) &  HEX &  FILLZEROS);     // ok
  Serial.print((unsigned)42,  FIELDSIZE(4) +  HEX +  FILLZEROS);     // not initutive, because HEX+HEX = ??
  Serial.print((unsigned)42,  FIELDSIZE(4),   HEX,   FILLZEROS);     // do not think this is technically possible, and if so (vararg) inefficient
  Serial.print((unsigned)42, (FIELDSIZE(4),   HEX,   FILLZEROS)  );  // unnatural (but could be made to work by overloading the operator,

  PrintFormat myPrintFormat1 = FIELDSIZE(4) & RADIX(4) & FILLZEROS;  // error : 'PrintFormat' is an inaccessible base of 'IntegerFormat'
  Serial.print((unsigned)42,  myPrintFormat1);
  Serial.print((unsigned)42,  myPrintFormat1 & ~FILLZEROS);

  PrintFormat myPrintFormat2 = FIELDSIZE(4) + RADIX(4) + FILLZEROS;  // error : no match for 'operator+' (operand types are 'const PrintFormat' and 'const IntegerFormat')
  Serial.print((unsigned)42,  myPrintFormat2);
  Serial.print((unsigned)42,  myPrintFormat2 -FILLZEROS);            // error : no match for 'operator-' (operand types are 'PrintFormat' and 'const PrintFormat')

  (FIELDSIZE(4) + HEX + HEX + HEX) == (FIELDSIZE(4) + HEX);          // error : no match for 'operator+' (operand types are 'const PrintFormat' and 'const IntegerFormat')
#endif

  Serial.println('\n');
}

// eof
