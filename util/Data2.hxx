#ifndef DATA2_HXX_
#define DATA2_HXX_

#include <string>
#include <iostream>
#include "LocalScopeAllocator.hxx"
#include "atomic.h"
#include "BugCatcher.hxx"
#include "RCObject.hxx"


#define NOT_FOUND -1
#define FIRST -2
#define FOUND 0
static const char SPACE[] = " ";

/** 
    Data represents binary data and strings in a thread-safe manner.
    This version is implemented as copy on copy buffers.  Note that
    the implementation of std::string in gcc prior to 3.1 is
    <b>not</b> thread-safe -- thus, Data is the only thread-safe
    option you may have.
    <p>
    <b>EXAMPLES</b>
    <p>
    <PRE>
      Data x;
      x = "asdf"; // set string to asdf
      x = "1024"; // set string to 1024
      x.convertInt() == 1024;  // this is true
      x.convertString() == string("1024"); // this is also true

      x += "a";  // now the string is 1024a

      x += "b";  // now it's 1024ab

      LocalScopeAllocator lo;
      printf("%s\n", x.getData(lo)); // this is how you get a char&#42; from a Data

      Data buf = "1000,1001,1002";

      bool matchFail;
      Data tok = buf.parse(",", &matchfail);

      // now, tok == "1000", buf == "1001,1002", matchfail == false

      tok = buf.parse(",", &matchfail);

      // at this point tok == "1001", buf == "1002", matchfail == false

      tok = buf.parse(",", &matchfail);

      // now, there is no , in buf (value: "1002" ) so the result is 
      // tok == "", buf == "1002", matchfaile == true

    </PRE>

 
 */
class Data : public BugCatcher
{
   public:
      /// "no position" -- used to indicate non-position when returning a value
      static const int npos;

      ///
      Data();
      ///
      Data( const char* str );
	/** construct data using an array of characters (can include NUL)
	    @param buffer   pointer to the buffer
	    @param length   number of characters to take from the buffer
	*/
      Data( const char* buffer, int length );
      ///
      Data( const Data& data );
      ///
      Data( const std::string& str);
      ///
      explicit Data(int value);
      ///
      explicit Data(unsigned long value);
      ///
      explicit Data(double value, int precision = 4);
      ///
      explicit Data(bool value);
      ///
      explicit Data(char c);
      
      ///
      virtual ~Data();

      ///
      bool operator>(const Data& ) const ;
      ///
      bool operator<(const Data& ) const;
      ///
      bool operator>(const char* ) const ;
      ///
      bool operator<(const char* ) const;
      ///
      Data& operator=(const char* str);
      ///
      Data& operator=(const Data& data);
      ///
      bool operator==( const char* str ) const;
      ///
      bool operator==( const Data& data ) const;
      ///
      bool operator!=( const char* str ) const;
      ///
      bool operator!=( const Data& data ) const;
      friend bool operator==( const char* str, const Data& d );
      friend bool operator!=( const char* str, const Data& d );

	/** 
	    Return a c-style string from a Data. 

	    <P>
	    The copy is made in the passed-in buffer.

	    @param buf   pointer to buffer to copy to 
	    @param len   length of buf

	    @return a c-style string
	*/

      const char* getData(char* buf, int len) const;

	/** 
	    Return a c-style string from a Data. 
	    <p>
	    the copy is kept in the LocalScopeAllocator.

	    @param lo   buffer containing the c-style string.
	    
	    @return a c-style string
	    @see LocalScopeAllocator#
	*/

      const char* getData(LocalScopeAllocator &lo ) const;
	/** 
	    @deprecated
	    Return a c-style string from a Data. 
	    <p>

	    this function returns a pointer to the internal buffer, so
	    it is not threadsafe.  That is, a simultaneous read and
	    write can cause undefined behavior in the reader.

	    @return a c-style string
	*/
      const char* logData() const; // return C style string
	/** 
	    @deprecated
	    Return a c-style string from a Data. 
	    <p>

	    this function returns a pointer to the internal buffer, so
	    it is not threadsafe.  That is, a simultaneous read and
	    write can cause undefined behavior in the reader.

	    @return a c-style string
	*/
      const char* c_str() const; // return C style string

      ///
      char getChar(int i) const;
      ///
      void setchar(int i, char c);
      ///
      void setChar(int i, char c) { setchar(i, c); }
        
      /// 
      char operator[](int i) const;
      ///Returns the length of the contained data
      int length() const;
      ///
      int size() const {return length();}
      ///
      bool empty() const {return size()==0;}

	/** Get the holding capacity of the object, which can be more
	 * then the actual length of the string. */

      int capacity() const { return mCapacity; }
      ///Pre-allocate buffer of the given size
      void setBufferSize(int size); // suggest a size

      ///
      int compare(const char* str, int length) const;
      ///
      int compare(const Data& data) const;
      ///
      int compareNoCase(const char* str, int length) const;
      ///
      int compareNoCase(const Data& data) const;

      ///
      Data operator+( const Data& data) const;
      ///
      Data operator+( const char* str) const;
      ///
      Data operator+( char c) const;
      ///
      Data& operator+=(const Data& d);
      ///
      Data& operator+=(const char*);
      ///
      Data& operator+=(const char c);

      ///
      void erase();
      ///
      Data& lowercase();
      ///
      Data& uppercase();

