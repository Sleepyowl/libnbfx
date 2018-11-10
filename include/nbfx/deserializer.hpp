#pragma once

#include "NbfxRecord.hpp"
#include "NbfxAttribute.hpp"
#include "NbfxValue.hpp"
#include "NbfxElement.hpp"

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <cstdint>
#include <stdexcept>
#include <codecvt>
#include <locale>
#include <iomanip>

namespace nbfx {

	namespace detail {
		namespace {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> utf_to_wstring;


            template<typename T, typename TIter>
            T read_and_advance(TIter& p)
            {
                auto from = p;
                p += sizeof(T);
                return *reinterpret_cast<const T*>(&*from);
            }

            template<typename TIter>
            datetime_t parseDateTime(TIter& p)
            {
                uint64_t raw = *reinterpret_cast<const uint64_t*>(&*p);
                p += sizeof(uint64_t);
                unsigned char tz = raw >> 62;

                if (tz == 1) /* UTC */ {
                    throw std::runtime_error("TZ correction not implemented");
                }

                std::chrono::nanoseconds nano{ ((raw & 0x3FFFFFFFFFFFFFFF) - 621355968000000000) * 100 };

                return datetime_t(nano);
            }

            template<typename TIter>
            uint64_t parseMultiByteInt21(TIter& p)
            {
                uint64_t result = 0;
                short segment = 0;
                do {
                    result |= (*p & 0x7F) << (7 * segment++);
                } while ((*(p++) & 0x80) != 0);

                return result;
            }

            template<typename TIter>
            std::wstring parseString(TIter& p) {
                // handle highest bit?
                long length = static_cast<long>(parseMultiByteInt21(p));

                std::string str(p, p + length);

                p += length;

                return utf_to_wstring.from_bytes(str);
            }

            template<typename TIter>
            std::wstring parseDictionaryString(TIter& p)
            {
                uint64_t id = parseMultiByteInt21(p);
                return L"D:" + std::to_wstring(id);
            }

            template<typename TIter>
            NbfxValue parseValue(TIter& p, bool* outWithEnd = nullptr) {
                TIter from, to;

                if (outWithEnd) {
                    *outWithEnd = *p & 1;
                }

                auto type = static_cast<NbfxRecordType>(*(p++) & 0xFE);

                switch (type) {
                    case NbfxRecordType::ZeroText:
                        return NbfxValue(static_cast<int64_t>(0));
                    case NbfxRecordType::OneText:
                        return NbfxValue(static_cast<int64_t>(1));
                    case NbfxRecordType::TrueText:
                        return NbfxValue(true);
                    case NbfxRecordType::FalseText:
                        return NbfxValue(false);

                        // TODO: remove code duplication for Chars*Text
                    case NbfxRecordType::Chars8Text:
                    {
                        auto len = *(p++);
                        from = p;
                        to = p + len;
                        p += len;
                        return utf_to_wstring.from_bytes(reinterpret_cast<const char*>(&*from),
                                                         reinterpret_cast<const char*>(&*to));
                    }
                    case NbfxRecordType::Chars16Text:
                    {
                        auto len = read_and_advance<uint16_t>(p);
                        from = p;
                        to = p + len;
                        p += len;
                        return utf_to_wstring.from_bytes(reinterpret_cast<const char*>(&*from),
                                                         reinterpret_cast<const char*>(&*to));
                    }
                    case NbfxRecordType::Chars32Text:
                    {
                        auto len = read_and_advance<uint32_t>(p);
                        from = p;
                        to = p + len;
                        p += len;
                        return utf_to_wstring.from_bytes(reinterpret_cast<const char*>(&*from),
                                                         reinterpret_cast<const char*>(&*to));
                    }

                    case NbfxRecordType::Int8Text:
                        return NbfxValue(static_cast<int64_t>(*reinterpret_cast<const signed char*>(&*(p++))));
                    case NbfxRecordType::Int16Text:
                        from = p;
                        p += 2;
                        return NbfxValue(static_cast<int64_t>(*reinterpret_cast<const short int*>(&*from)));
                    case NbfxRecordType::Int32Text:
                        from = p;
                        p += 4;
                        return NbfxValue(static_cast<int64_t>(*reinterpret_cast<const int*>(&*from)));
                    case NbfxRecordType::Int64Text:
                        from = p;
                        p += 8;
                        return NbfxValue(*reinterpret_cast<const int64_t*>(&*from));
                    case NbfxRecordType::UInt64Text:
                        from = p;
                        p += 8;
                        return NbfxValue(*reinterpret_cast<const uint64_t*>(&*from));
                    case NbfxRecordType::DateTimeText:
                    {
                        return NbfxValue(parseDateTime(p));
                    }

                        // TODO: remove code duplication for Bytes*Text
                    case NbfxRecordType::Bytes32Text:
                    {
                        auto len = *reinterpret_cast<const int32_t*>(&*p);
                        p += 4;
                        auto data = p;
                        p += len;
                        return std::vector<uint8_t>(data, data + len);
                    }
                    case NbfxRecordType::Bytes16Text:
                    {
                        auto len = *reinterpret_cast<const uint16_t*>(&*p);
                        p += 2;
                        auto data = p;
                        p += len;
                        return std::vector<uint8_t>(data, data + len);
                    }
                    case NbfxRecordType::Bytes8Text:
                    {
                        auto len = *reinterpret_cast<const uint8_t*>(&*p);
                        ++p;
                        auto data = p;
                        p += len;
                        return std::vector<uint8_t>(data, data + len);
                    }
                    case NbfxRecordType::FloatText:
                    {
                        std::wostringstream ss;
                        from = p;
                        p += sizeof(float);
                        return NbfxValue(*reinterpret_cast<const float*>(&*from));
                    }
                    case NbfxRecordType::DoubleText:
                    {
                        std::wostringstream ss;
                        from = p;
                        p += sizeof(double);
                        return NbfxValue(*reinterpret_cast<const double*>(&*from));
                    }
                    default:
                    {
                        std::ostringstream ss;
                        ss << "unexpected record type 0x"
                           << std::uppercase
                           << std::setfill('0')
                           << std::setw(2)
                           << std::hex
                           << static_cast<int>(type);

                        throw std::invalid_argument(ss.str());
                    }
                }
            }
		}





