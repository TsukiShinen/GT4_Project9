struct DirectionalLight
{
    float3 Color;
    float3 Direction;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
    float4x4 gViewProj;
    float3 gEyePosW;

    float4 gAmbientLight;

    int gNumDirectionalLights;
    DirectionalLight gDirectionalLights[10];
};

Texture2D mainTexture : register(t0);
SamplerState mainSample : register(s0);

cbuffer cbMaterial : register(b2)
{
	float4 gAlbedo;
    float4 gSpecular;
    float  gSmoothness;
    float  gFresnel;

    float2 gTiling;
};


struct VertexIn
{
	float3 PosL  : POSITION;
    float2 TexC    : TEXCOORD;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 PosW  : POSITION;
    float2 TexC    : TEXCOORD;
    float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;
	
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
    vout.PosH = mul(posW, gViewProj);

    vout.TexC = vin.TexC * gTiling;
    
    return vout;
}

float G1V(float dotNV, float k)
{
    return 1.0f / (dotNV * (1.0f - k) + k);
}

float LightingGGX_REF(float3 N, float3 V, float3 L, float roughness, float F0)
{
    float alpha = roughness * roughness;

    float3 H = normalize(V + L);

    float dotNL = saturate(dot(N, L));
    float dotNV = saturate(dot(N, V));
    float dotNH = saturate(dot(N, H));
    float dotLH = saturate(dot(L, H));

    // D
    float alphaSqr = alpha * alpha;
    float pi = 3.14159f;
    float denom = dotNH * dotNH * (alphaSqr - 1.0f) + 1.0f;
    float D = alphaSqr / (pi * denom * denom);

    // F
    float dotLH5 = pow(1.0f - dotLH, 5);
    float F = F0 + (1.0f - F0) * (dotLH5);

    // V
    float k = alpha / 2.0f;
    float vis = G1V(dotNL, k) * G1V(dotNV, k);

    float specular = dotNL * D * F * vis;
    return specular;
}

float4 CalculateLighting(DirectionalLight light, VertexOut pin)
{
    float4 diffuseColor = float4(0, 0, 0, 0);
    float4 specularColor = float4(0, 0, 0, 0);

    float3 lightDir = -normalize(light.Direction);
    float3 normal = normalize(pin.NormalW);
    float3 viewDir = normalize(gEyePosW - pin.PosW);

    // diffuse
    float diffuseFactor = max(dot(normal, lightDir), 0);
    diffuseColor = float4(light.Color * diffuseFactor, 1.0f);

    // specular
    if (diffuseFactor > 0)
    {
         specularColor = float4(float3(1, 1, 1) * LightingGGX_REF(normal, viewDir, lightDir, 1 - gSmoothness, gFresnel), 1);
    }

    return diffuseColor + specularColor;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 ambientColor = gAmbientLight;
    float4 specularDiffuseColor = float4(0, 0, 0, 0);
    
    int i = 0;
    for(i = 0; i < gNumDirectionalLights; ++i)
    {
        specularDiffuseColor += CalculateLighting(gDirectionalLights[i], pin);
    }

    return mainTexture.Sample(mainSample, pin.TexC) * gAlbedo * (ambientColor + specularDiffuseColor * gSpecular);
}