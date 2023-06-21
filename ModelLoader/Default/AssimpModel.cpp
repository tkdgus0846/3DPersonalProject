#include "stdafx.h"
#include "AssimpModel.h"
#include "AssimpMesh.h"
#include "AssimpBone.h"
#include "AssimpAnimation.h"

CAssimpModel::CAssimpModel()
{
}

CAssimpModel::~CAssimpModel()
{
	for (auto& Material : m_MaterialPaths)
	{
		for (auto& pPath : m_MaterialPaths)
			Safe_Delete(pPath);
	}

	m_MaterialPaths.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Delete(pMesh);

	m_Meshes.clear();

	for (auto& pBone : m_Bones)
		Safe_Delete(pBone);

	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Delete(pAnimation);

	m_Animations.clear();
}

HRESULT CAssimpModel::Initialize_Prototype(const string& pModelFilePath, TYPE eMeshType, _fmatrix PivotMatrix)
{
	_uint		iFlag = 0;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (TYPE_NONANIM == eMeshType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;

	m_iNumBones = m_Bones.size();

	if (FAILED(Ready_Meshes(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath.c_str())))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}



HRESULT CAssimpModel::Ready_Meshes(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CAssimpMesh* pMesh = CAssimpMesh::Create(m_Bones, m_pAIScene->mMeshes[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CAssimpModel::Ready_Materials(const char* pModelFilePath)
{
	/* 현재 모델에게 부여할 수 있는 재질(Diffuse, Normal, Specular etc) 텍스쳐의 갯수. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	m_MaterialPaths.resize(AI_TEXTURE_TYPE_MAX * m_iNumMaterials);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	strPath;

			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char		szDrive[MAX_PATH] = "";
			char		szDirectory[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char* szFullPath = new char[MAX_PATH]; 
			strcpy(szFullPath, szDrive);
			strcat(szFullPath, szDirectory);
			strcat(szFullPath, szFileName);
			strcat(szFullPath, szExt);

			m_MaterialPaths[i* AI_TEXTURE_TYPE_MAX + j] = (szFullPath);
		}
	}

	return S_OK;
}

HRESULT CAssimpModel::Ready_Bones(aiNode* pNode, CAssimpBone* pParent)
{
	CAssimpBone* pBone = CAssimpBone::Create(pNode, pParent, m_Bones.size());
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (size_t i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_Bones(pNode->mChildren[i], pBone);
	}

	return S_OK;
}

HRESULT CAssimpModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAssimpAnimation* pAnimation = CAssimpAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CAssimpModel* CAssimpModel::Create(const string& pModelFilePath, TYPE eMeshType, _fmatrix PivotMatrix)
{
	CAssimpModel* pInstance = new CAssimpModel();

if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eMeshType, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CAssimpModel");
		Safe_Delete(pInstance);
	}
	return pInstance;
}

ParsingData* CAssimpModel::Save_Data(HANDLE handle, ParsingData* data)
{
	ModelParsingData* modelParsingData = new ModelParsingData;

	DWORD		dwByte = 0;
	
	modelParsingData->iNumMeshes = m_iNumMeshes; // 저장
	modelParsingData->iNumMaterials = m_iNumMaterials; // 저장
	modelParsingData->iNumAnimations = m_iNumAnimations;
	modelParsingData->iNumBones = m_iNumBones;

	_int index = 0;
	for (int firstIndex = 0; firstIndex < m_iNumMaterials; firstIndex++)
	{
		for (int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			modelParsingData->MaterialPaths.push_back(m_MaterialPaths[index++]);
		}
	}
	
	
	modelParsingData->iNumAnimations = m_iNumAnimations; // 저장

	for (auto mesh : m_Meshes)
	{
		modelParsingData->MeshDatas.reserve(m_Meshes.size());
		mesh->Save_Data(handle, modelParsingData);
	}

	for (auto bone : m_Bones)
	{
		modelParsingData->BoneDatas.reserve(m_Bones.size());
		bone->Save_Data(handle, modelParsingData);
	}

	
	for (auto animation : m_Animations)
	{
		modelParsingData->AnimationDatas.reserve(m_Animations.size());
		animation->Save_Data(handle, modelParsingData);
	}

	///////////////////////////////////////
	////////////////////////////////////////
	///////////////////////////////// 아래부터 파일저장

	WriteFile(handle, &modelParsingData->iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	WriteFile(handle, &modelParsingData->iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	WriteFile(handle, &modelParsingData->iNumAnimations, sizeof(_uint), &dwByte, nullptr);
	WriteFile(handle, &modelParsingData->iNumBones, sizeof(_uint), &dwByte, nullptr);

	char str[MAX_PATH];

	///////// 텍스쳐 경로 저장///////////////
	for (auto path : modelParsingData->MaterialPaths)
	{
		if (path == nullptr)
			strcpy(str, "NULL");
		else				
			strcpy(str, path);

		WriteFile(handle, &str, sizeof(str), &dwByte, nullptr);
	}

	///////// 메쉬 정보 저장///////////////
	for (auto meshData : modelParsingData->MeshDatas)
	{
		strcpy(str, meshData.szName);
		//str = meshData.szName;
		WriteFile(handle, &str, sizeof(str), &dwByte, nullptr);
		WriteFile(handle, &meshData.iMaterialIndex, sizeof(meshData.iMaterialIndex), &dwByte, nullptr);
		WriteFile(handle, &meshData.iNumVertices, sizeof(meshData.iNumVertices), &dwByte, nullptr);
		WriteFile(handle, &meshData.iNumIndices, sizeof(meshData.iNumIndices), &dwByte, nullptr);

		for (auto Index : meshData.Indices)
		{
			WriteFile(handle, &Index, sizeof(Index), &dwByte, nullptr);
		}

		for (auto Vertex : meshData.Vertices)
		{
			WriteFile(handle, &Vertex, sizeof(Vertex), &dwByte, nullptr);
		}

		WriteFile(handle, &meshData.iNumBones, sizeof(meshData.iNumBones), &dwByte, nullptr);

		for (auto BoneIndex : meshData.BoneIndices)
		{
			WriteFile(handle, &BoneIndex, sizeof(BoneIndex), &dwByte, nullptr);
		}
	}

	///////// 뼈 정보 저장///////////////
	for (auto BoneData : modelParsingData->BoneDatas)
	{
		strcpy(str, BoneData.szName);
		//str = BoneData.szName;
		WriteFile(handle, &str, sizeof(str), &dwByte, nullptr);

		WriteFile(handle, &BoneData.TransformationMatrix, sizeof(BoneData.TransformationMatrix), &dwByte, nullptr);
		WriteFile(handle, &BoneData.CombinedTransformationMatrix, sizeof(BoneData.CombinedTransformationMatrix), &dwByte, nullptr);
		WriteFile(handle, &BoneData.OffsetMatrix, sizeof(BoneData.OffsetMatrix), &dwByte, nullptr);
		WriteFile(handle, &BoneData.iParentIndex, sizeof(BoneData.iParentIndex), &dwByte, nullptr);
		WriteFile(handle, &BoneData.iIndex, sizeof(BoneData.iIndex), &dwByte, nullptr);
	}

	///////// 애니메이션 정보 저장///////////////
	for (auto AnimData : modelParsingData->AnimationDatas)
	{
		WriteFile(handle, &AnimData.iNumChannels, sizeof(AnimData.iNumChannels), &dwByte, nullptr);

		for (auto ChannelData : AnimData.ChannelDatas)
		{
			strcpy(str, ChannelData.szName);
			//str = ChannelData.szName;
			WriteFile(handle, &str, sizeof(str), &dwByte, nullptr);

			WriteFile(handle, &ChannelData.iNumKeyFrames, sizeof(ChannelData.iNumKeyFrames), &dwByte, nullptr);

			for (int i = 0; i < ChannelData.iNumKeyFrames; i++)
			{
				WriteFile(handle, &ChannelData.KeyFrames[i], sizeof(ChannelData.KeyFrames[i]), &dwByte, nullptr);
			}
			
			WriteFile(handle, &ChannelData.iBoneIndex, sizeof(ChannelData.iBoneIndex), &dwByte, nullptr);
		}

		strcpy(str, AnimData.szName);
		//str = AnimData.szName;
		WriteFile(handle, &str, sizeof(str), &dwByte, nullptr);
		
		WriteFile(handle, &AnimData.Duration, sizeof(AnimData.Duration), &dwByte, nullptr);
		WriteFile(handle, &AnimData.TickPerSecond, sizeof(AnimData.TickPerSecond), &dwByte, nullptr);
	}


	Safe_Delete(modelParsingData);

	return nullptr;
}
