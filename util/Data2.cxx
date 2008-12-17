#include <algorithm>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#include "Data2.hxx"
#include "DataException.hxx"
#include "cpLog.h"
#if defined(__QNX__)
#define strcasecmp(a,b) stricmp(a,b)
#define strncasecmp(a,b,c) strnicmp(a,b,c)
#endif

using namespace std;
const int Data::npos = INT_MAX;

atomic_t Data::_cnt;
atomic_t Data::_total_buf;

Data::Data() 
   : mLength(0), 
     mBuf(0),
     mCapacity(63)
{
   atomic_inc(&_cnt);
   atomic_add(&_total_buf, mCapacity+1);

   mBuf = new char[mCapacity+1];
   mBuf[0] = 0;
}

Data::Data(int capacity, bool) 
   : mLength(0),
     mCapacity(capacity)
{
   atomic_inc(&_cnt);
   atomic_add(&_total_buf, mCapacity+1);
   mBuf = new char[capacity+1];
   mBuf[0] = 0;
}

Data::Data(const char* str, int length) 
   : mLength(length),
     mBuf(new char[mLength+1]),
     mCapacity(length)
{
   assert(str);
   atomic_inc(&_cnt);
   atomic_add(&_total_buf, mCapacity+1);
   memcpy(mBuf, str, length);
   mBuf[length] = 0;
}


Data::Data(const char* str) {
   assert(str);
   atomic_inc(&_cnt);
   mLength = strlen(str);
   mCapacity = mLength;
   atomic_add(&_total_buf, mCapacity+1);
   mBuf = new char[mLength+1];
   memcpy(mBuf, str, mLength+1);
}

Data::Data(const string& str) : 
   mLength(str.size()), 
   mBuf(new char[mLength+1]),
   mCapacity(mLength)
{
   atomic_inc(&_cnt);
   atomic_add(&_total_buf, mCapacity+1);
   memcpy(mBuf, str.c_str(), mLength+1);
}

Data::Data(int val)
   : mLength(0), 
     mBuf(0),
     mCapacity(0)
{
   atomic_inc(&_cnt);
   if (val == 0)
   {
      mBuf = new char[2];
      mBuf[0] = '0';
      mBuf[1] = 0;
      mLength = 1;
      mCapacity = 1;
      atomic_add(&_total_buf, mCapacity+1);
      return;
   }

   bool neg = false;
   
   int value = val;
   if (value < 0)
   {
      value = -value;
      neg = true;
   }

   int c = 0;
   int v = value;
   while (v /= 10)
   {
      c++;
   }

   if (neg)
   {
      c++;
   }

   mLength = c+1;
   mCapacity = c+1;
   mBuf = new char[c+2];
   mBuf[c+1] = 0;
   
   v = value;
   while (v)
   {
      mBuf[c--] = '0' + v%10;
      v /= 10;
   }

   if (neg)
   {
      mBuf[0] = '-';
   }

   atomic_add(&_total_buf, mCapacity+1);
}

Data::Data(double value, int precision)
   : mLength(0), 
     mBuf(0),
     mCapacity(0)

{
   assert(precision < 10);

   atomic_inc(&_cnt);

   double v = value;
   bool neg = (value < 0.0);
   
   if (neg)
   {
      v = -v;
   }

   Data m((unsigned long)v);

   // remainder
   v = v - floor(v);

   int p = precision;
   while (p--)
   {
      v *= 10;
   }

   int dec = (int)floor(v+0.5);

   Data d;

   if (dec == 0)
   {
      d = "0";
   }
   else
   {
      d.resize(precision);
      d.mBuf[precision] = 0;
      p = precision;
      // neglect trailing zeros
      bool significant = false;
      while (p--)
      {
         if (dec % 10 || significant)
         {
            significant = true;
            d.mLength++;
            d.mBuf[p] = '0' + (dec % 10);
         }
         else
         {
            d.mBuf[p] = 0;
         }
         
         dec /= 10;
      }
   }

   if (neg)
   {
      resize(m.length() + d.length() + 2);
      mBuf[0] = '-';
      memcpy(mBuf+1, m.mBuf, m.length());
      mBuf[1+m.length()] = '.';
      memcpy(mBuf+1+m.length()+1, d.mBuf, d.length()+1);
      mLength = m.length() + d.length() + 2;
   }
   else
   {
      resize(m.length() + d.length() + 1);
      memcpy(mBuf, m.mBuf, m.length());
      mBuf[m.length()] = '.';
      memcpy(mBuf+m.length()+1, d.mBuf, d.length()+1);
      mLength = m.length() + d.length() + 1;
   }
}

