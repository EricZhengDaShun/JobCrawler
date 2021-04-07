#pragma once

#include <string>
#include <fstream>
#include <iterator>
#include <exception>
#include <algorithm> 
#include <type_traits>

#include "Convert.hpp"

namespace FileHelper {
    
    template<typename RT>
    inline RT readAll(char* type, const std::string& source) 
    {
        const RT data(source.cbegin(), source.cend());
        return data;
    }

    template<typename RT>
    inline RT readAll(unsigned char* type, const std::string& source)
    {
        const RT data(source.cbegin(), source.cend());
        return data;
    }

    template<typename RT>
    inline RT readAll(wchar_t* type, const std::string& source)
    {
        const RT data = Convert::utf8ToWchar<std::string, RT>(source);
        return data;
    }

    template<typename RT = std::string>
    inline RT readAll(const std::string& fileName)
    {
        std::ifstream in(fileName, std::ifstream::in | std::ifstream::binary);
        std::noskipws(in);
        if (!in.is_open()) {
            const std::string message{ "open file: " + fileName + " fail !" };
            throw std::exception(message.c_str());
        }

        const std::string data((std::istream_iterator<char>(in)), std::istream_iterator<char>());
        typename RT::value_type* type = nullptr;
        const RT direct = readAll<RT>(type, data);
        return direct;
    }

    template<typename RT = std::string>
    inline RT readAll(const std::wstring& fileName)
    {
        const std::string fileNameStr = Convert::wcharToUtf8(fileName);
        return readAll<RT>(fileNameStr);
    }


    template<typename T>
    inline std::string convertToUtf8Data(char* type, const T& source) {
        const std::string data(source.cbegin(), source.cend());
        return data;
    }

    template<typename T>
    inline std::string convertToUtf8Data(unsigned char* type, const T& source) {
        const std::string data(source.cbegin(), source.cend());
        return data;
    }

    template<typename T>
    inline std::string convertToUtf8Data(wchar_t* type, const T& source) {
        const std::string data = Convert::wcharToUtf8(source);
        return data;
    }

    template<typename T = std::string>
    inline void write(const std::string& fileName, std::ofstream::openmode mode, const T& source)
    {
        std::ofstream out(fileName, mode);
        std::noskipws(out);
        if (!out.is_open()) {
            const std::string message{ "open file: " + fileName + " fail !" };
            throw std::exception(message.c_str());
        }

        typename T::value_type* type = nullptr;
        const std::string data = convertToUtf8Data(type, source);

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

}