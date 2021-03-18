#pragma once

#include <string>
#include <locale>
#include <exception>

namespace Convert {

    class ConvertLocaleException : public std::exception
    {
    public:
        virtual const char* what() const
#ifdef _GLIBCXX_NOTHROW
        _GLIBCXX_NOTHROW
#endif
        {
            return "Convert must be set locale";
        }
    };



    template<typename T = std::wstring, typename U = std::string>
    inline U wcharToUtf8(const T& source) {
        if (std::locale().name() == "C") {
            ConvertLocaleException e;
            throw e;
        }

        auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());

        const size_t maxSize = source.size() * facet.max_length();
        U direct(maxSize, '\0');

        const wchar_t* fromTemp = nullptr;
        char* toTemp = nullptr;
        std::mbstate_t mbstate{};

        facet.out(mbstate,
            source.data(), source.data() + source.size(), fromTemp,
            direct.data(), direct.data() + direct.size(), toTemp);

        direct.resize(toTemp - direct.data());
        return direct;
    }

    template<typename T = std::string, typename U = std::wstring>
    inline U utf8ToWchar(const T& source) {
        if (std::locale().name() == "C") {
            ConvertLocaleException e;
            throw e;
        }

        auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());

        U direct(source.size(), '\0');
        const char* fromTemp = nullptr;
        wchar_t* toTemp = nullptr;
        std::mbstate_t mbstate{};

        facet.in(mbstate,
            source.data(), source.data() + source.size(), fromTemp,
            direct.data(), direct.data() + direct.size(), toTemp);

        direct.resize(toTemp - direct.data());
        return direct;
    }
}
