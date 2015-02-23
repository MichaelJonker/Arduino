/* <template> copyright notice

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

  This code replaces the version of David A. Mellis.
*/

/* compilation & testing

# options to get it compiled outside Arduino environment (for debugging and code optimisation)
export Arduino=/Arduino/arduino-git/ # or whatever
cd $Arduino/examples/test_FormattedPrint

g++ -g \
 -DtestCode\
 -Dno_testCompilationErrors\
 -I$Arduino/hardware/arduino/avr/cores/arduino\
 -I$Arduino/hardware/arduino/sam/cores/arduino\
 PrintTest.cpp\
 2>&1| grep --text -E "error:"\
 ||\
 gdb \
 ./a.exe

# command for code inspection
$Arduino/arduino-1.0.5/hardware/tools/avr/bin/avr-g++ \
  -DtestCode \
  -I../../../arduino-1.0.5/hardware/arduino/cores/arduino \
  -I../../../arduino-1.0.5/hardware/arduino/variants/standard \
  -I../../../arduino-1.0.5/hardware/tools/avr/avr/include \
  -mmcu=atmega328p -Os test.cpp \
  -S \
  2>&1| grep  -E "error"

# note: includes are needed for WString.h and avr/pgmspace.h
*/

#ifdef testCode
#define __ATTR_PROGMEM__
#define pgm_read_byte(x) *(x)
#define __PGMSPACE_H_
#define PSTR
typedef const char* PGM_P;

#include <stddef.h>
#include <inttypes.h>
#endif

#define PrintFormatter_h // we do not want the printFormatter
#ifndef PrintFormatter_h // == PrintFormatter.h ===================================================================================
#define PrintFormatter_h

class Print;

/** The PrintFormatter class provides a way to define custom formats for printing basic types.
    By deriving from PrintFormatter and implementing the printFormatted(Print &out, T item) method,
    it will then be possible for users to print out basic types under special formatter control with
    Print::print(item, Formatter) and Print::println(item, Formatter) methods.
    
    The PrintFormatter can also to provide alternative formatting options for custom objects that
    have defined a Printable interface.
*/
class PrintFormatter
{
  public:
    // template<typename T>
    // templates may not be virtual
    // hm that is a causing a problem... But do we need this feature? Special prints can be handled through special classes that encapsulate the basic type.
    virtual size_t printFormatted(Print& out, unsigned long imem) const = 0;
};
#endif // PrintFormatter_h ========================================================================================================

#include "Print.h"

// still in development, to be published independently when more or less stable.
#ifndef PrintSequence_h // == PrintSequence.h =====================================================================================
#define PrintSequence_h
class Print;
class EndSequence
{
  friend class PrintSequence;
  virtual unsigned int printCloseString(Print& out) const { return 0;}
};

//TODO define derived class ClosedPrintSequence (which only has the method to Open() ), Alternative: use PrintSequence and OpenPrintSequence
class PrintSequence
{
protected:
  Print&      myPrint;
  PrintFormat myPrintFormat;
  PrintFormat myDefaultPrintFormat;
  int nchar;
  virtual unsigned int printOpenString(Print& out) const { return 0;}

public:
  PrintSequence(Print& aPrint) : myPrint(aPrint), myDefaultPrintFormat(0), myPrintFormat(0) { Open(); }
  void reset() { nchar=0; }
  int  end()   { return nchar; }

  inline PrintSequence& Open()                                  { nchar = printOpenString(myPrint);                                                  return *this; }
  inline unsigned int   operator<<(const EndSequence& item)     { int nres = nchar+item.printCloseString(myPrint); nchar = 0;                        return  nres; }

  inline PrintSequence& operator<<(const PrintFormat&   format) { myPrintFormat = myPrintFormat & format;                                            return *this; }
  inline PrintSequence& operator<<(const IntegerFormat& format) { myPrintFormat = myPrintFormat & format;                                            return *this; }
  inline PrintSequence& operator<<(const FloatFormat&   format) { myPrintFormat = myPrintFormat & format;                                            return *this; }
  inline PrintSequence& operator<<(const char item)             { nchar += myPrint.print(item);                myPrintFormat = myDefaultPrintFormat; return *this; }
  template <typename R>
  inline PrintSequence& operator<<(const R item)                { nchar += myPrint.print(item, myPrintFormat); myPrintFormat = myDefaultPrintFormat; return *this; }
};

// define as weak so it can be overridden;
// PrintSequence Print::getPrintSequence() __attribute__((weak));
PrintSequence Print::getPrintSequence() { return PrintSequence(*this); }

#endif //PrintSequence_h ==========================================================================================================


#ifdef testCode
#include "Print.cpp"

#include <stdio.h>
#include <string.h>

class Serial : public Print
{
  public:
  virtual size_t write(uint8_t c)
  {
    printf("%c", c);
    return 1;
  }
} serial;

void printTest(); // forward definition

int main()
{
  printTest();
}
#endif


#ifndef PrintTest_cpp // == PrintTest.cpp =========================================================================================
#define PrintTest_cpp

