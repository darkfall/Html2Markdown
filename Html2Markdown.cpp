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
    if(strncasecmp(str, "p", length) == 0) {
        return Html::P;
    }
    if(strncasecmp(str, "h1", length) == 0) {
        return Html::H1;
    }
    if(strncasecmp(str, "h2", length) == 0) {
        return Html::H2;
    }
    if(strncasecmp(str, "h3", length) == 0) {
        return Html::H3;
    }
    if(strncasecmp(str, "h4", length) == 0) {
        return Html::H4;
    }
    if(strncasecmp(str, "h5", length) == 0) {
        return Html::H5;
    }
    if(strncasecmp(str, "h6", length) == 0) {
        return Html::H6;
    }
    if(strncasecmp(str, "IMG", length) == 0) {
        return Html::IMG;
    }
    if(strncasecmp(str, "TD", length) == 0) {
        return Html::TD;
    }
    if(strncasecmp(str, "TL", length) == 0) {
        return Html::TL;
    }
    if(strncasecmp(str, "TR", length) == 0) {
        return Html::TR;
    }
    if(strncasecmp(str, "OL", length) == 0) {
        return Html::OL;
    }
    if(strncasecmp(str, "UL", length) == 0) {
        return Html::UL;
    }
    if(strncasecmp(str, "LI", length) == 0) {
        return Html::LI;
    }
    if(strncasecmp(str, "PRE", length) == 0) {
        return Html::PRE;
    }
    if(strncasecmp(str, "CODE", length) == 0) {
        return Html::CODE;
    }
    if(strncasecmp(str, "A", length) == 0) {
        return Html::A;
    }
    if(strncasecmp(str, "EM", length) == 0) {
        return Html::EM;
    }
    if(strncasecmp(str, "STRONG", length) == 0) {
        return Html::STRONG;
    }
    if(strncasecmp(str, "HTML", length) == 0) {
        return Html::HTML;
    }
    if(strncasecmp(str, "BODY", length) == 0) {
        return Html::BODY;
    }
    if(strncasecmp(str, "BLOCKQUOTE", length) == 0) {
        return Html::BLOCKQUOTE;
    }
    return Html::Unknown;
}

/**
 * html 'Single' tags, tags can end without </TAG_NAME>
 * like <br> <img src="...">
 **/
bool Html::IsSingleTagName(const char* name, size_t length) {
    if((strncasecmp(name, "img", length) == 0)) return true;
    if((strncasecmp(name, "br", length) == 0)) return true;
    if((strncasecmp(name, "hr", length) == 0)) return true;
    
    return false;
}

static bool IsSpecialCharacter(char chr) {
    return chr == '*' || chr == '{' || chr == '}' ||
    chr == '`' || chr == '(' || chr == ')' ||
    chr == '[' || chr == ']' || chr == '#' ||
    chr == '+' || chr == '-' || chr == '.' ||
    chr == '!' || chr == '_';
}

static void EscapeSpecialCharacters(std::string& str) {
    for(size_t i = 0; i < str.size();) {
        if(IsSpecialCharacter(str[i])) {
            str.insert(str.begin() + i, '\\');
            i += 2;
        }
        ++i;
    }
}

static void ReplaceHtmlEscapeCharacters(std::string& str) {
    
}

std::string Html2Markdown::ConvertBlockQuote(HtmlNode* node) {
    std::string processed;
    
    for(size_t i = 0; i < node->childs.size(); ++i) {
        std::string result = ConvertHtmlTree(node->childs[i]);
        
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

std::string Html2Markdown::ConvertHtmlTree(HtmlNode* node) {
    std::string result;
    
    switch(node->tag) {
        case Html::HTML:
        case Html::BODY:
            for(size_t i = 0; i < node->childs.size(); ++i) {
                result += ConvertHtmlTree(node->childs[i]);
            }
            break;
            
        case Html::A:            
            result += "[" + node->value + "](" + node->attributes["href"];
            if(node->attributes.find("title") != node->attributes.end())
                result += " \"" + node->attributes["title"] + "\"";
            result += ")";
            break;
            
        case Html::H1:
        case Html::H2:
        case Html::H3:
        case Html::H4:
        case Html::H5:
        case Html::H6:            
            for(int i = 0; i <= (node->tag - Html::H1); ++i) {
                result += "#";
            }
            result += " " + node->value + "\n";
            break;
            
        case Html::IMG:
            result += "![" + node->attributes["alt"] + "]";
            result += "(" + node->attributes["src"];
            if(node->attributes.find("title") != node->attributes.end())
                result += " \"" + node->attributes["title"] + "\"";
            result += ")";
            break;
            
        case Html::TD:
        case Html::TL:
        case Html::TR:
            break;
            
        case Html::P:            
            if(node->childs.size() == 0) {
                result += node->value + "\n\n";
            }
            else {
                size_t start_pos = 0;
                for(size_t i = 0; i < node->childs.size(); ++i) {
                    size_t end_pos = node->value.find(node->childs[i]->content);
                    
                    std::string block = node->value.substr(start_pos, end_pos-start_pos);
                    EscapeSpecialCharacters(block);
                    
                    result += block + ConvertHtmlTree(node->childs[i]);
                    
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
                result += ConvertHtmlTree(node->childs[i]);
            }
            result += "\n\n";
            break;
            
        case Html::LI: {            
            std::string pre;
            if(node->parent->tag == Html::UL)
                pre = "+ ";
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
                    if(node->childs[i]->tag == Html::P)
                        result += pre + ConvertHtmlTree(node->childs[i]);
                }
            } else 
                result += pre + node->value + "\n";
            break;
        }
            
        case Html::PRE:
            for(size_t i = 0; i < node->childs.size(); ++i) {
                if(node->childs[i]->tag == Html::CODE) {
                    ukn::StringTokenlizer tokens(node->childs[i]->value, "\n");
                    for(size_t i = 0; i < tokens.size(); ++i)
                        result += "\t" + tokens[i] + "\n";
                }
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
            result += "*" + node->value + "*";
            break;
            
        case Html::STRONG:            
            result += "**" + node->value + "**";
            break;
            
        case Html::BLOCKQUOTE:
            result += ConvertBlockQuote(node);
            break;
            
        default:
            break;
    }
    
    return result;
}

std::string Html2Markdown::Convert(const char* html, size_t length) {
    std::string html_doc = html;
    
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
                        state->content = state->value = html_doc.substr(i, end_tag_block - i + 1);
                    }
                    
                    size_t pre_attr = end_tag + 1;
                    for(size_t j = end_tag + 1; j <= end_tag_block; ++j) {
                        if((html[j] == ' ' && html[j-1] == '\"') || html[j] == '>') {
                            std::string attr_str = html_doc.substr(pre_attr, j - pre_attr);
                            
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
                assert(tag_stack.size() > 0);
                
                size_t end_content_index = i+1;
                while(html[end_content_index] != '>')
                    end_content_index++;
                
                tag_stack.back()->end_index = i-1;
                tag_stack.back()->value = html_doc.substr(tag_stack.back()->start_index, 
                                                          tag_stack.back()->end_index - tag_stack.back()->start_index + 1);
                tag_stack.back()->content = html_doc.substr(tag_stack.back()->start_content_index,
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
        result += (ConvertHtmlTree(nodes[i]));
    return result;
}

