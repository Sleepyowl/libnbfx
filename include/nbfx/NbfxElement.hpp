#include <utility>

#pragma once

#include "NbfxRecord.hpp"
#include "NbfxAttribute.hpp"
#include "NbfxValue.hpp"

#include <memory>
#include <algorithm>
#include <stack>
#include <queue>

namespace nbfx {

    namespace {
        /**
         * Selects matching element type based on namespace prefix
         */
        inline NbfxRecordType inferElementType(const std::wstring &prefix) {
            if (const auto s = prefix.size()) {
                const auto c0 = prefix[0];
                if (s == 1 && c0 >= L'a' && c0 <= L'z') {
                    return static_cast<NbfxRecordType>(
                            static_cast<uint8_t>(NbfxRecordType::PrefixElementA)
                            + static_cast<uint8_t>(prefix[0]) - 'a');
                }
                return NbfxRecordType::Element;
            }
            return NbfxRecordType::ShortElement;
        }
    }

    class NbfxElement : public NamedNbfxRecord {
    public:
        NbfxElement(NbfxElement&&) noexcept = default;

        NbfxElement(const NbfxElement&) = default;

        NbfxElement(NbfxRecordType type, QName name) : NamedNbfxRecord(type, std::move(name)) {}

        explicit NbfxElement(QName name) : NbfxElement(inferElementType(name.prefix()), std::move(name)) {}

        NbfxElement(const QName &name, std::initializer_list<NbfxAttribute> attributes, NbfxValue value) :
                NamedNbfxRecord(inferElementType(name.prefix()), name), m_attributes(attributes),
                m_value(std::move(value)) {}

        NbfxElement(const QName &name,
                    std::initializer_list<NbfxAttribute> attributes,
                    std::initializer_list<NbfxElement> children):
                NamedNbfxRecord(inferElementType(name.prefix()), name),
                m_attributes(attributes),
                m_children(children) {
        }


        std::vector<NbfxAttribute> &attributes() noexcept {
            return m_attributes;
        }

        const std::vector<NbfxAttribute> &attributes() const noexcept {
            return m_attributes;
        }

        std::vector<NbfxElement> &children() noexcept {
            return m_children;
        }

        const std::vector<NbfxElement> &children() const noexcept {
            return m_children;
        }

        NbfxValue &value() noexcept {
            return m_value;
        }

        const NbfxValue &value() const noexcept {
            return m_value;
        }

        /**
         * Returns first descendant with matching name or nullptr if none found
         *
         * Search is performed breath-first
         */
        const NbfxElement *find_descendant(const QName &qname) const {
            std::queue<std::reference_wrapper<const NbfxElement>> q1;
            q1.emplace(*this);

            while (!q1.empty()) {
                const auto &current = q1.front().get();
                q1.pop();

                if (current.qname() == qname) {
                    return &current;
                }

                for(const auto& ch: current.children()) {
                    q1.emplace(ch);
                }
            }

            return nullptr;
        }

        /**
         * Returns first child with matching name or nullptr if none found
         */
        const NbfxElement *first_child(const QName &qname) const {
            const auto it = std::find_if(begin(m_children), end(m_children),
                                         [qname](const auto& child){return child.qname() == qname;});

            return it == end(m_children) ? nullptr : &(*it);
        }

    private:
        std::vector<NbfxAttribute> m_attributes;
        std::vector<NbfxElement> m_children;
        NbfxValue m_value;
    };
}