		template<typename TIter>
		NbfxElement parseElement(TIter& p) {
			auto type = static_cast<NbfxRecordType>(*(p++));

			bool prefixed =
					type == NbfxRecordType::Element ||
					type == NbfxRecordType::DictionaryElement;

			bool dictionary =
					type == NbfxRecordType::DictionaryElement ||
					type == NbfxRecordType::ShortDictionaryElement ||
					(type >= NbfxRecordType::PrefixDictionaryElementA && type <= NbfxRecordType::PrefixDictionaryElementZ);

			std::wstring prefix;

			if (prefixed) {
				prefix = parseString(p);
			}
			else if (type >= NbfxRecordType::PrefixDictionaryElementA && type <= NbfxRecordType::PrefixDictionaryElementZ) {
				wchar_t prefix1 =
						L'a' +
						static_cast<unsigned char>(type) -
						static_cast<unsigned char>(NbfxRecordType::PrefixDictionaryElementA);

				prefix = { prefix1 };
			}
			else if (type >= NbfxRecordType::PrefixElementA && type <= NbfxRecordType::PrefixElementZ) {
				wchar_t prefix1 =
						L'a' +
						static_cast<unsigned char>(type) -
						static_cast<unsigned char>(NbfxRecordType::PrefixElementA);

				prefix = { prefix1 };
			}

			auto name = dictionary ? parseDictionaryString(p) : parseString(p);

			return NbfxElement(type, QName(prefix, name));
		}

		template<typename TIter>
		NbfxAttribute parseAttribute(TIter& p)
		{
			auto type = static_cast<NbfxRecordType>(*(p++));

			bool prefixed =
					type <= NbfxRecordType::DictionaryXmlnsAttribute
					? (static_cast<unsigned char>(type) & 1) == 1
					: false; // a-z prefix based on type

			bool dictionary =
					type == NbfxRecordType::ShortDictionaryAttribute ||
					type == NbfxRecordType::ShortDictionaryXmlnsAttribute ||
					type == NbfxRecordType::DictionaryAttribute ||
					type == NbfxRecordType::DictionaryXmlnsAttribute ||
					(type >= NbfxRecordType::PrefixDictionaryAttributeA && type <= NbfxRecordType::PrefixDictionaryAttributeZ);

			std::wstring prefix;
			std::wstring name;
			NbfxValue value;

			if (prefixed) {
				prefix = parseString(p);
			}
			else if (type >= NbfxRecordType::PrefixDictionaryAttributeA && type <= NbfxRecordType::PrefixDictionaryAttributeZ) {
				wchar_t prefix1 =
						L'a' +
						static_cast<unsigned char>(type) -
						static_cast<unsigned char>(NbfxRecordType::PrefixDictionaryAttributeA);

				prefix = { prefix1 };
			}
			else if (type >= NbfxRecordType::PrefixAttributeA && type <= NbfxRecordType::PrefixAttributeZ) {
				wchar_t prefix1 =
						L'a' +
						static_cast<unsigned char>(type) -
						static_cast<unsigned char>(NbfxRecordType::PrefixAttributeA);

				prefix = { prefix1 };
			}

			if (type < NbfxRecordType::ShortXmlnsAttribute || type > NbfxRecordType::DictionaryXmlnsAttribute) {
				name = dictionary ? parseDictionaryString(p) : parseString(p);
				value = parseValue(p);
			}
			else {
				name = L"xmlns";
				value = dictionary ? parseDictionaryString(p) : parseString(p);
			}

			return NbfxAttribute(type, QName(prefix, name), value);
		}


	}

	template<typename TIter>
	std::enable_if_t<sizeof(typename std::iterator_traits<TIter>::value_type) == 1, NbfxElement>
	parse(TIter p) {
        using namespace detail;
		auto type = static_cast<NbfxRecordType>(*p);

		if (!IsElement(type)) {
			throw std::invalid_argument("expected element as a topmost node");
		}

		std::stack<NbfxElement> stack;

		for (;;) {
			type = static_cast<NbfxRecordType>(*p);

			if (IsElement(type)) {
				stack.emplace(parseElement(p));
			}
			else if (IsAttribute(type)) {
				stack.top().attributes().emplace_back(parseAttribute(p));
			}
			else if (IsTextRecord(type)) {
				bool valueWithEnd;
				auto &value = stack.top().value();
				if (value.type() == NbfxValueType::Bytes && value.bytes().size()) {
					// weird thing with two bytes text records
					auto new_chunk = parseValue(p, &valueWithEnd);
					if (new_chunk.type() != NbfxValueType::Bytes) {
						throw std::runtime_error("expected bytes to append to bytes");
					}
					value.bytes().insert(value.bytes().end(), new_chunk.bytes().cbegin(), new_chunk.bytes().cend());
				}
				else {
					value = parseValue(p, &valueWithEnd);
				}
				if (valueWithEnd) {
					type = NbfxRecordType::EndElement;
					--p;
				}
			}

			if (type == NbfxRecordType::EndElement) {
				++p;

				auto element = std::move(stack.top());
				stack.pop();

				if (stack.size() == 0) {
					return element;
				}
				else {
					stack.top().children().emplace_back(std::move(element));
				}
			}

		}
	}
}

