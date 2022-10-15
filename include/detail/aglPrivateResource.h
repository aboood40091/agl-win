#pragma once

#include <container/Buffer.h>

#include <string>

namespace agl { namespace detail {

class PrivateResource
{
public:
    static std::string sShaderText;
    static Buffer<char> sShaderWorkBuffer;
};

} }
