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

void CAnimInstance::Tick(_float TimeDelta)
{
	//if (m_pCurAnimNode == nullptr) return;
	//if (m_bAnimationFinished == true) return;

	__super::Tick(TimeDelta);

	if (m_bAnimationStopped == true) return;

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

// �ִ� ���
void CAnimInstance::Push_Animation(const string& name, const AnimNode& animNode, string nextAnimNode, string retAnimNode)
{
	// �ִϸ��̼� ������ ��� �Ұ�����?
	//m_pModel->m_Animations[]
	_uint iIndex = 0;
	for (_uint animIndices : animNode.AnimIndices)
	{
		_float time = -1.0; 

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
	
	Resume_Animation();

	if (eType == ANIM_ALLBODY)
	{
		/*if (m_pCurAnimationName.compare("") != 0)
				Reset_Animation(m_pCurAnimationName);*/
		if (m_pCurAnimationName.compare(name) == 0) return;

		//cout << name << endl;

		Reset_Animation(name);

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
				//Reset_Animation(m_pCurLowerAnimationName);
				m_pCurLowerAnimationName = "";
				m_pCurLowerAnimNode = nullptr;
				m_pModel->m_bChange_LowerBodyToAllBody = true;
			}

			if (m_pCurUpperAnimNode)
			{
				//Reset_Animation(m_pCurUpperAnimationName);
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

		Reset_Animation(name);

		m_pCurLowerAnimationName = name;
		m_pCurLowerAnimNode = &m_AnimationNode[m_pCurLowerAnimationName];
		m_bLowerAnimationFinished = false;
		m_pModel->Set_AnimIndex(m_pCurLowerAnimNode->AnimIndices[m_pCurLowerAnimNode->iCurIndex], eType);

		if (m_pCurLowerAnimNode->bMotionCancel == true)
			m_pModel->Motion_Cancel();

		if (m_pCurAnimNode != nullptr)
		{
			//Reset_Animation(m_pCurAnimationName);
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

		
		Reset_Animation(name);

		m_pCurUpperAnimationName = name;
		m_pCurUpperAnimNode = &m_AnimationNode[m_pCurUpperAnimationName];
		m_bUpperAnimationFinished = false;
		m_pModel->Set_AnimIndex(m_pCurUpperAnimNode->AnimIndices[m_pCurUpperAnimNode->iCurIndex], eType);

		if (m_pCurUpperAnimNode->bMotionCancel == true)
			m_pModel->Motion_Cancel();

		if (m_pCurAnimNode != nullptr)
		{
			//Reset_Animation(m_pCurAnimationName);
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

_bool CAnimInstance::Animation_Finished(_int iIndex, ANIMTYPE eType)
{
	if (iIndex == -1)
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
	else
	{
		switch (eType)
		{
		case ANIM_ALLBODY:
			if (m_pCurAnimNode == nullptr) return false;
			return m_pModel->m_Animations[m_pCurAnimNode->AnimIndices[iIndex]]->Get_Finished();
		case ANIM_UPPERBODY:
			if (m_pCurUpperAnimNode == nullptr) return false;
			return m_pModel->m_Animations[m_pCurUpperAnimNode->AnimIndices[iIndex]]->Get_Finished();
		case ANIM_LOWERBODY:
			if (m_pCurLowerAnimNode == nullptr) return false;
			return m_pModel->m_Animations[m_pCurLowerAnimNode->AnimIndices[iIndex]]->Get_Finished();;
		}
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
	if (name.compare("VampireMale_Axes_Combo01_MoveCast_Exec") == 0)
		int sibal = 2;
	for (_uint index : m_AnimationNode[name].AnimIndices)
	{
		m_pModel->m_Animations[index]->Reset_Channels();
	}
	m_AnimationNode[name].iCurIndex = 0;
	// m_bAnimationFinished = false;
}

void CAnimInstance::Play_Animation(_float TimeDelta, AnimNode* node)
{
	m_pModel->Play_Animation(TimeDelta);

	_uint* iCurIndex = &node->iCurIndex;

	CAnimation* anim = m_pModel->m_Animations[node->AnimIndices[*iCurIndex]];
	if (m_pModel->Is_Changing_Animation())
	{
		
		return;
	}

	// ȥ�� �˾Ƽ� �� ����Ǵ� ���̳�.
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

					// �ִϸ��̼��� ������ ���� �Է��� �־��ٶ�� �Ѱ��ִ°��� �ʿ��ϴ�. �����غ���.

					
					return;

				}
			}
		}

		m_pModel->Set_AnimIndex(node->AnimIndices[*iCurIndex]);
	}
	// ���� ���� ������ �ҷ���� �ϴ³��̳�.
	else
	{
		if (m_bProceed == true)
		{

		}
	}
}

void CAnimInstance::Play_UpperBody_Animation(_float TimeDelta, AnimNode* node)
{
	m_pModel->Play_UpperBody_Animation(TimeDelta);

	_uint* iCurIndex = &node->iCurIndex;

	//cout << *iCurIndex << endl;
	CAnimation* anim = m_pModel->m_Animations[node->AnimIndices[*iCurIndex]];
	if (m_pModel->Is_Changing_Animation(ANIM_UPPERBODY))
	{
		/*cout << m_pModel->m_iUpperPrevAnimIndex << " " << m_pModel->m_iUpperCurrentAnimIndex << " Can't Start" << endl;*/
		return;
	}

	// ȥ�� �˾Ƽ� �� ����Ǵ� ���̳�.
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

					// �ִϸ��̼��� ������ ���� �Է��� �־��ٶ�� �Ѱ��ִ°��� �ʿ��ϴ�. �����غ���.

					
					return;

				}
			}
		}

		m_pModel->Set_AnimIndex(node->AnimIndices[*iCurIndex], ANIM_UPPERBODY);
	}
}

void CAnimInstance::Play_LowerBody_Animation(_float TimeDelta, AnimNode* node)
{
	m_pModel->Play_LowerBody_Animation(TimeDelta);

	_uint* iCurIndex = &node->iCurIndex;

	CAnimation* anim = m_pModel->m_Animations[node->AnimIndices[*iCurIndex]];
	if (m_pModel->Is_Changing_Animation(ANIM_LOWERBODY))
	{
		return;
	}

	// ȥ�� �˾Ƽ� �� ����Ǵ� ���̳�.
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

					// �ִϸ��̼��� ������ ���� �Է��� �־��ٶ�� �Ѱ��ִ°��� �ʿ��ϴ�. �����غ���.

					
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
