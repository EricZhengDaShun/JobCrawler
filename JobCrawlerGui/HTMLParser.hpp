#pragma once

#include <vector>
#include <string>


#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 4100 )
#pragma warning ( disable : 4132 )
#pragma warning ( disable : 4204 )
#pragma warning ( disable : 4245 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4305 )
#pragma warning ( disable : 4306 )
#pragma warning ( disable : 4456 )
#pragma warning ( disable : 4701 )
#pragma warning ( disable : 4702 )
#endif
#include "gumbo.h"
#pragma comment(lib, "gumbo.lib")
#ifdef _MSC_VER
#pragma warning ( pop )
#endif

class HTMLParser final
{
public:
    HTMLParser();
    ~HTMLParser();
    HTMLParser(const HTMLParser&) = delete;
    HTMLParser& operator=(const HTMLParser&) = delete;

    void setHTML(const std::string& html);

    std::vector<std::string> getAttrValue(const std::wstring& tagType, const std::wstring& attrName);
    std::vector<std::string> getInnerHTML(const std::wstring& tagType, const std::wstring& attrName, const std::wstring& attrValue);
    std::vector<std::string> getAttrValue(const std::wstring& tagType, const std::wstring& attrName, const std::wstring& attrValue, const std::wstring& targetAttrName);

private:
    std::vector<GumboNode*> filterNode(const std::wstring& tagType, const std::wstring& attrName, const std::wstring& attrValue);

private:
    GumboOutput* output;
};

