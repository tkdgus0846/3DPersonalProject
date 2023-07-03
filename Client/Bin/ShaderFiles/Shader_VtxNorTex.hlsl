
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3			g_LightPosition;
texture2D		g_Texture[2];

texture2D		g_BrushTexture;
vector			g_vBrushPos;
float			g_fBrushRadius;

vector			g_vCamPosition;

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

struct PS_IN_BRUSH
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

PS_IN_BRUSH VS_MAIN_BRUSH(VS_IN In)
{
	PS_IN_BRUSH			Out = (PS_IN_BRUSH)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	vector		vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
	Out.vNormal = normalize(vWorldNormal);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}




float4 PS_MAIN_BRUSH(PS_IN_BRUSH In) : SV_TARGET0
{
	float4 Color = (float4)0;

	vector Diffuse = g_Texture[0].Sample(LinearSampler, In.vTexUV * 30.f);

	vector vBrush = vector(0.f, 0.f, 0.f, 0.f);

	if (g_vBrushPos.x - g_fBrushRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRadius &&
		g_vBrushPos.z - g_fBrushRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRadius)
	{
		float2		vTexUV;

		vTexUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRadius)) / (2.f * g_fBrushRadius);
		vTexUV.y = ((g_vBrushPos.z - g_fBrushRadius) - In.vWorldPos.z) / (2.f * g_fBrushRadius);

		vBrush = g_BrushTexture.Sample(LinearSampler, vTexUV);
	}

	vector vMtrlDiffuse = Diffuse + vBrush;

	vector LightDir = In.vWorldPos - float4(g_LightPosition,1);
	vector vReflect = reflect(normalize(LightDir), normalize(In.vNormal));
	vector vLook = In.vWorldPos - g_vCamPosition;

	float		fShade = max(dot(normalize(LightDir) * -1.f, In.vNormal), 0.f);

	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);

	Color = vMtrlDiffuse * (fShade + 0.1f);

	return Color;
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

	pass BrushTerrain
	{
		VertexShader = compile vs_5_0 VS_MAIN_BRUSH();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_BRUSH();
	}




}