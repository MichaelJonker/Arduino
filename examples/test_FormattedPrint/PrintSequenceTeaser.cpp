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
 PrintSequenceTeaser.cpp\
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
  -mmcu=atmega328p -Os PrintSequenceTeaser.cpp \
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

#define Arduino_h
#define _define_static_PrintFormat_instances // force declaration of static PrintFormat instances here (in PrintFromat.h included from Print.h)
#include "Print.h"

#else
#include "Arduino.h"

#endif

#define PrintFormatter_h // nah, we do not want the printFormatter
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

#include <stdio.h>
#include <string.h>

class HardwareSerial : public Print
{
  public:
  virtual size_t write(uint8_t c)
  {
    printf("%c", c);
    return 1;
  }
} Serial;

void printTest(); // forward definition
void printSequenceTeaser();

int main()
{
  printTest();
  printSequenceTeaser();
}

#include "Print.cpp"
#include "PrintTest.cpp"
#endif


#ifndef PrintSequenceTeaser_cpp // == PrintSequenceTeaser.cpp =====================================================================
#define PrintSequenceTeaser_cpp

void printSequenceTeaser()
{
  Serial.print(F("\nrunning PrintSequence teaser... ===========================================================================\n"));
  Serial.print(F("\nSerial.getPrintSequence()<<\"one \"<<1<<\" two=\"<<2.1<<NEWLINE<<\" etc, all in one sequence...\"\n"));

  int n;
  n = Serial.getPrintSequence()<<"one "<<1<<", two="<<2.1<<NEWLINE<<" etc, all in one sequence...\n"
  <<F(" one:   ") <<   "Waar eens de boterbloepjes bloeiden, staat nu een ma-af palei   (s)\n"
  <<F(" F()'d: ") << F("Waar eens het geld werd uitgevonden, trap ik nu in de hondepoe  (p)\n")
  <<" <<2    : " <<2<<'\n'
  <<" <<3<<4 : " <<3<<4<<'\n'
  <<" <<3.4  : " <<3.4<<'\n'
  <<F(" << PRECISION(1) <<3.4 : ")<<  PRECISION(1) <<3.4<<'\n'
  <<F(" << PRECISION(2) <<3.4 : ")<<  PRECISION(2) <<3.4<<'\n'
  <<F(" << FIELDSIZE(4) << STRICTSIZE                 << 123456.54321 : ")<<  FIELDSIZE(4) << STRICTSIZE                  <<123456.54321<<'\n'
  <<F(" <<(FIELDSIZE(4) &  STRICTSIZE &  PRECISION(4))<< 123456.54321 : ")<< (FIELDSIZE(4) &  STRICTSIZE &  PRECISION(4)) <<123456.54321<<'\n'
  <<F(" << FIELDSIZE(4) << STRICTSIZE << PRECISION(4) << 123456.54321 : ")<<  FIELDSIZE(4) << STRICTSIZE << PRECISION(4)  <<123456.54321<<'\n'
  <<F(" <<(FIELDSIZE(4) &  STRICTSIZE &  HEX         )<< 123456.54321 : ")<< (FIELDSIZE(4) &  STRICTSIZE &  HEX         ) <<123456.54321<<" // this is not nice, HEX is interpreted as PRECISION(16)!\n"
  <<F(" << FIELDSIZE(4) << STRICTSIZE << HEX          << 123456.54321 : ")<<  FIELDSIZE(4) << STRICTSIZE << HEX           <<123456.54321<<" // this is not nice, HEX is interpreted as PRECISION(16)!\n"
  <<F(" << HEX << FIELDSIZE(4) << STRICTSIZE << PRECISION(4) <<123456.54321 : ")<<  HEX << FIELDSIZE(4) << STRICTSIZE << PRECISION(4) <<123456.54321<<" // idem and in this context HEX is merged (hence overridden) with PRECISION()\n"
  <<F(" << FIELDSIZE(4) << STRICTSIZE << PRECISION(4) << HEX <<123456.54321 : ")<<  FIELDSIZE(4) << STRICTSIZE << PRECISION(4) << HEX <<123456.54321<<" // and vice versa\n"
  <<F(" << F(\" I am working on a solution\\n\")  :")<<F(" I am working on a solution\n")
//<<" float9 "<<  FIELDSIZE(4) &  STRICTSIZE & PRECISION(4)  <<123456.54321<<'\n' // error : no match for ?operator&? (operand types are ?PrintSequence? and ?const PrintFormat?)
//<<" floatA "<< (3.4 << FIELDSIZE(8) & STRICTSIZE) <<'\n'                        // error : no match for ?operator<<? (operand types are ?double? and ?const PrintFormat?)
//<<" floatB "<< FIELDSIZE(8) & STRICTSIZE <<3.4<<'\n'                            // error : no match for ?operator<<? (operand types are ?const PrintFormat? and ?double?)
//<<" float "<<HEX & FIELDSIZE(4) % 3.4<<'\n'
//<<" float "<<PrintFormat(HEX&FIELDSIZE(4))% 3.4<<'\n'
  <<F(" <<EndSequence() : ")<<EndSequence();

  Serial.print(n);
  Serial.print(" characters printed\n");

  class MyEndSequence : public EndSequence
  {
    unsigned int printCloseString(Print& out) const { return out.print('}'); }
  } myEndSequence;
  // TODO introduce StartSequence ??


  Serial.println("\nPrintSequence using a custom endsequence:");
  n = Serial.getPrintSequence()<<"{\n"
  << F(" ach zal de mensheid ooit eens leren, te leven zonder bruut geweld...  ") <<NEWLINE
  << F(" etc... waar moet dat heen, hoe zal dat gaan, waar komt die rotsooi toch vandaan") <<NEWLINE
  << F(" to appreciate this, you should see the next line in the code and output\n")
  << F(" << myEndSequence;\n") << myEndSequence;

  PrintSequence out(Serial);
  out<<" : "<<n <<" characters printed\n";
  Serial.print(F("\nEnd PrintSequence teaser. Stay tuned, coming soon... ======================================================\n\n"));
}
#endif // PrintSequenceTeaser_cpp =================================================================================================



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
//- test on Arduino and release
- format control in PrintSequence
= in progress
  PrintSequence maintains a nextPrintFormat and defaultprintFormat
  After each print() invocation, nextPrintFormat will be reset to defaultprintFormat
  the  operator<<(PrintSequence, PrintFormat) will merge the PrintFormat operand with the nextPrintFormat value.
  the  operator<<(PrintSequence, EndSequence) will reset nextPrintFormat and defaultPrintFormat to the PrintSequence class default;

  example:
  PrintSequence myPrintSequence = Serial.getPrintSequence();
  PrintSequence myPrintSequence(Serial); // alternative
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
  Serial.print(myIpAdress, myIpAdressFormatter);
  // one may write
  Serial.print(IpAddressPrintAddapter(myIpAddress));
  
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
