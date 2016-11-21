#include "ITPEnginePCH.h"

IMPL_COMPONENT(SkeletalMeshComponent, MeshComponent, 32);

SkeletalMeshComponent::SkeletalMeshComponent(Actor& owner)
	:MeshComponent(owner)
{

}

void SkeletalMeshComponent::Draw(class Renderer& render)
{
	if ( mMesh )
	{
		render.DrawSkeletalMesh( mMesh->GetVertexArray(), mMesh->GetTexture( mTextureIndex ),
			mOwner.GetWorldTransform(), mPalette );
	}
}

void SkeletalMeshComponent::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if ( mAnimation )
	{
		mAnimationTime += deltaTime * mAnimationPlayRate;

		if ( mAnimationTime > mAnimation->GetLength() ) 
		{
			mAnimationTime -= mAnimation->GetLength();
		}
		ComputeMatrixPalette();
	}
}

float SkeletalMeshComponent::PlayAnimation(AnimationPtr anim, float playRate /*= 1.0f*/, float blendTime /*= 0.0f*/)
{
	DbgAssert(mSkeleton != nullptr, "Can't play an animation without a skeleton!");
	DbgAssert(mSkeleton->GetNumBones() == anim->GetNumBones(), 
		"Skeleton and animation have a different number of bones!");

	mAnimation = anim;
	mAnimationPlayRate = playRate;
	mAnimationTime = 0.0f;

	ComputeMatrixPalette();

	return anim->GetLength();
}

void SkeletalMeshComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);

	std::string skeleton;
	if (GetStringFromJSON(properties, "skeleton", skeleton))
	{
		mSkeleton = mOwner.GetGame().GetAssetCache().Load<Skeleton>(skeleton);
	}
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	auto bindPoses = mSkeleton->GetGlobalInvBindPoses();
	std::vector<Matrix4> outPoses(mSkeleton->GetNumBones());
	mAnimation->GetGlobalPoseAtTime( outPoses, mSkeleton, mAnimationTime );

	for ( size_t b = 0; b < mSkeleton->GetNumBones(); b++ )
	{
		mPalette.bones[b] = bindPoses[b] * outPoses[b];
	}
}
