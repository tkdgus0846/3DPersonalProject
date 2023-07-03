#pragma once

#include "Component.h"

BEGIN(Engine)

class CBlackBoard;
class CRootNode;

class ENGINE_DLL CBehaviorTree : public CComponent
{
public:
	struct BTDesc
	{
		CBlackBoard*	pBlackBoard = { nullptr };
		CRootNode*		pRootNode = { nullptr };
	};

public:
	void ChangeData(const string& name, any Data);


private:
	CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Tick(_double TimeDelta) override;

private:
	CBlackBoard*	m_pBlackBoard = { nullptr };
	CRootNode*		m_pRootNode = { nullptr };
	
public:
	// CComponent을(를) 통해 상속됨
	virtual CComponent* Clone(void* pArg) override;
	static CBehaviorTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};


END
