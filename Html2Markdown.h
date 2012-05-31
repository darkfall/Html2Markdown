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
        
        IMG,
        
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
        
        /* markdown extra tags */
        /* table */
        TABLE,
        THEAD,
        TBODY,
        TR,
        TH,
        TD,
        
        /* definition lists */
        DD,
        DL,
        DT,
        
        /* footnote */
        SUP,
        /* abbreviation */
        ABBR,
    };
    
    /* utilities */
    static Html::Tag StringToHtmlTag(const char* str, size_t length);
    static bool      IsSingleTagName(const char* name, size_t length);
};

class Html2Markdown {
public:    
    struct Configuration {
        enum HeaderStyle { 
            setext, 
            atx
        };
        
        enum CodeblockStyle {
            Indented, /* original markdown syntax */
            Fenced, /* introduced in markdown extra */
        };
        
        char listmark;
        char emmark;
        char strongmark;
        int  headerstyle;
        int  codeblockstype;
        /* enable markdown extra or not */
        bool enableextra;
        
        
        Configuration& setListMark(char lm) { this->listmark = lm; return *this; }
        Configuration& setEmMark(char em) { this->emmark = em; return *this; }
        Configuration& setStrongMark(char sm) { this->strongmark = sm; return *this; }
        Configuration& setHeaderStyle(Html2Markdown::Configuration::HeaderStyle hs) { this->headerstyle = hs; return *this; }
        Configuration& setCodeblockStyle(Html2Markdown::Configuration::CodeblockStyle cs) { this->codeblockstype = cs; return *this; }
        Configuration& setEnableExtra(bool flag) { this->enableextra = flag; return *this; }
        
        
        Configuration(char lm, char em, char sm, int hs, int cb, bool extra = false):
        listmark(lm),
        emmark(em),
        strongmark(sm),
        headerstyle(hs),
        codeblockstype(cb),
        enableextra(extra) { }
        
        Configuration():
        listmark('+'),
        emmark('*'),
        strongmark('*'),
        headerstyle(Html2Markdown::Configuration::atx),
        codeblockstype(Html2Markdown::Configuration::Indented),
        enableextra(false) { }
    }; 
        
    class Exception {
    public:
        Exception(const char* mssg);
        
        const char* what() const;
        
    private:
        const char* mMssg;
    };

    static Configuration DefaultConfiguration;

public:
    static std::string Convert(const char* text, size_t length, const Html2Markdown::Configuration& config = Html2Markdown::DefaultConfiguration);
    
private:
    struct HtmlNode;
    typedef std::vector<HtmlNode*> HtmlNodeTree;
    
    static void ParseHtmlTree(HtmlNodeTree& tree);
    static std::string ConvertHtmlTree(HtmlNode* node, const Html2Markdown::Configuration& config);
    static std::string ConvertBlockQuote(HtmlNode* node, const Html2Markdown::Configuration& config);
};

#endif
