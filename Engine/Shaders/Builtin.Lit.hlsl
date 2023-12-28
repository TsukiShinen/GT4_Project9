//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
 
//#include "lighting.hlsl"

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
    float  gSpecularPower;
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

    vout.TexC = vin.TexC;
    
    return vout;
}
/*
float3 CalcDirectional(float3 position, Material material, float3 eyePosition, float3 lightDir)
{
   // Phong diffuse
   float NDotL = dot(lightDir, material.normal);
   float3 finalColor = DirLightColor.rgb * saturate(NDotL);
   
   // Blinn specular
   float3 ToEye = eyePosition.xyz - position;
   ToEye = normalize(ToEye);
   float3 HalfWay = normalize(ToEye + lightDir);
   float NDotH = saturate(dot(HalfWay, material.normal));
   finalColor += DirLightColor.rgb * pow(NDotH, material.specExp) * material.specIntensity;
   
   return finalColor * material.diffuseColor.rgb;
}*/

float4 CalculateLighting(DirectionalLight light, VertexOut pin)
{
    float4 diffuseColor = float4(0, 0, 0, 0);
    float4 specularColor = float4(0, 0, 0, 0);

    float3 lightDir = -normalize(light.Direction);
    float3 normal = normalize(pin.NormalW);


    float diffuseFactor = dot(normal, lightDir);
    if (diffuseFactor > 0)
    {
         diffuseColor = float4(light.Color * diffuseFactor, 1.0f);

         float3 vertexToEye = normalize(gEyePosW - pin.PosW);
         float3 lightReflect = normalize(reflect(light.Direction, normal));
         
         float specularFactor = dot(vertexToEye, lightReflect);

         if (specularFactor > 0)
         {
            specularFactor = pow(specularFactor, gSpecularPower);
            specularColor = float4(light.Color, 1.0f) * gSpecular * specularFactor;
         }
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

    return mainTexture.Sample(mainSample, pin.TexC) * gAlbedo * (ambientColor + specularDiffuseColor);
    
}