
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3			g_LightPosition;
texture2D		g_Texture[2];

sampler LinearSampler = sampler_state
{	
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		Diffuse : TEXCOORD1;
};


/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	
	float3 worldNormal = mul(float4(In.vNormal, 0.0f), (g_WorldMatrix));
	float3 worldPosition = mul(In.vPosition, g_WorldMatrix);

	float3 LightDir = normalize(worldPosition - g_LightPosition);

	Out.Diffuse = dot(normalize(worldNormal), -LightDir);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		Diffuse : TEXCOORD1;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
float4	PS_MAIN(PS_IN In) : SV_TARGET0
{
	float4		vColor = (float4)0;

	vector albedo = g_Texture[0].Sample(LinearSampler, In.vTexUV * 30.f);

	float3 diffuse = saturate(In.Diffuse);

	float4 ambient = float4(0.2f, 0.2f, 0.2f, 1.f);


	vColor = albedo * (float4(diffuse,1)+ambient);

	return vColor;
}







technique11		DefaultTechnique
{
	pass Terrain
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}


}