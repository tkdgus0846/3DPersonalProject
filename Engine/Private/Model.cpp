#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include <DataParsing.h>

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iNumBones(rhs.m_iNumBones)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pOriginalAnimation : rhs.m_Animations)
		m_Animations.push_back(pOriginalAnimation->Clone());

	for (auto& pOriginalBone : rhs.m_Bones)
	{
		m_Bones.push_back(pOriginalBone->Clone());
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_AddRef(pTexture);
	}
}

const CBone* CModel::Get_Bone(const char* pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
		{
			if (!strcmp(pBoneName, pBone->Get_Name()))
				return true;
			else
				return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter);
}

void CModel::Set_AnimIndex(_uint iAnimIndex, ANIMTYPE eType)
{
	if (iAnimIndex >= m_iNumAnimations || Is_Changing_Animation(eType)) return;


	switch (eType)
	{
	case ANIM_ALLBODY:
		m_iCurrentAnimIndex = iAnimIndex;
		break;
	case ANIM_UPPERBODY:
		m_iUpperCurrentAnimIndex = iAnimIndex;
		break;
	case ANIM_LOWERBODY:
		m_iLowerCurrentAnimIndex = iAnimIndex;
		break;
	}
	
}

HRESULT CModel::Initialize_Prototype(const char* pModelFilePath, _fmatrix PivotMatrix)
{
	//_uint		iFlag = 0;

	/*if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);*/

	/*if (nullptr == m_pAIScene)
		return E_FAIL;*/

	/*char ch[MAX_PATH];
	strcpy(ch, pModelFilePath);*/
	string path = pModelFilePath;
	wstring finalPath = CConversion::StringToWstring(path);

	if (path.compare("../../ExtractModels/AnimModels/VampireMale_Standard_LOD00_rig/VampireMale_Standard_LOD00_rig.dat") == 0)
	{
		m_IsPlayer = true;
	}
	
	ModelParsingData* myData = (ModelParsingData*)CDataParsing::Load_File(finalPath.c_str(), this);

	if (myData->MaterialPaths.size() > 0)
		int i = 0;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_Bones(myData)))
		return E_FAIL;

	TYPE eType = TYPE_END;

	if (m_iNumAnimations > 0) eType = TYPE_ANIM;
	else eType = TYPE_NONANIM;

	if (FAILED(Ready_Meshes(myData, eType, PivotMatrix)))
		return E_FAIL;

	//if (FAILED(Ready_Materials(myData)))
	//	return E_FAIL;

	if (FAILED(Ready_Animations(myData)))
		return E_FAIL;


	Safe_Delete(myData);



	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Play_Animation(_double TimeDelta)
{

	if (Is_Changing_Animation())
	{
		m_lerpTimeAcc += TimeDelta;
		_int result = m_Animations[m_iPrevAnimIndex]->Lerp_NextAnimation(m_Animations[m_iCurrentAnimIndex], m_Bones, 0.2, m_lerpTimeAcc);

		if (result == 1)
		{
			m_lerpTimeAcc = 0.0;
			m_Animations[m_iPrevAnimIndex]->Reset_Channels();
			m_iPrevAnimIndex = m_iCurrentAnimIndex;
		}
		else if (result == 0)
		{
			for (auto& pBone : m_Bones)
			{
				pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
			}
			return;
		}
	}

	/* 현재 애니메이션에서 사용하는 뼈들을 찾아서 해당 뼈들의 TransformationMatrix를 갱신한다. */
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_Bones, TimeDelta);

	/* 모델에 표현되어있는 모든 뼈들의 CombinedTransformationMatrix */
	//  모델의 모든 뼈를 돌면서 자기 부모의 컴바인드 . 이 컴바인드를 만들어주는 
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}
}

