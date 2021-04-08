#include "HTMLParser.hpp"
#include "Convert.hpp"

#pragma warning ( disable : 26812 )

namespace {
    void findAllNode(
        GumboNode* node,
        std::vector<GumboNode*>& nodes,
        const GumboTag& tagType,
        const std::string& attrName,
        const std::string& attrValue)
    {
        if (node->type != GUMBO_NODE_ELEMENT) return;

        if (node->v.element.tag == tagType) {
            GumboAttribute* attr = ::gumbo_get_attribute(&node->v.element.attributes, attrName.c_str());
            if (attr != nullptr) {
                if (std::string(attr->value) == attrValue) {
                    nodes.push_back(node);

                } else if (attrName.empty()) {
                    nodes.push_back(node);

                } else if (attrValue.empty()) {
                    nodes.push_back(node);
                }

            } else if (attrName.empty()) {
                nodes.push_back(node);
            }
        }

        GumboVector* children = &node->v.element.children;
        for (size_t count = 0; count < children->length; ++count) {
            ::findAllNode(static_cast<GumboNode*>(children->data[count]), nodes, tagType, attrName, attrValue);
        }
        return;
    }

    std::vector<std::string> getAttrValue(GumboNode* node, const GumboTag& tagType, const std::string& attrName)
    {
        std::vector<std::string> result;
        if (node->type != GUMBO_NODE_ELEMENT) return result;
        if (node->v.element.tag == tagType) {
            GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, attrName.c_str());
            if (attr != nullptr) result.push_back(attr->value);
        }

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            auto v = getAttrValue(static_cast<GumboNode*>(children->data[i]), tagType, attrName);
            result.insert(result.cend(), v.cbegin(), v.cend());
        }
        return result;
    }

    std::string getNodeContent(GumboNode* node)
    {
        std::string result;
        if (node->type != GUMBO_NODE_ELEMENT) return result;
        if (node->v.element.children.length == 0) return result;

        GumboNode* text = static_cast<GumboNode*>(node->v.element.children.data[0]);

        if (text->type == GUMBO_NODE_TEXT) {
            result.assign(text->v.text.text);
        }
        return result;
    }

    GumboTag wstringToGumboTag(const std::wstring& tagName)
    {
        GumboTag tag = GUMBO_TAG_UNKNOWN;
        if (tagName == L"div") {
            tag = GUMBO_TAG_DIV;

        } else if (tagName == L"main") {
            tag = GUMBO_TAG_MAIN;

        } else if (tagName == L"section") {
            tag = GUMBO_TAG_SECTION;

        } else if (tagName == L"dd") {
            tag = GUMBO_TAG_DD;

        } else if (tagName == L"a") {
            tag = GUMBO_TAG_A;

        } else if (tagName == L"h2") {
            tag = GUMBO_TAG_H2;

        } else if (tagName == L"u") {
            tag = GUMBO_TAG_U;

        } else if (tagName == L"h1") {
            tag = GUMBO_TAG_H1;

        } else if (tagName == L"p") {
            tag = GUMBO_TAG_P;

        }

        return tag;
    }

}

HTMLParser::HTMLParser()
    : output(nullptr)
{

}

HTMLParser::~HTMLParser()
{
    if (output) {
        ::gumbo_destroy_output(&kGumboDefaultOptions, output);
    }
}

void HTMLParser::setHTML(const std::string& html)
{
    if (output) {
        ::gumbo_destroy_output(&kGumboDefaultOptions, output);
    }
    output = ::gumbo_parse(html.c_str());

    return;
}

std::vector<std::string> HTMLParser::getAttrValue(const std::wstring& tagType, const std::wstring& attrName)
{
    std::vector<GumboNode*> nodeBuf = filterNode(tagType, attrName, L"");
    std::vector<std::string> result;
    for (const auto& node : nodeBuf) {
        auto v = ::getAttrValue(node, ::wstringToGumboTag(tagType), Convert::wcharToUtf8(attrName));
        result.insert(result.end(), v.cbegin(), v.cend());
    }
    return result;
}

std::vector<std::string> HTMLParser::getInnerHTML(const std::wstring& tagType, const std::wstring& attrName, const std::wstring& attrValue)
{
    std::vector<GumboNode*> nodeBuf = filterNode(tagType, attrName, attrValue);
    std::vector<std::string> result;
    for (auto& node : nodeBuf) {
        auto v = ::getNodeContent(node);
        result.push_back(v);
    }
    return result;
}

std::vector<std::string> HTMLParser::getAttrValue(const std::wstring& tagType, const std::wstring& attrName, const std::wstring& attrValue, const std::wstring& targetAttrName)
{
    std::vector<GumboNode*> nodeBuf = filterNode(tagType, attrName, attrValue);
    std::vector<std::string> result;
    for (auto& node : nodeBuf) {
        auto v = ::getAttrValue(node, ::wstringToGumboTag(tagType), Convert::wcharToUtf8(targetAttrName));
        result.insert(result.end(), v.cbegin(), v.cend());
    }
    return result;
}

std::vector<GumboNode*> HTMLParser::filterNode(const std::wstring& tagType, const std::wstring& attrName, const std::wstring& attrValue)
{
    std::vector<GumboNode*> nodesBuf;
    if (output == nullptr) return nodesBuf;
    ::findAllNode(output->root, nodesBuf, ::wstringToGumboTag(tagType), Convert::wcharToUtf8(attrName), Convert::wcharToUtf8(attrValue));
    return nodesBuf;
}
