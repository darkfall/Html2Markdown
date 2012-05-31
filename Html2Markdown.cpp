//
//  Html2Markdown.cpp
//  Html2Markdown
//
//  Created by Robert Bu on 4/9/12.
//  Copyright (c) 2012 GameMaster Studio. All rights reserved.
//

#include "Html2Markdown.h"
#include "StringUtil.h"

#ifdef _WIN32
static int strncasecmp(const char* c1, const char* c2, int length) {
	return _stricmp(c1, c2);
}
#endif

#define STRCMP_NOCASE(c1, c2, len) strncasecmp((const char*)c1, (const char*)c2, len)
#define HTML_ESCAPE_LIST_LEN 95

struct {
    const char* escape;
    const char* chr;
    const char* escape2;
} HtmlEscape[HTML_ESCAPE_LIST_LEN] = {
    { "&#32;", " ", "&nbsp;" }, { "&#33;", "!", NULL }, { "&#34;", "\"", "&quot;" }, { "&#35;", "#", NULL },
    { "&#36;", "$", NULL }, { "&#37;", "%", NULL },{ "&#38;", "&", "&amp;" },{ "&#39;", "'", NULL },
    { "&#40;", "(", NULL }, { "&#41;", ")", NULL }, { "&#42;", "*", NULL }, { "&#43;", "+", NULL },
    { "&#44;", ",", NULL }, { "&#45;", "-", NULL }, { "&#46;", ".", NULL }, { "&#47;", "/", NULL },
    { "&#48;", "0", NULL }, { "&#49;", "1", NULL }, { "&#50;", "2", NULL }, { "&#51;", "3", NULL },
    { "&#52;", "4", NULL }, { "&#53;", "5", NULL }, { "&#54;", "6", NULL }, { "&#55;", "7", NULL },
    { "&#56;", "8", NULL }, { "&#57;", "9", NULL }, { "&#58;", ":", NULL },{ "&#59;", ";", NULL},
    { "&#60;", "<", "&lt;" }, { "&#61;", "=", NULL }, { "&#62;", ">", "&gt;" }, { "&#63;", "?", NULL },
    { "&#64;", "@", NULL }, { "&#65;", "A", NULL }, { "&#66;", "B", NULL }, { "&#67;", "C", NULL },
    { "&#68;", "D", NULL }, { "&#69;", "E", NULL }, { "&#70;", "F", NULL }, { "&#71;", "G", NULL },
    { "&#72;", "H", NULL }, { "&#73;", "I", NULL }, { "&#74;", "J", NULL }, { "&#75;", "K", NULL },
    { "&#76;", "L", NULL }, { "&#77;", "M", NULL }, { "&#78;", "N", NULL }, { "&#79;", "O", NULL },
    { "&#80;", "P", NULL }, { "&#81;", "Q", NULL }, { "&#82;", "R", NULL }, { "&#83;", "S", NULL },
    { "&#84;", "T", NULL }, { "&#85;", "U", NULL }, { "&#86;", "V", NULL }, { "&#87;", "W", NULL },
    { "&#88;", "X", NULL }, { "&#89;", "Y", NULL }, { "&#90;", "Z", NULL }, { "&#91;", "[", NULL },
    { "&#92;", "\\", NULL }, { "&#93;", "]", NULL }, { "&#94;", "^", NULL }, { "&#95;", "_", NULL },
    { "&#96;", "`", NULL }, { "&#97;", "a", NULL }, { "&#98;", "b", NULL }, { "&#99;", "c", NULL },
    { "&#100;", "d", NULL }, { "&#101;", "e", NULL }, { "&#102;", "f", NULL }, { "&#103;", "g", NULL },
    { "&#104;", "h", NULL }, { "&#105;", "i", NULL }, { "&#106;", "j", NULL }, { "&#107;", "k", NULL },
    { "&#108;", "l", NULL }, { "&#109;", "m", NULL }, { "&#110;", "n", NULL }, { "&#111;", "o", NULL },
    { "&#112;", "p", NULL }, { "&#113;", "q", NULL }, { "&#114;", "r", NULL }, { "&#115;", "s", NULL },
    { "&#116;", "t", NULL }, { "&#117;", "u", NULL },{ "&#118;", "v", NULL },{ "&#119;", "w", NULL }, 
    { "&#120;", "x", NULL }, { "&#121;", "y", NULL }, { "&#122;", "z", NULL }, { "&#123;", "{", NULL },
    { "&#124;", "|", NULL },{ "&#125;", "}", NULL }, { "&#126;", "~", NULL },
};

struct Html2Markdown::HtmlNode {
    Html::Tag tag;
    
