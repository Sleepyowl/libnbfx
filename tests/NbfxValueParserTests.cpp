#include "catch.hpp"
#include "nbfx.hpp"

#include <array>
#include <cstdint>
#include <string>

// Payloads are taken from MC-NBFX, 3 Structure Examples
using namespace nbfx;
using namespace detail;
using NbfxValueType = NbfxValueType;

void stringValueTest(std::initializer_list<uint8_t> data,
					 const std::wstring &expectedvalue, 
					 NbfxValueType expectedtype, 
					 bool expectedend)
{
	std::vector<uint8_t> buf(data);
	auto p = buf.cbegin();
	bool end;
	auto val = parseValue(p, &end);
	REQUIRE(p == buf.cend());				  // "Pointer advanced incorrectly";
	REQUIRE(end == expectedend);				  // "Wrong end result value";
	REQUIRE(val.type() == expectedtype);		  // "Wrong value type";
	REQUIRE(val.to_string() == expectedvalue); // L"Wrong value %s";
}

// TODO: rename test_cases

TEST_CASE("TestZeroTextParse", "[parseValue]")
{
	stringValueTest({0x80}, L"0", NbfxValueType::Integer, false);
}

TEST_CASE("TestZeroTextWithEndParse", "[parseValue]")
{
	stringValueTest({0x81}, L"0", NbfxValueType::Integer, true);
}

TEST_CASE("TestOneTextParse", "[parseValue]")
{
	stringValueTest({0x82}, L"1", NbfxValueType::Integer, false);
}

TEST_CASE("TestOneTextWithEndParse", "[parseValue]")
{
	stringValueTest({0x83}, L"1", NbfxValueType::Integer, true);
}

TEST_CASE("TestFalseTextParse", "[parseValue]")
{
	stringValueTest({0x84}, L"false", NbfxValueType::Boolean, false);
}

TEST_CASE("TestFalseTextWithEndParse", "[parseValue]")
{
	stringValueTest({0x85}, L"false", NbfxValueType::Boolean, true);
}

TEST_CASE("TestTrueTextParse", "[parseValue]")
{
	stringValueTest({0x86}, L"true", NbfxValueType::Boolean, false);
}

TEST_CASE("TestTrueTextWithEndParse", "[parseValue]")
{
	stringValueTest({0x87}, L"true", NbfxValueType::Boolean, true);
}

TEST_CASE("TestInt8TextParse", "[parseValue]")
{
	stringValueTest({0x88, 0xDE}, L"-34", NbfxValueType::Integer, false);
}

TEST_CASE("TestInt8TextWithEndParse", "[parseValue]")
{
	stringValueTest({0x89, 0x7F}, L"127", NbfxValueType::Integer, true);
}

TEST_CASE("TestInt16TextParse", "[parseValue]")
{
	stringValueTest({0x8A, 0x00, 0x80}, L"-32768", NbfxValueType::Integer, false);
}

TEST_CASE("TestInt16TextWithEndParse", "[parseValue]")
{
	stringValueTest({0x8B, 0xFF, 0x7F}, L"32767", NbfxValueType::Integer, true);
}

TEST_CASE("TestInt32TextParse", "[parseValue]")
{
	stringValueTest({0x8C, 0x15, 0xCD, 0x5B, 0x07}, L"123456789", NbfxValueType::Integer, false);
}

TEST_CASE("TestInt32TextWithEndParse", "[parseValue]")
{
	stringValueTest({0x8D, 0xFF, 0xFF, 0xFF, 0x7F}, L"2147483647", NbfxValueType::Integer, true);
}

TEST_CASE("TestInt64TextParse", "[parseValue]")
{
	stringValueTest({0x8E, 0x00, 0x00, 0x00,
					 0x80, 0x00, 0x00, 0x00, 0x00},
					L"2147483648", NbfxValueType::Integer, false);
}

TEST_CASE("TestInt64TextWithEndParse", "[parseValue]")
{
	stringValueTest({0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00}, L"1099511627776", NbfxValueType::Integer, true);
}

TEST_CASE("TestFloatTextParse", "[parseValue]")
{
	stringValueTest({0x90, 0xCD, 0xCC, 0x8C, 0x3F}, L"1.1", NbfxValueType::Float, false);
}

/*TEST_CASE("TestFloatTextSpecialCasesParse", "[parseValue]")
{
	// TODO: special cases: INF etc
}*/

TEST_CASE("TestFloatTextWithEndParse", "[parseValue]")
{
	stringValueTest({0x91, 0xCD, 0xCC, 0x01, 0x42}, L"32.45", NbfxValueType::Float, true);
}
