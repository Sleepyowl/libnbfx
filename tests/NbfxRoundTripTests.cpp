#include "catch.hpp"
#include "nbfx.hpp"

#include <array>
#include <cstdint>
#include <string>

using namespace nbfx;

void test(const nbfx::NbfxElement &record) {
    std::vector<uint8_t>    buffer;
    serialize(record, std::back_inserter(buffer));
    auto obj = parse(buffer.begin());

    std::vector<uint8_t>    buffer2;
    serialize(obj, std::back_inserter(buffer2));

    REQUIRE(std::equal(buffer.begin(), buffer.end(), buffer2.begin(), buffer2.end()));
}

TEST_CASE("Bytes8TextWithEndRoundTrip", "[nbfx::roundtrip]") {
    std::array<uint8_t, 8> pl = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };

    test(NbfxElement(L"Base64", {}, NbfxValue(pl)));
}