Data::Data(unsigned long value)
   : mLength(0), 
     mBuf(0),
     mCapacity(0)
{
   atomic_inc(&_cnt);
   if (value == 0)
   {
      mBuf = new char[2];
      mBuf[0] = '0';
      mBuf[1] = 0;
      mLength = 1;
      mCapacity=1;
      atomic_add(&_total_buf, mCapacity+1);
      return;
   }

   int c = 0;
   unsigned long v = value;
   while (v /= 10)
   {
      c++;
   }

   mLength = c+1;
   mCapacity = c+1;
   mBuf = new char[c+2];
   mBuf[c+1] = 0;
   
   v = value;
   while (v)
   {
      mBuf[c--] = '0' + v%10;
      v /= 10;
   }
   atomic_add(&_total_buf, mCapacity+1);
}

Data::Data(char c)
   : mLength(1), 
     mBuf(0),
     mCapacity(mLength)
{
   atomic_inc(&_cnt);
   atomic_add(&_total_buf, mCapacity+1);
   mBuf = new char[2];
   mBuf[0] = c;
   mBuf[1] = 0;
}

Data::Data(bool value)
   : mLength(0), 
     mBuf(0),
     mCapacity(0)
{
   static const char* truec = "true";
   static const char* falsec = "false";
   atomic_inc(&_cnt);

   if (value)
   {
      mBuf = new char[5];
      mLength = 4;
      mCapacity = 4;
      memcpy(mBuf, truec, 5);
   }
   else
   {
      mBuf = new char[6];
      mLength = 5;
      mCapacity = 5;
      memcpy(mBuf, falsec, 6);
   }
   atomic_add(&_total_buf, mCapacity+1);
}

Data::Data(const Data& rhs) 
   : mLength(rhs.mLength),
     mBuf(new char[mLength+1]), 
     mCapacity(mLength)
{
   atomic_inc(&_cnt);
   atomic_add(&_total_buf, mCapacity+1);
   memcpy(mBuf, rhs.mBuf, mLength+1);
}

Data::~Data()
{
   atomic_dec(&_cnt);
   atomic_sub(&_total_buf, mCapacity+1);
   delete[] mBuf;
}

Data&
Data::operator=(const char* str)
{
   assert(str);
   const int len = strlen(str);
   if (len > mCapacity)
   {
      resize(len);
   }
   mLength = len;
   memcpy(mBuf, str, mLength+1);

   return *this;
}

Data&
Data::operator=( const Data& data ) 
{
   if (&data != this)
   {
      if (data.length() > mCapacity)
      {
         resize(data.length());
      }
      mLength = data.length();
      memcpy(mBuf, data.mBuf, data.mLength+1);
   }
   
   return *this;
}

const char*
Data::logData() const
{
   return mBuf;
}

const char*
Data::c_str() const
{
   return mBuf;
}

const char*
Data::getData(char* buf, int len) const
{
   assert(len);
   strncpy(buf, mBuf, len-1);
   buf[len-1] = 0;
   return buf;
}

const char*
Data::getData(LocalScopeAllocator& lo) const
{
   lo.allocate(mLength+1);
   memcpy(lo, mBuf, mLength+1);
   return lo;
}

char
Data::getChar(int i) const
{
   assert(i<mLength);
   return mBuf[i];
}

void
Data::setchar(int i, char c)
{
   if (i >= mLength)
   {
      resize(i+1);
      mLength = i+1;
      memset(mBuf + mLength, 0, mCapacity-mLength+1);
   }
   
   assert(i<mLength);
   mBuf[i] = c;
}

char
Data::operator[](int i) const
{
   assert(i<mLength);
   return mBuf[i];
}

int
Data::length() const
{
   return mLength;
}

void
Data::setBufferSize(int newCapacity)
{
   resize(newCapacity);
}

