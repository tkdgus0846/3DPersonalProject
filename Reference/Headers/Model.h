#pragma once

#include "AnimInstance.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent, public IReadable
{
	friend class CAnimInstance;
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_float4x4 Get_PivotMatrix() {
		return m_PivotMatrix;
	}

	const class CBone* Get_Bone(const char* pBoneName);

public:
	void Set_AnimIndex(_uint iAnimIndex, ANIMTYPE eType = ANIM_ALLBODY);
	_uint Get_NumAnimations() const { return m_iNumAnimations; }

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

	virtual void Tick(_double TimeDelta) {};
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_double TimeDelta);
	void Play_UpperBody_Animation(_double TimeDelta);
	void Play_LowerBody_Animation(_double TimeDelta);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType MaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	void Loop_Animation(_uint iAnimIndex, _bool bLoop); 
	void Loop_Animation(const vector<_uint>& indexVec, _bool bLoop);
	vector<class CAnimation*>* Get_Animations() { return &m_Animations; }

	_bool Is_Changing_Animation(ANIMTYPE eType = ANIM_ALLBODY); 
	void Erase_LastFrame_Animation(_uint iIndex);
	void Erase_Frames_LessTime(_uint iIndex, _double time);
	void Remove_Mesh(const string& name, _uint iBoneNum);
	void Motion_Cancel() { m_iPrevAnimIndex = m_iCurrentAnimIndex; }

	void Collect_BodyBones_UpperLower(_int iUpperIndex, _int iLowerIndex);
	
private:
	// 매개변수로 가장 부모의 인덱스를 넣어주면 
	_bool Collect_UpperBodyBones_ByParentIndex(_int iCurIndex, _int iParentIndex);
	_bool Collect_LowerBodyBones_ByParentIndex(_int iCurIndex, _int iParentIndex);

private: /* For.Meshes  */
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private: /* For.Materials */
	_uint					m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>	m_Materials;

private: /* For.Bones*/
	vector<class CBone*>	m_Bones;

	/* 상하체 뼈 정보들 인덱스로 가지고 있음.*/
	unordered_set<_int>	m_UpperBodyBoneIndex;
	unordered_set<_int>	m_LowerBodyBoneIndex;

	_uint					m_iNumBones;

public:
	typedef vector<class CBone*>	BONES;

private:
	_int							m_iCurrentAnimIndex = { -1 };
	_int							m_iPrevAnimIndex = { -1 };
	_double							m_lerpTimeAcc = { 0.0 };

	_int							m_iUpperCurrentAnimIndex = { -1 };
	_int							m_iUpperPrevAnimIndex = { -1 };
	_double							m_UpperLerpTimeAcc = { 0.0 };

	_int							m_iLowerCurrentAnimIndex = { -1 };
	_int							m_iLowerPrevAnimIndex = { -1 };
	_double							m_LowerLerpTimeAcc = { 0.0 };

	////////////////////////////////////////////////////////////

	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

	/* 플레이어 모델 추출이 안되어서 직접 텍스쳐 경로를 넣어주는 작업을 한다.. */
	_bool							m_IsPlayer = { false };

private:
	_float4x4						m_PivotMatrix;

private:
	HRESULT Ready_Meshes(ModelParsingData* parsingData, TYPE eType, _fmatrix PivotMatrix);
	/*HRESULT Ready_Materials(ModelParsingData* parsingData);*/
	HRESULT Ready_Bones(ModelParsingData* parsingData);
	HRESULT Ready_Animations(ModelParsingData* parsingData);

	_bool Directly_Push_Material(char* newStr, _int iMatNum, _int iTextureType);


public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) final;
};

END