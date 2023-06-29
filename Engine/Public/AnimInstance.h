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
	

	// bLoop�� false �ϰ�쿡 ������ �� ������ ���ư� ���� ��带 ����Ű�� �ؾ��Ѵ�.
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
	// ���⼭ ���ϴ� �ִϸ��̼��̶��� �ִϸ��̼� �������� ����ϴ°��̴�.
	void Push_Animation(const string & name, const AnimNode& animNode, string nextAnimNode = "", string retAnimNode = "");
	void Proceed_Animation();
	void Apply_Animation(const string & name);
	_bool Next_Animation();
	const string& Get_CurAnimation_Name() const { return m_pCurAnimationName; }
	
	_bool Animation_Finished();
	const string& Get_NextNode_Name() const { return m_pCurAnimNode->nextAnimNode; }

private:
	// ���� �ִϸ��̼����� �Ѿ�� ������ �����Ȳ ������ �ʱ�ȭ������.
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
