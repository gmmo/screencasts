#pragma once

#include "..\Common\DeviceResources.h"
#include <regex>
#include <string>
#include <vector>
#include <atomic>
#include <collection.h>
#include <ppltasks.h>
#include <concurrent_vector.h>


namespace Cloth
{
    // This sample renderer instantiates a basic rendering pipeline.
    class FileLoad
    {
    public:
        FileLoad() {};

        std::vector<byte>           ReadFileSync(const std::wstring& filename);
        void                        ParseFile(const std::wstring& filename);
        std::vector<std::string>    Tokenize(const std::string str, const std::regex regex);
        std::vector<std::string>    TokenizeDefault(const std::string str);
        void                        EnumerateAppFiles();

    private:
        std::vector<byte>           m_data;
        bool                        m_loading;
    };
}

