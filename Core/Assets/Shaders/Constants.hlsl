// We want to use row major matrices
#pragma pack_matrix(row_major)

// Per-Camera Constants
cbuffer CameraConstants: register( b0 )
{
	float4x4 viewMat;
	float3 worldCamPos;
};

// Per-Object Constants
cbuffer ObjectConstants: register( b1 )
{
	float4x4 worldMat;
};

// Lighting
struct PointLightData
{
	float3 diffuseColor;
	float3 specularColor;
	float3 position;
	float specularPower;
	float innerRadius;
	float outerRadius;
	bool enabled;
};

cbuffer LightingConstants: register( b2 )
{
	float3 ambientLight;
	PointLightData pointLights[8];
};

SamplerState DefaultSampler : register( s0 );
Texture2D DiffuseTexture : register( t0 );