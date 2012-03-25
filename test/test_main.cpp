/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2012, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include "ns_base.h"
#define STR(X) #X

// Test target
#include "ns_utils.h"

void test_nifver2str();
void test_kfmver2str();
void test_nifstr2ver();
void test_kfmstr2ver();

static struct {
	int t, p, f;
	void Run(void (*test)(), std::string name)
	{
		if (test) {
			Init (name);
			test ();
			PrintStats ();
		}
	}
	void Init(std::string testName)
	{
		t = 0;
		p = 0;
		f = 0;
		std::cout << " Test:\'" << testName << "\':" << std::endl;
	}
	void PrintStats()
	{
		std::cout << "  ------------------------------------------"
			<< std::endl;
		std::cout
			<< "  Tests: " << t << ", "
			<< "Passed: " << p << ", "
			<< "Failed: " << f
			<< std::endl;
	}
} TEST;

static struct {
	int run, f, p;
	void Init()
	{
		run = 0;
		f = 0;
		p = 0;
	}
	void PrintStats()
	{
		std::cout << "=========================================="
			<< std::endl;
		std::cout
			<< "Tests: " << run << ", "
			<< "Passed: " << p << ", "
			<< "Failed: " << f
			<< std::endl;
	}
} TESTS;

#define TEST(ITM)\
{\
	TESTS.run++;\
 	TEST.Run (test_##ITM, STR(ITM));\
	if (TEST.f > 0)\
		TESTS.f++;\
	else\
		TESTS.p++;\
}

int
main(int argc, char **argv)
{
	TESTS.Init ();

	TEST(nifver2str)
	TEST(kfmver2str)
	TEST(nifstr2ver)
	TEST(kfmstr2ver)

	TESTS.PrintStats ();
	return 0;
}

#define MAKE_SURE(C)\
{\
	TEST.t++;\
	if ((C)) {\
		TEST.p++;\
		std::cout << "  \'" << #C << "\'" << "... passed." << std::endl;\
	}\
	else {\
		TEST.f++;\
		std::cout << "  \'" << #C << "\'" << "... failed." << std::endl;\
	}\
}

void
test_nifver2str()
{
	MAKE_SURE(nifver2str (1) == "0.001")
	MAKE_SURE(nifver2str (0x00000001) == "0.001")
	MAKE_SURE(nifver2str (0x00000100) == "0.01")
	MAKE_SURE(nifver2str (0x00010000) == "0.1")
	MAKE_SURE(nifver2str (0x02000000) ==  "2.0")
	MAKE_SURE(nifver2str (0x0200000b) == "2.0011")
	MAKE_SURE(nifver2str (0x02000b0b) ==  "2.01111")
	MAKE_SURE(nifver2str (0x02000b00) == "2.011")
	MAKE_SURE(nifver2str (0x02010304) == "2.134")
	MAKE_SURE(nifver2str (NF_V03030013) == "3.3.0.13")
	MAKE_SURE(nifver2str (NF_V03030013-1) != "3.3.0.12")
	MAKE_SURE(nifver2str (NF_V03030013-1) == "3.3012")
	MAKE_SURE(nifver2str (0x01000000) == "1.0")
	MAKE_SURE(nifver2str (0) == "")
	MAKE_SURE(nifver2str (-1) == "255.255.255.255")
}

void
test_kfmver2str()
{
	MAKE_SURE(kfmver2str (1) == "0.0.0.1")
	MAKE_SURE(kfmver2str (0) == "")
	MAKE_SURE(kfmver2str (-1) == "ff.ff.ff.ff")
	MAKE_SURE(kfmver2str (0x000A0B0C) == "0.a.b.c")
	MAKE_SURE(kfmver2str (0x0200000B) == "2.0.0.b")
}

void
test_nifstr2ver()
{
	MAKE_SURE(nifstr2ver ("1.234") == 0x01020304)
	MAKE_SURE(nifstr2ver ("128.234") == 0x80020304)
	MAKE_SURE(nifstr2ver ("1.2") == 0x01020000)
	MAKE_SURE(nifstr2ver ("1.23") == 0x01020300)
	MAKE_SURE(nifstr2ver ("") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3") == 0x01020300)
	MAKE_SURE(nifstr2ver ("1..3") == 0)
	MAKE_SURE(nifstr2ver (".") == 0)
	MAKE_SURE(nifstr2ver ("..") == 0)
	MAKE_SURE(nifstr2ver ("...") == 0)
	MAKE_SURE(nifstr2ver ("1.") == 0)
	MAKE_SURE(nifstr2ver (".1") == 0)
	MAKE_SURE(nifstr2ver ("1234.1") == 0)
	MAKE_SURE(nifstr2ver ("1.1234") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3.4") == 0x01020304)
	MAKE_SURE(nifstr2ver ("1111.2.3.4") == 0)
	MAKE_SURE(nifstr2ver ("1.2222.3.4") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3.4444") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3.4.5") == 0)
	MAKE_SURE(nifstr2ver ("255.2.3.4") == 0xFF020304)
	MAKE_SURE(nifstr2ver ("256.2.3.4") == 0)
	MAKE_SURE(nifstr2ver ("0.0.0.0") == 0)
	MAKE_SURE(nifstr2ver ("1..3.4") == 0)
	MAKE_SURE(nifstr2ver ("1.2.333.4") == 0)
	MAKE_SURE(nifstr2ver ("255.255.255.255") == 0xFFFFFFFF)
	MAKE_SURE(nifstr2ver ("255.255.255.2554") == 0)
	MAKE_SURE(nifstr2ver ("25x.25y.25z.25w") == 0)
	MAKE_SURE(nifstr2ver ("255") == 0x000000ff)
	MAKE_SURE(nifstr2ver ("0255") == 0x000000ff)
	MAKE_SURE(nifstr2ver ("0000000000255") == 0x000000ff)
	MAKE_SURE(nifstr2ver ("256") == 0x00000100)
	MAKE_SURE(nifstr2ver ("00000000000255") == 0)
	MAKE_SURE(nifstr2ver ("1.2.4b") == 0x01024b00)
	MAKE_SURE(nifstr2ver ("1.2.4b") != 0x01020400)
}

void
test_kfmstr2ver()
{
	MAKE_SURE(nifstr2ver ("1.234") == 0x01020304)
	MAKE_SURE(nifstr2ver ("128.234") == 0x80020304)
	MAKE_SURE(nifstr2ver ("1.2") == 0x01020000)
	MAKE_SURE(nifstr2ver ("1.23") == 0x01020300)
	MAKE_SURE(nifstr2ver ("") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3") == 0x01020300)
	MAKE_SURE(nifstr2ver ("1..3") == 0)
	MAKE_SURE(nifstr2ver (".") == 0)
	MAKE_SURE(nifstr2ver ("..") == 0)
	MAKE_SURE(nifstr2ver ("...") == 0)
	MAKE_SURE(nifstr2ver ("1.") == 0)
	MAKE_SURE(nifstr2ver (".1") == 0)
	MAKE_SURE(nifstr2ver ("1234.1") == 0)
	MAKE_SURE(nifstr2ver ("1.1234") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3.4") == 0x01020304)
	MAKE_SURE(nifstr2ver ("1111.2.3.4") == 0)
	MAKE_SURE(nifstr2ver ("1.2222.3.4") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3.4444") == 0)
	MAKE_SURE(nifstr2ver ("1.2.3.4.5") == 0)
	MAKE_SURE(nifstr2ver ("255.2.3.4") == 0xFF020304)
	MAKE_SURE(nifstr2ver ("256.2.3.4") == 0)
	MAKE_SURE(nifstr2ver ("0.0.0.0") == 0)
	MAKE_SURE(nifstr2ver ("1..3.4") == 0)
	MAKE_SURE(nifstr2ver ("1.2.333.4") == 0)
	MAKE_SURE(nifstr2ver ("255.255.255.255") == 0xFFFFFFFF)
	MAKE_SURE(nifstr2ver ("255.255.255.2554") == 0)
	MAKE_SURE(nifstr2ver ("25x.25y.25z.25w") == 0)
	MAKE_SURE(nifstr2ver ("255") == 0x000000ff)
	MAKE_SURE(nifstr2ver ("0255") == 0x000000ff)
	MAKE_SURE(nifstr2ver ("0000000000255") == 0x000000ff)
	MAKE_SURE(nifstr2ver ("256") == 0x00000100)
	MAKE_SURE(nifstr2ver ("00000000000255") == 0)
	MAKE_SURE(nifstr2ver ("1.2.4b") == 0x01024b00)
	MAKE_SURE(nifstr2ver ("1.2.4b") != 0x01020400)
}
