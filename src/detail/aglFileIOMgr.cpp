#include <detail/aglFileIOMgr.h>
#include <detail/aglPrivateResource.h>

#include <misc/rio_MemUtil.h>

namespace agl { namespace detail {

FileIOMgr* FileIOMgr::sInstance = NULL;

bool FileIOMgr::save(const void* p_buf, u32 size, const DialogArg& arg)
{
    bool ret = false;

    // TODO: scoped lock
    //mCriticalSection.lock();
    do
    {
        RIO_ASSERT(!arg.mPath.empty());
        const std::string& path = arg.mPath;
        bool use_dialog = false;

        rio::FileHandle handle;

        rio::FileDevice* device = mFileDevice->tryOpen(&handle, path, rio::FileDevice::FILE_OPEN_FLAG_WRITE);
        if (device)
        {
            u8* temp_buf = static_cast<u8*>(rio::MemUtil::alloc(size, rio::FileDevice::cBufferMinAlignment));
            rio::MemUtil::copy(temp_buf, p_buf, size);

            handle.write(temp_buf, size);
            device->close(&handle);

            rio::MemUtil::free(temp_buf);

            if (arg.mOutPath)
                arg.mOutPath->operator=(path);

            ret = true;
        }
        else
        {
            if (use_dialog)
            {
                // Stripped in release build:
                // sead::hostio::ShowMessageBox(
                //     sead::FormatFixedSafeString<1024>(
                //         "%sの保存に失敗しました。\n保存先のアクセス権を確認して下さい。",
                //         file_info.path.cstr()
                //     ),
                //     "Caption=ファイル保存失敗, Icon=Error",
                //     ""
                // );
            }
        }
        break;
    }
    while (false);
    //mCriticalSection.unlock();

    return ret;
}

s32 FileIOMgr::load(const DialogArg& arg)
{
    s32 ret = -1;

    // TODO: scoped lock
    //mCriticalSection.lock();
    do
    {
        s32 index = -1;
        File* pBinary = NULL;

        for (Buffer<File>::iterator it = mBuffer.begin(), it_end = mBuffer.end(); it != it_end; ++it)
        {
            if (it->mpData == NULL)
            {
                index = it.getIndex();
                pBinary = &(*it);
                break;
            }
        }
        RIO_ASSERT(0 <= index);

        RIO_ASSERT(!arg.mPath.empty());
        const std::string& path = arg.mPath;

        rio::FileDevice::LoadArg load_arg;
        load_arg.path = path;
        load_arg.alignment = arg.mLoadAlignment;

        RIO_ASSERT(pBinary->mpData == nullptr);
        pBinary->mpData = mFileDevice->tryLoad(load_arg);
        if (pBinary->mpData)
        {
            pBinary->mSize = load_arg.read_size;

            if (arg.mOutPath)
                arg.mOutPath->operator=(path);

            ret = index;
        }
        break;
    }
    while (false);
    //mCriticalSection.unlock();

    return ret;
}

void FileIOMgr::close(s32 index)
{
    // TODO: scoped lock
    //mCriticalSection.lock();
    {
        if (mBuffer[index].mpData)
        {
            delete[] mBuffer[index].mpData;
            mBuffer[index].mpData = NULL;
            mBuffer[index].mSize = 0;
        }
    }
    //mCriticalSection.unlock();
}

FileIOMgr::DialogArg::DialogArg()
    : mDefaultFilter()
    , mDefaultFilename()
    , mDefaultID("agl_default")
    , mPath()
    , mOutPath(NULL)
    , mLoadAlignment(0x20) // Why not rio::FileDevice::cBufferMinAlignment is beyond me
{
}

} }
