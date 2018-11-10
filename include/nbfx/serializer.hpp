#pragma once

#include "nbfx/NbfxElement.hpp"
#include "nbfx/NbfxAttribute.hpp"

#include <vector>
#include <cstdint>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <iomanip>
#include <codecvt>
#include <cstdint>
#include <algorithm>

namespace nbfx {

	namespace {
		constexpr uint64_t ticks_between_epochs = 621355968000000000ull;

		template<typename TIter>
		class NbfxWriter {
		public:
			explicit NbfxWriter(const TIter &it) : m_it(it) {}

		private:
			std::wstring_convert<std::codecvt_utf8<wchar_t>> utf_to_wstring;
			TIter m_it;

			void put(uint8_t byte) {
				*m_it++ = byte;
			}

			template<class T, typename = typename T::iterator>
			void put(const T &str) {
				std::copy(str.cbegin(), str.cend(), m_it);
			}

			void write_uint31(uint64_t val) {
				do {
					put(static_cast<uint8_t>(val > 0x7Fu ? val | 0x80u : val));
					val = val >> 7u;
				} while (val != 0);
			}

			void write_name(const std::wstring &name) {
				auto utf8 = utf_to_wstring.to_bytes(name);
				auto utf8_size = utf8.size();
				write_uint31(utf8_size);
				put(utf8);
			}

		public:
			void write(const NbfxElement &el, bool sort_members) {
				const auto &prefix = el.prefix();
				const auto &name = el.name();

				write_element(prefix, name);

				for (const auto &attr : el.attributes()) {
					write(attr);
				}

				auto& children = el.children();


				if (sort_members) {
					std::vector<std::reference_wrapper<const NbfxElement>> sorted_children(children.begin(), children.end());
					std::stable_sort(begin(sorted_children),
									 end(sorted_children),
									 [](auto left, auto right) {
										 return left.get().name() < right.get().name();
									 });

					for (auto ch : sorted_children) {
						write(ch.get(), true);
					}
				} else {
					for(const auto& child: children) {
						write(child, false);
					}
				}


				write(el.value(), true);
			}

			void write(const NbfxAttribute &attr) {
				auto type = attr.type();
				auto name = attr.name();
				auto prefix = attr.prefix();
				put(static_cast<uint8_t>(type));
				switch (type) {
					//case NbfxRecordType::DictionaryXmlnsAttribute:
					case NbfxRecordType::XmlnsAttribute:
						write_name(prefix);
						//case NbfxRecordType::ShortDictionaryXmlnsAttribute:
					case NbfxRecordType::ShortXmlnsAttribute:
						write_name(attr.value().string());
						break;
					case NbfxRecordType::Attribute:
						write_name(prefix);
						// fallthrough intentional
					case NbfxRecordType::ShortAttribute:
					case NbfxRecordType::ShortDictionaryAttribute:
					default:
						write_name(name);
						write(attr.value());
						break;
				}
			}

			void write(const NbfxValue &text, bool withEnd = false) {
				switch (text.type()) {
					case NbfxValueType::String: {
						auto utf8 = utf_to_wstring.to_bytes(text.string());
						write_string(utf8, withEnd);
						break;
					}
					case NbfxValueType::Bytes:
						write_vector(text.bytes(), withEnd);
						break;
					case NbfxValueType::Boolean:
						write_bool(text.boolean(), withEnd);
						break;
					case NbfxValueType::Integer:
						write_int(text.integer(), withEnd);
						break;
					case NbfxValueType::UInt64:
						write_uint64(text.uint64(), withEnd);
						break;
					case NbfxValueType::DateTime:
						write_datetime(text.datetime(), withEnd);
						break;
					case NbfxValueType::Null:
						if (withEnd) {
							write_element_end();
						}
						break;
					default:
						throw std::runtime_error("value type serialization not implemented");
				}
			}


			void write_element(const std::wstring &name) {
				put(0x40);
				write_name(name);
			}

			void write_element(const std::wstring &prefix, const std::wstring &name) {

				switch (prefix.size()) {
					case 0:
						return write_element(name);
					case 1: {
						auto p = prefix[0];
						if (p >= L'a' && p <= L'z') {
							put(0x5E + static_cast<uint8_t>(p) - 'a');
							break;
						}
					}
					default:
						put(0x41);
						write_name(prefix);
				};
				write_name(name);
			}

			void write_element_end() {
				put(1);
			}

			void write_attribute(const std::string &name) {
				put(0x04);
				if (name.size() > 127) {
					throw std::invalid_argument("name is too long");
				}
				put(name.size());
				put(name);
			}

