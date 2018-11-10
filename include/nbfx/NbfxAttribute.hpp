#include <utility>

#pragma once

#include "QName.hpp"

#include "NbfxRecord.hpp"
#include "NbfxValue.hpp"

namespace nbfx {

    namespace {
        NbfxRecordType inferAttributeType(const QName &qname) {
            const auto s = qname.prefix().size();

            if (qname.name() == L"xmlns") {
                return s ? NbfxRecordType::XmlnsAttribute : NbfxRecordType::ShortXmlnsAttribute;
            }

            if (s) {
                const auto c0 = *qname.prefix().begin();

                if (s == 1 && c0 >= L'a' && c0 <= L'z') {
                    return static_cast<NbfxRecordType>(
                            static_cast<uint8_t>(NbfxRecordType::PrefixAttributeA)
                            + static_cast<uint8_t>(c0) - 'a');
                }
                return NbfxRecordType::Attribute;
            }

            return NbfxRecordType::ShortAttribute;
        }
    }

    class NbfxAttribute : public NamedNbfxRecord {
    public:
        NbfxAttribute(NbfxRecordType type, const QName &name, NbfxValue value) :
                NamedNbfxRecord(type, name),
                m_value(std::move(value)) {}

        NbfxAttribute(const QName &name, const NbfxValue &value) :
                NbfxAttribute(inferAttributeType(name), name, value) {}


        NbfxValue value() const {
            return m_value;
        }

    private:
        NbfxValue m_value;
    };
}