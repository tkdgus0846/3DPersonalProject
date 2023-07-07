#pragma once

#include "Component.h"
#include "BlackBoard.h"
#include "RootNode.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CBehaviorTree : public CComponent
{
public:
	struct BTDesc
	{
		BTDesc(CGameObject* pOwner)
			: pBlackBoard(CBlackBoard::Create())
			, pRootNode(CRootNode::Create()) 
		{
			pBlackBoard->AddData("Owner", pOwner);
		}
		BTDesc(CGameObject* pOwner, CBlackBoard* _pBlackBoard, CRootNode* _pRootNode)
			: pBlackBoard(_pBlackBoard)
			, pRootNode(_pRootNode)
		{
			pBlackBoard->AddData("Owner", pOwner);
		}

		CBlackBoard*	pBlackBoard = { nullptr };
		CRootNode*		pRootNode = { nullptr };
	};

public:
	void ChangeData(const string& name, any Data);
	any GetData(const string& name);
	void Stop() { m_bRun = false; }
	void Resume() { m_bRun = true; }


private:
	CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Tick(_float TimeDelta) override;

private:
	CBlackBoard*	m_pBlackBoard = { nullptr };
	CRootNode*		m_pRootNode = { nullptr };
	_bool			m_bRun = { true };
	
public:
	// CComponent을(를) 통해 상속됨
	virtual CComponent* Clone(void* pArg) override;
	static CBehaviorTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};


END