void
Data::resize(int newCapacity)
{
   if (newCapacity < mCapacity)
   {
      return;
   }
   char* oldBuf = mBuf;
   mBuf = new char[newCapacity+1];
   memcpy(mBuf, oldBuf, mCapacity+1); //+1 means we get the trailing zero too.

   atomic_add(&_total_buf, newCapacity - mCapacity);

   mCapacity = newCapacity;
   delete[] oldBuf;
}

bool
Data::operator==(const char* str) const
{
   assert(str);
   return strcmp(mBuf, str) == 0;
}

bool
Data::operator==( const Data& data) const
{
   return strcmp(mBuf, data.mBuf) == 0;
}

bool
Data::operator!=(const char* str) const
{
   assert(str);
   return strcmp(mBuf, str) != 0;
}

bool
Data::operator!=(const Data& data) const
{
   return strcmp(mBuf, data.mBuf) != 0;
}

bool
operator==(const char* str, const Data& data)
{
   assert(str);
   return strcmp(str, data.mBuf) == 0;
}

bool
operator!=(const char* str, const Data& data)
{
   assert(str);
   return strcmp(str, data.mBuf) != 0;
}

bool
Data::operator>(const Data& data) const
{
   return strcmp(mBuf, data.mBuf) > 0;
}

bool
Data::operator<(const Data& data) const
{
   return strcmp(mBuf, data.mBuf) < 0;
}

bool
Data::operator>(const char* data) const
{
   return strcmp(mBuf, data) > 0;
}

bool
Data::operator<(const char* data) const
{
   return strcmp(mBuf, data) < 0;
}

int
Data::compare(const char* str, int length) const
{
   return strncmp(mBuf, str, length);
}

int
Data::compare(const Data& data) const
{
   return strncmp(mBuf, data.mBuf, data.length());
}

int
Data::compareNoCase(const char* str, int length) const
{
   return strncasecmp(mBuf, str, length);
}

int
Data::compareNoCase(const Data& data) const
{
   return strncasecmp(mBuf, data.mBuf, data.length());
}

Data
Data::operator+(const Data& data) const
{
   // reserve
   Data ret(mLength + data.mLength, true);
   memcpy(ret.mBuf, mBuf, mLength);
   memcpy(ret.mBuf+mLength, data.mBuf, data.mLength+1);
   ret.mLength = mLength + data.mLength;
   return ret;
}

Data
Data::operator+(const char* str) const
{
   assert(str);
   // reserve
   int l = strlen(str);
   Data ret(length() + l, true);
   memcpy(ret.mBuf, mBuf, mLength);
   memcpy(ret.mBuf+mLength, str, l+1);
   ret.mLength = mLength + l;
   return ret;
}


Data
Data::operator+(const char c) const
{
   // reserve
   Data ret(length() + 1, true);
   memcpy(ret.mBuf, mBuf, mLength);
   ret.mBuf[mLength] = c;
   ret.mBuf[mLength+1] = 0;
   ret.mLength = mLength + 1;
   return ret;
}


Data&
Data::operator+=(const Data& data)
{
   if (mCapacity < mLength + data.mLength)
   {
      resize(mLength + data.mLength);
   }

   memcpy(mBuf+mLength, data.mBuf, data.mLength+1);
   mLength += data.mLength;
   return *this;
}

Data&
Data::operator+=(const char* str)
{
   assert(str);
   int l = strlen(str);
   if (mCapacity < mLength + l)
   {
      resize(mLength + l);
   }

   memcpy(mBuf+mLength, str, l+1);
   mLength += l;
   return *this;
}


Data&
Data::operator+=(const char c)
{
   if (mCapacity < mLength + 1)
   {
      resize(mLength + 1);
   }

   mBuf[mLength] = c;
   mBuf[mLength+1] = 0;
   mLength += 1;
   return *this;
}


Data&
Data::lowercase()
{
   char* p = mBuf;
   while ((*p = tolower(*p)) != 0)
   {
      p++;
   }
   return *this;
}

Data&
Data::uppercase()
{
   char* p = mBuf;
   while ((*p = toupper(*p)) != 0)
   {
      p++;
   }
   return *this;
}

void
Data::erase()
{
   mLength = 0;
   mBuf[0] = 0;
}

