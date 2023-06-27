#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public CBounding::tagColliderDesc
	{
		_float		fRadius;

	} BOUNDINGSPHERE;

private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBounding_Sphere(const CBounding_Sphere& rhs);
	virtual ~CBounding_Sphere() = default;

public:
	const BoundingSphere* Get_Bounding() const {
		return m_pSphere;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pBoundingDesc);

public:
	virtual void Tick(_fmatrix TransformMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)override;
	virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding) override;
private:
	BoundingSphere*				m_pSphere_Original = { nullptr };
	BoundingSphere*				m_pSphere = { nullptr };
	
public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END