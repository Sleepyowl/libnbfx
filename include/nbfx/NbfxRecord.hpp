#pragma once

#include "QName.hpp"

#include <cstdint>
#include <string>
#include <chrono>

namespace nbfx {

	enum struct NbfxRecordType : uint8_t;

	using datetime_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

	/**
	 * NBFX Record
	 */
	class NbfxRecord {
	public:
		NbfxRecordType type() const noexcept { return m_type; }

	protected:
		explicit NbfxRecord(NbfxRecordType type) noexcept : m_type(type) {}
		virtual ~NbfxRecord() = default;

	private:
		NbfxRecordType m_type;
	};

	/**
	 * Named NBFX record (element or attribute)
	 */
	class NamedNbfxRecord : public NbfxRecord {
	public:
		const std::wstring& prefix() const noexcept {
			return m_name.prefix();
		}

        const std::wstring& name() const noexcept {
			return m_name.name();
		}

		const QName& qname() const noexcept {
		    return m_name;
		}

	protected:
		NamedNbfxRecord(NamedNbfxRecord&&) noexcept = default;
		NamedNbfxRecord(const NamedNbfxRecord&) = default;
		NamedNbfxRecord(NbfxRecordType type, QName name): NbfxRecord(type), m_name(std::move(name)) {}
		~NamedNbfxRecord() override = default;

	private:
		QName   m_name;
	};

