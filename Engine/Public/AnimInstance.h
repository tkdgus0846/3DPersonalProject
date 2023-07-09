#pragma once
#include "Component.h"

BEGIN(Engine)

enum ANIMTYPE
{
	ANIM_UPPERBODY, ANIM_LOWERBODY, ANIM_ALLBODY
};

struct ENGINE_DLL AnimNode
{
	// 애니메이션 적용을 상체만 할건지 하체만할건지
	
	_bool bLoop;
	vector<_uint> AnimIndices; // 어떤 애니메이션들을 묶을건지
	vector<_float> eraseLessTime; // 프레임 몇초부터 자를건지
	_bool bProceedSelf = { true }; // 자동 진행되게 핡건지
	_uint iCurIndex = { 0 }; // 현재 진행되고있는 애니메이션 인덱스
	_bool bMotionCancel = { false }; // 모션 캔슬할건지
	_bool bResetAfterFinish = { false }; // 애니메이션 끝나고 리셋 할건지

	// bLoop가 false 일경우에 동작이 다 끝나고 돌아갈 다음 노드를 가르키게 해야한다.
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
	// 여기서 말하는 애니메이션이란건 애니메이션 묶음들을 얘기하는것이다.
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
	// 다음 애니메이션으로 넘어갈때 원래의 진행상황 정보를 초기화해주자.
	void Reset_Animation(const string& name);
	_uint Key(const string & name, _int index = 0) { return m_AnimationNode[name].AnimIndices[index]; }

	void Play_Animation(_float TimeDelta, AnimNode* node);
	void Play_UpperBody_Animation(_float TimeDelta, AnimNode* node);
	void Play_LowerBody_Animation(_float TimeDelta, AnimNode* node);

private:
	unordered_map<string, AnimNode> m_AnimationNode;
	class CModel*					m_pModel = { nullptr };

	/* 몸을 한번에 쓰는 애니메이션을 적용 시켰을 때.*/
	string							m_pCurAnimationName;
	string							m_pPrevAnimationName;

	AnimNode*						m_pCurAnimNode = { nullptr };
	_bool							m_bProceed = { false };
	_bool							m_bAnimationFinished = { false };

	/* 상체를 쓰는 애니메이션을 적용 시켰을 때.*/
	string							m_pCurUpperAnimationName;
	string							m_pPrevUpperAnimationName;

	AnimNode*						m_pCurUpperAnimNode = { nullptr };
	_bool							m_bUpperProceed = { false };
	_bool							m_bUpperAnimationFinished = { false };

	/* 하체를 쓰는 애니메이션을 적용 시켰을 때. */
	string							m_pCurLowerAnimationName;
	string							m_pPrevLowerAnimationName;

	AnimNode*						m_pCurLowerAnimNode = { nullptr };
	_bool							m_bLowerProceed = { false };
	_bool							m_bLowerAnimationFinished = { false };

	/* 현재 전체 몸을 쓰는 애니메이션을 쓰는지 아니면 상하체 분리 애니메이션을 쓰는지 에 대한 변수.*/
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
