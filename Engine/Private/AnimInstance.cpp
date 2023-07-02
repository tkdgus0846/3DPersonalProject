#include "..\Public\AnimInstance.h"
#include "GameInstance.h"
#include "Animation.h"

CAnimInstance::CAnimInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CComponent(pDevice, pContext)

{
}

CAnimInstance::CAnimInstance(const CAnimInstance& rhs)
	: CComponent(rhs)
{
}

HRESULT CAnimInstance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimInstance::Initialize(void* pArg)
{
	m_pModel = (CModel*)pArg;
	Safe_AddRef(m_pModel);
	return S_OK;
}

void CAnimInstance::Tick(_double TimeDelta)
{
	//if (m_pCurAnimNode == nullptr) return;
	//if (m_bAnimationFinished == true) return;

	__super::Tick(TimeDelta);

	///* 지울것 */
	//if (m_pCurLowerAnimNode)
	//	Play_LowerBody_Animation(TimeDelta, m_pCurLowerAnimNode);

	if (m_pCurUpperAnimNode && m_pCurLowerAnimNode)
	{
		Play_UpperBody_Animation(TimeDelta, m_pCurUpperAnimNode);
		Play_LowerBody_Animation(TimeDelta, m_pCurLowerAnimNode);
	}
	else if (m_pCurAnimNode)
	{
		Play_Animation(TimeDelta, m_pCurAnimNode);
	}		
}

void CAnimInstance::Push_Animation(const string& name, const AnimNode& animNode, string nextAnimNode, string retAnimNode)
{
	// 애니메이션 루프를 어떻게 할것인지?
	//m_pModel->m_Animations[]
	_uint iIndex = 0;
	for (_uint animIndices : animNode.AnimIndices)
	{
		_double time = -1.0; 

		if (animNode.eraseLessTime.size() > iIndex)
			time = animNode.eraseLessTime[iIndex++];

		if (time >= 0.0)
			m_pModel->Erase_Frames_LessTime(animIndices, time);
 			
		m_pModel->m_Animations[animIndices]->Set_Loop(animNode.bLoop);
	}
	m_AnimationNode[name] = animNode;
	m_AnimationNode[name].retAnimNode = retAnimNode;
	m_AnimationNode[name].nextAnimNode = nextAnimNode;

	
}

void CAnimInstance::Proceed_Animation()
{
	m_bProceed = true;
}

void CAnimInstance::Apply_Animation(const string& name, ANIMTYPE eType)
{
	

	if (eType == ANIM_ALLBODY)
	{
		/*if (m_pCurAnimationName.compare("") != 0)
				Reset_Animation(m_pCurAnimationName);*/
		if (m_pCurAnimationName.compare(name) == 0) return;
		//Reset_Animation(m_pCurAnimationName);

		m_pCurAnimationName = name;
		m_pCurAnimNode = &m_AnimationNode[m_pCurAnimationName];
		m_bAnimationFinished = false;
		m_pModel->Set_AnimIndex(m_pCurAnimNode->AnimIndices[m_pCurAnimNode->iCurIndex]);

		if (m_pCurAnimNode->bMotionCancel == true)
			m_pModel->Motion_Cancel();

		
		if (m_pCurLowerAnimNode != nullptr || m_pCurUpperAnimNode != nullptr)
		{
			if (m_pCurLowerAnimNode)
			{
				Reset_Animation(m_pCurLowerAnimationName);
				m_pCurLowerAnimationName = "";
				m_pCurLowerAnimNode = nullptr;
				m_pModel->m_bChange_LowerBodyToAllBody = true;
			}

			if (m_pCurUpperAnimNode)
			{
				Reset_Animation(m_pCurUpperAnimationName);
				m_pCurUpperAnimationName = "";
				m_pCurUpperAnimNode = nullptr;
				m_pModel->m_bChange_UpperBodyToAllBody = true;
			}
			
		}
		
		m_bUseAllBodyAnim = true;
		m_bUseUpperBodyAnim = false;
		m_bUseLowerBodyAnim = false;

		
	}
	
	if (eType == ANIM_LOWERBODY)
	{
		/*if (m_pCurAnimationName.compare("") != 0)
				Reset_Animation(m_pCurAnimationName);*/
		if (m_pCurLowerAnimationName.compare(name) == 0) return;
		//Reset_Animation(m_pCurLowerAnimationName);

		m_pCurLowerAnimationName = name;
		m_pCurLowerAnimNode = &m_AnimationNode[m_pCurLowerAnimationName];
		m_bLowerAnimationFinished = false;
		m_pModel->Set_AnimIndex(m_pCurLowerAnimNode->AnimIndices[m_pCurLowerAnimNode->iCurIndex], eType);

		if (m_pCurLowerAnimNode->bMotionCancel == true)
			m_pModel->Motion_Cancel();

		if (m_pCurAnimNode != nullptr)
		{
			Reset_Animation(m_pCurAnimationName);
			m_pCurAnimationName = "";
			m_pCurAnimNode = nullptr;

			m_pModel->m_bChange_AllBodyToLowerBody = true;
		}

		m_bUseAllBodyAnim = false;
		m_bUseLowerBodyAnim = true;
	}

	if (eType == ANIM_UPPERBODY)
	{
		/*if (m_pCurAnimationName.compare("") != 0)
				Reset_Animation(m_pCurAnimationName);*/
		if (m_pCurUpperAnimationName.compare(name) == 0) return;
		//Reset_Animation(m_pCurUpperAnimationName);

		m_pCurUpperAnimationName = name;
		m_pCurUpperAnimNode = &m_AnimationNode[m_pCurUpperAnimationName];
		m_bUpperAnimationFinished = false;
		m_pModel->Set_AnimIndex(m_pCurUpperAnimNode->AnimIndices[m_pCurUpperAnimNode->iCurIndex], eType);

		if (m_pCurUpperAnimNode->bMotionCancel == true)
			m_pModel->Motion_Cancel();

		if (m_pCurAnimNode != nullptr)
		{
			Reset_Animation(m_pCurAnimationName);
			m_pCurAnimationName = "";
			m_pCurAnimNode = nullptr;

			m_pModel->m_bChange_AllBodyToUpperBody = true;
		}

		m_bUseAllBodyAnim = false;
		m_bUseUpperBodyAnim = true;
	}
	
}

