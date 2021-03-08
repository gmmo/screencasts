#include "pch.h"
#include "FileLoad.h"

#include "..\Common\DirectXHelper.h"

#include <regex>
#include <string>
#include <vector>
#include <atomic>
#include <collection.h>
#include <ppltasks.h>
#include <concurrent_vector.h>

using namespace concurrency;
using namespace std;
using namespace Cloth;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::Storage;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Platform;

std::vector<byte> FileLoad::ReadFileSync(const std::wstring& filename)
{
    m_loading = true;

    auto loadTask = DX::ReadDataAsync(filename);

    auto createTask = loadTask.then([this](const std::vector<byte>& fileData) {
        m_data = fileData;
    }
    );

    createTask.then([this]() {
        m_loading = false; }
    );

    while (m_loading)
    {
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }

    std::wstring str = L"loaded = ";
    str += filename;
    str += L"\n";

    OutputDebugStringW(str.c_str());

    return m_data;
}

std::vector<std::string> FileLoad::Tokenize(const std::string str, const std::regex regex)
{
    using namespace std;

    std::vector<string> result;
    sregex_token_iterator it(str.begin(), str.end(), regex, -1);
    sregex_token_iterator reg_end;
    for (; it != reg_end; ++it)
    {
        if (!it->str().empty()) //token could be empty:check
            result.emplace_back(it->str());
    }

    return result;
}

std::vector<std::string> FileLoad::TokenizeDefault(const std::string str)
{
    using namespace std;
    regex re("[\\s,]+");
    return Tokenize(str, re);
}

void FileLoad::ParseFile(const std::wstring& filename)
{
    auto    file = ReadFileSync(filename);
    size_t  size = file.size();
    char*   pText = (char *)&file[0];

    std::vector<std::string> lineList;
    char *pStartStr = pText;

    for (size_t byte = 0; byte < size - 1;)
    {
        if ((0xd == pText[byte]) && ((0xa == pText[byte + 1])))
        {
            pText[byte] = 0x00;
            pText[byte + 1] = 0x00;
            std::string str(pStartStr);
            if (str != "")
            {
                bool comment = ('/' == pStartStr[0] && '/' == pStartStr[1]);
                if (!comment)
                {
                    lineList.push_back(str);
                }
            }
            pStartStr = &pText[byte + 2];
            byte++;
        }

        byte++;
    }

    for (auto it = lineList.begin(); it != lineList.end(); it++)
    {
        auto tokens = TokenizeDefault(*it);
    }
}

void FileLoad::EnumerateAppFiles()
{
    // Get the app's installation folder
    StorageFolder^ appFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;

    create_task(appFolder->GetFilesAsync()).then([=](IVectorView<StorageFile^>^ filesInFolder) {
        //Iterate over the results and print the list of files
        // to the visual studio output window
        for (auto it = filesInFolder->First(); it->HasCurrent; it->MoveNext())
        {
            StorageFile^ file = it->Current;
            String^ output = file->Name + "\n";
            OutputDebugString(output->Begin());
        }
    });
}