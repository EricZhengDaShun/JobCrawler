#pragma once

#include <string>
#include <fstream>
#include <iterator>
#include <exception>
#include <algorithm> 
#include <type_traits>

#include "Convert.hpp"

namespace FileHelper {

    class FileHelperException : public std::exception
    {
    public:
        explicit FileHelperException(const std::string& msg)
            : message(msg)
        {

        }

        virtual const char* what() const
    #ifdef _GLIBCXX_NOTHROW
        _GLIBCXX_NOTHROW
    #endif
        {
            return message.c_str();
        }

    private:
        std::string message;
    };


    template<typename T = std::string>
    inline T readAll(const std::string& fileName)
    {
        std::ifstream in(fileName, std::ifstream::in | std::ifstream::binary);
        std::noskipws(in);
        if (!in.is_open()) {
            const std::string message{ "open file: " + fileName + " fail !" };
            FileHelperException e{message};
            throw e;
        }

        T direct;
        const std::string data((std::istream_iterator<char>(in)), std::istream_iterator<char>());
        if (std::is_same<typename T::value_type, char>::value || std::is_same<typename T::value_type, unsigned char>::value) {
            direct.assign(data.cbegin(), data.cend());

        }  else if (std::is_same<typename T::value_type, wchar_t>::value) {
            const std::wstring dataWstr = Convert::utf8ToWchar(data);
            direct.assign(dataWstr.cbegin(), dataWstr.cend());
        
        } else {
            FileHelperException e{ "readAll can't detect type" };
            throw e;
        }

        return direct;
    }



    template<typename T = std::string>
    inline void write(const std::string& fileName, std::ofstream::openmode mode, const T& source)
    {
        std::ofstream out(fileName, mode);
        std::noskipws(out);
        if (!out.is_open()) {
            const std::string message{ "open file: " + fileName + " fail !" };
            FileHelperException e{message};
            throw e;
        }

        std::string data;
        if (std::is_same<typename T::value_type, char>::value || std::is_same<typename T::value_type, unsigned char>::value) {
            data.assign(source.cbegin(), source.cend());

        } else if (std::is_same<typename T::value_type, wchar_t>::value) {
            std::wstring dataWstr(source.cbegin(), source.cend());
            data = Convert::wcharToUtf8(dataWstr);

        } else {
            FileHelperException e{ "write can't detect type" };
            throw e;
        }

        std::ostream_iterator<char> outIt(out);
        std::copy(data.begin(), data.end(), outIt);
        out.flush();
        return;
    }

    template<typename T = std::string>
    inline void writeAppend(const std::string& fileName, const T& source)
    {
        std::ofstream::openmode mode{ std::ofstream::out | std::ofstream::app | std::ofstream::binary };
        write(fileName, mode, source);
        return;
    }

    template<typename T = std::string>
    inline void writeAppend(const std::wstring& fileName, const T& source)
    {
        const std::string fileNameStr = Convert::wcharToUtf8(fileName);
        writeAppend(fileNameStr, source);
        return;
    }

    inline void writeAppend(const std::string& fileName, const char* source)
    {
        writeAppend(fileName, std::string(source));
        return;
    }

    inline void writeAppend(const std::string& fileName, const wchar_t* source)
    {
        writeAppend(fileName, std::wstring(source));
        return;
    }


    template<typename T = std::string>
    inline void overwrite(const std::string& fileName, const T& source)
    {
        std::ofstream::openmode mode{ std::ofstream::out | std::ofstream::trunc | std::ofstream::binary };
        write(fileName, mode, source);
        return;
    }

    inline void overwrite(const std::string& fileName, const char* source)
    {
        overwrite(fileName, std::string(source));
        return;
    }

    inline void overwrite(const std::string& fileName, const wchar_t* source)
    {
        overwrite(fileName, std::wstring(source));
        return;
    }

    template<typename T = std::string>
    inline void overwrite(const std::wstring& fileName, const T& source)
    {
        const std::string fileNameStr = Convert::wcharToUtf8(fileName);
        overwrite(fileNameStr, source);
        return;
    }

    template<typename T = std::string>
    inline T readAll(const std::wstring& fileName)
    {
        const std::string fileNameStr = Convert::wcharToUtf8(fileName);
        return readAll<T>(fileNameStr);
    }

}
