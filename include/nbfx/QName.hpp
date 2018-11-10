#pragma once

#include <string>

namespace nbfx {

    /**
     * Qualified name
     */
    class QName final {
    public:
        /* implicit */ QName(std::wstring::const_pointer name):m_prefix(), m_name(name){}
        /* implicit */ QName(std::wstring name):m_prefix(), m_name(std::move(name)){}
        QName(std::wstring prefix, std::wstring name):m_prefix(std::move(prefix)), m_name(std::move(name)){}

        const std::wstring& prefix() const noexcept { return m_prefix; }
        const std::wstring& name() const noexcept { return m_name; }
        bool is_local() const noexcept { return m_prefix.empty(); }

        bool operator == (const QName& b) const noexcept {
            return m_prefix == b.m_prefix && m_name == b.m_name;
        }

        bool operator != (const QName& b) const noexcept {
            return !(*this == b);
        }

    private:
        std::wstring    m_prefix;
        std::wstring    m_name;
    };

}

template<>
struct std::hash<nbfx::QName> {
    size_t operator()(const nbfx::QName& name) const noexcept {
        std::hash<std::wstring> str_hash;
        return str_hash(name.name());
    }
};