    std::string tag_str;
    std::string value;
    std::string content;
    
    size_t start_index;
    size_t end_index;
    
    size_t start_content_index;
    
    HtmlNode* parent;
    
    typedef std::vector<HtmlNode*> ChildList;
    ChildList childs;
    
    typedef std::map<std::string, std::string> AttributeMap;
    AttributeMap attributes;
    
    HtmlNode(): 
    tag(Html::Unknown),
    start_index(0),
    end_index(0),
    parent(0) { }
};

Html::Tag Html::StringToHtmlTag(const char* str, size_t length) {
    if(STRCMP_NOCASE(str, "p", length) == 0) {
        return Html::P;
    }
    if(STRCMP_NOCASE(str, "A", length) == 0) {
        return Html::A;
    }
    if(STRCMP_NOCASE(str, "h1", length) == 0) {
        return Html::H1;
    }
    if(STRCMP_NOCASE(str, "h2", length) == 0) {
        return Html::H2;
    }
    if(STRCMP_NOCASE(str, "h3", length) == 0) {
        return Html::H3;
    }
    if(STRCMP_NOCASE(str, "h4", length) == 0) {
        return Html::H4;
    }
    if(STRCMP_NOCASE(str, "h5", length) == 0) {
        return Html::H5;
    }
    if(STRCMP_NOCASE(str, "h6", length) == 0) {
        return Html::H6;
    }
    if(STRCMP_NOCASE(str, "IMG", length) == 0) {
        return Html::IMG;
    }
    if(STRCMP_NOCASE(str, "OL", length) == 0) {
        return Html::OL;
    }
    if(STRCMP_NOCASE(str, "UL", length) == 0) {
        return Html::UL;
    }
    if(STRCMP_NOCASE(str, "LI", length) == 0) {
        return Html::LI;
    }
    if(STRCMP_NOCASE(str, "TH", length) == 0) {
        return Html::TH;
    }
    if(STRCMP_NOCASE(str, "TD", length) == 0) {
        return Html::TD;
    }
    if(STRCMP_NOCASE(str, "TR", length) == 0) {
        return Html::TR;
    }
    if(STRCMP_NOCASE(str, "DD", length) == 0) {
        return Html::DD;
    }
    if(STRCMP_NOCASE(str, "DL", length) == 0) {
        return Html::DL;
    }
    if(STRCMP_NOCASE(str, "DT", length) == 0) {
        return Html::DT;
    }
    if(STRCMP_NOCASE(str, "EM", length) == 0) {
        return Html::EM;
    }
    
    if(STRCMP_NOCASE(str, "PRE", length) == 0) {
        return Html::PRE;
    }
    if(STRCMP_NOCASE(str, "SUP", length) == 0) {
        return Html::SUP;
    }
    
    if(STRCMP_NOCASE(str, "CODE", length) == 0) {
        return Html::CODE;
    }
    
    if(STRCMP_NOCASE(str, "STRONG", length) == 0) {
        return Html::STRONG;
    }
    if(STRCMP_NOCASE(str, "HTML", length) == 0) {
        return Html::HTML;
    }
    if(STRCMP_NOCASE(str, "BODY", length) == 0) {
        return Html::BODY;
    }
    if(STRCMP_NOCASE(str, "BLOCKQUOTE", length) == 0) {
        return Html::BLOCKQUOTE;
    }
    if(STRCMP_NOCASE(str, "ABBR", length) == 0) {
        return Html::ABBR;
    }
    
    
    if(STRCMP_NOCASE(str, "table", length) == 0) {
        return Html::TABLE;
    }
    if(STRCMP_NOCASE(str, "thead", length) == 0) {
        return Html::THEAD;
    }
    if(STRCMP_NOCASE(str, "tbody", length) == 0) {
        return Html::TBODY;
    }
  
    return Html::Unknown;
}

/**
 * html 'Single' tags, tags can end without </TAG_NAME>
 * like <br> <img src="...">
 **/
bool Html::IsSingleTagName(const char* name, size_t length) {
    if((STRCMP_NOCASE(name, "img", length) == 0)) return true;
    if((STRCMP_NOCASE(name, "br", length) == 0)) return true;
    if((STRCMP_NOCASE(name, "hr", length) == 0)) return true;
    
    return false;
}

static bool IsSpecialCharacter(char chr) {
    return chr == '*' || chr == '{' || chr == '}' ||
            chr == '`' || chr == '(' || chr == ')' ||
            chr == '[' || chr == ']' || chr == '#' ||
            chr == '+' || chr == '-' || chr == '.' ||
            chr == '!' || chr == '_';
}

