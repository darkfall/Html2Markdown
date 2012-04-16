//
//  StringUtil.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_StringUtil_h
#define Project_Unknown_StringUtil_h

#include <vector>
#include <string>
#include <sstream>

namespace ukn {
    
    typedef std::string ukn_string;
   
    class StringTokenlizer {
    public:
        typedef std::vector<ukn_string> TokenContainer;
        typedef TokenContainer::iterator Iterator;
        typedef Iterator iterator;
        
        StringTokenlizer();
        StringTokenlizer(const ukn_string& str);
        StringTokenlizer(const ukn_string& str, const ukn_string& deli, bool append_seq_del = false);
        
        void parse(const ukn_string& str);
        void parse(const ukn_string& str, const ukn_string& deli, bool append_seq_del = false);
        
        ukn_string first();
        ukn_string last();
        
        ukn_string front();
        ukn_string back();
        
        iterator begin();
        iterator end();
        
        size_t size() const;
        
        void operator=(const ukn_string& str);
        void operator=(const char* str);
        
        const TokenContainer& getTokens() const;
        
        ukn_string operator[](size_t index);
        
    protected:
        bool isSpace(char c);
        bool isAlpha(char c);
        
        bool mAppendDelimSeq;
        TokenContainer mTokens;
    };

    
} // namespace ukn

#endif
