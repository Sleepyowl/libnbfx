#include "catch.hpp"
#include "nbfx.hpp"

#include <array>
#include <cstdint>
#include <string>


using namespace nbfx;
using namespace nbfx::detail;

void test(const nbfx::NbfxElement &record, std::initializer_list<uint8_t> expected) {
    std::vector<uint8_t> ex_buf(expected);
    std::vector<uint8_t> result;
    serialize(record, std::back_inserter(result));
    REQUIRE(ex_buf.size() == result.size());
    REQUIRE(std::equal(std::cbegin(ex_buf), std::cend(ex_buf), std::cbegin(result)));
}

// Payloads are taken from MC-NBFX, 3 Structure Examples

TEST_CASE("ShortElement serializes according to spec", "[nbfx::serialize]") {
    test(NbfxElement(L"doc"), {0x40, 0x03, 0x64, 0x6F, 0x63, 0x01});
}

TEST_CASE("Element serializes according to spec", "[nbfx::serialize]") {
    test(NbfxElement(QName(L"pre", L"doc")),
         {0x41, 0x03, 0x70, 0x72, 0x65,
          0x03, 0x64, 0x6F, 0x63, 0x01});
}

TEST_CASE("PrefixElementS serializes according to spec", "[nbfx::serialize]") {
    test(NbfxElement(QName(L"s", L"MyMessage"), {NbfxAttribute(QName(L"s", L"xmlns"), NbfxValue(L"http://abc"))}, NbfxValue()),
         {
                 0x70, 0x09, 0x4D, 0x79, 0x4D,
                 0x65, 0x73, 0x73, 0x61, 0x67,
                 0x65, 0x09, 0x01, 0x73, 0x0A,
                 0x68, 0x74, 0x74, 0x70, 0x3A,
                 0x2F, 0x2F, 0x61, 0x62, 0x63,
                 0x01,
         });
}

TEST_CASE("nbfx_serializer sorts child elements alphabetically", "[nbfx::serialize]") {
    NbfxElement el(QName(L"s", L"Parent"), {}, {
            NbfxElement(QName(L"s", L"qwerty")),
            NbfxElement(QName(L"s", L"kremlin")),
            NbfxElement(QName(L"s", L"zombie")),
            NbfxElement(QName(L"s", L"ansible"))
    });

    std::vector<uint8_t> buffer;
    serialize(el, std::back_inserter(buffer));
    const auto result = parse(buffer.begin());

    REQUIRE(result.children().size() == 4);
    REQUIRE(result.children().at(0).name() == L"ansible");
    REQUIRE(result.children().at(1).name() == L"kremlin");
    REQUIRE(result.children().at(2).name() == L"qwerty");
    REQUIRE(result.children().at(3).name() == L"zombie");
}

TEST_CASE("nbfx_serializer keeps order of child elements with same name", "[nbfx::serialize]") {
    NbfxElement el(QName(L"s", L"Parent"), {}, {
            NbfxElement(QName(L"s", L"ansible"), {NbfxAttribute(L"o", NbfxValue(0))}, {}),
            NbfxElement(QName(L"s", L"ansible"), {NbfxAttribute(L"o", NbfxValue(1))}, {}),
            NbfxElement(QName(L"s", L"ansible"), {NbfxAttribute(L"o", NbfxValue(2))}, {}),
            NbfxElement(QName(L"s", L"ansible"), {NbfxAttribute(L"o", NbfxValue(3))}, {})
    });

    std::vector<uint8_t> buffer;
    serialize(el, std::back_inserter(buffer));
    const auto result = parse(buffer.begin());

    REQUIRE(result.children().size() == 4);
    for (auto i = 0u; i < 4u; ++i) {
        REQUIRE(result.children().at(i).attributes().at(0).value().integer() == i);
    }
}

TEST_CASE("Bytes8TextWithEndSerialize", "[nbfx::serialize]") {
    std::array<uint8_t, 8> pl = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };

    test(NbfxElement(L"Base64", {}, NbfxValue(pl)), {
            0x40, 0x06, 0x42, 0x61, 0x73, 0x65, 0x36, 0x34, 0x9F, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
            0x06, 0x07
    });
}