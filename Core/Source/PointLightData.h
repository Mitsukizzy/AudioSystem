#pragma once
#include "Math.h"

const size_t MAX_POINT_LIGHTS = 8;

struct PointLightData
{
	PointLightData();

	Vector3 diffuseColor;
	float padding;
	Vector3 specularColor;
	float padding2;
	Vector3 position;
	float specularPower;
	float innerRadius;
	float outerRadius;
	uint32_t enabled;
	float padding3;
};
