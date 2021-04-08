#include <iostream>
#include <string>
#include <locale>
#include <vector>

#include "HTMLParser.hpp"

int main()
{
    std::locale::global(std::locale("zh_TW.UTF-8"));
    std::ios::sync_with_stdio(false);

    const std::string html = R"(<!DOCTYPE html>
<html>
<body>

<h2 title="I'm a header" dd123 cc="666">The title Attribute</h2>

<p title="I'm a tooltip">Mouse over this paragraph, to display the title attribute as a tooltip.</p>

</body>
</html>)";

    HTMLParser htmlParser;
    htmlParser.setHTML(html);
    const std::vector<std::string> pTitleData = htmlParser.getAttrValue(L"p", L"title");
    const std::vector<std::string> h2InnerData = htmlParser.getInnerHTML(L"h2", L"dd123", L"");
    const std::vector<std::string> targetData = htmlParser.getAttrValue(L"h2", L"dd123", L"", L"cc");
    return 0;
}