      ///
      std::string convertString() const;

	/** convert the Data to an int.  If there is leading
	 * whitespace before the number, it will be skipped.
	 * trailing whitespace and non-numeric numbers will stop
	 * conversion at that point.
	 */
      int convertInt() const;
      ///
      long convertLong() const;
      ///
      double convertDouble() const;

      /** 
	  @deprecated
	  match the string and return the text prior to the match.

	  If a match is found, and replace is true, the object is set
	  to the remainder after the matched string.
	  <p>
	  otherwise, the object is left alone.

	  @param matchItem         the string to be matched
	  @param data              data before the matched string
	  @param replace           whether to replace the matched data.
	  @param replaceWith       the data to replace the matched data
	  
	  @return NOT_FOUND if the string was not found, or FOUND if
	  it is found.
       */

      int match(const char* matchItem,
                Data* data,
                bool replace = false,
                Data replaceWith = "");


      /** 
          match (and eat) the first contiguous block composed of the
          characters in match, which is outside of double quotes and
          angle brackets. 

	  <p>
          This is designed for use in separating a list of
          parameters at the commas (e.g. Contact:)


	  @param matchFail if it is not NULL, the bool will be set to
	  true if the match fails, and false otherwise.


	  @return the data prior to the matched characters.  If no
	  characters match, return an empty Data.

      */
      Data parseOutsideQuotes(const char* match, 
                              bool useQuote,
                              bool useAngle,
                              bool* matchFail = 0 );

      /** 
          match (and eat) the first contiguous block of matched
          characters.

	  @param match   the characters to be matched.

	  @param matchFail a pointer to a bool.  set true if no
	  characters in match can be found, false if any characters in
	  match are found.  If matchFail is NULL, it is ignored.

	  @return the data prior to the first block of matching
	  characters.  if no match, returns "".

	  @see #matchChar
      */
      Data parse(const char* match, bool* matchFail = 0 );
      /** match (and eat) any single character in match.

	  @param match set the characters to be matched.  at most one
	  will match.

	  @param matchedChar If it is not NULL, it will be set to the
	  matching character, or \0 if not matched to anything.  if it
	  is NULL, it is ignored.

	  @return characters before the match, or the empty string
          if no characters match.
      */
      Data matchChar(const char* match, char* matchedChar = 0);
      /** get the next line in the text, delimited by \r\n or \n .

          Differs from parse("\r\n", matchFail) in that if there is
          a blank line (which has the contiguous text \r\n\r\n),
          parse will merely skip the empty line, while getLine will
          return the empty line as an empty Data.

	  @param matchFail if it is not NULL, it will be set to true
	  if the match fails, or false otherwise.

	  @return the next line in the text.

      */
      Data getLine(bool* matchFail = 0 );

      /// removes spaces before and after a string.
      Data& removeSpaces();
      /// remove leading white space.
      Data& removeLWS();

      /// expand expands headers (depreciated)
      void expand(const Data& startFrom,
                  const Data& findstr,
                  const Data& replstr,
                  const Data& delimiter);

      /** returns a substring of this object
          @param first      the first character to be part of the substring
          @param last       the last character of the substring, or -1 to 
          mean the last character overall
          thus, x.substring(0, -1) == x.
      */
      Data substring(int first, int last = -1) const;
      /** returns a substring of this object
          @param first      the first character to be part of the substring
          @param count      the number of characters to be part of the substring
      */
      Data substr(int first, int count = Data::npos) const 
      {
         return substring(first, (count == Data::npos 
                                  ? -1
                                  : first+count));
      }

      /// do a case-insensitive match
      friend bool isEqualNoCase( const Data& left,
                                 const Data& right ) ;

      /// do a case-insensitive match
      friend bool isEqualNoCase( const char* left,
                                 const Data& right ) ;

      /// find a string in the object
      int find(const Data& match, int start = 0) const;

      /// find a string in the object
      int find(const char* match, int start = 0) const;

      /**Searches beginning at pos, for the first character that is 
        *equal to any character within find.
        */
      int find_first_of(Data find, int start = 0) const;

      /**Searches beginning at pos, for the first character that is not 
        *equal to any character within find
        */
      int find_first_not_of(Data find, int start = 0) const;

      ///Same as find_first_of() except searches backword 
      int find_last_of(Data find, int start = 0) const;
      ///Same as find_first_not_of() except searches backword 
      int find_last_not_of(Data find, int start = 0) const;

      Data& replace(int startPos, int numElements, const Data& replacement);

      friend std::ostream& operator<<(std::ostream& s, const Data& data);

      static int getInstanceCount() { return atomic_read(&_cnt); }
      static int getTotalBuff() { return atomic_read(&_total_buf); }

   private:
      Data(int capacity, bool); // for internal use
      void resize(int s);
      
      Data& replace(int startpos, int endpos, const char* replaceStr, int replaceLength);

      int mLength;
      char* mBuf;
      int mCapacity;

      /** Used to detect memory usage issues */
      static atomic_t _cnt;
      static atomic_t _total_buf;
};

bool isEqualNoCase( const Data& left,
                    const Data& right );

bool isEqualNoCase( const char* left,
                    const Data& right ) ;

inline
Data
operator+(const char* c, const Data& d)
{
   return Data(c) + d;
}

#endif
