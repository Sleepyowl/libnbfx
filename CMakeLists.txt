cmake_minimum_required(VERSION 3.5)
project (libnbfx)

set (SOURCES src/NbfxValue.cpp)

add_library (nbfx ${SOURCES})
target_compile_features(nbfx PUBLIC cxx_std_17)
target_include_directories (nbfx PUBLIC
	include
)

set (TEST_SOURCES
	./tests/test_main.cpp
	./tests/NbfxSerializerTests.cpp
	./tests/NbfxValueParserTests.cpp
	./tests/NbfxElementParserTests.cpp
	./tests/NbfxAttributeParserTests.cpp
    ./tests/NbfxRoundTripTests.cpp
	./tests/NbfxElementTests.cpp)

add_executable(nbfx_test ${TEST_SOURCES})
target_link_libraries(nbfx_test nbfx)

enable_testing()
add_test(NAME NbfxTestSuite COMMAND nbfx_test)
