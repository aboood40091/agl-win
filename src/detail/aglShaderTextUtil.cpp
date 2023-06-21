#include <detail/aglShaderTextUtil.h>
#include <misc/rio_MemUtil.h>

#include <map>
#include <unordered_map>

namespace agl { namespace detail {

void ShaderTextUtil::replaceMacro(std::string* p_text, const std::unordered_map<std::string, const std::string>& macro_map_)
{
    RIO_ASSERT(p_text != nullptr);
  //RIO_ASSERT(p_text->length() == std::strlen(p_text->c_str()));

    std::string::size_type text_pos = 0;

    std::map<std::string, const std::string> macro_map(macro_map_.begin(), macro_map_.end());

    while (text_pos < p_text->length())
    {
        const std::string::size_type include_directive_begin = p_text->find_first_of('#', text_pos);
        if (include_directive_begin == std::string::npos)
            break;

        text_pos = p_text->find_first_not_of(" \t", include_directive_begin + 1, 2);
        if (text_pos == std::string::npos)
            break;

        if (p_text->compare(text_pos, 6, "define", 6) != 0)
            continue;

        if (text_pos + 6 >= p_text->length() ||
            (p_text->at(text_pos + 6) != ' ' &&
             p_text->at(text_pos + 6) != '\t'))
        {
          //RIO_ASSERT(false);
            break;
        }

        text_pos = p_text->find_first_not_of(" \t", text_pos + 7, 2);
        if (text_pos == std::string::npos)
        {
          //RIO_ASSERT(false);
            break;
        }

        std::string::size_type line_feed_pos = p_text->find_first_of("\r\n", text_pos, 2);
        if (line_feed_pos == std::string::npos)
            line_feed_pos = p_text->length();

        auto itr_match_macro = macro_map.lower_bound(p_text->substr(text_pos, line_feed_pos - text_pos));
        if (itr_match_macro == macro_map.begin())
            continue;

        --itr_match_macro;

        bool found = false;

        while (true)
        {
            if (p_text->compare(text_pos, itr_match_macro->first.length(), itr_match_macro->first) != 0)
                break;

            if (text_pos + itr_match_macro->first.length() >= p_text->length() ||
                (p_text->at(text_pos + itr_match_macro->first.length()) != ' ' &&
                 p_text->at(text_pos + itr_match_macro->first.length()) != '\t'))
            {
                if (itr_match_macro == macro_map.begin())
                    break;

                --itr_match_macro;
                continue;
            }

            found = true;
            break;
        }

        if (!found)
            continue;

        const std::string& match_macro = itr_match_macro->first;
      //RIO_ASSERT(match_macro.length() == std::strlen(match_macro.c_str()));

        const std::string& macro_value = itr_match_macro->second;
      //RIO_ASSERT(macro_value.length() == std::strlen(macro_value.c_str()));

        const std::string::size_type define_len = 9 + match_macro.length() + macro_value.length(); // 9 == strlen("#define") + strlen(" ") + strlen(" ")
        char* const define_buf = new char[define_len + 1];
        std::snprintf(define_buf, define_len + 1, "#define %s %s", match_macro.c_str(), itr_match_macro->second.c_str());
      //RIO_ASSERT(define_len == std::strlen(define_buf));

        p_text->replace(include_directive_begin, line_feed_pos - include_directive_begin, define_buf, define_len);

        delete[] define_buf;

        text_pos = include_directive_begin + define_len;

        macro_map.erase(itr_match_macro);
    }

  //RIO_ASSERT(p_text->length() == std::strlen(p_text->c_str()));
}

void ShaderTextUtil::createRawText(std::string* p_text, const std::unordered_map<std::string, const std::string>& source_map)
{
    RIO_ASSERT(p_text != nullptr);
  //RIO_ASSERT(p_text->length() == std::strlen(p_text->c_str()));

    std::string::size_type text_pos = 0;

    while (text_pos < p_text->length())
    {
        const std::string::size_type include_directive_begin = p_text->find_first_of('#', text_pos);
        if (include_directive_begin == std::string::npos)
            break;

        text_pos = p_text->find_first_not_of(" \t\r\n", include_directive_begin + 1, 4);
        if (text_pos == std::string::npos)
            break;

        if (p_text->compare(text_pos, 7, "include", 7) != 0)
            continue;

        const std::string::size_type include_name_begin = p_text->find_first_of('\"', text_pos);
        if (include_name_begin == std::string::npos)
            continue;

        const std::string::size_type include_directive_end = p_text->find_first_of('\"', include_name_begin + 1);
        if (include_directive_end == std::string::npos)
            continue;

        const auto& itr_source = source_map.find(p_text->substr(include_name_begin + 1, include_directive_end - (include_name_begin + 1)));
        if (itr_source == source_map.end())
            break;

      //const std::string& source_name = itr_source->first;
      //RIO_ASSERT(source_name.length() == std::strlen(source_name.c_str()));

        const std::string& source_text = itr_source->second;
      //RIO_ASSERT(source_text.length() == std::strlen(source_text.c_str()));

        p_text->replace(include_directive_begin, include_directive_end + 1 - include_directive_begin, source_text);

        text_pos = include_directive_begin + source_text.length();
    }

  //RIO_ASSERT(p_text->length() == std::strlen(p_text->c_str()));
}

} }