_bool CAnimInstance::Next_Animation(ANIMTYPE eType)
{
	switch (eType)
	{
		case ANIM_ALLBODY:
		{
			if (m_pCurAnimNode == nullptr) return false;
			if (m_pCurAnimNode->nextAnimNode.compare("") == 0)
				return false;

			Apply_Animation(m_pCurAnimNode->nextAnimNode, eType);
			return true;
		}
		case ANIM_UPPERBODY:
		{
			if (m_pCurUpperAnimNode == nullptr) return false;
			if (m_pCurUpperAnimNode->nextAnimNode.compare("") == 0)
				return false;

			Apply_Animation(m_pCurUpperAnimNode->nextAnimNode, eType);
			return true;
		}
		case ANIM_LOWERBODY:
		{
			if (m_pCurLowerAnimNode == nullptr) return false;
			if (m_pCurLowerAnimNode->nextAnimNode.compare("") == 0)
				return false;

			Apply_Animation(m_pCurLowerAnimNode->nextAnimNode, eType);
			return true;
		}
	}

	
}

_bool CAnimInstance::Animation_Finished(ANIMTYPE eType)
{
	switch (eType)
	{
	case ANIM_ALLBODY:
		return m_bAnimationFinished;
	case ANIM_UPPERBODY:
		return m_bUpperAnimationFinished;
	case ANIM_LOWERBODY:
		return m_bLowerAnimationFinished;
	}
}

const string& CAnimInstance::Get_NextNode_Name(ANIMTYPE eType)
{
	switch (eType)
	{
	case ANIM_ALLBODY:
		if (m_pCurAnimNode == nullptr) return "";
		return m_pCurAnimNode->nextAnimNode;
	case ANIM_UPPERBODY:
		if (m_pCurUpperAnimNode == nullptr) return "";
		return m_pCurUpperAnimNode->nextAnimNode;
	case ANIM_LOWERBODY:
		if (m_pCurLowerAnimNode == nullptr) return "";
		return m_pCurLowerAnimNode->nextAnimNode;
	}
	
}

void CAnimInstance::Reset_Animation(const string& name)
{
	for (_uint index : m_AnimationNode[name].AnimIndices)
	{
		m_pModel->m_Animations[index]->Reset_Channels();
	}
	m_AnimationNode[name].iCurIndex = 0;
	// m_bAnimationFinished = false;
}

