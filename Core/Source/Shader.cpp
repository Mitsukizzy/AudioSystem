#include "ITPEnginePCH.h"
#include <SDL/SDL_log.h>

Shader::Shader(class Game& game)
	:Asset(game)
	,mGraphicsDriver(mGame.GetRenderer().GetGraphicsDriver())
{
}

Shader::~Shader()
{
}

void Shader::SetActive()
{
	// Set this shader as active, and set
	// any constant buffers
	mGraphicsDriver.SetVertexShader( mVertexShader );
	mGraphicsDriver.SetPixelShader( mPixelShader );

	mGraphicsDriver.SetVSConstantBuffer( mPerCameraBuffer, 0 );
	mGraphicsDriver.SetPSConstantBuffer( mPerCameraBuffer, 0 );

	mGraphicsDriver.SetVSConstantBuffer( mPerObjectBuffer, 1 );
	mGraphicsDriver.SetPSSamplerState( mDefaultSampler, 0 );

	// Set the lighting constant buffer
	mGraphicsDriver.SetPSConstantBuffer( mLightingBuffer, 2 );

	// Set the matrix palette buffer, if it exists
	mGraphicsDriver.SetVSConstantBuffer( mMatrixPaletteBuffer, 3 );
}

void Shader::CreateMatrixPaletteBuffer()
{	
	// Skeleton Bones
	Matrix4 temp;
	mMatrixPaletteBuffer = mGraphicsDriver.CreateGraphicsBuffer( &temp, sizeof( MatrixPalette ), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic );
	mGraphicsDriver.SetVSConstantBuffer( mMatrixPaletteBuffer, 3 );
}

void Shader::UploadPerCameraConstants()
{
	void* buffer = mGraphicsDriver.MapBuffer( mPerCameraBuffer );
	memcpy( buffer, &mPerCamera, sizeof( PerCameraConstants ) );
	mGraphicsDriver.UnmapBuffer( mPerCameraBuffer );
}

void Shader::UploadPerObjectConstants()
{
	void* buffer = mGraphicsDriver.MapBuffer( mPerObjectBuffer );
	memcpy( buffer, &mPerObject, sizeof( PerObjectConstants ) );
	mGraphicsDriver.UnmapBuffer( mPerObjectBuffer );
}

void Shader::UploadLightingConstants()
{
	void* buffer = mGraphicsDriver.MapBuffer( mLightingBuffer );
	memcpy( buffer, &mLighting, sizeof( LightingConstants ) );
	mGraphicsDriver.UnmapBuffer( mLightingBuffer );
}

void Shader::UploadMatrixPalette(const struct MatrixPalette& palette)
{
	void* buffer = mGraphicsDriver.MapBuffer( mMatrixPaletteBuffer );
	memcpy( buffer, &palette, sizeof( MatrixPalette ) );
	mGraphicsDriver.UnmapBuffer( mMatrixPaletteBuffer );
}

void Shader::BindTexture(TexturePtr texture, int slot)
{
	texture->SetActive(slot);
}

bool Shader::Load( const char* fileName, class AssetCache* cache )
{
	bool successVertex = mGraphicsDriver.CompileShaderFromFile( fileName, "VS", "vs_4_0", mCompiledVS );
	bool successPixel = mGraphicsDriver.CompileShaderFromFile( fileName, "PS", "ps_4_0", mCompiledPS );

	if ( !successVertex || !successPixel )
	{
		return false;
	}

	mVertexShader = mGraphicsDriver.CreateVertexShader( mCompiledVS );
	mPixelShader = mGraphicsDriver.CreatePixelShader( mCompiledPS );

	mGraphicsDriver.SetVertexShader( mVertexShader );
	mGraphicsDriver.SetPixelShader( mPixelShader );

	// Create constant buffers
	mPerCameraBuffer = mGraphicsDriver.CreateGraphicsBuffer( &mPerCamera, sizeof( mPerCamera ), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic );
	mPerObjectBuffer = mGraphicsDriver.CreateGraphicsBuffer( &mPerObject, sizeof( mPerObject ), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic );

	mGraphicsDriver.SetVSConstantBuffer( mPerCameraBuffer, 0 );
	mGraphicsDriver.SetPSConstantBuffer( mPerCameraBuffer, 0 );

	mGraphicsDriver.SetVSConstantBuffer( mPerObjectBuffer, 1 );

	// Lighting
	mLightingBuffer = mGraphicsDriver.CreateGraphicsBuffer( &mLighting, sizeof( mLighting ), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic );
	mGraphicsDriver.SetPSConstantBuffer( mLightingBuffer, 2 );

	// Sampler State
	mDefaultSampler = mGraphicsDriver.CreateSamplerState();
	mGraphicsDriver.SetPSSamplerState( mDefaultSampler, 0 );
	
	return true;
}
