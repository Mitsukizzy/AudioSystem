#include "ITPEnginePCH.h"

Renderer::Renderer(Game& game)
	:mGame(game)
	,mWindow(nullptr)
	,mWidth(0)
	,mHeight(0)
{

}

Renderer::~Renderer()
{
	// Clear components...
	mDrawComponents.clear();
	mComponents2D.clear();

	mDepthBuffer.reset();
	mSpriteDepthState.reset();
	mMeshDepthState.reset();
	mRasterState.reset();

	mSpriteBlendState.reset();
	mMeshBlendState.reset();

	mSpriteShader.reset();
	mSpriteVerts.reset();

	mMeshShaders.clear();

	// Shutdown the input cache and graphics driver
	mInputLayoutCache.reset();
	mGraphicsDriver.reset();

	if (mWindow != nullptr)
	{
		SDL_DestroyWindow(mWindow);
	}
}

bool Renderer::Init(int width, int height)
{
	// Create our SDL window
	mWindow = SDL_CreateWindow("ITP Engine 2 Demo! Featuring: Low Level Audio System!", 100, 100, width, height, 
		0);

	if (!mWindow)
	{
		SDL_Log("Could not create window.");
		return false;
	}

	mGraphicsDriver = std::make_shared<GraphicsDriver>(GetActiveWindow());
	mInputLayoutCache = std::make_shared<InputLayoutCache>();

	mWidth = width;
	mHeight = height;

	if (!InitFrameBuffer())
	{
		return false;
	}

	if (!InitShaders())
	{
		return false;
	}

	if (!InitSpriteVerts())
	{
		return false;
	}

	return true;
}

void Renderer::RenderFrame()
{
	Clear();
	DrawComponents();
	Present();
}

void Renderer::AddComponent(DrawComponentPtr component)
{
	if (IsA<SpriteComponent>(component) || IsA<FontComponent>(component))
	{
		mComponents2D.emplace(component);
	}
	else
	{
		mDrawComponents.emplace(component);
	}
}

void Renderer::RemoveComponent(DrawComponentPtr component)
{
	if (IsA<SpriteComponent>(component) || IsA<FontComponent>(component))
	{
		auto iter = mComponents2D.find(component);
		if (iter != mComponents2D.end())
		{
			mComponents2D.erase(iter);
		}
	}
	else
	{
		auto iter = mDrawComponents.find(component);
		if (iter != mDrawComponents.end())
		{
			mDrawComponents.erase(iter);
		}
	}
}

void Renderer::AddPointLight(PointLightComponentPtr light)
{
	mPointLights.emplace( light );
	UpdatePointLights();
}

void Renderer::RemovePointLight(PointLightComponentPtr light)
{
	mPointLights.erase( light );
	UpdatePointLights();
}

void Renderer::UpdatePointLights()
{
	std::unordered_map<EMeshShader, ShaderPtr>::iterator iter = mMeshShaders.begin();
	for ( iter ; iter != mMeshShaders.end(); iter++ )
	{
		std::unordered_set<PointLightComponentPtr>::iterator sIter = mPointLights.begin();
		int i = 0;
		for ( sIter ; sIter != mPointLights.end(); sIter++ )
		{
			iter->second->GetLightingConstants().pointLights[i] = ( *sIter )->GetData();
			i++;
		}

		// Set extra indices in array to enabled false if less than 8 light points
		for ( int j = i; j < 8; j++ )
		{
			iter->second->GetLightingConstants().pointLights[j].enabled = false;
		}
		iter->second->UploadLightingConstants();
	}
}

void Renderer::DrawSprite(TexturePtr texture, const Matrix4& worldTransform)
{
	mSpriteShader->SetActive();
	mSpriteShader->GetPerObjectConstants().worldMat = worldTransform;
	mSpriteShader->UploadPerObjectConstants();
	mSpriteShader->BindTexture( texture, 0 );
	DrawVertexArray( mSpriteVerts );
}

void Renderer::DrawMesh(VertexArrayPtr vertArray, TexturePtr texture, const Matrix4& worldTransform, EMeshShader type /*= EMS_Basic*/)
{
	auto meshShader = mMeshShaders.find( type )->second;
	meshShader->SetActive();
	meshShader->GetPerObjectConstants().worldMat = worldTransform;
	meshShader->UploadPerObjectConstants();
	meshShader->BindTexture( texture, 0 );
	DrawVertexArray( vertArray );
}

