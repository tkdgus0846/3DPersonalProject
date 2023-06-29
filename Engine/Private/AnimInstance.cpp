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
	if (m_pCurAnimNode == nullptr) return;
	//if (m_bAnimationFinished == true) return;

	__super::Tick(TimeDelta);

	AnimNode* node = m_pCurAnimNode;

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
					if (node->nextAnimNode.compare("") != 0)
					{
						
					}

					m_bAnimationFinished = true;

					// 애니메이션이 끝나기 전에 입력이 있었다라면 넘겨주는것이 필요하다. 생각해보자.
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

void CAnimInstance::Apply_Animation(const string& name)
{
	m_pCurAnimationName = name;
	m_pCurAnimNode = &m_AnimationNode[m_pCurAnimationName];
	m_bAnimationFinished = false;
	m_pModel->Set_AnimIndex(m_pCurAnimNode->AnimIndices[m_pCurAnimNode->iCurIndex]);
}

_bool CAnimInstance::Next_Animation()
{
	if (m_pCurAnimNode->nextAnimNode.compare("") == 0) return false;

	Apply_Animation(m_pCurAnimNode->nextAnimNode);
	return true;
}

_bool CAnimInstance::Animation_Finished()
{
	return m_bAnimationFinished;
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
