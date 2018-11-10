#include "catch.hpp"
#include "nbfx.hpp"

#include <array>
#include <cstdint>
#include <string>

using namespace nbfx;
using namespace detail;

// Payloads are taken from MC-NBFX, 3 Structure Examples

TEST_CASE("TestShortAttributeParse", "[parseAttribute]")
{
	/*
				attr="false"
				*/
	std::vector<uint8_t> data = {
		0x04, 0x04, 0x61, 0x74, 0x74, 0x72, 0x84};

	auto p = data.cbegin();
	auto attr = parseAttribute(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly
	REQUIRE(attr.name() == L"attr"); // attribute name is incorrect
	REQUIRE(attr.value().type() == NbfxValueType::Boolean); // attribute value type is incorrect
	REQUIRE(attr.value().to_string() == L"false"); // attribute value is incorrect
}

TEST_CASE("TestXmlsAttributeParse", "[parseAttribute]")
{
	/*
				xmlns:pre="http://abc"
				*/
	std::vector<uint8_t> data = {
		0x09, 0x03, 0x70, 0x72, 0x65, 0x0A, 0x68, 0x74,
		0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x61, 0x62, 0x63};
	auto p = data.cbegin();
	auto attr = parseAttribute(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly

	// a bit counter-intuitive, but xmlns:p is parsed like p is a prefix and xmlns is a name
	REQUIRE(attr.name() == L"xmlns"); // attribute name is incorrect
	REQUIRE(attr.prefix() == L"pre"); // attribute prefix is incorrect
	REQUIRE(attr.value().type() == NbfxValueType::String); // attribute value type is incorrect
	REQUIRE(attr.value().to_string() == L"http://abc"); // attribute value is incorrect
}

TEST_CASE("TestShortXmlsAttributeParse", "[parseAttribute]")
{
	/*
				xmlns:pre="http://abc"
				*/
	std::vector<uint8_t> data = {
		0x08, 0x0A, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F,
		0x2F, 0x61, 0x62, 0x63};
	auto p = data.cbegin();
	auto attr = parseAttribute(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly

	REQUIRE(attr.name() == L"xmlns"); // attribute name is incorrect
	REQUIRE(attr.prefix() == L""); // attribute prefix is incorrect
	REQUIRE(attr.value().type() == NbfxValueType::String); // attribute value type is incorrect
	REQUIRE(attr.value().to_string() == L"http://abc"); // attribute value is incorrect
}

TEST_CASE("TestAttributeParse", "[parseAttribute]")
{
	/*
				pre:attr="false"
				*/
	std::vector<uint8_t> data = {
		0x05, 0x03, 0x70, 0x72, 0x65, 0x04, 0x61, 0x74,
		0x74, 0x72, 0x84};

	auto p = data.cbegin();
	auto attr = parseAttribute(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly

	REQUIRE(attr.prefix() == L"pre"); // attribute prefix is incorrect
	REQUIRE(attr.name() == L"attr"); // attribute name is incorrect
	REQUIRE(attr.value().type() == NbfxValueType::Boolean); // attribute value type is incorrect
	REQUIRE(attr.value().to_string() == L"false"); // attribute value is incorrect
}

TEST_CASE("TestPrefixAttributeKParse", "[parseAttribute]")
{
	/*
				k:attr="true"
				*/
	std::vector<uint8_t> data = {
		0x30, 0x04, 0x61, 0x74, 0x74, 0x72, 0x86};
	auto p = data.cbegin();
	auto attr = parseAttribute(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly

	REQUIRE(attr.name() == L"attr"); // attribute name is incorrect
	REQUIRE(attr.prefix() == L"k"); // attribute prefix is incorrect
	REQUIRE(attr.value().type() == NbfxValueType::Boolean); // attribute value type is incorrect
	REQUIRE(attr.value().to_string() == L"true"); // attribute value is incorrect
}
