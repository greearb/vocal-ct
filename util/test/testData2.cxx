#include <set>
#include "Data2.hxx"
#include "CopyOnWriteData.hxx"
#include "DataException.hxx"

int
main()
{
   {
      {
         Data c('c');
         assert(c == "c");
         assert(c.length() == 1);
      }
      
      {
         Data z(0);
         assert(z == "0");
         assert(z.length() == 1);
         
         Data i(123);
         assert(i == "123");
         assert(i.length() == 3);

         unsigned long vul = 12345678;
         Data ul(vul);
         cerr << "!!! " << ul << endl;
         assert(ul == "12345678");
         assert(ul.length() == 8);

         unsigned long vulz = 0;
         Data ulz(vulz);
         assert(ulz == "0");
         assert(ulz.length() == 1);

         Data dz(0.0);
         cerr << "!! " << dz << endl;
         assert(dz == "0.0");         
         assert(dz.length() == 3);

         Data dz3(0.0, 3);
         cerr << "!! " << dz << endl;
         assert(dz3 == "0.0");
         assert(dz3.length() == 3);

         Data d0(123.0);
         assert(d0 == "123.0");
         assert(d0.length() == 5);

         Data d(123.12345678);
         assert(d == "123.1235");
         assert(d.length() == 8);

         Data dp(123.12345678, 6);
         cerr << "!! " << dp << endl;
         assert(dp == "123.123457");
         assert(dp.length() == 10);

         Data in(-123);
         assert(in == "-123");
         assert(in.length() == 4);

         Data dn(-123.12345678);
         cerr << "!! " << dn << endl;
         assert(dn == "-123.1235");
         cerr << "!! " << dn.length() << endl;
         assert(dn.length() == 9);

         Data d0n(-123);
         assert(d0n == "-123");
         assert(d0n.length() == 4);

         Data ndp(-123.12345678, 6);
         assert(ndp == "-123.123457");
         assert(ndp.length() == 11);

         Data dot02(0.02);
         cerr << dot02 << endl;
         assert(dot02 == "0.02");
         assert(dot02.length() == 4);

         Data b1(true);
         assert(b1 == "true");
         assert(b1.length() == 4);
         
         Data b0(false);
         assert(b0 == "false");
         assert(b0.length() == 5);
      }

      {
         Data d1("123\r\n456\r\n");
         assert(d1.removeLWS() == "123\r\n456\r\n");

         Data d2("123\r\n   456\r\n");
         assert(d2.removeLWS() == "123     456\r\n");

         Data d3("123\n   456\r\n");
         assert(d3.removeLWS() == "123\n   456\r\n");

         Data d4("   123\r\n   456\r\n");
         assert(d4.removeLWS() == "   123     456\r\n");

         Data d5("   123\r\n   456\r\n");
         assert(d5.removeLWS() == "   123     456\r\n");

         Data d6("   1\t23\t\r\n  \t 456\r\n");
         assert(d6.removeLWS() == "   1\t23\t    \t 456\r\n");

         Data d7("A\r\n\r\nB\r\n");
         assert(d7.removeLWS() == "A\r\n\r\nB\r\n");
         
         Data d8("1\t23\t\r\n  \t 456\r\n\t\t     \r\n    \r\nfoo\r\n");
         assert(d8.removeLWS() == "1\t23\t    \t 456  \t\t           \r\nfoo\r\n");
      }
      
      {
         Data d("abcdef");
         assert(d.getChar(0) == 'a');
         assert(d.getChar(1) == 'b');
         assert(d.getChar(2) == 'c');
         assert(d.getChar(3) == 'd');
         assert(d.getChar(4) == 'e');
         assert(d.getChar(5) == 'f');

         d.setchar(0,'0');
         assert(d.getChar(0) == '0');
         d.setchar(5,'1');
         assert(d.getChar(5) == '1');            
         d.setchar(6,'2');
         assert(d.getChar(6) == '2');

         Data e;
         e.setchar(0,'a');
         assert(e == "a");
         e.setchar(5,'b');
         assert(e.getChar(5) == 'b');
         assert(e == "a");         
      }
      
      {
         assert(Data(0) == "0");
         assert(Data(1) == "1");
         assert(Data(-1) == "-1");
         assert(Data(11) == "11");      
         assert(Data(1234567) == "1234567");
         assert(Data(-1234567) == "-1234567");
      }

      {
         Data mixed("MiXed");
         assert(mixed.lowercase() == "mixed");
         cerr << "uppercase[" << mixed.uppercase() << "]" << endl;
         assert(mixed.uppercase() == "MIXED");
      
         Data e;
         assert(e.uppercase() == "");
      
         Data m("1234!@#$!@aA#$2354");
         assert(m.lowercase() == "1234!@#$!@aa#$2354");
         assert(m.uppercase() == "1234!@#$!@AA#$2354");
      }
   
      {
         Data e("");
         cerr << e.find("") << endl;
         assert(e.find("") == Data::npos);
         assert(e.find("a") == Data::npos);

         Data d;
         assert(d.find("") == Data::npos);
         assert(d.find("a") == Data::npos);
      
         Data f("asdfasdfrafs");
         assert(f.find("") == 0);
         assert(f.find("a") == 0);
         assert(f.find("asdfasdfrafs") == 0);
         assert(f.find("fs") == 10);
         assert(f.find("fr") == 7);
         assert(f.find("...") == Data::npos);
         assert(f.find("waytoolargetofind") == Data::npos);

         assert(f.find(Data("")) == 0);
         assert(f.find(Data("a")) == 0);
         assert(f.find(Data("asdfasdfrafs")) == 0);
         assert(f.find(Data("fs")) == 10);
         assert(f.find(Data("fr")) == 7);
      }
      
      {
         Data empt;
         Data empt1;
         assert(empt.length() == 0);
         assert(empt == empt);
         assert(empt == empt1);
         assert(empt1 == empt);
         assert(empt1 == "");

         assert(!(empt != empt));
         assert(!(empt != empt1));
         assert(!(empt1 != empt));
         assert(!(empt1 != ""));
      
         assert("" == empt1);
         assert("sdf" != empt1);
         assert(Data("SAfdsaf") != empt1);
         empt = empt;
         empt = empt1;
         empt = "sdfasf";
      }
      {   
         Data a("a");
         Data b("b");
         assert(a != b);
         assert(a < b);
         a = b;
         assert(a==b);
         Data c(a);
         assert(c==a);
         assert(c==b);
         assert(a==b);
      }
   
      {
         char* tmp = "sadfasdf";
         Data c(tmp, strlen(tmp));
         Data d(c);
         Data e(string("sdgdsg"));
         assert(c < e);
         assert(e > c);
         assert(c < "zzz");
         assert(c > "aaa");
         assert(!(c > c));
         assert(!(c < c));
      }
      {
         char* tmp = "sadfasdf";
         Data c(tmp, 4);
         assert(c == "sadf");
      }
      {
         Data d("qwerty");
         char buf[10];
         assert(strcmp(d.getData(buf, 10), "qwerty") == 0);
         assert(strcmp(d.getData(buf, 2), "q") == 0);
         assert(strcmp(d.getData(buf, 1), "") == 0);

         Data e;
         assert(strcmp(e.getData(buf, 10), "") == 0);
         assert(strcmp(e.getData(buf, 2), "") == 0);
         assert(strcmp(e.getData(buf, 1), "") == 0);
      }

      {
         Data d("qwerty");
         LocalScopeAllocator lo;
         assert(strcmp(d.getData(lo), "qwerty") == 0);
      }
      {
         Data e;
         LocalScopeAllocator lo;
         assert(strcmp(e.getData(lo), "") == 0);
      }   

      {
         Data d("qwerty");
         assert(strcmp(d.logData(), "qwerty") == 0);
      }
      {
         Data e;
         assert(strcmp(e.logData(), "") == 0);
      }   

      {
         Data c = "sadfsdf";
         Data d;
         d = c;
         assert(c == d);
      }
      {
         string f("asdasd");
         Data d = f;
         assert(d == f);
      }
      {
         string f("asdasd");
         Data d = f;
         assert(d.logData() == f);
      }
      {
         Data d("123");
         assert(d.length() == 3);
      }
      {
         Data d("123");
         assert(d[0] == '1');
         assert(d[1] == '2');
         assert(d[2] == '3');
      }
      {
         Data d;
         d.setBufferSize(7);
         d = "123456";
         assert(d.length() == 6);
      }
      {
         Data d("theSame1");
         string s("theSame2");
         assert(d.compare(s.c_str(), 7) == 0);
      }
      {
         Data d("thesame1");
         string s("theSame2");
         assert(d.compareNoCase(s.c_str(), s.size()-1) == 0);
      }
      {
         Data d("theSame1");
         Data e("thesame1");
         d.compareNoCase(e);
      }
      {
         Data d("one");
         Data c("two");
         d += c;
         assert(d == "onetwo");

         Data empt;
         assert(empt + d == d);
         assert(empt + d == "onetwo");
         assert(empt + "three" == "three");
      }
      
      {
         Data d("one");
         d += 'c';
         assert(d == "onec");
         assert(d.length() == 4);
      }

      {
         Data d("one");
         Data e = d + 'c';
         assert(e == "onec");
         assert(e.length() == 4);
      }
      
      {
         Data s;
         s = "c=";
         assert(s == "c=");
         s += "foo";
         assert(s == "c=foo");
         s += "\r\n";
         s += "bar";
         s += "\r\n";
         assert (s == "c=foo\r\nbar\r\n");
      }
      {
         Data a("one");
         Data b("two");
         Data c("three");
      
         assert(a+b+c == "onetwothree");
      }
   
      {
         Data d("one");
         cerr << "one + two = " << (d + "two") << endl;
         assert((d + "two") == "onetwo");
      }
      {
         Data d("asdasdasd");
         d.erase();
         assert(d.length() == 0);
      }
      {
         Data d("asdasdasd");
         string s = d.convertString();
         assert(s == "asdasdasd");
      }
      {
         Data ten("10");
         assert(ten.convertInt() == 10);

         Data neg11(-11);
         assert(-11 == neg11.convertInt());

         Data zero("0");
         assert(0 == zero.convertInt());

         Data fr("fribble");
         assert(0 == fr.convertInt());

         Data e;
         //assert(0 == e.convertInt());
      }
      {
         Data zero("0");
         assert(zero.convertDouble() == 0.0);
         
         Data ten("10");
         assert(ten.convertDouble() == 10.0);

         Data zeroDot("0.0");
         assert(zeroDot.convertDouble() == 0.0);
         
         Data tenDot("10.0");
         assert(ten.convertDouble() == 10.0);

         Data eleven("11.11");
         assert(eleven.convertDouble() == 11.11);

         Data dotEleven("0.11");
         assert(dotEleven.convertDouble() == 0.11);

         Data dEleven("0.11");
         assert(dEleven.convertDouble() == 0.11);
      }

      {
         cerr << "!MatchNoReplace!" << endl;
         Data d("beforeZZafter");
         Data res;
         int k = d.match("ZZ", &res);
         cerr << "Retval: " << k << " res: " << res << " orig: " << d << endl;
         assert(res == "before");
         assert(d == "beforeZZafter");
         assert(k == FOUND);
      }
      {
         cerr << "!MatchReplace!" << endl;
         Data d("beforeZZafter");
         Data res;
         int k = d.match("ZZ", &res, true, "YYY");
         cerr << "Retval: " << k << " res: " << res << " orig: " << d << endl;
         assert(res == "before");
         assert(d == "YYYafter");
         assert(k == FOUND);
      }
      {
         cerr << "!MatchNotFound!" << endl;
         Data d("beforeZZafter");
         Data res;
         int k = d.match("qq", &res);
         cerr << "Retval: " << k << " res: " << " orig: " << d << endl;
         assert(res == "");
         assert(d == "beforeZZafter");
         assert(k == NOT_FOUND);
      }
      {
         cerr << "!MatchNotFound!" << endl;
         Data d("beforeZZafter");
         Data res;
         int k = d.match("qq", &res);
         cerr << "Retval: " << k << " res: " << " orig: " << d << endl;
         assert(res == "");
         assert(d == "beforeZZafter");
         assert(k == NOT_FOUND);
      }
      {
         cerr << "!MatchFirst!" << endl;
         Data d("ZZbeforeafter");
         Data res;
         int k = d.match("ZZ", &res);
         cerr << "Retval: " << k << " res: " << " orig: " << d << endl;
         assert(res == "");
         assert(d == "ZZbeforeafter");
         assert(k == FIRST);
      }
      {
         cerr << "!MatchFirstReplace!" << endl;
         Data d("ZZZZbeforeafter");
         Data res;
         int k = d.match("ZZZZ", &res, true, "QQQ");
         cerr << "Retval: " << k << " res: " << res << " orig: " << d << endl;
         assert(res == "");
         assert(d == "QQQbeforeafter");
         assert(k == FIRST);
      }

      {
         cerr << "parse test" << endl;
         Data d("STUFFijijijMORE STUFFij");
         bool matchFail;
         Data result = d.parse("ij", &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parse test fail" << endl;
         Data d("STUFFiiiiiMORE STUFFi");
         bool matchFail;
         Data result = d.parse("kj", &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "STUFFiiiiiMORE STUFFi");
      }

      {
         cerr << "parseOutsideQuotes" << endl;
         Data d("STUFFijijijMORE STUFFij");
         bool matchFail;
         Data result = d.parseOutsideQuotes("ij", true, false, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail" << endl;
         Data d("STUFFiiiiiMORE STUFFi");
         bool matchFail;
         Data result = d.parseOutsideQuotes("kj", true, false, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "STUFFiiiiiMORE STUFFi");
      }

      {
         cerr << "parseOutsideQuotes, quotes" << endl;
         Data d("\"iiiiiiNOPEjjjjjj\"STUFFijijijMORE STUFFij");
         bool matchFail;
         Data result = d.parseOutsideQuotes("ij", true, false, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "\"iiiiiiNOPEjjjjjj\"STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, quotes" << endl;
         Data d("\"iiiiiiNOPEjjjjjj\"STUFFiiiiiMORE STUFFii");
         bool matchFail;
         Data result = d.parseOutsideQuotes("kj", true, false, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "\"iiiiiiNOPEjjjjjj\"STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "parseOutsideQuotes, <>" << endl;
         Data d("<iiiiiiNOPEjjjjjj>STUFFijijijMORE STUFFij");
         bool matchFail;
         Data result = d.parseOutsideQuotes("ij", false, true, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "<iiiiiiNOPEjjjjjj>STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, <>" << endl;
         Data d("<iiiiiiNOPEjjjjjj>STUFFiiiiiMORE STUFFii");
         bool matchFail;
         Data result = d.parseOutsideQuotes("kj", false, true, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "<iiiiiiNOPEjjjjjj>STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "parseOutsideQuotes, <\"\">" << endl;
         Data d("<\"iiiiiiNOPEjjjjjj\">STUFFijijijMORE STUFFij");
         bool matchFail;
         Data result = d.parseOutsideQuotes("ij", false, true, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "<\"iiiiiiNOPEjjjjjj\">STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, <\"\">" << endl;
         Data d("<\"iiiiiiNOPEjjjjjj\">STUFFiiiiiMORE STUFFii");
         bool matchFail;
         Data result = d.parseOutsideQuotes("kj", false, true, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "<\"iiiiiiNOPEjjjjjj\">STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "matchChar" << endl;
         Data d("SKIPiiiiiMORE STUFFi");
         bool matchFail;
         Data result = d.parse("ij", &matchFail);
         assert(!matchFail);
         assert(result == "SKIP");
         assert(d == "MORE STUFFi");
      }

      {
         cerr << "parseOutsideQuotes, \"<>\"" << endl;
         Data d("\"<iiiiiiNOPEjjjjjj>\"STUFFijijijMORE STUFFij");
         bool matchFail;
         Data result = d.parseOutsideQuotes("ij", true, false, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "\"<iiiiiiNOPEjjjjjj>\"STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, \"<>\"" << endl;
         Data d("\"<iiiiiiNOPEjjjjjj>\"STUFFiiiiiMORE STUFFii");
         bool matchFail;
         Data result = d.parseOutsideQuotes("kj", true, false, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "\"<iiiiiiNOPEjjjjjj>\"STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "matchChar" << endl;
         Data d("SKIPiiiiiMORE STUFFi");
         bool matchFail;
         Data result = d.parse("ij", &matchFail);
         assert(!matchFail);
         assert(result == "SKIP");
         assert(d == "MORE STUFFi");
      }

      {
         cerr << "matchChar" << endl;
         Data d("SKIPiiiiiMORE STUFFi");
         bool matchFail;
         Data result = d.parse("kj", &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "SKIPiiiiiMORE STUFFi");
      }

      {
         cerr << "getLine" << endl;
         bool matchFail;
         Data d("line 1\r\nline 2\r\nline 3\r\n");
      
         Data line1 = d.getLine(&matchFail);
         assert(!matchFail);
         assert(line1 == "line 1");

         Data line2 = d.getLine(&matchFail);
         assert(!matchFail);
         assert(line2 == "line 2");

         Data line3 = d.getLine(&matchFail);
         assert(!matchFail);
         assert(line3 == "line 3");

         Data line4 = d.getLine(&matchFail);
         assert(matchFail);
         assert(line4 == "");
      }

      {
         cerr << "removeSpaces" << endl;
         Data d("         SPACES        ");
         assert(d.removeSpaces() == "SPACES");
         assert(d.removeSpaces() == "SPACES");

         Data b("      BEFORE");
         assert(b.removeSpaces() == "BEFORE");


         Data a("AFTER     ");
         assert(a.removeSpaces() == "AFTER");

         Data e("             ");
         assert(e.removeSpaces() == "");
      }

      {
         cerr << "substring" << endl;

         Data d("123456789");
         assert(d.substring(3, 5) == "45");
         assert(d.substring(3, -1) == "456789");
         assert(d.substring(3, 3) == "");
         assert(d.substring(0, 3) == "123");
         assert(d.substring(0, 0) == "");
         assert(d.substring(0, -1) == d);
      }

      {
         cerr << "isEqualNoCase" << endl;
         assert(isEqualNoCase(Data("MIxed"), Data("mixed")));
         assert(isEqualNoCase(Data(""), Data("")));
         assert(!isEqualNoCase(Data(""), Data("a")));
         assert(!isEqualNoCase(Data("b"), Data("a")));

         assert(isEqualNoCase("MIxed", Data("mixed")));
         assert(isEqualNoCase("", Data("")));
         assert(!isEqualNoCase("", Data("a")));
         assert(!isEqualNoCase("b", Data("a")));
      }
   }

   {
      {
         assert(CopyOnWriteData(0) == "0");
         assert(CopyOnWriteData(1) == "1");
         assert(CopyOnWriteData(-1) == "-1");
         assert(CopyOnWriteData(11) == "11");      
         assert(CopyOnWriteData(1234567) == "1234567");
         assert(CopyOnWriteData(-1234567) == "-1234567");
      }

      {
         CopyOnWriteData mixed("MiXed");
         mixed.lowercase();
         assert(mixed == "mixed");
         mixed.uppercase();
         assert(mixed == "MIXED");
      
         CopyOnWriteData e;
         e.uppercase();
         assert(e == "");
      
         CopyOnWriteData m("1234!@#$!@aA#$2354");
         m.lowercase();
         assert(m == "1234!@#$!@aa#$2354");
         m.uppercase();
         assert(m == "1234!@#$!@AA#$2354");
      }
   
      {
         CopyOnWriteData e("");
         cerr << e.find("") << endl;
         assert(e.find("") == 0);
         assert(e.find("a") == CopyOnWriteData::npos);

         CopyOnWriteData d;
         assert(d.find("") == 0);
         assert(d.find("a") == CopyOnWriteData::npos);
      
         CopyOnWriteData f("asdfasdfrafs");
         cerr << f.find("") << endl;
         assert(f.find("") == CopyOnWriteData::npos);
         assert(f.find("a") == 0);
         assert(f.find("asdfasdfrafs") == 0);
         assert(f.find("fs") == 10);
         assert(f.find("fr") == 7);
         assert(f.find("...") == CopyOnWriteData::npos);
         assert(f.find("waytoolargetofind") == CopyOnWriteData::npos);

         assert(f.find(CopyOnWriteData("")) == CopyOnWriteData::npos);
         assert(f.find(CopyOnWriteData("a")) == 0);
         assert(f.find(CopyOnWriteData("asdfasdfrafs")) == 0);
         assert(f.find(CopyOnWriteData("fs")) == 10);
         assert(f.find(CopyOnWriteData("fr")) == 7);
      }
      
      {
         CopyOnWriteData empt;
         CopyOnWriteData empt1;
         assert(empt.length() == 0);
         assert(empt == empt);
         assert(empt == empt1);
         assert(empt1 == empt);
         assert(empt1 == "");

         assert(!(empt != empt));
         assert(!(empt != empt1));
         assert(!(empt1 != empt));
         assert(!(empt1 != ""));
      
         assert("" == empt1);
         assert("sdf" != empt1);
         assert(CopyOnWriteData("SAfdsaf") != empt1);
         empt = empt;
         empt = empt1;
         empt = "sdfasf";
      }
      {   
         CopyOnWriteData a("a");
         CopyOnWriteData b("b");
         assert(a != b);
         assert(a < b);
         a = b;
         assert(a==b);
         CopyOnWriteData c(a);
         assert(c==a);
         assert(c==b);
         assert(a==b);
      }
   
      {
         char* tmp = "sadfasdf";
         CopyOnWriteData c(tmp, strlen(tmp));
         CopyOnWriteData d(c);
         CopyOnWriteData e(string("sdgdsg"));
         assert(c < e);
         assert(e > c);
         assert(c < "zzz");
         assert(c > "aaa");
         assert(!(c > c));
         assert(!(c < c));
      }
      {
         char* tmp = "sadfasdf";
         CopyOnWriteData c(tmp, 4);
         assert(c == "sadf");
      }
      {
         CopyOnWriteData d("qwerty");
         char buf[10];
         assert(strcmp(d.getData(buf, 10), "qwerty") == 0);
         assert(strcmp(d.getData(buf, 2), "q") == 0);
         assert(strcmp(d.getData(buf, 1), "") == 0);

         CopyOnWriteData e;
         assert(strcmp(e.getData(buf, 10), "") == 0);
         assert(strcmp(e.getData(buf, 2), "") == 0);
         assert(strcmp(e.getData(buf, 1), "") == 0);
      }

      {
         CopyOnWriteData d("qwerty");
         LocalScopeAllocator lo;
         assert(strcmp(d.getData(lo), "qwerty") == 0);
      }
      {
         CopyOnWriteData e;
         LocalScopeAllocator lo;
         assert(strcmp(e.getData(lo), "") == 0);
      }   

      {
         CopyOnWriteData d("qwerty");
         assert(strcmp(d.logData(), "qwerty") == 0);
      }
      {
         CopyOnWriteData e;
         assert(strcmp(e.logData(), "") == 0);
      }   

      {
         CopyOnWriteData c = "sadfsdf";
         CopyOnWriteData d;
         d = c;
         assert(c == d);
      }
      {
         string f("asdasd");
         CopyOnWriteData d = f;
         assert(d == f);
      }
      {
         string f("asdasd");
         CopyOnWriteData d = f;
         assert(d.logData() == f);
      }
      {
         CopyOnWriteData d("123");
         assert(d.length() == 3);
      }
      {
         CopyOnWriteData d("123");
         assert(d[0] == '1');
         assert(d[1] == '2');
         assert(d[2] == '3');
      }
      {
         CopyOnWriteData d;
         d.setBufferSize(7);
         d = "123456";
         assert(d.length() == 6);
      }
      {
         CopyOnWriteData d("theSame1");
         //assert(d.compare("theSame2", 7) == 0);
      }
      {
         CopyOnWriteData d("thesame1");
         string s("theSame2");
         //assert(d.compareNoCase(s.c_str(), s.size()-1) == 0);
      }
      {
         CopyOnWriteData d("theSame1");
         CopyOnWriteData e("thesame1");
         d.compareNoCase(e);
      }
      {
         CopyOnWriteData d("one");
         CopyOnWriteData c("two");
         d += c;
         assert(d == "onetwo");

         CopyOnWriteData empt;
         assert(empt + d == d);
         assert(empt + d == "onetwo");
         assert(empt + "three" == "three");
      }
      {
         CopyOnWriteData a("one");
         CopyOnWriteData b("two");
         CopyOnWriteData c("three");
      
         assert(a+b+c == "onetwothree");
      }
   
      {
         CopyOnWriteData d("one");
         cerr << "one + two = " << (d + "two") << endl;
         assert((d + "two") == "onetwo");
      }
      {
         CopyOnWriteData d("asdasdasd");
         d.erase();
         assert(d.length() == 0);
      }
      {
         CopyOnWriteData d("asdasdasd");
         string s = d.convertString();
         assert(s == "asdasdasd");
      }
      {
         CopyOnWriteData ten("10");
         assert(ten.convertInt() == 10);

         CopyOnWriteData neg11(-11);
         //assert(-11 == neg11.convertInt());

         CopyOnWriteData zero("0");
         assert(0 == zero.convertInt());

         CopyOnWriteData fr("fribble");
         assert(0 == fr.convertInt());

         CopyOnWriteData e;
         //assert(0 == e.convertInt());
      }

      {
         cerr << "!MatchNoReplace!" << endl;
         CopyOnWriteData d("beforeZZafter");
         CopyOnWriteData res;
         int k = d.match("ZZ", &res);
         cerr << "Retval: " << k << " res: " << res << " orig: " << d << endl;
         assert(res == "before");
         assert(d == "beforeZZafter");
         assert(k == FOUND);
      }
      {
         cerr << "!MatchReplace!" << endl;
         CopyOnWriteData d("beforeZZafter");
         CopyOnWriteData res;
         int k = d.match("ZZ", &res, true, "YYY");
         cerr << "Retval: " << k << " res: " << res << " orig: " << d << endl;
         assert(res == "before");
         assert(d == "YYYafter");
         assert(k == FOUND);
      }
      {
         cerr << "!MatchNotFound!" << endl;
         CopyOnWriteData d("beforeZZafter");
         CopyOnWriteData res;
         int k = d.match("qq", &res);
         cerr << "Retval: " << k << " res: " << " orig: " << d << endl;
         assert(res == "");
         assert(d == "beforeZZafter");
         assert(k == NOT_FOUND);
      }
      {
         cerr << "!MatchNotFound!" << endl;
         CopyOnWriteData d("beforeZZafter");
         CopyOnWriteData res;
         int k = d.match("qq", &res);
         cerr << "Retval: " << k << " res: " << " orig: " << d << endl;
         assert(res == "");
         assert(d == "beforeZZafter");
         assert(k == NOT_FOUND);
      }
      {
         cerr << "!MatchFirst!" << endl;
         CopyOnWriteData d("ZZbeforeafter");
         CopyOnWriteData res;
         int k = d.match("ZZ", &res);
         cerr << "Retval: " << k << " res: " << " orig: " << d << endl;
         assert(res == "");
         assert(d == "ZZbeforeafter");
         assert(k == FIRST);
      }
      {
         cerr << "!MatchFirstReplace!" << endl;
         CopyOnWriteData d("ZZZZbeforeafter");
         CopyOnWriteData res;
         int k = d.match("ZZZZ", &res, true, "QQQ");
         cerr << "Retval: " << k << " res: " << res << " orig: " << d << endl;
         assert(res == "");
         assert(d == "QQQbeforeafter");
         assert(k == FIRST);
      }

      {
         cerr << "parse test" << endl;
         CopyOnWriteData d("STUFFijijijMORE STUFFij");
         bool matchFail;
         CopyOnWriteData result = d.parse("ij", &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parse test fail" << endl;
         CopyOnWriteData d("STUFFiiiiiMORE STUFFi");
         bool matchFail;
         CopyOnWriteData result = d.parse("kj", &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "STUFFiiiiiMORE STUFFi");
      }

      {
         cerr << "parseOutsideQuotes" << endl;
         CopyOnWriteData d("STUFFijijijMORE STUFFij");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("ij", true, false, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail" << endl;
         CopyOnWriteData d("STUFFiiiiiMORE STUFFi");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("kj", true, false, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "STUFFiiiiiMORE STUFFi");
      }

      {
         cerr << "parseOutsideQuotes, quotes" << endl;
         CopyOnWriteData d("\"iiiiiiNOPEjjjjjj\"STUFFijijijMORE STUFFij");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("ij", true, false, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "\"iiiiiiNOPEjjjjjj\"STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, quotes" << endl;
         CopyOnWriteData d("\"iiiiiiNOPEjjjjjj\"STUFFiiiiiMORE STUFFii");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("kj", true, false, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "\"iiiiiiNOPEjjjjjj\"STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "parseOutsideQuotes, <>" << endl;
         CopyOnWriteData d("<iiiiiiNOPEjjjjjj>STUFFijijijMORE STUFFij");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("ij", false, true, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "<iiiiiiNOPEjjjjjj>STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, <>" << endl;
         CopyOnWriteData d("<iiiiiiNOPEjjjjjj>STUFFiiiiiMORE STUFFii");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("kj", false, true, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "<iiiiiiNOPEjjjjjj>STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "parseOutsideQuotes, <\"\">" << endl;
         CopyOnWriteData d("<\"iiiiiiNOPEjjjjjj\">STUFFijijijMORE STUFFij");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("ij", false, true, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "<\"iiiiiiNOPEjjjjjj\">STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, <\"\">" << endl;
         CopyOnWriteData d("<\"iiiiiiNOPEjjjjjj\">STUFFiiiiiMORE STUFFii");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("kj", false, true, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "<\"iiiiiiNOPEjjjjjj\">STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "matchChar" << endl;
         CopyOnWriteData d("SKIPiiiiiMORE STUFFi");
         bool matchFail;
         CopyOnWriteData result = d.parse("ij", &matchFail);
         assert(!matchFail);
         assert(result == "SKIP");
         assert(d == "MORE STUFFi");
      }

      {
         cerr << "parseOutsideQuotes, \"<>\"" << endl;
         CopyOnWriteData d("\"<iiiiiiNOPEjjjjjj>\"STUFFijijijMORE STUFFij");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("ij", true, false, &matchFail);
         cerr << "result = " << result << endl;
         cerr << "rest = " << d << endl;
         assert(!matchFail);
         assert(result == "\"<iiiiiiNOPEjjjjjj>\"STUFF");
         assert(d == "MORE STUFFij");
      }

      {
         cerr << "parseOutsideQuotes test fail, \"<>\"" << endl;
         CopyOnWriteData d("\"<iiiiiiNOPEjjjjjj>\"STUFFiiiiiMORE STUFFii");
         bool matchFail;
         CopyOnWriteData result = d.parseOutsideQuotes("kj", true, false, &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "\"<iiiiiiNOPEjjjjjj>\"STUFFiiiiiMORE STUFFii");
      }

      {
         cerr << "matchChar" << endl;
         CopyOnWriteData d("SKIPiiiiiMORE STUFFi");
         bool matchFail;
         CopyOnWriteData result = d.parse("ij", &matchFail);
         assert(!matchFail);
         assert(result == "SKIP");
         assert(d == "MORE STUFFi");
      }

      {
         cerr << "matchChar" << endl;
         CopyOnWriteData d("SKIPiiiiiMORE STUFFi");
         bool matchFail;
         CopyOnWriteData result = d.parse("kj", &matchFail);
         assert(matchFail);
         assert(result == "");
         assert(d == "SKIPiiiiiMORE STUFFi");
      }

      {
         cerr << "getLine" << endl;
         bool matchFail;
         CopyOnWriteData d("line 1\r\nline 2\r\nline 3\r\n");
      
         CopyOnWriteData line1 = d.getLine(&matchFail);
         assert(!matchFail);
         assert(line1 == "line 1");

         CopyOnWriteData line2 = d.getLine(&matchFail);
         assert(!matchFail);
         assert(line2 == "line 2");

         CopyOnWriteData line3 = d.getLine(&matchFail);
         assert(!matchFail);
         assert(line3 == "line 3");

         CopyOnWriteData line4 = d.getLine(&matchFail);
         assert(matchFail);
         assert(line4 == "");
      }

      {
         cerr << "removeSpaces" << endl;
         CopyOnWriteData d("         SPACES        ");
         d.removeSpaces();
         assert(d == "SPACES");
         d.removeSpaces();
         assert(d == "SPACES");

         CopyOnWriteData b("      BEFORE");
         b.removeSpaces();
         assert(b == "BEFORE");


         CopyOnWriteData a("AFTER     ");
         a.removeSpaces();
         assert(a == "AFTER");

         CopyOnWriteData e("             ");
         e.removeSpaces();
         assert(e == "");
      }

      {
         cerr << "substring" << endl;

         CopyOnWriteData d("123456789");
         assert(d.substring(3, 5) == "45");
         assert(d.substring(3, -1) == "456789");
         assert(d.substring(3, 3) == "");
         assert(d.substring(0, 3) == "123");
         assert(d.substring(0, 0) == "");
         assert(d.substring(0, -1) == d);
      }

      {
         cerr << "isEqualNoCase" << endl;
         assert(isEqualNoCase(CopyOnWriteData("MIxed"), CopyOnWriteData("mixed")));
         assert(isEqualNoCase(CopyOnWriteData(""), CopyOnWriteData("")));
         assert(!isEqualNoCase(CopyOnWriteData(""), CopyOnWriteData("a")));
         assert(!isEqualNoCase(CopyOnWriteData("b"), CopyOnWriteData("a")));

         assert(isEqualNoCase("MIxed", CopyOnWriteData("mixed")));
         assert(isEqualNoCase("", CopyOnWriteData("")));
         assert(!isEqualNoCase("", CopyOnWriteData("a")));
         assert(!isEqualNoCase("b", CopyOnWriteData("a")));
      }
   }
}
