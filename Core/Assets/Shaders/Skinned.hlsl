#include "Constants.hlsl"

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float3 mNorm : NORMAL;
	uint4  mBoneIndices : BONEINDICES;
	float4 mBoneWeights : BONEWEIGHTS;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float2 mTex: TEXCOORD0;
	float3 mWorldNorm : WORLDNORM;
	float3 mWorldPos : WORLDPOS;
};

cbuffer SkeletonConstants : register(b3)
{
	float4x4 bones[96];
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// Skinning
	float4 pos = float4(input.mPos, 1.0f);
	pos = input.mBoneWeights.x * mul(pos, bones[input.mBoneIndices.x])
		 + input.mBoneWeights.y * mul(pos, bones[input.mBoneIndices.y])
		 + input.mBoneWeights.z * mul(pos, bones[input.mBoneIndices.z])
		 + input.mBoneWeights.w * mul(pos, bones[input.mBoneIndices.w]);

	output.mPos = mul(pos, worldMat);
	output.mPos = mul( output.mPos, viewMat );
	output.mTex = input.mTex;

	// Convert model-space position and normal into a world-space position and normal
	output.mWorldPos = mul( float4( input.mPos, 1.0f ), worldMat ).xyz;
	output.mWorldNorm = mul( float4( input.mNorm, 0.0f ), worldMat ).xyz;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float3 phong = ambientLight;
	float3 norm = normalize( input.mWorldNorm );

	for ( int i = 0; i < 8; i++ )
	{
		if ( pointLights[i].enabled )
		{
			// Compute L and normalize
			float3 lightDir = normalize( pointLights[i].position - input.mWorldPos );
			float3 dist = distance( pointLights[i].position, input.mWorldPos );
			float3 smooth = smoothstep( pointLights[i].innerRadius, pointLights[i].outerRadius, dist );

			// If N dot L > 0, light should affect this pixel
			if ( dot( norm, lightDir ) > 0 )
			{
				float3 diffuse = pointLights[i].diffuseColor * dot( norm, lightDir );
				phong += lerp( diffuse, float3( 0.0f, 0.0f, 0.0f ), smooth );
				float3 view = normalize( worldCamPos -input.mWorldPos );
				float3 refl = reflect( -lightDir, norm );
				float3 specular = pointLights[i].specularColor * pow( max( 0.0f, dot( refl, view ) ), pointLights[i].specularPower );
				phong += lerp( specular, float3( 0.0f, 0.0f, 0.0f ), smooth );
			}
		}
	}

	return DiffuseTexture.Sample( DefaultSampler, input.mTex ) * saturate( float4( phong, 1.0f ) );
}