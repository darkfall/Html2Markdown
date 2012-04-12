//
//  Html2Markdown.h
//  Html2Markdown
//
//  Created by Robert Bu on 4/9/12.
//  Copyright (c) 2012 GameMaster Studio. All rights reserved.
//

#ifndef Html2Markdown_Html2Markdown_h
#define Html2Markdown_Html2Markdown_h

#include <string>
#include <cassert>
#include <vector>
#include <map>

struct Html {
    // tags that markdown supports
    enum Tag {
        Unknown = -1,
        
        H1  = 1,
        H2,
        H3,
        H4,
        H5,
        H6,
        
        IMG  = 10,
        
        TD = 20,
        TL,
        TR,
        
        P,
        
        OL,
        UL,
        LI,
        
        PRE,
        CODE,
        
        A,
        
        EM,
        
        STRONG,
        
        HTML,
        BODY,
        BLOCKQUOTE,
    };
    
    /* utilities */
    static Html::Tag StringToHtmlTag(const char* str, size_t length);
    static bool      IsSingleTagName(const char* name, size_t length);
};

class Html2Markdown {
public:
    static std::string Convert(const char* html, size_t length);
    
    class Exception {
    public:
        Exception(const char* mssg);
        
        const char* what() const;
        
    private:
        const char* mMssg;
    };
    
private:
    struct HtmlNode;
    typedef std::vector<HtmlNode*> HtmlNodeTree;
    
    static void ParseHtmlTree(HtmlNodeTree& tree);
    static std::string ConvertHtmlTree(HtmlNode* node);
    static std::string ConvertBlockQuote(HtmlNode* node);
};

#endif