void CAnimInstance::Play_Animation(_double TimeDelta, AnimNode* node)
{
	m_pModel->Play_Animation(TimeDelta);

	_uint* iCurIndex = &node->iCurIndex;

	CAnimation* anim = m_pModel->m_Animations[node->AnimIndices[*iCurIndex]];
	if (m_pModel->Is_Changing_Animation())
	{
		
		return;
	}

	// 혼자 알아서 쭉 진행되는 놈이냐.
	if (node->bProceedSelf == true)
	{
		if (anim->Get_Finished())
		{
			*iCurIndex += 1;
			if (*iCurIndex >= node->AnimIndices.size())
			{
				if (node->bLoop == true)
					*iCurIndex = 0;
				else
				{
					*iCurIndex -= 1;

					m_bAnimationFinished = true;

					// 애니메이션이 끝나기 전에 입력이 있었다라면 넘겨주는것이 필요하다. 생각해보자.

					if (node->bResetAfterFinish)
						Reset_Animation(m_pCurAnimationName);
					//Apply_Animation(node->nextAnimNode);
					return;

				}
			}
		}

		m_pModel->Set_AnimIndex(node->AnimIndices[*iCurIndex]);
	}
	// 내가 직접 진행을 불러줘야 하는놈이냐.
	else
	{
		if (m_bProceed == true)
		{

		}
	}
}

void CAnimInstance::Play_UpperBody_Animation(_double TimeDelta, AnimNode* node)
{
	m_pModel->Play_UpperBody_Animation(TimeDelta);

	_uint* iCurIndex = &node->iCurIndex;

	CAnimation* anim = m_pModel->m_Animations[node->AnimIndices[*iCurIndex]];
	if (m_pModel->Is_Changing_Animation(ANIM_UPPERBODY))
	{
		/*cout << m_pModel->m_iUpperPrevAnimIndex << " " << m_pModel->m_iUpperCurrentAnimIndex << " Can't Start" << endl;*/
		return;
	}

	// 혼자 알아서 쭉 진행되는 놈이냐.
	if (node->bProceedSelf == true)
	{
		if (anim->Get_Finished())
		{
			*iCurIndex += 1;
			if (*iCurIndex >= node->AnimIndices.size())
			{
				if (node->bLoop == true)
					*iCurIndex = 0;
				else
				{
					*iCurIndex -= 1;

					m_bUpperAnimationFinished = true;

					// 애니메이션이 끝나기 전에 입력이 있었다라면 넘겨주는것이 필요하다. 생각해보자.

					if (node->bResetAfterFinish)
						Reset_Animation(m_pCurUpperAnimationName);
					//Apply_Animation(node->nextAnimNode);
					return;

				}
			}
		}

		m_pModel->Set_AnimIndex(node->AnimIndices[*iCurIndex], ANIM_UPPERBODY);
	}
}

void CAnimInstance::Play_LowerBody_Animation(_double TimeDelta, AnimNode* node)
{
	m_pModel->Play_LowerBody_Animation(TimeDelta);

	_uint* iCurIndex = &node->iCurIndex;

	CAnimation* anim = m_pModel->m_Animations[node->AnimIndices[*iCurIndex]];
	if (m_pModel->Is_Changing_Animation(ANIM_LOWERBODY))
	{
		return;
	}

	// 혼자 알아서 쭉 진행되는 놈이냐.
	if (node->bProceedSelf == true)
	{
		if (anim->Get_Finished())
		{
			*iCurIndex += 1;
			if (*iCurIndex >= node->AnimIndices.size())
			{
				if (node->bLoop == true)
					*iCurIndex = 0;
				else
				{
					*iCurIndex -= 1;

					m_bLowerAnimationFinished = true;

					// 애니메이션이 끝나기 전에 입력이 있었다라면 넘겨주는것이 필요하다. 생각해보자.

					if (node->bResetAfterFinish)
						Reset_Animation(m_pCurLowerAnimationName);
					//Apply_Animation(node->nextAnimNode);
					return;

				}
			}
		}

		m_pModel->Set_AnimIndex(node->AnimIndices[*iCurIndex], ANIM_LOWERBODY);
	}
}

CAnimInstance* CAnimInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimInstance* pInstance = new CAnimInstance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimInstance* CAnimInstance::Clone(void* pArg)
{
	CAnimInstance* pInstance = new CAnimInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimInstance::Free()
{
	Safe_Release(m_pModel);
	__super::Free();
}