void CModel::Play_UpperBody_Animation(_double TimeDelta)
{
	if (Is_Changing_Animation(ANIM_UPPERBODY))
	{
		m_UpperLerpTimeAcc += TimeDelta;
		_int result = m_Animations[m_iUpperPrevAnimIndex]->Lerp_NextAnimation(m_Animations[m_iUpperCurrentAnimIndex], m_Bones, 0.2, m_UpperLerpTimeAcc);

		if (result == 1)
		{
			m_UpperLerpTimeAcc = 0.0;
			m_Animations[m_iUpperPrevAnimIndex]->Reset_Channels();
			m_iUpperPrevAnimIndex = m_iUpperCurrentAnimIndex;
		}
		else if (result == 0)
		{
			for (auto& pBone : m_Bones)
			{
				pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
			}
			return;
		}
	}

	m_Animations[m_iUpperCurrentAnimIndex]->Invalidate_TransformationMatrix_Upper(m_Bones, TimeDelta, m_UpperBodyBoneIndex);
 
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}
}

void CModel::Play_LowerBody_Animation(_double TimeDelta)
{
	if (Is_Changing_Animation(ANIM_LOWERBODY))
	{
		m_LowerLerpTimeAcc += TimeDelta;
		_int result = m_Animations[m_iLowerPrevAnimIndex]->Lerp_NextAnimation(m_Animations[m_iLowerCurrentAnimIndex], m_Bones, 0.2, m_LowerLerpTimeAcc);

		if (result == 1)
		{
			m_LowerLerpTimeAcc = 0.0;
			m_Animations[m_iLowerPrevAnimIndex]->Reset_Channels();
			m_iLowerPrevAnimIndex = m_iLowerCurrentAnimIndex;
		}
		else if (result == 0)
		{
			for (auto& pBone : m_Bones)
			{
				pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
			}
			return;
		}
	}

	m_Animations[m_iLowerCurrentAnimIndex]->Invalidate_TransformationMatrix_Lower(m_Bones, TimeDelta, m_LowerBodyBoneIndex);

	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}
}

HRESULT CModel::Bind_Material(CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType MaterialType)
{
	if (iMeshIndex >= m_iNumMeshes ||
		MaterialType >= AI_TEXTURE_TYPE_MAX)
		return E_FAIL;

	if (m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType] == nullptr) return S_OK;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	
	/* iMeshIndex번째 메시가 사용하느 ㄴ뼈들의 행려을 가져와서 BoneMatrices에 넣어준다. */
	m_Meshes[iMeshIndex]->Get_Matrices(m_Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);

	return S_OK;
}

void CModel::Loop_Animation(_uint iAnimIndex, _bool bLoop)
{
	m_Animations[iAnimIndex]->Set_Loop(bLoop);
}

void CModel::Loop_Animation(const vector<_uint>& indexVec, _bool bLoop)
{
	for (auto& item : indexVec)
	{
		m_Animations[item]->Set_Loop(bLoop);
	}
}

_bool CModel::Is_Changing_Animation(ANIMTYPE eType)
{
	if (eType == ANIM_ALLBODY)
	{
		if (m_iPrevAnimIndex == -1)
			m_iPrevAnimIndex = m_iCurrentAnimIndex;

		return m_iPrevAnimIndex != m_iCurrentAnimIndex;
	}
	else if (eType == ANIM_UPPERBODY)
	{
		if (m_iUpperPrevAnimIndex == -1)
			m_iUpperPrevAnimIndex = m_iUpperCurrentAnimIndex;

		return m_iUpperPrevAnimIndex != m_iUpperCurrentAnimIndex;
	}
	else
	{
		if (m_iLowerPrevAnimIndex == -1)
			m_iLowerPrevAnimIndex = m_iLowerCurrentAnimIndex;

		return m_iLowerPrevAnimIndex != m_iLowerCurrentAnimIndex;
	}	
}

void CModel::Erase_LastFrame_Animation(_uint iIndex)
{
	m_Animations[iIndex]->Erase_LastFrame_Animation();


}

void CModel::Erase_Frames_LessTime(_uint iIndex, _double time)
{
	m_Animations[iIndex]->Erase_Frames_LessTime(time);
}