void Renderer::DrawSkeletalMesh(VertexArrayPtr vertArray, TexturePtr texture, const Matrix4& worldTransform, const struct MatrixPalette& palette)
{	
	auto meshShader = mMeshShaders.find( EMS_Skinned )->second;
	meshShader->SetActive();
	meshShader->GetPerObjectConstants().worldMat = worldTransform;
	meshShader->UploadPerObjectConstants();
	meshShader->BindTexture( texture, 0 );
	meshShader->UploadMatrixPalette( palette );
	DrawVertexArray( vertArray );
}

void Renderer::DrawVertexArray(VertexArrayPtr vertArray)
{
	vertArray->SetActive();
	mGraphicsDriver->DrawIndexed( vertArray->GetIndexCount(), 0, 0 );
}

void Renderer::UpdateViewMatrix(const Matrix4& view)
{
	mView = view;

	std::unordered_map<EMeshShader, ShaderPtr>::iterator iter = mMeshShaders.begin();
	for ( iter ; iter != mMeshShaders.end(); iter++ )
	{
		iter->second->GetPerCameraConstants().viewMat = mView * mProj;
		iter->second->GetPerCameraConstants().worldCamPos = mView.GetTranslation();
	}
}

void Renderer::SetAmbientLight(const Vector3& color)
{	
	std::unordered_map<EMeshShader, ShaderPtr>::iterator iter = mMeshShaders.begin();
	for ( iter ; iter != mMeshShaders.end(); iter++ )
	{
		iter->second->GetLightingConstants().ambientLight = color;
		iter->second->UploadLightingConstants();
	}
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
	// Convert screenPoint to device coordinates (between -1 and +1)
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (mWidth) * 0.5f;
	deviceCoord.y /= (mHeight) * 0.5f;

	// First, undo the projection
	Matrix4 unprojection = mProj;
	unprojection.Invert();
	Vector3 unprojVec = TransformWithPerspDiv(deviceCoord, unprojection);

	// Now undo the view matrix
	Matrix4 uncamera = mView;
	uncamera.Invert();
	return Transform(unprojVec, uncamera);
}

void Renderer::Clear()
{
	mGraphicsDriver->ClearBackBuffer( Vector3( 0, 0, 0 ), 1 );
	mGraphicsDriver->ClearDepthStencil( mDepthBuffer, 1.0f );
}

void Renderer::DrawComponents()
{
	// Enable depth buffering and disable blending
	mGraphicsDriver->SetDepthStencilState( mMeshDepthState );
	mGraphicsDriver->SetBlendState( mMeshBlendState );

	// Upload per camera constants
	std::unordered_map<EMeshShader, ShaderPtr>::iterator iter = mMeshShaders.begin();
	for ( iter ; iter != mMeshShaders.end(); iter++ )
	{
		iter->second->UploadPerCameraConstants();
	}

	// Update point lights
	UpdatePointLights();

	// Draw the normal components
	for (auto& comp : mDrawComponents)
	{
		if (comp->IsVisible())
		{
			comp->Draw(*this);
		}
	}

	// Disable depth buffering and enable blending
	mGraphicsDriver->SetDepthStencilState( mSpriteDepthState );
	mGraphicsDriver->SetBlendState( mSpriteBlendState );	

	// Draw the 2D components
	for (auto& comp : mComponents2D)
	{
		if (comp->IsVisible())
		{
			comp->Draw(*this);
		}
	}
}

void Renderer::Present()
{
	mGraphicsDriver->Present();
}

bool Renderer::InitFrameBuffer()
{
	mDepthBuffer = mGraphicsDriver->CreateDepthStencil( mWidth, mHeight );
	mGraphicsDriver->SetDepthStencil( mDepthBuffer );

	mMeshDepthState = mGraphicsDriver->CreateDepthStencilState( true, ECF_Less );
	mSpriteDepthState = mGraphicsDriver->CreateDepthStencilState( false, ECF_Always );

	mSpriteBlendState = mGraphicsDriver->CreateBlendState( true );
	mMeshBlendState = mGraphicsDriver->CreateBlendState( false );

	mRasterState = mGraphicsDriver->CreateRasterizerState( EFM_Solid );
	mGraphicsDriver->SetRasterizerState( mRasterState );
	return true;
}

