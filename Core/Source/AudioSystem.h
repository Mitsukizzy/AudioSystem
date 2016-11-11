#pragma once
#include "Component.h"

class AudioSystem : public Component
{
	DECL_COMPONENT(AudioSystem, Component);
public:
	AudioSystem(Actor& owner);
};

DECL_PTR(AudioSystem);
