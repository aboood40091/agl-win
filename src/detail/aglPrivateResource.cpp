#include <detail/aglPrivateResource.h>

static char sShaderWorkBufferData[0x40000];

namespace agl { namespace detail {

std::string PrivateResource::sShaderText(0x100000, '\0');                           // 1 MB
Buffer<char> PrivateResource::sShaderWorkBuffer(0x40000, sShaderWorkBufferData);    // 256 KB

} }
