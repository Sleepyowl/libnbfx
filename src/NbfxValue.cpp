#include "nbfx/NbfxValue.hpp"
#include <codecvt>
#include <locale>

namespace nbfx
{
	namespace {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> utf_to_wstring;
	}

	std::wstring NbfxValue::to_string() const
	{
		switch (m_type)
		{
		case NbfxValueType::String:
			return m_string;
		case NbfxValueType::Integer:
			return std::to_wstring(m_value.integer);
		case NbfxValueType::UInt64:
			return std::to_wstring(m_value.uint64);
		case NbfxValueType::Float:
		{
			std::wostringstream ss;
			ss << m_value.float_single;
			return ss.str();
		}
		case NbfxValueType::Double:
		{
			std::wostringstream ss;
			ss << m_value.float_double;
			return ss.str();
		}
		case NbfxValueType::DateTime:
		{
			// TODO: format according to RFC 3339
			using namespace std::chrono;
			auto tt = system_clock::to_time_t(time_point_cast<microseconds>(datetime()));
			const char* str = ctime(&tt);
			return str ? utf_to_wstring.from_bytes(str) : L"(error)";
		}
		case NbfxValueType::Boolean:
			return m_value.boolean ? L"true" : L"false";
		default:
			break;
		}
		return L"(error)";
	}
}