void CModel::Remove_Mesh(const string& name, _uint iBoneNum)
{
	for (auto iter = m_Meshes.begin(); iter != m_Meshes.end(); iter++)
	{
		if (name.compare((*iter)->GetszName()) == 0 && (*iter)->GetBoneNum() == iBoneNum)
		{
			Safe_Release(*iter);
			m_Meshes.erase(iter);
			m_iNumMeshes--;
			return;
		}
	}
}

unordered_set<_int>	TmpBodyBoneIndex;

void CModel::Collect_BodyBones_UpperLower(_int iUpperIndex, _int iLowerIndex)
{
	/// for문 돌면서 싹다 체크
	for (_int i = m_Bones.size() - 1; i > iUpperIndex; i--)
	{
		_bool bSuccess = Collect_UpperBodyBones_ByParentIndex(i, iUpperIndex);

		if (bSuccess)
		{
			m_UpperBodyBoneIndex.insert(TmpBodyBoneIndex.begin(), TmpBodyBoneIndex.end());
		}
		
		TmpBodyBoneIndex.clear();
	}

	for (_int i = m_Bones.size() - 1; i > iLowerIndex; i--)
	{
		_bool bSuccess = Collect_LowerBodyBones_ByParentIndex(i, iLowerIndex);

		if (bSuccess)
		{
			m_LowerBodyBoneIndex.insert(TmpBodyBoneIndex.begin(), TmpBodyBoneIndex.end());
		}
		
		TmpBodyBoneIndex.clear();
	}

	m_UpperBodyBoneIndex.insert(iUpperIndex);
	m_LowerBodyBoneIndex.insert(iLowerIndex);

	cout << "//////////////Upper Body Bones////////////// " << endl;
	for (auto& item : m_UpperBodyBoneIndex)
	{
		string str = m_Bones[item]->Get_Name();
		cout << str << endl;
	}
	cout << "\n\n//////////////Lower Body Bones////////////// " << endl;
	for (auto& item : m_LowerBodyBoneIndex)
	{
		string str = m_Bones[item]->Get_Name();
		cout << str << endl;
	}
	
}

_bool CModel::Collect_UpperBodyBones_ByParentIndex(_int iCurIndex, _int iParentIndex)
{
	if (iCurIndex == iParentIndex) return true;
	if (iCurIndex < iParentIndex || iCurIndex < 0) return false;

	TmpBodyBoneIndex.insert(iCurIndex);
	return Collect_UpperBodyBones_ByParentIndex(m_Bones[iCurIndex]->Get_ParentIndex(), iParentIndex);
}

_bool CModel::Collect_LowerBodyBones_ByParentIndex(_int iCurIndex, _int iParentIndex)
{
	if (iCurIndex == iParentIndex) return true;
	if (iCurIndex < iParentIndex || iCurIndex < 0) return false;

	if (m_UpperBodyBoneIndex.find(iCurIndex) != m_UpperBodyBoneIndex.end())
		return false;

	TmpBodyBoneIndex.insert(iCurIndex);
	return Collect_LowerBodyBones_ByParentIndex(m_Bones[iCurIndex]->Get_ParentIndex(), iParentIndex);
}

