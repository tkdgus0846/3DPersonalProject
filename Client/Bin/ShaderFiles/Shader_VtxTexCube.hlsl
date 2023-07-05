#include "Shader_Client_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE		g_Texture;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};


/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	float4x4 matWVP, matWV; 
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
float4	PS_MAIN(PS_IN In) : SV_TARGET0
{
	float4		vColor = (float4)0;

	float4 albedo = g_Texture.Sample(LinearSampler, In.vTexUV);

	vColor = albedo;

	return vColor;
}

technique11		DefaultTechnique
{
	pass Cube
	{
		SetRasterizerState(RS_Cull_CW);
		SetDepthStencilState(DSS_Depth_Disable, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}