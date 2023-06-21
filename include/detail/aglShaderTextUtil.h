#pragma once

#include <misc/rio_Types.h>

#include <string>
#include <unordered_map>

namespace agl { namespace detail {

class ShaderTextUtil
{
public:
    static void replaceMacro(std::string* p_text, const std::unordered_map<std::string, const std::string>& macro_map);
    static void createRawText(std::string* p_text, const std::unordered_map<std::string, const std::string>& source_map);
};

} }
