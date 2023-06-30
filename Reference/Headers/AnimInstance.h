#pragma once
#include "Component.h"

BEGIN(Engine)

enum ANIMTYPE
{
	ANIM_UPPERBODY, ANIM_LOWERBODY, ANIM_ALLBODY
};

struct ENGINE_DLL AnimNode
{
	// �ִϸ��̼� ������ ��ü�� �Ұ��� ��ü���Ұ���
	
	_bool bLoop;
	vector<_uint> AnimIndices;
	vector<_double> eraseLessTime;
	_bool bProceedSelf = { true };
	_uint iCurIndex = { 0 };
	_bool bMotionCancel = { false };
	_bool bResetAfterFinish = { true };

	// bLoop�� false �ϰ�쿡 ������ �� ������ ���ư� ���� ��带 ����Ű�� �ؾ��Ѵ�.
	string retAnimNode;
	string nextAnimNode;
	string upperBodyAnimNode;
	string lowerBodyAnimNode;
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
	void Apply_Animation(const string & name, ANIMTYPE eType = ANIM_ALLBODY);
	_bool Next_Animation(ANIMTYPE eType = ANIM_ALLBODY);
	const string& Get_CurAnimation_Name() const { return m_pCurAnimationName; }
	
	_bool Animation_Finished(ANIMTYPE eType = ANIM_ALLBODY);
	const string& Get_NextNode_Name(ANIMTYPE eType = ANIM_ALLBODY);

private:
	// ���� �ִϸ��̼����� �Ѿ�� ������ �����Ȳ ������ �ʱ�ȭ������.
	void Reset_Animation(const string& name);
	_uint Key(const string & name, _int index = 0) { return m_AnimationNode[name].AnimIndices[index]; }

	void Play_Animation(_double TimeDelta, AnimNode* node);
	void Play_UpperBody_Animation(_double TimeDelta, AnimNode* node);
	void Play_LowerBody_Animation(_double TimeDelta, AnimNode* node);

private:
	unordered_map<string, AnimNode> m_AnimationNode;
	class CModel*					m_pModel = { nullptr };

	/* ���� �ѹ��� ���� �ִϸ��̼��� ���� ������ ��.*/
	string							m_pCurAnimationName;
	AnimNode*						m_pCurAnimNode = { nullptr };
	_bool							m_bProceed = { false };
	_bool							m_bAnimationFinished = { false };

	/* ��ü�� ���� �ִϸ��̼��� ���� ������ ��.*/
	string							m_pCurUpperAnimationName;
	AnimNode*						m_pCurUpperAnimNode = { nullptr };
	_bool							m_bUpperProceed = { false };
	_bool							m_bUpperAnimationFinished = { false };

	/* ��ü�� ���� �ִϸ��̼��� ���� ������ ��. */
	string							m_pCurLowerAnimationName;
	AnimNode*						m_pCurLowerAnimNode = { nullptr };
	_bool							m_bLowerProceed = { false };
	_bool							m_bLowerAnimationFinished = { false };

	



public:
	static CAnimInstance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAnimInstance* Clone(void* pArg);
	virtual void Free() override;
};

END