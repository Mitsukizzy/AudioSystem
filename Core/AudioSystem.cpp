#include "ITPEnginePCH.h"
#include <SDL/SDL_mixer.h>

IMPL_COMPONENT(AudioSystem, Component, 100);

AudioSystem::AudioSystem(Actor& owner)
	:Component(owner)
{

}