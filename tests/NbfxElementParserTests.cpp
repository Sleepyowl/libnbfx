#include "catch.hpp"
#include "nbfx.hpp"

#include <array>
#include <cstdint>
#include <string>

using namespace nbfx;
using namespace detail;

// Payloads are taken from MC-NBFX, 3 Structure Examples
TEST_CASE("TestShortElementParse", "[parseElement]")
{
	/*
				<doc></doc>
				*/
	std::vector<uint8_t> data = {
		0x40, 0x03, 0x64, 0x6F, 0x63};
	auto p = data.cbegin();
	auto elem = parseElement(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly

	REQUIRE(elem.type() == NbfxRecordType::ShortElement);
	REQUIRE(elem.prefix() == L"");							   // Element prefix is wrong
	REQUIRE(elem.name() == L"doc");							   // Element name is wrong
	REQUIRE(elem.children().size() == 0u);					   // Element children count is wrong
	REQUIRE(elem.attributes().size() == 0u);				   // Element atribute count is wrong
	REQUIRE(elem.value().type() == NbfxValueType::Null); // Element value is wrong
}

TEST_CASE("TestElementParse", "[parseElement]")
{
	/*
				<pre:doc></pre:doc>
				*/
	std::vector<uint8_t> data = {
		0x41, 0x03, 0x70, 0x72, 0x65, 0x03, 0x64, 0x6F,
		0x63};
	auto p = data.cbegin();
	auto elem = parseElement(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly

	REQUIRE(elem.type() == NbfxRecordType::Element);
	REQUIRE(elem.prefix() == L"pre");						   // Element prefix is wrong
	REQUIRE(elem.name() == L"doc");							   // Element name is wrong
	REQUIRE(elem.children().size() == 0u);					   // Element children count is wrong
	REQUIRE(elem.attributes().size() == 0u);				   // Element atribute count is wrong
	REQUIRE(elem.value().type() == NbfxValueType::Null); // Element value is wrong
}

TEST_CASE("TestPrefixElementSParse", "[parseElement]")
{
	/*
				<s:MyMessage></s:MyMessage>
				*/
	std::vector<uint8_t> data = {
		0x70, 0x09, 0x4D, 0x79, 0x4D, 0x65, 0x73, 0x73,
		0x61, 0x67, 0x65};
	auto p = data.cbegin();
	auto elem = parseElement(p);
	REQUIRE(p == data.cend()); // Pointer advanced incorrectly

	REQUIRE(elem.type() == NbfxRecordType::PrefixElementS);
	REQUIRE(elem.prefix() == L"s");							   // Element prefix is wrong
	REQUIRE(elem.name() == L"MyMessage");					   // Element name is wrong
	REQUIRE(elem.children().size() == 0u);					   // Element children count is wrong
	REQUIRE(elem.attributes().size() == 0u);				   // Element atribute count is wrong
	REQUIRE(elem.value().type() == NbfxValueType::Null); // Element value is wrong
}
