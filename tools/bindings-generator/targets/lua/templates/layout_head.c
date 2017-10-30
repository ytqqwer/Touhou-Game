\#include "${out_file}.hpp"
#if $macro_judgement
$macro_judgement
#end if
#for header in $headers
    #set relative = os.path.relpath(header, $search_path)
\#include "${relative.replace(os.path.sep, '/')}"
#end for
\#include "scripting/lua-bindings/manual/tolua_fix.h"
\#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#if $cpp_headers
#for header in $cpp_headers
\#include "${header}"
#end for
#end if