static void UnescapeSpecialCharacters(std::string& str) {
    for(size_t i = 0; i < str.size();) {
        if(IsSpecialCharacter(str[i]) &&
           str[i-1] == '\\') {
            str.replace(i-1, 2, std::string(1, str[i]));
        }
        ++i;
    }
}

static void ReplaceHtmlEscapeCharacters(std::string& str) {
    if(str.find('&') != std::string::npos) {
        for(size_t i = 0; i < HTML_ESCAPE_LIST_LEN; ++i) {
            size_t escape_pos = str.find(HtmlEscape[i].escape);
            if(escape_pos != std::string::npos) {
                while(escape_pos != std::string::npos) {
                    str.replace(escape_pos, strlen(HtmlEscape[i].escape), HtmlEscape[i].chr);
                    escape_pos = str.find(HtmlEscape[i].escape, escape_pos+1);
                }
            }
            
            if(HtmlEscape[i].escape2 != NULL) {
                escape_pos = str.find(HtmlEscape[i].escape2);
                if(escape_pos != std::string::npos) {
                    while(escape_pos != std::string::npos) {
                        str.replace(escape_pos, strlen(HtmlEscape[i].escape2), HtmlEscape[i].chr);
                        escape_pos = str.find(HtmlEscape[i].escape2, escape_pos+1);
                    }
                }
            }
        }
    }
}

inline std::string ProcessHtmlString(const std::string& str) {
    std::string val = str;
    ReplaceHtmlEscapeCharacters(val);
    UnescapeSpecialCharacters(val);
    return val;
}

static void PostProcessString(std::string& str) {
    
}

Html2Markdown::Configuration Html2Markdown::DefaultConfiguration('+', 
                                                                 '*', 
                                                                 '*', 
                                                                 Html2Markdown::Configuration::atx,
                                                                 Html2Markdown::Configuration::Indented,
                                                                 false);

Html2Markdown::Exception::Exception(const char* exp): mMssg(exp) { }
const char* Html2Markdown::Exception::what() const { return mMssg; }

std::string Html2Markdown::ConvertBlockQuote(HtmlNode* node, const Html2Markdown::Configuration& config) {
    std::string processed;
    
    for(size_t i = 0; i < node->childs.size(); ++i) {
        std::string result = ConvertHtmlTree(node->childs[i], config);
        
        ukn::StringTokenlizer lines(result, "\n");
        for(size_t i = 0; i < lines.size(); ++i) {
            const std::string& line = lines[i];
            size_t start_index = 0;
            while(line[start_index] == ' ')
                start_index++;
            
            processed += "> " + line.substr(start_index, line.size()) + "\n";
        }
        if(i != node->childs.size() - 1)
            processed += "> \n";
    }
    
    return processed;
}

