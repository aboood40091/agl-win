#pragma once

#include <utility/aglParameterList.h>
#include <utility/aglResParameter.h>

#include <string>

namespace agl { namespace utl {

class IParameterIO : public IParameterList
{
public:
    IParameterIO();
    IParameterIO(const char* type, u32 version = 0);
    virtual ~IParameterIO() { }

public:
    virtual bool save(const char*, u32) const;
    virtual void applyResParameterArchive(ResParameterArchive arc);
    virtual void applyResParameterArchiveLerp(ResParameterArchive arc_a, ResParameterArchive arc_b, f32 t);

protected:
    virtual void callbackInvalidVersion_(ResParameterArchive arc) { }

public:
    void setDelegate(void* delegate)
    {
        mpDelegate = delegate;
    }

protected:
    std::string mType;
    u32 mVersion;
    void* mpDelegate; // sead delegate
    std::string _d4;
};
//static_assert(sizeof(IParameterIO) == 0x1E0, "agl::utl::IParameterIO size mismatch");

} }
