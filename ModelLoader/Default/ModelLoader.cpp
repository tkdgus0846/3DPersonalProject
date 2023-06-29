#include "stdafx.h"

#include "AssimpModel.h"
#include "DataParsing.h"

//vector<CAssimpModel*> modelVector;

void Load(path modelPath, wstring extractPath, CAssimpModel::TYPE type)
{
    // $(SolutionDir)Models\ 경로에 있는 모든 모델파일들을 불러온다. 모델 파일 하나당 파일 하나!
    for (const directory_entry& entry : recursive_directory_iterator(modelPath))
    {
        if (entry.path().extension() == ".fbx")
        {
            cout << entry.path() << endl;
            CAssimpModel* model = CAssimpModel::Create(entry.path().string(), type);
            
            wstring path = extractPath;
            path = path + entry.path().filename().wstring();

            size_t lastDotIndex = path.find_last_of(L".");
            wstring PathExceptExtension = path.substr(0, lastDotIndex); // 확장자를 제외한 파일명
            wstring extension = path.substr(lastDotIndex + 1); // 확장자
            
            std::filesystem::path filePath = path;
            wstring fileName = filePath.stem().wstring();

            wstring PathWithFileName = PathExceptExtension + L"/" + fileName;
            path = PathWithFileName + L".dat";


            wstring folderPath = PathExceptExtension;  // 폴더 경로

            // 폴더 생성
            if (!CreateDirectory(folderPath.c_str(), NULL))
            {
                // 폴더 생성 실패
                if (GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    MSG_BOX("Failed to Create Folder");
                    return;
                }
            }

            CDataParsing::Save_File(path.c_str(), model);
            Safe_Delete(model);
        }
    }
}

void Close()
{
    /*for (auto& model : modelVector)
    {
        Safe_Delete(model);
    }*/
}

int main()
{
   // Load("../../Models/AnimModels/", L"../../ExtractModels/AnimModels/", CAssimpModel::TYPE_ANIM);
    Load("../../Models/NonAnimModels/", L"../../ExtractModels/NonAnimModels/", CAssimpModel::TYPE_NONANIM);
    Close();
    return 0;
}


