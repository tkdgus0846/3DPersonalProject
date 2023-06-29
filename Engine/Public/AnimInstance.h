#pragma once
#include "Component.h"

BEGIN(Engine)

struct ENGINE_DLL AnimNode
{
	_bool bLoop;
	vector<_uint> AnimIndices;
	vector<_double> eraseLessTime;
	_bool bProceedSelf = { true };
	_uint iCurIndex = { 0 };
	

	// bLoop가 false 일경우에 동작이 다 끝나고 돌아갈 다음 노드를 가르키게 해야한다.
	string retAnimNode;
	string nextAnimNode;
};

//
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
	void Push_Animation(const string & name, const AnimNode& animNode, string nextAnimNode = "", string retAnimNode = "");
	void Proceed_Animation();
	void Apply_Animation(const string & name);
	_bool Next_Animation();
	const string& Get_CurAnimation_Name() const { return m_pCurAnimationName; }
	
	_bool Animation_Finished();
	const string& Get_NextNode_Name() const { return m_pCurAnimNode->nextAnimNode; }

private:
	// 다음 애니메이션으로 넘어갈때 원래의 진행상황 정보를 초기화해주자.
	void Reset_Animation(const string& name);
	_uint Key(const string & name, _int index = 0) { return m_AnimationNode[name].AnimIndices[index]; }

private:
	unordered_map<string, AnimNode> m_AnimationNode;
	class CModel*					m_pModel = { nullptr };
	string							m_pCurAnimationName;
	AnimNode*						m_pCurAnimNode = { nullptr };
	_bool							m_bProceed = { false };
	_bool							m_bAnimationFinished = { false };



public:
	static CAnimInstance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAnimInstance* Clone(void* pArg);
	virtual void Free() override;
};

END