int Data::convertInt() const
{
   int val = 0;
   char* p = mBuf;
   int l = mLength;
   int s = 1;

   while (isspace(*p++))
   {
      l--;
   }
   p--;
   
   if (*p == '-')
   {
      s = -1;
      p++;
      l--;
   }
   
   while (l--)
   {
      char c = *p++;
      if ((c >= '0') && (c <= '9'))
      {
         val *= 10;
         val += c - '0';
      }
      else
      {
         return s*val;
      }
   }

   return s*val;
}

long Data::convertLong() const
{
   long val = 0;
   char* p = mBuf;
   int l = mLength;
   int s = 1;

   while (isspace(*p++))
   {
      l--;
   }
   p--;

   if (*p == '-')
   {
      s = -1;
      p++;
      l--;
   }
   
   while (l--)
   {
      char c = *p++;
      if ((c >= '0') && (c <= '9'))
      {
         val *= 10;
         val += c - '0';
      }
      else
      {
         return s*val;
      }
   }

   return s*val;
}

string Data::convertString() const
{
   return string(mBuf);
}

double Data::convertDouble() const
{
   long val = 0;
   char* p = mBuf;
   int s = 1;

   while (isspace(*p++));
   p--;
   
   if (*p == '-')
   {
      s = -1;
      p++;
   }
   
   while (isdigit(*p))
   {
      val *= 10;
      val += *p - '0';
      p++;
   }

   if (*p == '.')
   {
      p++;
      long d = 0;
      double div = 1.0;
      while (isdigit(*p))
      {
         d *= 10;
         d += *p - '0';
         div *= 10;
         p++;
      }
      return s*(val + d/div);
   }

   return s*val;
}

int 
Data::match(const char* match,
            Data* retModifiedData,
            bool replace,
            Data replaceWith)
{
   assert(match);
   
   int retVal = FIRST;
   int matchLength = strlen(match);

   int replacePos = find(match);
   if (replacePos == Data::npos) {
      return NOT_FOUND;
   }

   if (retModifiedData != 0) {
      *retModifiedData = Data(mBuf, replacePos);
      if (retModifiedData->length() != 0) {
         retVal = FOUND;
      }
   }

   if (replace) {
      if (mLength - replacePos - matchLength + replaceWith.length() <= mCapacity) {
         memmove(mBuf + replaceWith.length(), 
                 mBuf + replacePos + matchLength,
                 mLength - replacePos - matchLength + 1);
         memmove(mBuf, replaceWith.mBuf, replaceWith.length());
         mLength = mLength - replacePos - matchLength + replaceWith.length();
      }
      else {
         // apparently we never need to grow on replace
         assert(0);
      }
   }
   return retVal;
}

bool isIn(char c, const char* match) {
   char p;
   while((p = *match++)) {
      if (p == c) {
         return true;
      }
   }
   return false;
}

Data 
Data::parse(const char* match, bool* matchFail ) {
   assert(match);
   
   int start = 0;
   bool foundAny = false;

   // find start
   while (!foundAny && (start < mLength)) {
      foundAny = isIn(mBuf[start++], match);
   }

   // find end
   if (foundAny) {
      int end = --start;
      while (end < mLength && isIn(mBuf[end], match)) {
         end++;
      }
      //      end--;

      Data result(mBuf, start);

      memmove(mBuf, mBuf + end, mLength - end + 1);
      mLength = mLength - end;
      mBuf[mLength] = '\0';

      assert(mLength >= 0);
      if (matchFail) {
         *matchFail = false;
      }
      return result;
   }
   else {
      if (matchFail) {
         *matchFail = true;
      }
      Data result;//(0, true);
      return result;
   }
}//parse