HRESULT CModel::Ready_Meshes(ModelParsingData* parsingData,  TYPE eType, _fmatrix PivotMatrix)
{

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_Bones, &parsingData->MeshDatas[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bones(ModelParsingData* parsingData)
{
	for (int i = 0; i < m_iNumBones; i++)
	{
		CBone* pBone = CBone::Create(&parsingData->BoneDatas[i]);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(ModelParsingData* parsingData)
{
	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(&parsingData->AnimationDatas[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

_bool CModel::Directly_Push_Material(char* newStr, _int iMatNum, _int iTextureType)
{
	_bool result = false;
	if (m_IsPlayer)
	{
		if (iMatNum == 0)
		{
			if (iTextureType == aiTextureType_DIFFUSE)
			{
				strcpy(newStr, "../../Models/AnimModels/VampireMale_Standard_LOD00_rig/Vampire_male_bc.png");
				result = true;
			}
			if (iTextureType == aiTextureType_EMISSIVE)
			{
				strcpy(newStr, "../../Models/AnimModels/VampireMale_Standard_LOD00_rig/Vampire_male_mask.png");
				result = true;
			}
			if (iTextureType == aiTextureType_METALNESS)
			{
				strcpy(newStr, "../../Models/AnimModels/VampireMale_Standard_LOD00_rig/Vampire_male_m.png");
				result = true;
			}
			if (iTextureType == aiTextureType_NORMALS)
			{
				strcpy(newStr, "../../Models/AnimModels/VampireMale_Standard_LOD00_rig/Vampire_male_n.png");
				result = true;
			}
		}

		if (iMatNum == 1)
		{
			if (iTextureType == aiTextureType_DIFFUSE)
			{
				strcpy(newStr, "../../Models/AnimModels/VampireMale_Standard_LOD00_rig/VampireMale_VampireLord_Cloak01_bc.png");
				result = true;
			}
			if (iTextureType == aiTextureType_METALNESS)
			{
				strcpy(newStr, "../../Models/AnimModels/VampireMale_Standard_LOD00_rig/VampireMale_VampireLord_Cloak01_m.png");
				result = true;
			}
			if (iTextureType == aiTextureType_NORMALS)
			{
				strcpy(newStr, "../../Models/AnimModels/VampireMale_Standard_LOD00_rig/VampireMale_VampireLord_Cloak01_n.png");
				result = true;
			}
		}
	}

	return result;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_Release(pTexture);
	}

	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();
}

ParsingData* CModel::Load_Data(HANDLE handle, ParsingData* data)
{
	///////////////////////////////////////
	////////////////////////////////////////
	///////////////////////////////// 아래부터 파일불러오기
	ModelParsingData* myData = new ModelParsingData;

	DWORD dwByte = 0;
	char str[MAX_PATH];

	ReadFile(handle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	ReadFile(handle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	ReadFile(handle, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);
	ReadFile(handle, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	myData->iNumMeshes = m_iNumMeshes; // 저장
	myData->iNumMaterials = m_iNumMaterials; // 저장
	myData->iNumAnimations = m_iNumAnimations;
	myData->iNumBones = m_iNumBones;

	///////// 텍스쳐 경로 저장/////////////// 요건 나중에
	/*m_Materials.push_back();
	for (auto path : m_MaterialPaths)
	{
		if (path == nullptr) continue;
		str = path;
		WriteFile(handle, &str, sizeof(str), &dwByte, nullptr);
	}*/

	for (int i = 0; i < m_iNumMaterials; i++)
	{
		MESHMATERIAL			MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			ReadFile(handle, &str, sizeof(str), &dwByte, nullptr);

			if (strcmp("NULL", str) == 0)
			{
				if (Directly_Push_Material(str, i, j) == false)
					continue;
			}

			wstring wszFullPath = CConversion::StringToWstring(str);

			myData->MaterialPaths.push_back(string(str));
			MeshMaterial.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, wszFullPath.c_str(), 1);
			if (nullptr == MeshMaterial.pMtrlTexture[j])
				return nullptr;
		}
		m_Materials.push_back(MeshMaterial);
	}

	
	// 리드파일 해서 myData로 올리기만 하면됨 .

	CMesh mesh;
	CBone bone;
	CAnimation Animation;

	///////// 메쉬 정보 저장///////////////
	for (int i = 0; i < m_iNumMeshes; i++)
	{
		mesh.Load_Data(handle, myData);
	}

	///////// 뼈 정보 저장///////////////
	for (int i = 0; i < m_iNumBones; i++)
	{
		bone.Load_Data(handle, myData);
	}

	///////// 애니메이션 정보 저장///////////////
	for (int i = 0; i < m_iNumAnimations; i++)
	{
		Animation.Load_Data(handle, myData);
	}

	return myData;
}
