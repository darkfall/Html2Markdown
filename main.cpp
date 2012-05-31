//
//  main.c
//  Html2Markdown
//
//  Created by Robert Bu on 4/9/12.
//  Copyright (c) 2012 GameMaster Studio. All rights reserved.
//

#include <stdio.h>

#include "Html2Markdown.h"

//#include "src/markdown.h"
//#include "html/html.h"

static const char* html = "<html><body><h1>hello&nbsp;world</h1><h3>Hello world2</h3><p>hahahhaha test<a href=\"www.google.com\" title=\"Google\">Google!</a>test<ul><li>ll1</li><li>ll2</li></ul></p><ul><li><p>list1</p></li><li>list2</li></ul><dl><dt>Apple</dt><dd>Pomaceous fruit of plants of the genus Malus in the family Rosaceae.</dd><dt>Orange</dt><dd>The fruit of an evergreen tree of the genus Citrus.</dd></dl><table><thead><tr><th>First Header</th><th>Second Header</th></tr></thead><tbody><tr><td>Content Cell</td><td>Content Cell</td></tr><tr><td>Content Cell</td><td>Content Cell</td></tr></tbody></table></body></html>";

int main (int argc, const char * argv[])
{
  /*  FILE* file = fopen("markdown.txt", "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    void* doc = malloc(size);
    fread(doc, 1, size, file);
    fclose(file);
    
    html_renderopt opt;
    sd_callbacks cb;
    
    sdhtml_renderer(&cb, &opt, 0);
    
    sd_markdown* md = sd_markdown_new(0, 16, &cb, &opt);
    buf* ob = bufnew(16);
    sd_markdown_render(ob, (uint8_t*)doc, size, md);
    
    printf("%s\n", ob->data);
    */
    std::string result = Html2Markdown::Convert((const char*)html, 
                                                strlen(html),
                                                Html2Markdown::Configuration()
                                                    .setEnableExtra(true)
                                                    .setCodeblockStyle(Html2Markdown::Configuration::Fenced));
        
    printf("%s\n", result.c_str());

    return 0;
}

