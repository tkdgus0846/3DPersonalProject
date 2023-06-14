#include "stdafx.h"

#include "AssimpModel.h"
#include "DataParsing.h"

vector<CAssimpModel*> modelVector;

void Load(path modelPath)
{
    // $(SolutionDir)Models\ 경로에 있는 모든 모델파일들을 불러온다. 모델 파일 하나당 파일 하나!

    for (const directory_entry& entry : recursive_directory_iterator(modelPath))
    {
        if (entry.path().extension() == ".fbx")
        {
            CAssimpModel* model = CAssimpModel::Create(entry.path().string());
            
            CDataParsing::Save_File(L"이름", model);
            modelVector.push_back(model);
        }
    }
}

void Close()
{
    for (auto& model : modelVector)
    {
        Safe_Delete(model);
    }
}

int main()
{
    Load("../../Models/");
    Close();
}