std::string Html2Markdown::ConvertHtmlTree(HtmlNode* node, const Html2Markdown::Configuration& config) {
    std::string result;
    
    switch(node->tag) {
        case Html::HTML:
        case Html::BODY:
            for(size_t i = 0; i < node->childs.size(); ++i) {
                result += ConvertHtmlTree(node->childs[i], config);
            }
            break;
            
        case Html::A: {
            std::string converted_val = ProcessHtmlString(node->value);
            HtmlNode::AttributeMap::const_iterator title_it = node->attributes.find("title");
            HtmlNode::AttributeMap::const_iterator href_it = node->attributes.find("href");
            
            if(href_it == node->attributes.end()) {
                throw Html2Markdown::Exception("no href attribute found in a link tag");
            }
            
            // automic links
            if(converted_val.find('@') != std::string::npos || 
               (title_it != node->attributes.end() && title_it->second == href_it->second)) {
                result += "<" + converted_val + ">";
            } else {
                result += "[" + node->value + "](" + href_it->second;
                if(title_it != node->attributes.end()) {
                    result += " \"" + node->attributes["title"] + "\"";
                }
                result += ")";
            }
            break;
        }
            
        case Html::H1:
        case Html::H2:
            if(config.headerstyle == Html2Markdown::Configuration::atx) {
                for(int i = 0; i <= (node->tag - Html::H1); ++i) {
                    result += "#";
                }
                result += " " + node->value + "\n\n";
            } else {
                std::string converted_val = node->value;
                result += converted_val + "\n";
                if(node->tag == Html::H1) {
                    result += std::string(converted_val.length(), '=') + "\n\n";
                } else {
                    result += std::string(converted_val.length(), '-') + "\n\n";
                }
            }
            break;
        case Html::H3:
        case Html::H4:
        case Html::H5:
        case Html::H6:            
            for(int i = 0; i <= (node->tag - Html::H1); ++i) {
                result += "#";
            }
            result += " " + node->value + "\n\n";
            break;
            
        case Html::IMG: {
            HtmlNode::AttributeMap::const_iterator src_it = node->attributes.find("src");
            HtmlNode::AttributeMap::const_iterator alt_it = node->attributes.find("alt");
            HtmlNode::AttributeMap::const_iterator title_it = node->attributes.find("title");
            
            if(src_it == node->attributes.end()) throw Html2Markdown::Exception("no src attribute found in img tag");
            if(alt_it == node->attributes.end()) throw Html2Markdown::Exception("no alt attribute found in img tag");
            
            result += "![" + alt_it->second+ "]";
            result += "(" + src_it->second;
            if(title_it != node->attributes.end())
                result += " \"" + title_it->second + "\"";
            result += ")";
            break;
        }
            
        case Html::P:            
            if(node->childs.size() == 0) {
                result += node->value + "\n\n";
            }
            else {
                size_t start_pos = 0;
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    size_t end_pos = node->value.find(node->childs[i]->content);
                    
                    std::string block = node->value.substr(start_pos, end_pos-start_pos);
                    result += block + ConvertHtmlTree(node->childs[i], config);
                    
                    start_pos = end_pos + node->childs[i]->content.size();
                }
                if(start_pos < node->value.size())
                    result += node->value.substr(start_pos, node->value.size());
                
                result += "\n\n";
            }
            break;
            
        case Html::OL:
        case Html::UL:
            result += "\n";
            for(size_t i = 0; i < node->childs.size(); ++i) {
                result += ConvertHtmlTree(node->childs[i], config);
            }
            result += "\n";
            break;
            
        case Html::LI: {            
            std::string pre;
            if(node->parent->tag == Html::UL) {
                pre = std::string(1, config.listmark) + " ";
            }
            else if(node->parent->tag == Html::OL) {
                for(size_t i = 0; i < node->parent->childs.size(); ++i)
                    if(node->parent->childs[i] == node) {
                        char buf[4];
                        sprintf(buf, "%d. ", (int)i+1);
                        pre = buf;
                    }
                
            }
            if(node->childs.size() != 0) {
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    result += pre + ConvertHtmlTree(node->childs[i], config);
                }
            } else 
                result += pre + node->value + "\n";
            break;
        }
            
        case Html::PRE:
            if(config.codeblockstype == Html2Markdown::Configuration::Indented) {
               
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    if(node->childs[i]->tag == Html::CODE) {
                        ukn::StringTokenlizer tokens(node->childs[i]->value, "\n");
                        for(size_t i = 0; i < tokens.size(); ++i)
                            result += "\t" + tokens[i] + "\n";
                        
                    }
                }
                
            } else {
                
                result += "~~~~";
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    if(node->childs[i]->tag == Html::CODE) {
                        result += node->childs[i]->value;
                    }
                }
                result += "~~~~";
                
            }
    
			result += "\n\n";
            break;
            
        case Html::CODE:
            if(node->value.find("`") == std::string::npos)
                result += "`";
            else
                result += "`` ";
            result += node->value;
            if(node->value.find("`") == std::string::npos)
                result += "`";
            else
                result += " ``";
            break;
            
        case Html::EM:            
            result += std::string(1, config.emmark) + node->value + std::string(1, config.emmark);
            break;
            
        case Html::STRONG:            
            result += std::string(2, config.strongmark) + node->value + std::string(2, config.strongmark);
            break;
            
        case Html::BLOCKQUOTE:
            result += ConvertBlockQuote(node, config);
            break;
                  
        default:
            break;
    }
    
    /* markdown extra tags */
    if(config.enableextra) {
        switch(node->tag) {
            case Html::TABLE:
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    result += ConvertHtmlTree(node->childs[i], config);
                }
                result += "\n\n";
                break;
                
            case Html::THEAD: {
                std::string head;
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    head += ConvertHtmlTree(node->childs[i], config);
                }
                result += head;
                std::string tail = head;
                std::replace_if(tail.begin(),
                                tail.end(),
                                std::bind2nd(std::not_equal_to<char>(), '|'),
                                '-');
                
                // replace back \n
                tail[tail.size()-1] = '\n';
                result += tail;
                break;
            }
            case Html::TBODY:
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    result += ConvertHtmlTree(node->childs[i], config);
                }
                break;
                
            case Html::TR:
                result += "|";
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    HtmlNode* childnode = node->childs[i];
                    if(childnode->tag == Html::TH ||
                       childnode->tag == Html::TD) {
                        result += ConvertHtmlTree(node->childs[i], config); 
                        
                        result += "|";
                    }
                }
                result += "\n";
                break;
                
            case Html::TH:
            case Html::TD:
                if(node->childs.size() == 0)
                    result += node->value;
                else {
                    for(size_t i = 0; i < node->childs.size(); ++i) {
                        result += ConvertHtmlTree(node->childs[i], config);
                    }
                }
                break;
                
            case Html::DL:
                result += "\n";
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    result += ConvertHtmlTree(node->childs[i], config);
                }
                result += "\n";
                break;
                
            case Html::DT:
                result += "\n";
                result += node->value;
                result += "\n";
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    result += ConvertHtmlTree(node->childs[i], config);
                }
                break;
                
            case Html::DD:
                if(node->childs.size() != 0) {
                    for(size_t i = 0; i < node->childs.size(); ++i) {
                        if(node->childs[i]->tag == Html::P) {
                            result += "\n:\t" + node->childs[i]->value + "\n";
                        }
                    }
                } else {
                    result += ":\t" + node->value + "\n";
                }
                break;
                
            case Html::SUP:
                break;
                
            case Html::ABBR:
                break;
                
            default:
                break;
        }
    }
    
    return result;
}

