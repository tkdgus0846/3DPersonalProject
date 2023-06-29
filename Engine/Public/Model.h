#pragma once

#include "Component.h"

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
	void Set_AnimIndex(_uint iAnimIndex) {
		if (iAnimIndex >= m_iNumAnimations || Is_Changing_Animation())
			return;
		// 위에 잠깐만 주석
		m_iCurrentAnimIndex = iAnimIndex;
	}
	_uint Get_NumAnimations() const { return m_iNumAnimations; }

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

	virtual void Tick(_double TimeDelta) {};
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_double TimeDelta);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType MaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	void Loop_Animation(_uint iAnimIndex, _bool bLoop); 
	void Loop_Animation(const vector<_uint>& indexVec, _bool bLoop);
	vector<class CAnimation*>* Get_Animations() { return &m_Animations; }
	_bool Is_Changing_Animation() const { return m_iPrevAnimIndex != m_iCurrentAnimIndex; }
	void Erase_LastFrame_Animation(_uint iIndex);
	void Erase_Frames_LessTime(_uint iIndex, _double time);
	void Remove_Mesh(const string& name, _uint iBoneNum);

private: /* For.Meshes  */
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private: /* For.Materials */
	_uint					m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>	m_Materials;

private: /* For.Bones*/
	vector<class CBone*>	m_Bones;
	_uint					m_iNumBones;

public:
	typedef vector<class CBone*>	BONES;

private:
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iPrevAnimIndex = { 0 };
	_double							m_lerpTimeAcc = { 0.0 };

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