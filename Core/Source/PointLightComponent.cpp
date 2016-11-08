#include "ITPEnginePCH.h"

IMPL_COMPONENT(PointLightComponent, Component, MAX_POINT_LIGHTS);

PointLightComponent::PointLightComponent(Actor& owner)
	:Component(owner)
{
	SetEnabled( true );
	SetPosition( owner.GetPosition() );
}

void PointLightComponent::Register()
{
	Super::Register();
	mOwner.GetGame().GetRenderer().AddPointLight(ThisPtr());
}

void PointLightComponent::Unregister()
{
	Super::Unregister();
	mOwner.GetGame().GetRenderer().RemovePointLight(ThisPtr());
}

void PointLightComponent::OnUpdatedTransform()
{
	SetPosition( mOwner.GetPosition() );
}

void PointLightComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);
	
	// PointLight values set from JSON
	Vector3 diffuseColor;
	if ( GetVectorFromJSON( properties, "diffuseColor", diffuseColor ) )
	{
		SetDiffuseColor( diffuseColor );
	}

	Vector3 specularColor;
	if ( GetVectorFromJSON( properties, "specularColor", specularColor ) )
	{
		SetSpecularColor( specularColor );
	}

	float innerRadius;
	if ( GetFloatFromJSON( properties, "innerRadius", innerRadius ) )
	{
		SetInnerRadius( innerRadius );
	}

	float outerRadius;
	if ( GetFloatFromJSON( properties, "outerRadius", outerRadius ) )
	{
		SetOuterRadius( outerRadius );
	}

	float specularPower;
	if ( GetFloatFromJSON( properties, "specularPower", specularPower ) )
	{
		SetSpecularPower( specularPower );
	}
}

