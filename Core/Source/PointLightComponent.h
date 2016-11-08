#pragma once
#include "Component.h"
#include "PointLightData.h"

class PointLightComponent : public Component
{
	DECL_COMPONENT(PointLightComponent, Component);
public:
	PointLightComponent(Actor& owner);

	void Register() override;
	void Unregister() override;

	void OnUpdatedTransform() override;

	const PointLightData& GetData() const { return mData; }

	// Getter/setter functions for each variable in PointLightData
	void SetDiffuseColor( Vector3 diffuseColor ) {
		mData.diffuseColor = diffuseColor; 
	};
	Vector3 GetDiffuseColor() {
		return mData.diffuseColor;
	};

	void SetSpecularColor( Vector3 specularColor ){
		mData.specularColor = specularColor;
	};
	Vector3 GetSpecularColor(){
		return mData.specularColor;
	};

	void SetPosition( Vector3 position ){
		mData.position = position;
	};
	Vector3 GetPosition(){
		return mData.position;
	};

	void SetSpecularPower( float specularPower ){
		mData.specularPower = specularPower;
	};
	float GetSpecularPower(){
		return mData.specularPower;
	};

	void SetInnerRadius( float innerRadius ){
		mData.innerRadius = innerRadius;
	};
	float GetInnerRadius(){
		return mData.innerRadius;
	};

	void SetOuterRadius( float outerRadius ){
		mData.outerRadius = outerRadius;
	};
	float GetOuterRadius(){
		return mData.outerRadius;
	};

	void SetEnabled( bool enabled ){
		mData.enabled = enabled;
	};
	float GetEnabled(){
		return mData.enabled;
	};

	void SetProperties(const rapidjson::Value& properties) override;
private:
	PointLightData mData;
};

DECL_PTR(PointLightComponent);
