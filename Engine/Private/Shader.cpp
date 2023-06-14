#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_iNumPasses(rhs.m_iNumPasses)
	, m_InputLayouts(rhs.m_InputLayouts)
{
	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = { 0 };
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	/* 이 셰이더 파일에 구현되어있는 0번째 테크니커를 가져와. */
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);
	/* 테크니커의 정볼르 저장한ㄷ다. */
	pTechnique->GetDesc(&TechniqueDesc);

	/* 테크니커안에 정의되어있는 패스의 갯수 .*/
	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		/* i번째 패스를 얻어온다. */
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

		/* 패스의 정보를 얻어온다 WHY? 패스안에 정점을 받아주고 있는 코드에 대한 정의가 있기 때문에. */
		D3DX11_PASS_DESC	PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);
		pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pValue, _uint iLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어오낟. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pValue, 0, iLength);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어오낟. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pVariableMatrix = pVariable->AsMatrix();
	
	if (nullptr == pVariableMatrix)
		return E_FAIL;

	/* 해당 컴객체로 변수에 값을 던진다. */
	return pVariableMatrix->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어오낟. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pVariableMatrix = pVariable->AsMatrix();
	if (nullptr == pVariableMatrix)
		return E_FAIL;

	/* 해당 컴객체로 변수에 값을 던진다. */
	return pVariableMatrix->SetMatrixArray((_float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::Bind_Float3(const _char* pConstantName, const _float3* pFloat3)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어오낟. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectVectorVariable* pVariableVector = pVariable->AsVector();

	if (nullptr == pVariableVector)
		return E_FAIL;

	_float4 myFloat4 = _float4(pFloat3->x, pFloat3->y, pFloat3->z, 0.f);

	/* 해당 컴객체로 변수에 값을 던진다. */
	return pVariableVector->SetFloatVector((_float*)&myFloat4);
}

HRESULT CShader::Bind_Float4(const _char* pConstantName, const _float4* pFloat4)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어오낟. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectVectorVariable* pVariableVector = pVariable->AsVector();

	if (nullptr == pVariableVector)
		return E_FAIL;

	/* 해당 컴객체로 변수에 값을 던진다. */
	return pVariableVector->SetFloatVector((_float*)&pFloat4);
}

HRESULT CShader::Bind_ShaderResource(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어오낟. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariableShaderResource = pVariable->AsShaderResource();
	if (nullptr == pVariableShaderResource)
		return E_FAIL;

	/* 해당 컴객체로 변수에 값을 던진다. */
	return pVariableShaderResource->SetResource(pSRV);
}

HRESULT CShader::Bind_ShaderResources(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVArray, _uint iNumTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어오낟. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariableShaderResource = pVariable->AsShaderResource();
	if (nullptr == pVariableShaderResource)
		return E_FAIL;

	/* 해당 컴객체로 변수에 값을 던진다. */
	return pVariableShaderResource->SetResourceArray(ppSRVArray, 0, iNumTexture);
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	pTechnique->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);
	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
