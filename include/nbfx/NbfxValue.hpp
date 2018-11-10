#include <utility>

#pragma once

#include "NbfxRecord.hpp"

#include <vector>
#include <cassert>
#include <sstream>
#include <iterator>
#include <iostream>
#include <array>

namespace nbfx {
	enum struct NbfxValueType {
		Null,
		Boolean,
		Integer,
		UInt64,
		Float,
		Double,
		Decimal,
		DateTime,
		String,
		Bytes
	};

	class NbfxValue
	{
	public:

		NbfxValue(NbfxValue&&) noexcept = default;
		NbfxValue(const NbfxValue&) = default;

		NbfxValue&	operator = (const NbfxValue& rho) {
			m_type = rho.m_type;
			m_value = rho.m_value;
			m_bytes = rho.m_bytes;
			m_string = rho.m_string;
		}

		NbfxValue&	operator = (NbfxValue&& rho) noexcept {
			m_type = rho.m_type;
			m_value = rho.m_value;
			std::swap(m_bytes, rho.m_bytes);
			std::swap(m_string, rho.m_string);
		}


		NbfxValue() :m_type(NbfxValueType::Null) {}
		explicit NbfxValue(bool val) : m_type(NbfxValueType::Boolean)
		{
			m_value.boolean = val;
		}

		explicit NbfxValue(int8_t val) : m_type(NbfxValueType::Integer)
		{
			m_value.integer = val;
		}

		explicit NbfxValue(int16_t val) : m_type(NbfxValueType::Integer)
		{
			m_value.integer = val;
		}

		explicit NbfxValue(int32_t val) : m_type(NbfxValueType::Integer)
		{
			m_value.integer = val;
		}

		explicit NbfxValue(uint8_t val) : m_type(NbfxValueType::Integer)
		{
			m_value.integer = val;
		}

		explicit NbfxValue(uint16_t val) : m_type(NbfxValueType::Integer)
		{
			m_value.integer = val;
		}

		explicit NbfxValue(uint32_t val) : m_type(NbfxValueType::Integer)
		{
			m_value.integer = val;
		}

		explicit NbfxValue(int64_t val) : m_type(NbfxValueType::Integer)
		{
			m_value.integer = val;
		}

		explicit NbfxValue(uint64_t val) : m_type(NbfxValueType::UInt64)
		{
			m_value.uint64 = val;
		}

		explicit NbfxValue(float val) : m_type(NbfxValueType::Float)
		{
			m_value.float_single = val;
		}

		explicit NbfxValue(double val) : m_type(NbfxValueType::Float)
		{
			m_value.float_double = val;
		}
		
		template<typename Duration>
		explicit NbfxValue(std::chrono::time_point<std::chrono::system_clock, Duration> val) :
			m_type(NbfxValueType::DateTime)
		{
			using namespace std::chrono;
			m_value.nanoseconds_since_epoch = duration_cast<nanoseconds>(val.time_since_epoch()).count();
		}

		/* implicit */ NbfxValue(std::wstring val) :m_type(NbfxValueType::String), m_string(std::move(val)) {}
		/* implicit */ NbfxValue(const wchar_t* val) :m_type(NbfxValueType::String), m_string(val) {}

		/* implicit */ NbfxValue(std::vector<uint8_t> val) :m_type(NbfxValueType::Bytes), m_bytes(std::move(val)) {}
		/* implicit */ NbfxValue(const std::vector<char>& val) :m_type(NbfxValueType::Bytes), m_bytes(val.data(), val.data()+val.size()) {}

		template<size_t S>
		explicit NbfxValue(const std::array<uint8_t, S>& a) :m_type(NbfxValueType::Bytes), m_bytes()
		{
			std::copy(std::cbegin(a), std::cend(a), std::back_inserter(m_bytes));
		}

		// value accessors
		NbfxValueType type() const noexcept { return m_type; }

		int64_t integer() const {
			if (m_type != NbfxValueType::Integer)
			{
				throw std::runtime_error("Invalid value type, expected Integer");
			}
			return m_value.integer;
		}

		uint64_t uint64() const {
			if (m_type != NbfxValueType::UInt64)
			{
				throw std::runtime_error("Invalid value type, expected UInt64");
			}
			return m_value.uint64;
		}

		bool boolean() const {
			if (m_type != NbfxValueType::Boolean)
			{
				throw std::runtime_error("Invalid value type, expected Boolean");
			}
			return m_value.boolean;
		}

		float float_single() const {
			if (m_type != NbfxValueType::Float)
			{
				throw std::runtime_error("Invalid value type, expected Float");
			}
			return m_value.float_single;
		}

		double float_double() const {
			if (m_type != NbfxValueType::Double)
			{
				throw std::runtime_error("Invalid value type, expected Double");
			}
			return m_value.float_double;
		}

		const std::vector<uint8_t>& bytes() const
		{
			if (m_type != NbfxValueType::Bytes)
			{
				throw std::runtime_error("Invalid value type, expected Bytes");
			}
			return m_bytes;
		}

		std::vector<uint8_t>& bytes()
		{
			if (m_type != NbfxValueType::Bytes)
			{
				throw std::runtime_error("Invalid value type, expected Bytes");
			}
			return m_bytes;
		}

		const std::wstring& string() const
		{
			if (m_type != NbfxValueType::String)
			{
				std::cerr << "Type is" << static_cast<int>(m_type) << std::endl;
				throw std::runtime_error("Invalid value type, expected String");
			}
			return m_string;
		}

		datetime_t datetime() const
		{
			if (m_type != NbfxValueType::DateTime)
			{
				throw std::runtime_error("Invalid value type, expected DateTime");
			}
			auto dur = datetime_t::duration(m_value.nanoseconds_since_epoch);
			return datetime_t(dur);
		}

		std::wstring to_string() const;

	private:
		union value_t
		{
			uint64_t nanoseconds_since_epoch;
			bool boolean;
			int64_t integer;
			uint64_t uint64;
			float float_single;
			double float_double;
		};

		NbfxValueType m_type;
		value_t m_value;
		std::vector<uint8_t>	m_bytes;
		std::wstring m_string;
	};
}
