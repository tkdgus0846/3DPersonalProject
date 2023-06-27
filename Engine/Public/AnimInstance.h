#pragma once
#include "Component.h"

BEGIN(Engine)

struct ENGINE_DLL AnimNode
{
	enum TYPE
	{
		SINGLE_LOOP, SINGLE_NONLOOP, MULTIPLE_LOOP, MULTIPLE_NONLOOP
	};
	TYPE eAnimType;
	_bool bLoop;
	vector<_uint> AnimIndices;
	_bool bProceedSelf;
	_uint iCurIndex;
};

class ENGINE_DLL CAnimInstance final : public CComponent
{
private:
	CAnimInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CAnimInstance(const CAnimInstance& rhs);
	virtual ~CAnimInstance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_double TimeDelta) override;

public:
	// 여기서 말하는 애니메이션이란건 애니메이션 묶음들을 얘기하는것이다.
	void Push_Animation(const string & name, const AnimNode& animNode);
	void Proceed_Animation();
	void Apply_Animation(const string & name);
	
	_bool Animation_Finished();

private:
	void Reset_Animation();
	_uint Key(const string & name, _int index = 0) { return m_AnimationNode[name].AnimIndices[index]; }

private:
	unordered_map<string, AnimNode> m_AnimationNode;
	class CModel*					m_pModel = { nullptr };
	string							m_pCurAnimationName;

public:
	static CAnimInstance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAnimInstance* Clone(void* pArg);
	virtual void Free() override;
};

END
