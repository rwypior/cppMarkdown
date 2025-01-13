target_sources(cppMarkdown PUBLIC
	"document.cpp"  "headingelement.cpp" "paragraphelement.cpp" "textentry.cpp" 
	"blockquoteelement.cpp" "listelement.cpp" "linebreakelement.cpp"
	"../include/cppmarkdowncommon.h" "../include/textentry.h" "../include/paragraphelement.h" 
	"../include/headingelement.h" "../include/blockquoteelement.h" "../include/document.h"   
	"../include/listelement.h" "../include/linebreakelement.h"
 "../include/codeelement.h" "codeelement.cpp" "../include/lineelement.h" "lineelement.cpp" "../include/blankelement.h" "blankelement.cpp" "cppmarkdowncommon.cpp" "../include/extensions.h" "extensions.cpp")

 target_sources(cppMarkdown PUBLIC
	"../include/ext/tableelement.h" "ext/tableelement.cpp")

 target_sources(cppMarkdown PRIVATE )