bool Renderer::InitShaders()
{
	// Load sprite shader and basic shader
	// And create input layouts needed for both
	mSpriteShader = mGame.GetAssetCache().Load<Shader>( "Shaders/Sprite.hlsl" );
	if ( !mSpriteShader )
	{
		return false;
	}

	auto basicMesh = mGame.GetAssetCache().Load<Shader>( "Shaders/BasicMesh.hlsl" );
	if ( !basicMesh )
	{
		return false;
	}

	mMeshShaders.emplace( EMS_Basic, basicMesh );

	mProj = Matrix4::CreatePerspectiveFOV( 
		Math::ToRadians( 70.0f ),
		static_cast<float>( mWidth ),
		static_cast<float>( mHeight ),
		25.0f,
		10000.0f );

	
	InputLayoutElement layoutElemsNorm[] = {
		{ "POSITION", 0, EGF_R32G32B32_Float, 0 },
		{ "NORMAL", 0, EGF_R32G32B32_Float, 12 },
		{ "TEXCOORD", 0, EGF_R32G32_Float, 24 },
	};

	InputLayoutPtr layoutNorm = mGraphicsDriver->CreateInputLayout( layoutElemsNorm, 3, basicMesh->GetCompiledVS() );
	mInputLayoutCache->RegisterLayout( "positionnormaltexcoord", layoutNorm );	

	InputLayoutElement layoutElems[] = {
		{ "POSITION", 0, EGF_R32G32B32_Float, 0 },
		{ "TEXCOORD", 0, EGF_R32G32_Float, 3 * sizeof( float ) },
	};

	InputLayoutPtr layout = mGraphicsDriver->CreateInputLayout( layoutElems, 2, mSpriteShader->GetCompiledVS() );
	mInputLayoutCache->RegisterLayout( "positiontexcoord", layout );

	mSpriteShader->GetPerCameraConstants().viewMat = Matrix4::CreateOrtho( static_cast<float>( mWidth ), static_cast<float>( mHeight ), 1000.0f, -1000.0f );
	mSpriteShader->UploadPerCameraConstants();

	// Load phong shader
	auto phongShader = mGame.GetAssetCache().Load<Shader>( "Shaders/Phong.hlsl" );
	if ( !phongShader )
	{
		return false;
	}

	phongShader->GetPerCameraConstants().viewMat = Matrix4::CreateOrtho( static_cast<float>( mWidth ), static_cast<float>( mHeight ), 1000.0f, -1000.0f );
	phongShader->UploadPerCameraConstants();

	mMeshShaders.emplace( EMS_Phong, phongShader );

	InputLayoutPtr layoutPhong = mGraphicsDriver->CreateInputLayout( layoutElemsNorm, 3, phongShader->GetCompiledVS() );
	mInputLayoutCache->RegisterLayout( "positionnormaltexcoord", layoutPhong );	


	// Load skinned shader and create appropriate input layout
	auto skinShader = mGame.GetAssetCache().Load<Shader>( "Shaders/Skinned.hlsl" );
	if ( !skinShader )
	{
		return false;
	}
	skinShader->GetPerCameraConstants().viewMat = Matrix4::CreateOrtho( static_cast<float>( mWidth ), static_cast<float>( mHeight ), 1000.0f, -1000.0f );
	skinShader->UploadPerCameraConstants();

	mMeshShaders.emplace( EMS_Skinned, skinShader );
	skinShader->CreateMatrixPaletteBuffer();

	InputLayoutElement layoutElemsSkin[] = {
		{ "POSITION", 0, EGF_R32G32B32_Float, 0 },
		{ "NORMAL", 0, EGF_R32G32B32_Float, 12 },
		{ "BONEINDICES", 0, EGF_R8G8B8A8_UInt, 24 },
		{ "BONEWEIGHTS", 0, EGF_R8G8B8A8_UNorm, 28 },
		{ "TEXCOORD", 0, EGF_R32G32_Float, 32 }
	};

	InputLayoutPtr layoutSkin = mGraphicsDriver->CreateInputLayout( layoutElemsSkin, 5, skinShader->GetCompiledVS() );
	mInputLayoutCache->RegisterLayout( "positionnormalbonesweightstexcoord", layoutSkin );	

	return true;
}

bool Renderer::InitSpriteVerts()
{
	// Create the vertex array for sprites
	float verts[] =
	{
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,  // top left
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // top right
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom left
	};

	uint16_t indices[] =
	{
		0, 1, 2, // <top left, top right, bottom right>
		2, 3, 0, // <bottom right, bottom left, top left>
	};

	mSpriteVerts = VertexArray::Create(GetGraphicsDriver(), GetInputLayoutCache(),
		verts, 4, 20, "positiontexcoord", indices, 6);

	return true;
}