			void write_attribute(char prefix, const std::string &name) {
				assert(prefix >= 'a' && prefix <= 'z');
				put(0x26 + prefix - 'a');
				if (name.size() > 127) {
					throw std::invalid_argument("name is too long");
				}
				put(name.size());
				put(name);
			}

			void write_xmlns(const std::string &prefix, const std::string &uri) {
				put(0x09);

				if (prefix.size() > 127) {
					throw std::invalid_argument("prefix is too long");
				}
				put(prefix.size());
				put(prefix);

				if (uri.size() > 127) {
					throw std::invalid_argument("uri is too long");
				}
				put(uri.size());
				put(uri);
			}

			void write_bool(bool v, bool withend = false) {
				put((v ? 0x86 : 0x84) + static_cast<uint8_t>(withend));
			}

			void write_int(int64_t v, bool withend = false) {
				constexpr auto M_32 = ~0ull << 31u;
				constexpr auto M_16 = ~0ull << 15u;
				constexpr auto M_8 = ~0ull << 7u;
				constexpr auto M_S = ~0ull >> 1u;
				const auto v_u = static_cast<uint64_t>(v & M_S);
				if (v_u & M_32) {
					return write_int < int64_t > (v, withend);
				} else if (v_u & M_16) {
					return write_int < int32_t > (static_cast<int32_t>(v), withend);
				} else if (v_u & M_8) {
					return write_int < int16_t > (static_cast<int16_t>(v), withend);
				} else {
					return write_int < int8_t > (static_cast<int8_t>(v), withend);
				}
			}

			template<typename T>
			void write_int(T v, bool withend = false) {
				auto size = sizeof(T);
				auto code = 0x88u + static_cast<uint8_t>(withend);

				switch (size) {
					case 2:
						code += 2;
						break;
					case 4:
						code += 4;
						break;
					case 8:
						code += 6;
					default:
						break;
				}

				put(code);
				for (auto i = 0u; i < size; ++i) {
					put(v & 0xFFu);
					v = v >> 8u;
				}
			}

			void write_uint64(uint64_t v, bool withend = false) {
				put(0xB2u + static_cast<uint8_t>(withend));
				for (auto i = 0u; i < 8u; ++i) {
					put(v & 0xFFu);
					v = v >> 8u;
				}
			}

			template<typename T>
			void write_strvec(const T &buf, bool asText, bool withend) {
				size_t len = buf.size();
				uint8_t sl = len & 0xFFFF0000u ? 4u : len & 0xFFFFFF00u ? 2u : 0;
				uint8_t code = (asText ? 0x98u : 0x9Eu) + sl + static_cast<uint8_t>(withend);
				put(code);

				put(len & 0xFFu);

				if (sl) {
					auto tmp = len >> 8u;
					for (auto i = 1u; i < sl; ++i, tmp = tmp >> 8u) {
						put(tmp & 0xFFu);
					}
				}

				put(buf);
			}

			void write_string(const std::string &str, bool withend = false) {
				write_strvec(str, true, withend);
			}

			void write_vector(const std::vector<uint8_t> &buf, bool withend = false) {
				write_strvec(buf, false, withend);
			}

			template<size_t S>
			void write_vector(const std::array<uint8_t, S> &arr, bool withend = false) {
				std::vector<uint8_t> tmp;
				std::copy(std::cbegin(arr), std::cend(arr), std::back_inserter(tmp));
				write_strvec(tmp, false, withend);
			}

			void write_datetime(datetime_t tp, bool withend = false) {
				auto v = tp.time_since_epoch().count() / 100ull + ticks_between_epochs;

				v = v | 0x8000000000000000ull;

				put(0x96u + static_cast<uint8_t>(withend));
				for (auto i = 0u; i < 8u; ++i) {
					put(v & 0xFFu);
					v = v >> 8u;
				}
			}

			template<typename T>
			void write_hex(const T &arr, bool withend = false) {
				std::stringstream ss;

				ss << std::uppercase
				   << std::setfill('0')
				   << std::hex;

				for (auto p = std::cbegin(arr), e = std::cend(arr); p != e; ++p) {
					ss << std::setw(2) << static_cast<unsigned short>(*p);
				}

				write_strvec(ss.str(), true, withend);
			}
		};
	}

	template<typename TIt>
	void serialize(const NbfxElement& root, TIt out_iterator, bool sort_members = true) {
		NbfxWriter<TIt>	writer(out_iterator);
		writer.write(root, sort_members);
	}
}
