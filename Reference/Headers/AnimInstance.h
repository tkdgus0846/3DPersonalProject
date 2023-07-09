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
	vector<_uint> AnimIndices; // � �ִϸ��̼ǵ��� ��������
	vector<_float> eraseLessTime; // ������ ���ʺ��� �ڸ�����
	_bool bProceedSelf = { true }; // �ڵ� ����ǰ� ������
	_uint iCurIndex = { 0 }; // ���� ����ǰ��ִ� �ִϸ��̼� �ε���
	_bool bMotionCancel = { false }; // ��� ĵ���Ұ���
	_bool bResetAfterFinish = { false }; // �ִϸ��̼� ������ ���� �Ұ���

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
	virtual void	Tick(_float TimeDelta) override;

public:
	// ���⼭ ���ϴ� �ִϸ��̼��̶��� �ִϸ��̼� �������� ����ϴ°��̴�.
	void Push_Animation(const string & name, const AnimNode& animNode, string nextAnimNode = "", string retAnimNode = "");
	void Proceed_Animation();
	void Apply_Animation(const string & name, ANIMTYPE eType = ANIM_ALLBODY);
	_bool Next_Animation(ANIMTYPE eType = ANIM_ALLBODY);
	const string& Get_CurAnimation_Name() const { return m_pCurAnimationName; }
	void Stop_Animation() { m_bAnimationStopped = true; }
	void Resume_Animation() { m_bAnimationStopped = false; }
	
	_bool Animation_Finished(_int iIndex = -1, ANIMTYPE eType = ANIM_ALLBODY);
	const string& Get_NextNode_Name(ANIMTYPE eType = ANIM_ALLBODY);

	_bool Is_Use_AllBodyAnim() const { return m_bUseAllBodyAnim; }
	_bool Is_Use_UpperBodyAnim() const { return m_bUseUpperBodyAnim; }
	_bool Is_Use_LowerBodyAnim() const { return m_bUseLowerBodyAnim; }

	AnimNode* Get_AllBody_CurAnimNode() { return m_pCurAnimNode; }
	AnimNode* Get_UpperBody_CurAnimNode() { return m_pCurUpperAnimNode; }
	AnimNode* Get_LowerBody_CurAnimNode() { return m_pCurLowerAnimNode; }


private:
	// ���� �ִϸ��̼����� �Ѿ�� ������ �����Ȳ ������ �ʱ�ȭ������.
	void Reset_Animation(const string& name);
	_uint Key(const string & name, _int index = 0) { return m_AnimationNode[name].AnimIndices[index]; }

	void Play_Animation(_float TimeDelta, AnimNode* node);
	void Play_UpperBody_Animation(_float TimeDelta, AnimNode* node);
	void Play_LowerBody_Animation(_float TimeDelta, AnimNode* node);

private:
	unordered_map<string, AnimNode> m_AnimationNode;
	class CModel*					m_pModel = { nullptr };

	/* ���� �ѹ��� ���� �ִϸ��̼��� ���� ������ ��.*/
	string							m_pCurAnimationName;
	string							m_pPrevAnimationName;

	AnimNode*						m_pCurAnimNode = { nullptr };
	_bool							m_bProceed = { false };
	_bool							m_bAnimationFinished = { false };

	/* ��ü�� ���� �ִϸ��̼��� ���� ������ ��.*/
	string							m_pCurUpperAnimationName;
	string							m_pPrevUpperAnimationName;

	AnimNode*						m_pCurUpperAnimNode = { nullptr };
	_bool							m_bUpperProceed = { false };
	_bool							m_bUpperAnimationFinished = { false };

	/* ��ü�� ���� �ִϸ��̼��� ���� ������ ��. */
	string							m_pCurLowerAnimationName;
	string							m_pPrevLowerAnimationName;

	AnimNode*						m_pCurLowerAnimNode = { nullptr };
	_bool							m_bLowerProceed = { false };
	_bool							m_bLowerAnimationFinished = { false };

	/* ���� ��ü ���� ���� �ִϸ��̼��� ������ �ƴϸ� ����ü �и� �ִϸ��̼��� ������ �� ���� ����.*/
	_bool							m_bUseAllBodyAnim = { false };
	_bool							m_bUseUpperBodyAnim = { false };
	_bool							m_bUseLowerBodyAnim = { false };

	_bool							m_bAnimationStopped = { false };

	



public:
	static CAnimInstance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAnimInstance* Clone(void* pArg);
	virtual void Free() override;
};

END