#include "Print.h"

//long&  asLong (const float& aFloat) { return (long&)  aFloat;} // interpret a float as long
//float& asFloat(const long&  aLong)  { return (float&) aLong; } // interpret a long  as float

void printTest()
{
  unsigned int nc;
  unsigned int i;

  serial.println("\nRadix test:");
  IntegerFormat di = HEX & FIELDSIZE(2) & FILLZEROS;
  IntegerFormat ti = RADIX(0) & FIELDSIZE(4);
  serial.print( 0, di); nc=serial.print(42u, ti & RADIX( 0) & FIELDSIZE(8)); serial.print(" nc="); serial.println(nc); // will result in the default radix
  serial.print( 1, di); nc=serial.print(42u, ti & RADIX( 1) & FIELDSIZE(8)); serial.print(" nc="); serial.println(nc); // should become binary format?
  serial.print( 2, di); nc=serial.print(42u, ti & RADIX( 2) & FIELDSIZE(8)); serial.print(" nc="); serial.println(nc);
  ti = ti & FIELDSIZE(8);
  for(i=3; i<35; i++)
  {
    serial.print( i, di); nc=serial.print(42u, ti & RADIX( i) ); serial.print(" nc="); serial.println(nc);
  }

  serial.println("\nPrecision test with printPattern");
  IntegerFormat df = DEC & FIELDSIZE(2);
  PrintFormat   tf = FIELDSIZE(8);
  for(i=3; i<32; i++)
  {
    serial.print( i, df);
    nc =serial.print(41.9876543210987654321098765432109876543210, tf & PRECISION( i) );
    nc+=serial.print("  ");
    nc+=serial.printPattern((char*) " - -.- - |", 42-nc, nc%10);
    serial.print(" nc="); serial.println(nc,FIELDSIZE(2));
  }

  serial.printPattern((char*) " - -.- - |\0undocumented feature :", 42, 11);
  serial.println();

  serial.println(F("test print FlashStringHelper: serial.print(F(\"waar eens de boterbloemples bloeiden, staat nu een ma-af palei ... s\"))"));

  serial.println("\ndefault test:");
  serial.println(0.);

  serial.println("\nspecial float test");
  {
  nc=serial.println("test positive float overflow");
  FloatFormat ff = FIELDSIZE(15) & PRECISION(2);
  nc=serial.print("0x"); nc +=serial.print(      0xffffff60u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffff60u); nc +=serial.print( float(0xffffff60u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffff70u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffff70u); nc +=serial.print( float(0xffffff70u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffff7fu, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffff7fu); nc +=serial.print( float(0xffffff7fu), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffff80u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffff80u); nc +=serial.print( float(0xffffff80u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffff81u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffff81u); nc +=serial.print( float(0xffffff81u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffff90u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffff90u); nc +=serial.print( float(0xffffff90u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffffa0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffffa0u); nc +=serial.print( float(0xffffffa0u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffffb0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffffb0u); nc +=serial.print( float(0xffffffb0u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffffc0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffffc0u); nc +=serial.print( float(0xffffffc0u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffffd0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffffd0u); nc +=serial.print( float(0xffffffd0u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffffe0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffffe0u); nc +=serial.print( float(0xffffffe0u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xfffffff0u, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xfffffff0u); nc +=serial.print( float(0xfffffff0u), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(      0xffffffffu, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(      0xffffffffu); nc +=serial.print( float(0xffffffffu), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.println("test negative float overflow");
  nc=serial.print("0x"); nc +=serial.print((unsigned long)4294967136., HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print((unsigned long)4294967136.); nc +=serial.print(       -4294967136., ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print((unsigned long)4294967152., HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print((unsigned long)4294967152.); nc +=serial.print(       -4294967152., ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print((unsigned long)4294967167., HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print((unsigned long)4294967167.); nc +=serial.print(       -4294967167., ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print((unsigned long)4294967168., HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print((unsigned long)4294967168.); nc +=serial.print(       -4294967168., ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print((unsigned long)4294967169., HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print((unsigned long)4294967169.); nc +=serial.print(       -4294967169., ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print((unsigned long)4294967184., HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print((unsigned long)4294967184.); nc +=serial.print(       -4294967184., ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print((unsigned long)4294967200., HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print((unsigned long)4294967200.); nc +=serial.print(       -4294967200., ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.println("test negative float nan & inf"); // see also http://en.wikipedia.org/wiki/IEEE_754-1985 and 
  nc=serial.print("0x"); nc +=serial.print(0x7F800000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0x7F800000, FIELDSIZE(10)); nc +=serial.print(asFloat(0x7F800000), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(0x7F800001, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0x7F800001, FIELDSIZE(10)); nc +=serial.print(asFloat(0x7F800001), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(0x7FC00000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0x7FC00000, FIELDSIZE(10)); nc +=serial.print(asFloat(0x7FC00000), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(0x7FFFFFFF, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0x7FFFFFFF, FIELDSIZE(10)); nc +=serial.print(asFloat(0x7FFFFFFF), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(0xFF800000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0xFF800000, FIELDSIZE(10)); nc +=serial.print(asFloat(0xFF800000), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(0xFF800001, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0xFF800001, FIELDSIZE(10)); nc +=serial.print(asFloat(0xFF800001), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(0xFFC00000, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0xFFC00000, FIELDSIZE(10)); nc +=serial.print(asFloat(0xFFC00000), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  nc=serial.print("0x"); nc +=serial.print(0xFFFFFFFF, HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(0xFFFFFFFF, FIELDSIZE(10)); nc +=serial.print(asFloat(0xFFFFFFFF), ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));

  float negnan = asFloat(0xFFFFFFFF);
  nc=serial.println("test negative float nan conversion");
  nc=serial.print("0x"); nc +=serial.print(asLong(negnan), HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(asLong(negnan), FIELDSIZE(10)); nc +=serial.print(negnan, ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  if(asLong(negnan)&0x80000000) { asLong(negnan) = asLong(negnan) & ~0x80000000; }
  nc=serial.print("0x"); nc +=serial.print(asLong(negnan), HEX & FIELDSIZE(8) & FILLZEROS); nc +=serial.print(" "); nc +=serial.print(asLong(negnan), FIELDSIZE(10)); nc +=serial.print(negnan, ff); nc +=serial.print(" | "); nc +=serial.println(nc,FIELDSIZE(2));
  }
  serial.println("\ntest FIELDSIZE(5)");
  {
  serial.println(       0, DEC & FIELDSIZE(5));
  serial.println(       1, DEC & FIELDSIZE(5));
  serial.println(      12, DEC & FIELDSIZE(5));
  serial.println(     123, DEC & FIELDSIZE(5));
  serial.println(    1234, DEC & FIELDSIZE(5));
  serial.println(   12345, DEC & FIELDSIZE(5));
  serial.println(  123456, DEC & FIELDSIZE(5));
  serial.println( 1234567, DEC & FIELDSIZE(5));
  serial.println(-      1, DEC & FIELDSIZE(5));
  serial.println(-     12, DEC & FIELDSIZE(5));
  serial.println(-    123, DEC & FIELDSIZE(5));
  serial.println(-   1234, DEC & FIELDSIZE(5));
  serial.println(-  12345, DEC & FIELDSIZE(5));
  serial.println(- 123456, DEC & FIELDSIZE(5));
  serial.println(-1234567, DEC & FIELDSIZE(5));

  serial.println("\ntest FIELDSIZE(5) & STRICTSIZE");

  serial.println(       0, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(       1, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(      12, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(     123, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(    1234, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(   12345, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(  123456, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println( 1234567, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(-      1, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(-     12, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(-    123, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(-   1234, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(-  12345, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(- 123456, DEC & FIELDSIZE(5) & STRICTSIZE);
  serial.println(-1234567, DEC & FIELDSIZE(5) & STRICTSIZE);

  serial.println("\ntest FIELDSIZE(5) & STRICTSIZE & FORCESIGN");
  serial.println(       0, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(       1, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(      12, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(     123, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(    1234, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(   12345, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(  123456, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println( 1234567, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(-      1, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(-     12, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(-    123, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(-   1234, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(-  12345, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(- 123456, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);
  serial.println(-1234567, DEC & FIELDSIZE(5) & STRICTSIZE & FORCESIGN);

  serial.println("\ntest FIELDSIZE(1) & STRICTSIZE");
  serial.println(       0, DEC & FIELDSIZE(1) & STRICTSIZE);
  serial.println(       1, DEC & FIELDSIZE(1) & STRICTSIZE);
  serial.println(      12, DEC & FIELDSIZE(1) & STRICTSIZE);
  serial.println(     123, DEC & FIELDSIZE(1) & STRICTSIZE);
  serial.println(-      1, DEC & FIELDSIZE(1) & STRICTSIZE);
  serial.println(-     12, DEC & FIELDSIZE(1) & STRICTSIZE);
  serial.println(-    123, DEC & FIELDSIZE(1) & STRICTSIZE);
  serial.println("\ntest FIELDSIZE(1) & STRICTSIZE & FORCESIGN");
  serial.println(       0, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  serial.println(       1, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  serial.println(      12, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  serial.println(     123, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  serial.println(-      1, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  serial.println(-     12, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  serial.println(-    123, DEC & FIELDSIZE(1) & STRICTSIZE & FORCESIGN);
  }
  serial.println("\ntest alignments FIELDSIZE(8)");
  {
  serial.print( 1234, DEC & FIELDSIZE(8) & FILLZEROS );                         serial.println(" : & FILLZEROS");
  serial.print( 1234, DEC & FIELDSIZE(8) );                                     serial.println(" : &");
  serial.print( 1234, DEC & FIELDSIZE(8) & ALIGNLEFT );                         serial.println(" : & ALIGNLEFT");
  serial.print( 1234, DEC & FIELDSIZE(8) & ALIGNLEFT & FILLZEROS );             serial.println(" : & ALIGNLEFT & FILLZEROS");
  serial.print(-1234, DEC & FIELDSIZE(8) & FILLZEROS );                         serial.println(" : & FILLZEROS");
  serial.print(-1234, DEC & FIELDSIZE(8) );                                     serial.println(" : &");
  serial.print(-1234, DEC & FIELDSIZE(8) & ALIGNLEFT );                         serial.println(" : & ALIGNLEFT");
  serial.print(-1234, DEC & FIELDSIZE(8) & ALIGNLEFT & FILLZEROS );             serial.println(" : & ALIGNLEFT & FILLZEROS");
  serial.println("\ntest alignments with FIELDSIZE(8) & FORCESIGN");
  serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN & FILLZEROS );             serial.println(" : & FILLZEROS");
  serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN );                         serial.println(" : &");
  serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT );             serial.println(" : & ALIGNLEFT");
  serial.print( 1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT & FILLZEROS ); serial.println(" : & ALIGNLEFT & FILLZEROS");
  serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN & FILLZEROS );             serial.println(" : & FILLZEROS");
  serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN );                         serial.println(" : &");
  serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT );             serial.println(" : & ALIGNLEFT");
  serial.print(-1234, DEC & FIELDSIZE(8) & FORCESIGN & ALIGNLEFT & FILLZEROS ); serial.println(" : & ALIGNLEFT & FILLZEROS");
  }
  PrintFormat pf0 = FORCESIGN;
#ifdef testCompilationErrors
  PrintFormat pf1 = HEX;                                    // error : ‘PrintFormat’ is an inaccessible base of ‘IntegerFormat’
  PrintFormat pf2 = PRECISION(4);                           // error : ‘PrintFormat’ is an inaccessible base of ‘FloatFormat’
  PrintFormat pf3 = HEX & PRECISION(4);                     // error : conversion from ‘invalid_PrintFormat_operation’ to non-scalar type ‘PrintFormat’ requested
  PrintFormat pf4 = PRECISION(4) & HEX;                     // error : conversion from ‘invalid_PrintFormat_operation’ to non-scalar type ‘PrintFormat’ requested
  PrintFormat pf5 = PRECISION(4) & FIELDSIZE(4) & HEX;      // error : conversion from ‘invalid_PrintFormat_operation’ to non-scalar type ‘PrintFormat’ requested
  PrintFormat pf6 = PRECISION(4) & FIELDSIZE(4) & 42;       // error : conversion from ‘invalid_PrintFormat_operation’ to non-scalar type ‘PrintFormat’ requested


  int i0 = HEX & PRECISION(4);                              // error : cannot convert ‘invalid_PrintFormat_operation’ to ‘int’ in initialization
  int i1 = PRECISION(4) & HEX;                              // error : cannot convert ‘invalid_PrintFormat_operation’ to ‘int’ in initialization
  int i2 = PRECISION(4) & FIELDSIZE(4) & HEX;               // error : cannot convert ‘invalid_PrintFormat_operation’ to ‘int’ in initialization
  int i3 = PRECISION(4) & FIELDSIZE(4) & 42;                // error : conversion from ‘invalid_PrintFormat_operation’ to non-scalar type ‘PrintFormat’ requested

  
  
  serial.print((unsigned)42, FIELDSIZE(4) & PRECISION(4) & FILLZEROS & FORCESIGN);  // error : ‘PrintFormat’ is an inaccessible base of ‘FloatFormat’
  serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & FILLZEROS & FORCESIGN);  // error : ‘PrintFormat’ is an inaccessible base of ‘FloatFormat’
  
  serial.print((unsigned)42, HEX & PRECISION(4) & FIELDSIZE(4) & FILLZEROS & FORCESIGN);                              // error : ambiguous overload for ‘operator&’ (operand types are ‘const IntegerFormat’ and ‘const FloatFormat’)
  serial.print((unsigned)42, PRECISION(4) & HEX & FIELDSIZE(4) & FILLZEROS & FORCESIGN);                              // error : ambiguous overload for ‘operator&’ (operand types are ‘const FloatFormat’ and ‘const IntegerFormat’)
  serial.print((unsigned)42, 42 & PRECISION(4) & FIELDSIZE(4) & HEX & FILLZEROS & FORCESIGN);                         // error : ambiguous overload for ‘operator&’ (operand types are ‘int’ and ‘const FloatFormat’)
  serial.print((unsigned)42, PRECISION(4) & 42 & FIELDSIZE(4) & HEX & FILLZEROS & FORCESIGN);                         // error : ambiguous overload for ‘operator&’ (operand types are ‘const FloatFormat’ and ‘int’)
  serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & HEX & 42 & FILLZEROS & FORCESIGN);                         // error : ambiguous overload for ‘operator&’ (operand types are ‘FloatFormat’ and ‘const IntegerFormat’)
  serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & HEX & FILLZEROS & FORCESIGN); // PrintFormat               // error : ambiguous overload for ‘operator&’ (operand types are ‘FloatFormat’ and ‘const IntegerFormat’)
  serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & 42 & HEX & FILLZEROS & FORCESIGN); // IntegerFormat        // error : ambiguous overload for ‘operator&’ (operand types are ‘FloatFormat’ and ‘int’)
  serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & HEX ); // & FILLZEROS & FORCESIGN); // PrintFormat         // error : ambiguous overload for ‘operator&’ (operand types are ‘FloatFormat’ and ‘const IntegerFormat’)
  serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & 42 ); // & HEX & FILLZEROS & FORCESIGN); // IntegerFormat  // error : ambiguous overload for ‘operator&’ (operand types are ‘FloatFormat’ and ‘int’)
  
  
  
  serial.print((unsigned)42, FIELDSIZE(4) & HEX & PRECISION(4) & FILLZEROS & FORCESIGN);  // error : ambiguous overload for ‘operator&’ (operand types are ‘IntegerFormat’ and ‘const FloatFormat’)
  serial.print((unsigned)42, FIELDSIZE(4) & PRECISION(4) & HEX & FILLZEROS & FORCESIGN);  // error : ambiguous overload for ‘operator&’ (operand types are ‘FloatFormat’ and ‘const IntegerFormat’)
  serial.print((unsigned)42, PRECISION(4) & FIELDSIZE(4) & FILLZEROS & FORCESIGN);        // error : call of overloaded ‘print(unsigned int, FloatFormat)’ is ambiguous

  // no match for operator
  serial.print((unsigned)42,  FIELDSIZE(4) || HEX || FILLZEROS);
  serial.print((unsigned)42,  FIELDSIZE(4) |  HEX |  FILLZEROS);
  serial.print((unsigned)42,  FIELDSIZE(4) && HEX && FILLZEROS);
  serial.print((unsigned)42,  FIELDSIZE(4) &  HEX &  FILLZEROS);     // ok
  serial.print((unsigned)42,  FIELDSIZE(4) +  HEX +  FILLZEROS);     // not initutive, because HEX+HEX = ??
  serial.print((unsigned)42,  FIELDSIZE(4),   HEX,   FILLZEROS);     // do not think this is technically possible, and if so (vararg) inefficient
  serial.print((unsigned)42, (FIELDSIZE(4),   HEX,   FILLZEROS)  );  // unnatural (but could be made to work by overloading the operator,
  
  PrintFormat myPrintFormat1 = FIELDSIZE(4) & RADIX(4) & FILLZEROS;  // error : ‘PrintFormat’ is an inaccessible base of ‘IntegerFormat’
  serial.print((unsigned)42,  myPrintFormat1);
  serial.print((unsigned)42,  myPrintFormat1 & ~FILLZEROS);
  
  PrintFormat myPrintFormat2 = FIELDSIZE(4) + RADIX(4) + FILLZEROS;  // error : no match for ‘operator+’ (operand types are ‘const PrintFormat’ and ‘const IntegerFormat’)
  serial.print((unsigned)42,  myPrintFormat2);
  serial.print((unsigned)42,  myPrintFormat2 -FILLZEROS);            // error : no match for ‘operator-’ (operand types are ‘PrintFormat’ and ‘const PrintFormat’)
  
  (FIELDSIZE(4) + HEX + HEX + HEX) == (FIELDSIZE(4) + HEX);          // error : no match for ‘operator+’ (operand types are ‘const PrintFormat’ and ‘const IntegerFormat’)
#endif

  serial.println('\n');

  int n;
  n = serial.getPrintSequence()
  <<F(" F()'d strings: ") << F("Waar eens het geld werd uitgevonden, trap ik nu in de hondepoe  ... p")
  <<" Two    " <<2<<'\n'
  <<" A      " <<3<<4<<'\n'
  <<" float1 "<<3.4<<'\n'
  <<" float2 "<<  FIELDSIZE(4) << STRICTSIZE                  <<123456.54321<<'\n'
  <<" float3 "<< (FIELDSIZE(4) &  STRICTSIZE &  PRECISION(4)) <<123456.54321<<'\n'
  <<" float4 "<<  FIELDSIZE(4) << STRICTSIZE << PRECISION(4)  <<123456.54321<<'\n'
  <<" float5 "<< (FIELDSIZE(4) &  STRICTSIZE &  HEX         ) <<123456.54321<<'\n'
  <<" float6 "<<  FIELDSIZE(4) << STRICTSIZE << HEX           <<123456.54321<<'\n'
  <<" float7 "<<  HEX << FIELDSIZE(4) << STRICTSIZE << PRECISION(4) <<123456.54321<<'\n'
  <<" float8 "<<  FIELDSIZE(4) << STRICTSIZE << PRECISION(4) << HEX <<123456.54321<<'\n'
//<<" float9 "<<  FIELDSIZE(4) &  STRICTSIZE & PRECISION(4)  <<123456.54321<<'\n' // error : no match for ‘operator&’ (operand types are ‘PrintSequence’ and ‘const PrintFormat’)
//<<" floatA "<< (3.4 << FIELDSIZE(8) & STRICTSIZE) <<'\n'                        // error : no match for ‘operator<<’ (operand types are ‘double’ and ‘const PrintFormat’)
//<<" floatB "<< FIELDSIZE(8) & STRICTSIZE <<3.4<<'\n'                            // error : no match for ‘operator<<’ (operand types are ‘const PrintFormat’ and ‘double’)
//<<" float "<<HEX & FIELDSIZE(4) % 3.4<<'\n'
//<<" float "<<PrintFormat(HEX&FIELDSIZE(4))% 3.4<<'\n'
  <<EndSequence();

  serial.print(n);
  serial.print(" characters printed\n");

  class MyEndSequence : public EndSequence
  {
    unsigned int printCloseString(Print& out) const { return out.println("}"); }
  };
  class MyEndSequence myEndSequence;
  

  n = serial.getPrintSequence()<<"{\n"
  <<" One " << F(" ach zal de mensheid ooit eens leren, te leven zonder bruut geweld...  ") <<NEWLINE
  <<" Two " << F(" etc... waar moet dat heen, hoe zal dat gaan, waar komt die rotsooi toch vandaan") <<NEWLINE
  <<myEndSequence;

  PrintSequence out(serial);
  out<<n <<" characters printed\n";
  out<<"How about that\n";
}
#endif //PrintTest_cpp ============================================================================================================



/* Research:
// useful constructs only available in C++11
// using PrintFormat::PrintFormat; // inheriting constructors
// PrintFormat() = delete;         // inheriting constructors, default / delete
// template<typename T_lhs, typename T_rhs> inline friend IntegerFormat operator&(T_lhs lhs, T_rhs rhs) = delete; // default or delete 
// template<typename... T> inline friend void operator&(T...) = delete                                            // variadic templates
//
// use of SFINAE
http://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error
http://stackoverflow.com/questions/982808/c-sfinae-examples           (=== TypeSink example ====)
http://stackoverflow.com/questions/26220144/disable-default-template-and-only-use-specialization-through-sfinae
http://stackoverflow.com/questions/7986125/disabling-hiding-functions-in-a-template-based-on-compile-time-constants

//hiding or unhiding members in derived classes (not sure this will work for constructors)
http://www.learncpp.com/cpp-tutorial/116-adding-changing-and-hiding-members-in-a-derived-class/

// what is explicit good for?? (http://programmers.stackexchange.com/questions/197893/why-are-constructors-not-inherited)

can one access a base class from a derived class (http://www.cplusplus.com/forum/general/35681/)
class Derived : Base {} d;
d.base::


http://stackoverflow.com/questions/1628768/why-does-an-overridden-function-in-the-derived-class-hide-other-overloads-of-the


c++ traits (see reference: http://www.cplusplus.com/reference/type_traits/is_integral/

disabling casts: http://www.gockelhut.com/c++/articles/disable_implicit_casts
void foo(uint32_t x);
// delete all implied one parameter foo functions:
template <typename T> void foo(T) = delete;
//or in general, delete all implied foo functions
template <typename... T>  void func_name(T...) = delete

declare constructor as explicit could do?
*/

/* PrintSequence

http://en.cppreference.com/w/cpp/language/operator_precedence
Precedence  Operator          O Description                                                 Associativity 
1           ::                x Scope resolution                                            Left-to-right  
2           ++ --               Suffix/postfix increment and decrement  
            type() type{}     ? Function-style type cast  
            ()                  Function call  
            []                  Array subscripting  
            .                 x Element selection by reference  
            ->                  Element selection through pointer  
3           ++ --               Prefix increment and decrement                              Right-to-left  
            +  -                Unary plus and minus  
            !  ~                Logical NOT and bitwise NOT  
            (type)            ? C-style type cast  
            *                   Indirection (dereference)  
            &                   Address-of  
            sizeof            ? Size-of[note 1]  
            new, new[]          Dynamic memory allocation  
            delete, delete[]    Dynamic memory deallocation  
4           .*                x Pointer to member                                           Left-to-right  
               ->*              Pointer to member                                           Left-to-right  
5           *  /  %             Multiplication, division, and remainder  
6           +  -                Addition and subtraction  
7           << >>               Bitwise left shift and right shift  
8           <  <=               For relational operators < and = respectively  
            >  >=               For relational operators > and = respectively  
9           == !=               For relational = and ? respectively  
10          &                   Bitwise AND  
11          ^                   Bitwise XOR (exclusive or)  
12          |                   Bitwise OR (inclusive or)  
13          &&                  Logical AND  
14          ||                  Logical OR  
15          ?:                x Ternary conditional[note 2]                                 Right-to-left  
            =                   Direct assignment (provided by default for C++ classes)  
            += -=               Assignment by sum and difference  
            *= /= %=            Assignment by product, quotient, and remainder  
            <<= >>=             Assignment by bitwise left shift and right shift  
            &= ^= |=            Assignment by bitwise AND, XOR, and OR  
16          throw             ? Throw operator (for exceptions)  
17          ,                   Comma                                                       Left-to-right  
 
DFDDFPS DefaultFormated_DefaultDefaultFormat_PrintSequence
SFDDFPS  StringFormated_DefaultDefaultFormat_PrintSequence
IFDDFPS IntegerFormated_DefaultDefaultFormat_PrintSequence
FFDDFPS   FloatFormated_DefaultDefaultFormat_PrintSequence

DFDSFPS DefaultFormated_StringDefaultFormat_PrintSequence
SFDSFPS  StringFormated_StringDefaultFormat_PrintSequence
IFDSFPS IntegerFormated_StringDefaultFormat_PrintSequence
FFDSFPS   FloatFormated_StringDefaultFormat_PrintSequence

DFDIFPS DefaultFormated_IntegerDefaultFormat_PrintSequence
SFDIFPS  StringFormated_IntegerDefaultFormat_PrintSequence
IFDIFPS IntegerFormated_IntegerDefaultFormat_PrintSequence
FFDIFPS   FloatFormated_IntegerDefaultFormat_PrintSequence

DFDFFPS DefaultFormated_FloatDefaultFormat_PrintSequence
SFDFFPS  StringFormated_FloatDefaultFormat_PrintSequence
IFDFFPS IntegerFormated_FloatDefaultFormat_PrintSequence
FFDFFPS   FloatFormated_FloatDefaultFormat_PrintSequence

operation <<PrintFormat - - - - - - - - - - - - - - -  <<primitive - - - - - - - - - -  <<
          PF        SF          IF          FF         S           I           F
DF_df     DF_df     SF_df       IF_df       FF_df      DF_df       DF_df       DF_df  
SF_df     SF_df     SF_df       xx          xx         DF_df       xx          xx   
IF_df     IF_df     xx          IF_df       xx         xx          DF_df       xx   
FF_df     FF_df     xx          xx          FF_df      xx          xx          DF_df
                                                                                    
DF_sf     DF_sf     SF_sf       IF_sf       FF_sf      SF_sf     !-IF_sf     !-FF_sf
SF_sf     SF_sf     SF_sf       xx          xx         SF_sf       xx          xx   
IF_sf     IF_sf     xx          IF_sf       xx         xx        !-IF_sf       xx   
FF_sf     FF_sf     xx          xx          FF_sf      xx          xx        !-FF_sf
                                                                                    
DF_if     DF_if     SF_if       IF_if       FF_if    !-IF_if       IF_if     !-FF_if
SF_if     SF_if     SF_if       xx          xx       !-IF_if       xx          xx   
IF_if     IF_if     xx          IF_if       xx         xx          IF_if       xx   
FF_if     FF_if     xx          xx          FF_if      xx          xx        !-FF_if
                                                                                    
DF_ff     DF_ff     SF_ff       IF_ff       FF_ff    !-SF_ff     !-IF_ff       FF_ff
SF_ff     SF_ff     SF_ff       xx          xx       !-SF_ff       xx          xx   
IF_ff     IF_ff     xx          IF_ff       xx         xx        !-IF_ff       xx   
FF_ff     FF_ff     xx          xx          FF_ff      xx          xx          FF_ff

out<<HEX<<32<<" "<<32
*/

/* Still todo:

implement
//- radix offset handling
//- precision offset handling
//- fieldsize offset handling
//- ALIGNLEFT
//- STRICTSIZE
//- SIGNED  inf nan ovf
//- ALIGNED inf nan ovf
//- use printString in printNumber
//- add inline padding function printPadding(char padding, unsigned char size)
//- printPadding to return number of characters written;
//- use printPadding in printString
//- use printPadding to add precision space for inf nan ovf
//- add inline pattern function printPattern(char* pattern, unsigned char size)
//- add tests cases for nan and inf
//- recognize negative nan
- test on Arduino and release
- format control in PrintSequence
= in progress
  PrintSequence maintains a nextPrintFormat and defaultprintFormat
  After each print() invocation, nextPrintFormat will be reset to defaultprintFormat
  the  operator<<(PrintSequence, PrintFormat) will merge the PrintFormat operand with the nextPrintFormat value.
  the  operator<<(PrintSequence, EndSequence) will reset nextPrintFormat and defaultPrintFormat to the PrintSequence class default;

  example:
  PrintSequence myPrintSequence = serial.getPrintSequence();
  PrintSequence myPrintSequence(serial); // alternative
  myPrintSeqeunce<<"{test 0x"<<HEX<<FILLZEROS<<FIELDSIZE(8)<<363880<<" again "<<363880<<"}\n"<<
  will print: {test 0x00009010 again 363880}

= The PrintSequence defaultPrintFormat can be changed by an operation on the PrintSequence
  This should be an intuitive operation, options are:
  - a new operator e.g. 
    myPrintSequence <<"Constants:" % PRECISION(2) % FIELDSIZE(8)  << " pi="<<3.1415<<" e="<<2.71828<<" sqrt(2)="<< 1.414;
    myPrintSequence <<"Constants:" %(PRECISION(2) & FIELDSIZE(8)) << " pi="<<3.1415<<" e="<<2.71828<<" sqrt(2)="<< 1.414;
  - a DEFAULT object of class SetFormatDefault that can be used with the << operator:
    class SetFormatDefault {} DEFAULT
    operator<<(SetFormatDefault item) { }
    myPrintSequence <<"Constants:" <<PRECISION(2) <<FIELDSIZE(8) <<DEFAULT   << " pi="<<3.1415<<" e="<<2.71828<<" sqrt(2)="<< 1.414;
    myPrintSequence <<"Constants:" <<DEFAULT(PRECISION(2)&FIELDSIZE(8))      << " pi="<<3.1415<<" e="<<2.71828<<" sqrt(2)="<< 1.414;
    myPrintSequence <<"Constants:" .setDefault(PRECISION(2) & FIELDSIZE(8) ) << " pi="<<3.1415<<" e="<<2.71828<<" sqrt(2)="<< 1.414;

- adaptable PrintSequence (e.g. to add sequence header and sequence trailer); Provide hooks to add e.g. CRC)
- add state handling to a printSequence (to distinguish open and closed sequences
- change size from unsigned int to unsigned char in print(char* cstr, unsigned char size)
- printString specific options
- optimize floatPrint
- StringFormat
- ScientificFormat
- printScientific

FIELDSIZE(n), RADIX(n), PRECISION(n)
a value of n=0 indicates the default.
IntegerFormat mfb = DEC & FIELDSIZE(4);
IntegerFormat mf1 = mfb & HEX;          //should yield HEX & FIELDSIZE(4);
IntegerFormat mf2 = mfb & FIELDSIZE(2); //should yield DEC & FIELDSIZE(2);

Hence FIELDSIZE(0), RADIX(0), PRECISION(0) are bot available. We can get the 0 values back if we could have a negation ~
other solution, each format has a bit mask showing the active fields, this bit mask is only used for combining format fields


Study:
- implementation of .withSize() .withEtc() functions
- footprint
- invalid_PrintFormat_operation sink, i.e. define a 'black hole' class that absorbs all non defined operations involving classes derived from PrintFormat  

Naming conventions
- namespace ArduinoPrint, vs nested namespace Arduino::Print::
- using namespace by default?

- PrintFormat
- IntegerFormat
- FloatFormat
- StringFormat
- ScientificFormat
- NumericFormat

- RADIX(n)      note: free choice RADIX alternative would be BASE(n)
                      RADIX(0) means unchanged (or default) radix.
                      RADIX(1) should be RADIX(10) or binary format
- BIN           note: is implemented as RADIX(2)
- OCT           idem RADIX(8)
- DEC           idem RADIX(10)
- HEX           idem RADIX(16)
- FIELDSIZE(n)  note: FIELDSIZE does not count forced signs, and float fractions
- ALIGNLEFT     note: not allowed for float format
- FORCESIGN
- FILLZEROS     note FILLZEROS with ALIGNLEFT will fill dashes, should we rename FILLZEROS with FILLALT
- STRICTSIZE    note better alternative ENFORCEDSIZE, or maybe STRICTSIZE(n)
- PRECISION(n)
- EXPONENT EXPONENT(n) SCIENTIFIC(n)  Note, still to be implemented, but what format to be chosen


breaking of old code
- RADIX(0) now gives default, used to give raw format, RADIX91) gives Decimal, should give raw format? swap RADIX(0) and RADIX(1)?
- decimal point in float is not suppressed (or should precision parameter include the decimal point)

Discussion
- suppress decimal point option?    (one could convert floats to int before printing, one can add a decimal point after printing)
- implement .withSize() etc... explain the fear for an "unhealthy" explosion of added methods for all possible derived classes.
- customFormatter, i.e. print(elementary_type, customFormatter)
  a custom formatter class is lengthy and boring to implement: one cannot define templates for virtual methods (dat is logisch he!)
- To avoid defining a myriad of virtual function for every basic type, one may concentrate the virtual methods down to a small selection of
  virtual functions, i.e. printInteger, printFloat, printString and printPrintable
  However, the custom formatter class remains questionable, encapsulation of data in a printable adapter class may be more appropriate:
  
  // Example,instead of
  serial.print(myIpAdress, myIpAdressFormatter);
  // one may write
  serial.print(IpAddressPrintAddapter(myIpAddress));
  
  // with
  class IpAddressPrintAddapter : Printable
  {
    union
    {
      unsigned long _long;
      unsigned char field[4];
    } myIpAddress;
    IpAddressPrintAddapter(unsigned long anIpAddress) : myIpAddress._long(anIpAddress) {}
    virtual unsigned int toPrint(Print& out)
    {
      for(int i=0;i<4; i++) {if(i!=0) print('.'); print(myIpAddress.field[i]);}
    }
  };
  
 
  
  
*/
// eof