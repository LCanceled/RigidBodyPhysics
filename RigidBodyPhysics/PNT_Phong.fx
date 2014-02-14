
struct Material {
	float4 amb; 
	float4 dif; 
	float4 spe;  
	float  sPow; 
};

struct Light {
	float4 amb; 
	float4 dif; 
	float4 spe;
	float3 vec;  
};

SamplerState sLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer cbPerSubset {
	Material g_Mat;
};

cbuffer cbPerObj {
	matrix g_WVP;
	matrix g_World;
};

cbuffer cbPerFrame {
	float3 g_CamPos;
};

cbuffer cbPerChange {
	Light g_Light;
};

Texture2D g_Tex;

struct VS_INPUT
{
	float3 pos	: POSITION;	//position
	float3 nor	: NORMAL;	//normal
	float2 tex	: TEXCOORD;	//UVs
};

struct PS_INPUT
{
	float4 wvpPos	: SV_POSITION;	//world view project transformed position
	float3 wNor	: NORMAL;	//world transformed normal
	float3 wVVec	: TEXCOORD0;	//world transformed view vector
	float2 wTex	: TEXCOORD1;	//UVs
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output	= (PS_INPUT)0;
	output.wvpPos	= mul(float4(input.pos, 1.f), g_WVP);
	output.wNor	= mul(float4(input.nor, 0.f), g_World).xyz;
	output.wVVec	= mul(float4(input.pos, 0.f), g_World).xyz - g_CamPos;
	output.wTex	= input.tex;

	return output;
}


PS_INPUT VS_Simple(VS_INPUT input)
{
	PS_INPUT output	= (PS_INPUT)0;
	output.wvpPos	= mul(float4(input.pos, 1.f), g_WVP);

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 color = g_Tex.Sample(sLinear, input.wTex);
	clip(color.a - .3f);

	float3 nor = normalize(input.wNor).xyz;
	float3 vVec = normalize(input.wVVec).xyz;
	
	//ambient
	float4 amb = (g_Mat.amb*g_Light.amb);
	
	//diffuse
	float perDif = max(0, dot(-g_Light.vec, nor));
	float4 dif = perDif*(g_Mat.dif*g_Light.dif);
	
	//specular
	float3 rVec = reflect(-g_Light.vec, nor);
	float perSpe = pow(abs(max(0, dot(vVec, rVec))), g_Mat.sPow);
	float4 spe = perSpe*(g_Mat.spe*g_Light.spe);

	return color*(amb+dif) + spe;
}

float4 PS_Simple(PS_INPUT input) : SV_Target
{
	return float4(0.f, 0.f, 1.f, 1.f);
}

float4 PS_Simple2(PS_INPUT input) : SV_Target
{
	return float4(1.f, 0.f, 0.f, 1.f);
}

DepthStencilState DepthWrite {
	DepthEnable = TRUE;
};

RasterizerState NoCull {
	CullMode = None;
};

RasterizerState NoCullWire {
	CullMode = None;
	FillMode = WireFrame;
};


technique10 Render
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
	SetDepthStencilState(DepthWrite, 0);
	SetRasterizerState(NoCull);
	//SetRasterizerState(NoCullWire);
     }
     pass P1
     {
        SetVertexShader(CompileShader(vs_4_0, VS_Simple()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_Simple()));
	SetDepthStencilState(DepthWrite, 0);
	SetRasterizerState(NoCull);
     }
     pass P2
     {
        SetVertexShader(CompileShader(vs_4_0, VS_Simple()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_Simple2()));
	SetDepthStencilState(DepthWrite, 0);
	SetRasterizerState(NoCull);
     }
}
