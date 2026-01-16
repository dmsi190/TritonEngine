// filesystem_manager.hpp

#pragma once

#include "object.hpp"
#include "types.hpp"

namespace realware
{
    class cDataFile : public cFactoryObject
    {
        REALWARE_CLASS(cDataFile)

    public:
        explicit cDataFile(cContext* context);
        virtual ~cDataFile() override final;

        void Open(const std::string& path, types::boolean isText);

    private:
        cDataBuffer* _data = nullptr;
    };

    class cFileSystem : public iObject
    {
    public:
        explicit cFileSystem(cContext* context);
        ~cFileSystem() = default;

        cDataFile* CreateDataFile(const std::string& path, types::boolean isText);
        void DestroyDataFile(cDataFile* buffer);
    };
}