Data 
Data::parseOutsideQuotes(const char* match, 
                         bool useQuote, 
                         bool useAngle, 
                         bool* matchFail )
{
   int pos = 0;

   bool inDoubleQuotes = false;
   bool inAngleBrackets = false;

   bool foundAny = false;

   while (!foundAny && (pos < mLength)) {
      char p = mBuf[pos];

      switch (p) {
         case '"':
            if (!inAngleBrackets && useQuote) {
               inDoubleQuotes = !inDoubleQuotes;
            }
            break;
         case '<':
            if (!inDoubleQuotes && useAngle) {
               inAngleBrackets = true;
            }
            break;
         case '>':
            if (!inDoubleQuotes && useAngle) {
               inAngleBrackets = false;
            }
            break;
         default:
            break;
      }

      if (!inDoubleQuotes && !inAngleBrackets && isIn(p, match)) {
         foundAny = true;
      }
      pos++;
   }
    
   int pos2 = pos;

   while (foundAny && 
          (pos2 < mLength) &&
          isIn(mBuf[pos2], match))
   {
      pos2++;
   }

   if (foundAny) {
      Data result(mBuf, pos-1);

      memmove(mBuf, mBuf + pos2, mLength - pos2 + 1);
      mLength -= pos2;

      if (matchFail) {
         *matchFail = false;
      }
      return result;
   }
   else {
      if (matchFail) {
         *matchFail = true;
      }
      Data result;
      return result;
   }
}

Data 
Data::matchChar(const char* match, char* matchedChar ) {
   int pos = 0;

   bool foundAny = false;

   while (!foundAny && (pos < mLength)) {
      char p = mBuf[pos];
      if( isIn(p, match)) {
         foundAny = true;
         if (matchedChar) {
            *matchedChar = p;
         }
      }
      pos++;
   }

   if (foundAny) {
      Data result(mBuf, pos-1);
      memmove(mBuf, mBuf+pos, mLength-pos+1);
      mLength -= pos;
      return result;
   }
   else {
      Data result;
      if (matchedChar) {
         *matchedChar = '\0';
      }
      return result;
   }
}

Data 
Data::getLine(bool* matchFail )
{
   const int STARTING = 0;
   const int HAS_CR = 1;
   const int HAS_LF = 2;
   const int HAS_CRLF = 3;

   int state = STARTING;
   int pos = 0;

   bool foundAny = false;
   while (!foundAny && (pos < mLength))
   {
      char p = mBuf[pos];
      switch(p)
      {
         case '\r':
            state = HAS_CR;
            break;
         case '\n':
            if (state == HAS_CR)
            {
               state = HAS_CRLF;
            }
            else
            {
               state = HAS_LF;
            }
            foundAny = true;
            break;
         default:
            state = STARTING;
      }
      pos++;
   }

   int pos2 = pos;

   if (state == HAS_CRLF) {
      pos--;
   }

   if (foundAny) {
      Data result(mBuf, pos-1);
       
      memmove(mBuf, mBuf+pos2, mLength-pos2+1);
      mLength -= pos2;

      if (matchFail) {
         *matchFail = false;
      }
      return result;
   }
   else {
      if (matchFail) {
         *matchFail = true;
      }
      Data result;
      return result;
   }
}

Data 
Data::substring(int first, int last) const
{
   if (last == -1 ||
       last == Data::npos)
   {
      last = mLength;
   }
   assert(first <= last);
   assert(first >= 0);
   assert(last <= mLength);
   return Data(mBuf+first, last-first);
}

bool 
isEqualNoCase(const Data& left, const Data& right )
{
   if (left.mLength != right.mLength)
   {
      return false;
   }
   return strncasecmp(left.mBuf, right.mBuf, left.mLength) == 0;
}

bool
isEqualNoCase(const char* left, const Data& right )
{
   return strcasecmp(left, right.mBuf) == 0;
}

int
Data::find(const Data& match, int start) const
{
   assert(start >= 0);
   assert(start <= mLength);
   char* pos = std::search(mBuf+start, mBuf+mLength,
                           match.mBuf, match.mBuf + match.mLength);
   if (pos == mBuf+mLength)
   {
      if (match.mLength == 0)
      {
         if (mLength == 0)
         {
            return Data::npos;
         }
         return 0;
      }
      return Data::npos;
   }
   return pos - mBuf;
}

int
Data::find_first_of(Data find, int start) const
{
   string f = find.convertString();
   string me = this->convertString();
   string::size_type p = me.find_first_of(f, start);
   if (p == string::npos)
   {
      return Data::npos;
   }
   else
   {
      return p;
   }
}