std::string Html2Markdown::Convert(const char* str, size_t length, const Html2Markdown::Configuration& config) {
    std::string processed_str = ProcessHtmlString(str);
    const char* html = processed_str.c_str();
    
    std::vector<HtmlNode*> nodes;
    std::vector<HtmlNode*> tag_stack;
    
    size_t i =0 ;
    while(i < length) {
        if(html[i] == '<') {
            size_t end_tag_block = i+1;
            size_t end_tag = i+1;
            while(html[end_tag_block] != '>') {
                if(end_tag == i+1 && html[end_tag_block] == ' ')
                    end_tag = end_tag_block;
                
                end_tag_block++;
            }
            if(end_tag == i+1)
                end_tag = end_tag_block;
            
            std::string tag_str(html + (i+1), end_tag - (i+1));
            
            bool is_single = Html::IsSingleTagName(tag_str.c_str(), tag_str.size());
            
            if(tag_str[0] != '/' || is_single) {
                Html::Tag tag = Html::StringToHtmlTag(tag_str.c_str(), tag_str.size());
                
                if(tag != Html::Unknown) {
                    HtmlNode* state = new HtmlNode();
                    state->tag                  = tag;
                    state->start_index          = end_tag_block+1;
                    state->tag_str              = tag_str;
                    state->start_content_index  = i;
                   
                    if(tag_stack.size() > 0) {
                        state->parent = tag_stack.back();
                        state->parent->childs.push_back(state);
                    }
                    
                    if(!state->parent)
                        nodes.push_back(state);
                    
                    if(!is_single)
                        tag_stack.push_back(state);
                    else {
                        state->content = state->value = std::string(html + i, end_tag_block - i + 1);
                    }
                    
                    size_t pre_attr = end_tag + 1;
                    for(size_t j = end_tag + 1; j <= end_tag_block; ++j) {
                        if((html[j] == ' ' && html[j-1] == '"') || html[j] == '>') {
                            std::string attr_str(html + pre_attr, j - pre_attr);
                            
                            size_t equal_pos = attr_str.find('=');
                            std::string attr_name = attr_str.substr(0, equal_pos);
                            std::string attr_val = attr_str.substr(equal_pos + 2, attr_str.size() - attr_name.size() - 3);
                            
                            pre_attr = j+1;
                            state->attributes[attr_name] = attr_val;
                        }
                    }
                }
                
                i = end_tag_block;
                continue;
                
            } else {
                if(tag_stack.size() == 0) {
                    throw Html2Markdown::Exception(("unmatched tag found, tag " + tag_str).c_str());
                }
                
                size_t end_content_index = i+1;
                while(html[end_content_index] != '>')
                    end_content_index++;
                
                tag_stack.back()->end_index = i-1;
                tag_stack.back()->value = std::string(html + tag_stack.back()->start_index, 
                                                      tag_stack.back()->end_index - tag_stack.back()->start_index + 1);
                tag_stack.back()->content = std::string(html + tag_stack.back()->start_content_index,
                                                        end_content_index - tag_stack.back()->start_content_index+1);
                tag_stack.pop_back();
                
                i = end_content_index;
                continue;
            }
        }
        i++;
    }
    
    std::string result;
    for(size_t i = 0; i < nodes.size(); ++i)
        result += (ConvertHtmlTree(nodes[i], config));
    
    PostProcessString(result);
    return result;
}

