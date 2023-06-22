#include <detail/aglPrivateResource.h>

namespace agl { namespace detail {

std::string PrivateResource::sShaderText(0x100000, '\0');   // 1 MB

} }