int
Data::find_first_not_of(Data find, int start) const
{
   string f = find.convertString();
   string me = this->convertString();
   string::size_type p = me.find_first_not_of(f, start);
   if (p == string::npos)
   {
      return Data::npos;
   }
   else
   {
      return p;
   }
}

int
Data::find_last_of(Data find, int start) const
{
   string f = find.convertString();
   string me = this->convertString();
   string::size_type p = me.find_last_of(f, start);
   if (p == string::npos)
   {
      return Data::npos;
   }
   else
   {
      return p;
   }
}

int
Data::find_last_not_of(Data find, int start) const
{
   string f = find.convertString();
   string me = this->convertString();
   string::size_type p = me.find_last_not_of(f, start);
   if (p == string::npos)
   {
      return Data::npos;
   }
   else
   {
      return p;
   }
}

int
Data::find(const char* match, int start) const
{
   assert(start >= 0);
   assert(start <= mLength);
   const int matchLength = strlen(match);

   char* m = const_cast<char*>(match);
   char* b = const_cast<char*>(mBuf);
   char* pos = std::search(b+start, b+mLength,
                           m, m+matchLength);
   assert(pos <= b+mLength);
   assert(pos >= mBuf);
   if (pos == b+mLength)
   {
      if (matchLength == 0)
      {
         if (mLength == 0)
         {
            return Data::npos;
         }
         return 0;
      }
      return Data::npos;
   }
   return pos - mBuf;
}

Data&
Data::removeSpaces() {
   int firstChar = 0;
   
   while ((firstChar < mLength) && mBuf[firstChar] == ' ') {
      firstChar++;
   }
   
   int lastChar = mLength-1;
   while ((lastChar > 0) && (mBuf[lastChar] == ' ')) {
      lastChar--;
   }
   
   if (firstChar > lastChar) {
      erase();
   }          
   else {
      memmove(mBuf, mBuf+firstChar, lastChar-firstChar+1);
      mLength = lastChar-firstChar+1;
      mBuf[mLength] = 0;
   }
   return *this;
}
    
// the functionality of this method has changed from the implementation in
// CopyOnWrite. It now looks for folded lines which mean a CRLF followed by
// whitespace and replaces the CRLF with two spaces (' '). This should allow the
// parser to continue since lines will no longer be folded 
Data&
Data::removeLWS() {
   char* p = mBuf;
   char* end = mBuf + mLength - 2;
   while (p != 0 && p < end) {
      // look for CRLF + ' ' or '\t' since this indicates line folding
      // will not go past end, since && will short circuit when == 0
      if (*p == '\r' && *(p+1) == '\n' && (*(p+2) == ' ' || (*(p+2) == '\t'))) {
         *p++ = ' '; // replace \r with ' '
         *p++ = ' ';// replace \n with ' '
      }
      else {
         p++;
      }
   }

   return *this;
}

void
Data::expand(const Data& startFrom, 
             const Data& findstr, 
             const Data& replstr, 
             const Data& delimiter)
{
   assert(0);
   int startPos = find(startFrom);
   if (startPos < Data::npos)
   {
      int delimPos = find(delimiter, startPos);
      int findPos = find(findstr, startPos);

      while (findPos < delimPos)
      {
         //found replstr, replace
         replace(findPos, findstr.mLength, replstr.mBuf, replstr.mLength);
         //find next.
         delimPos = find(delimiter, findPos + replstr.mLength);
         findPos = find(findstr, findPos);
      }
   }
}

Data& 
Data::replace(int startPos, int numElements, const Data& replacement)
{
   assert((startPos + numElements) <= mLength);
   return replace(startPos, startPos + numElements, replacement.c_str(), replacement.size());
}

Data&
Data::replace(int startPos, int endPos, const char* replaceStr, int replaceLength)
{
   assert(replaceStr);
   assert(startPos < endPos);
   assert(endPos <= mLength);

   resize(startPos + replaceLength + (mLength - endPos));

   memmove(mBuf + startPos + replaceLength, 
           mBuf + endPos, mLength-endPos + 1);
   
   memmove(mBuf + startPos, replaceStr, replaceLength);
   mLength = startPos + replaceLength + (mLength - endPos);
   return *this;
}

ostream&
operator<<(ostream& s, const Data& data)
{
   s << data.mBuf;
   return s;
}

