//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
 
#include "lighting.hlsl"

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

cbuffer cbMaterial : register(b2)
{
	float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float  gRoughness;
};


struct VertexIn
{
	float3 PosL  : POSITION;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 PosW  : POSITION;
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

float4 PS(VertexOut pin) : SV_Target
{/*
    Material mat = { gDiffuseAlbedo, gFresnelR0, shininess };
    CalcDirectional(pin.PosW, mat, gEyePosW, gDirectionalLights[0].Direction);*/


// Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);
    
    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
    
	// Indirect lighting.
    float4 ambient = gAmbientLight*gDiffuseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { gDiffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    
    float4 directLight = ComputeLighting(gDirectionalLights, gNumDirectionalLights, mat, pin.PosW, 
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse material.
    litColor.a = gDiffuseAlbedo.a;

    return litColor;
    
}