	/**
	 * NBFX record type. Values match binary representation.
	 */
	enum struct NbfxRecordType : uint8_t {
		EndElement = 0x01,
		Comment = 0x02,
		Array = 0x03,
		ShortAttribute = 0x04,
		Attribute = 0x05,
		ShortDictionaryAttribute = 0x06,
		DictionaryAttribute = 0x07,
		ShortXmlnsAttribute = 0x08,
		XmlnsAttribute = 0x09,
		ShortDictionaryXmlnsAttribute = 0x0A,
		DictionaryXmlnsAttribute = 0x0B,
		PrefixDictionaryAttributeA = 0x0C,
		PrefixDictionaryAttributeB = 0x0D,
		PrefixDictionaryAttributeC = 0x0E,
		PrefixDictionaryAttributeD = 0x0F,
		PrefixDictionaryAttributeE = 0x10,
		PrefixDictionaryAttributeF = 0x11,
		PrefixDictionaryAttributeG = 0x12,
		PrefixDictionaryAttributeH = 0x13,
		PrefixDictionaryAttributeI = 0x14,
		PrefixDictionaryAttributeJ = 0x15,
		PrefixDictionaryAttributeK = 0x16,
		PrefixDictionaryAttributeL = 0x17,
		PrefixDictionaryAttributeM = 0x18,
		PrefixDictionaryAttributeN = 0x19,
		PrefixDictionaryAttributeO = 0x1A,
		PrefixDictionaryAttributeP = 0x1B,
		PrefixDictionaryAttributeQ = 0x1C,
		PrefixDictionaryAttributeR = 0x1D,
		PrefixDictionaryAttributeS = 0x1E,
		PrefixDictionaryAttributeT = 0x1F,
		PrefixDictionaryAttributeU = 0x20,
		PrefixDictionaryAttributeV = 0x21,
		PrefixDictionaryAttributeW = 0x22,
		PrefixDictionaryAttributeX = 0x23,
		PrefixDictionaryAttributeY = 0x24,
		PrefixDictionaryAttributeZ = 0x25,
		PrefixAttributeA = 0x26,
		PrefixAttributeB = 0x27,
		PrefixAttributeC = 0x28,
		PrefixAttributeD = 0x29,
		PrefixAttributeE = 0x2A,
		PrefixAttributeF = 0x2B,
		PrefixAttributeG = 0x2C,
		PrefixAttributeH = 0x2D,
		PrefixAttributeI = 0x2E,
		PrefixAttributeJ = 0x2F,
		PrefixAttributeK = 0x30,
		PrefixAttributeL = 0x31,
		PrefixAttributeM = 0x32,
		PrefixAttributeN = 0x33,
		PrefixAttributeO = 0x34,
		PrefixAttributeP = 0x35,
		PrefixAttributeQ = 0x36,
		PrefixAttributeR = 0x37,
		PrefixAttributeS = 0x38,
		PrefixAttributeT = 0x39,
		PrefixAttributeU = 0x3A,
		PrefixAttributeV = 0x3B,
		PrefixAttributeW = 0x3C,
		PrefixAttributeX = 0x3D,
		PrefixAttributeY = 0x3E,
		PrefixAttributeZ = 0x3F,
		ShortElement = 0x40,
		Element = 0x41,
		ShortDictionaryElement = 0x42,
		DictionaryElement = 0x43,
		PrefixDictionaryElementA = 0x44,
		PrefixDictionaryElementB = 0x45,
		PrefixDictionaryElementC = 0x46,
		PrefixDictionaryElementD = 0x47,
		PrefixDictionaryElementE = 0x48,
		PrefixDictionaryElementF = 0x49,
		PrefixDictionaryElementG = 0x4A,
		PrefixDictionaryElementH = 0x4B,
		PrefixDictionaryElementI = 0x4C,
		PrefixDictionaryElementJ = 0x4D,
		PrefixDictionaryElementK = 0x4E,
		PrefixDictionaryElementL = 0x4F,
		PrefixDictionaryElementM = 0x50,
		PrefixDictionaryElementN = 0x51,
		PrefixDictionaryElementO = 0x52,
		PrefixDictionaryElementP = 0x53,
		PrefixDictionaryElementQ = 0x54,
		PrefixDictionaryElementR = 0x55,
		PrefixDictionaryElementS = 0x56,
		PrefixDictionaryElementT = 0x57,
		PrefixDictionaryElementU = 0x58,
		PrefixDictionaryElementV = 0x59,
		PrefixDictionaryElementW = 0x5A,
		PrefixDictionaryElementX = 0x5B,
		PrefixDictionaryElementY = 0x5C,
		PrefixDictionaryElementZ = 0x5D,
		PrefixElementA = 0x5E,
		PrefixElementB = 0x5F,
		PrefixElementC = 0x60,
		PrefixElementD = 0x61,
		PrefixElementE = 0x62,
		PrefixElementF = 0x63,
		PrefixElementG = 0x64,
		PrefixElementH = 0x65,
		PrefixElementI = 0x66,
		PrefixElementJ = 0x67,
		PrefixElementK = 0x68,
		PrefixElementL = 0x69,
		PrefixElementM = 0x6A,
		PrefixElementN = 0x6B,
		PrefixElementO = 0x6C,
		PrefixElementP = 0x6D,
		PrefixElementQ = 0x6E,
		PrefixElementR = 0x6F,
		PrefixElementS = 0x70,
		PrefixElementT = 0x71,
		PrefixElementU = 0x72,
		PrefixElementV = 0x73,
		PrefixElementW = 0x74,
		PrefixElementX = 0x75,
		PrefixElementY = 0x76,
		PrefixElementZ = 0x77,
		ZeroText = 0x80,
		ZeroTextWithEndElement = 0x81,
		OneText = 0x82,
		OneTextWithEndElement = 0x83,
		FalseText = 0x84,
		FalseTextWithEndElement = 0x85,
		TrueText = 0x86,
		TrueTextWithEndElement = 0x87,
		Int8Text = 0x88,
		Int8TextWithEndElement = 0x89,
		Int16Text = 0x8A,
		Int16TextWithEndElement = 0x8B,
		Int32Text = 0x8C,
		Int32TextWithEndElement = 0x8D,
		Int64Text = 0x8E,
		Int64TextWithEndElement = 0x8F,
		FloatText = 0x90,
		FloatTextWithEndElement = 0x91,
		DoubleText = 0x92,
		DoubleTextWithEndElement = 0x93,
		DecimalText = 0x94,
		DecimalTextWithEndElement = 0x95,
		DateTimeText = 0x96,
		DateTimeTextWithEndElement = 0x97,
		Chars8Text = 0x98,
		Chars8TextWithEndElement = 0x99,
		Chars16Text = 0x9A,
		Chars16TextWithEndElement = 0x9B,
		Chars32Text = 0x9C,
		Chars32TextWithEndElement = 0x9D,
		Bytes8Text = 0x9E,
		Bytes8TextWithEndElement = 0x9F,
		Bytes16Text = 0xA0,
		Bytes16TextWithEndElement = 0xA1,
		Bytes32Text = 0xA2,
		Bytes32TextWithEndElement = 0xA3,
		StartListText = 0xA4,
		EndListText = 0xA6,
		EmptyText = 0xA8,
		EmptyTextWithEndElement = 0xA9,
		DictionaryText = 0xAA,
		DictionaryTextWithEndElement = 0xAB,
		UniqueIdText = 0xAC,
		UniqueIdTextWithEndElement = 0xAD,
		TimeSpanText = 0xAE,
		TimeSpanTextWithEndElement = 0xAF,
		UuidText = 0xB0,
		UuidTextWithEndElement = 0xB1,
		UInt64Text = 0xB2,
		UInt64TextWithEndElement = 0xB3,
		BoolText = 0xB4,
		BoolTextWithEndElement = 0xB5,
		UnicodeChars8Text = 0xB6,
		UnicodeChars8TextWithEndElement = 0xB7,
		UnicodeChars16Text = 0xB8,
		UnicodeChars16TextWithEndElement = 0xB9,
		UnicodeChars32Text = 0xBA,
		UnicodeChars32TextWithEndElement = 0xBB,
		QNameDictionaryText = 0xBC,
		QNameDictionaryTextWithEndElement = 0xBD,
	};

	/**
	 * Checks if the record type is element
	 */
	constexpr bool IsElement(NbfxRecordType type) noexcept {
		return type >= NbfxRecordType::ShortElement && type <= NbfxRecordType::PrefixElementZ;
	}

    /**
     * Checks if the record type is attribute
     */
	constexpr bool IsAttribute(NbfxRecordType type) noexcept {
		return type >= NbfxRecordType::ShortAttribute && type <= NbfxRecordType::PrefixAttributeZ;
	}

    /**
     * Checks if the record type is a text record (used for names etc)
     */
	constexpr bool IsTextRecord(NbfxRecordType type) noexcept {
		return type >= NbfxRecordType::ZeroText && type <= NbfxRecordType::QNameDictionaryTextWithEndElement;
	}
}