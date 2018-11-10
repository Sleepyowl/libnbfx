#include "catch.hpp"
#include "nbfx/NbfxElement.hpp"


TEST_CASE("findDescendant searches breadth-first", "[nbfx::NbfxElement]") {
    nbfx::NbfxElement   el(L"root", {}, {
            nbfx::NbfxElement(L"C", {}, nbfx::NbfxValue(true)),
            nbfx::NbfxElement(L"A", {}, {
                    nbfx::NbfxElement(L"C", {}, nbfx::NbfxValue(false))
            })
    });

    nbfx::NbfxElement   el2(L"root", {}, {
            nbfx::NbfxElement(L"A", {}, {
                    nbfx::NbfxElement(L"C", {}, nbfx::NbfxValue(false))
            }),
            nbfx::NbfxElement(L"C", {}, nbfx::NbfxValue(true)),
    });

    const auto c = el.find_descendant(L"C");
    const auto c2 = el2.find_descendant(L"C");
    REQUIRE(c != nullptr);
    REQUIRE(c2 != nullptr);
    REQUIRE(c->value().boolean());
    REQUIRE(c2->value().boolean());
}