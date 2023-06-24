#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
public:
	enum TYPE
	{
		TYPE_DYNAMIC, TYPE_STATIC 
	};
protected:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints, TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;
	void Edit_Point(_uint iIndex, const _float3& pos);

public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, TYPE eType = TYPE_STATIC);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END