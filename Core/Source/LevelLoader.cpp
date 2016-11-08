#include "ITPEnginePCH.h"

LevelLoader::LevelLoader(Game& game)
	:mGame(game)
{
	SetupSpawnMaps();
}

void LevelLoader::SetupSpawnMaps()
{
	// Actor spawn map
	mActorSpawnMap.emplace("Actor", &Actor::SpawnWithProperties);
	mActorSpawnMap.emplace("KillVolume", &KillVolume::SpawnWithProperties);
	mActorSpawnMap.emplace("Player", &Player::SpawnWithProperties);

	// Component spawn map
	mCompSpawnMap.emplace("BoxComponent", 
		ComponentInfo(BoxComponent::StaticType(), &BoxComponent::CreateWithProperties));
	mCompSpawnMap.emplace("CameraComponent",
		ComponentInfo(CameraComponent::StaticType(), &CameraComponent::CreateWithProperties));
	mCompSpawnMap.emplace("CharacterMoveComponent",
		ComponentInfo(CharacterMoveComponent::StaticType(), &CharacterMoveComponent::CreateWithProperties));
	mCompSpawnMap.emplace("MeshComponent", 
		ComponentInfo(MeshComponent::StaticType(), &MeshComponent::CreateWithProperties));
	mCompSpawnMap.emplace("PointLightComponent",
		ComponentInfo(PointLightComponent::StaticType(), &PointLightComponent::CreateWithProperties));
	mCompSpawnMap.emplace("SkeletalMeshComponent",
		ComponentInfo(SkeletalMeshComponent::StaticType(), &SkeletalMeshComponent::CreateWithProperties));
}

void LevelLoader::Load(const char* fileName)
{
	std::ifstream file(fileName);

	if (!file.is_open())
	{
		SDL_Log("Level file %s not found", fileName);
		return;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		SDL_Log("Level file %s is not valid JSON", fileName);
		DbgAssert(false, "Level file is not valid JSON!");
		return;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itplevel" ||
		ver != 1)
	{
		SDL_Log("Level %s is not a know level file format", fileName);
		return;
	}

	// TODO: Lab 3

	// Step 1: Setup any overall world properties
	Vector3 ambientLight;
	GetVectorFromJSON( doc["world"], "ambientLight", ambientLight );
	mGame.GetRenderer().SetAmbientLight( ambientLight );

	// Step 2: Setup the actors (and each of their components)
	rapidjson::Value& actors = doc["actors"];

	// Loop through all actors
	for ( rapidjson::SizeType i = 0; i < actors.Size(); i++ )
	{
		std::string type;
		GetStringFromJSON( actors[i], "type", type );
		rapidjson::Value& props = actors[i]["properties"];

		auto actor = mActorSpawnMap.find( type )->second( mGame, props );

		// Loop through update components for this actor
		rapidjson::Value& updatedComps = actors[i]["updatedComponents"];
		for ( rapidjson::SizeType j = 0; j < updatedComps.Size(); j++ )
		{
			std::string compType;
			GetStringFromJSON( updatedComps[j], "type", compType );
			rapidjson::Value& compProps = updatedComps[j]["properties"];
			auto newType = mCompSpawnMap.find( compType )->second.mType;

			auto newCompFromType = actor->GetComponentFromType( newType );
			newCompFromType->SetProperties( compProps );
		}

		// Loop through new components for this actor
		rapidjson::Value& newComps = actors[i]["newComponents"];
		for ( rapidjson::SizeType k = 0; k < newComps.Size(); k++ )
		{
			// To CreateWithProperties, need actor ref, update type, and comp properties
			std::string compType;
			GetStringFromJSON( newComps[k], "type", compType );

			std::string compUpdate;
			GetStringFromJSON( newComps[k], "update", compUpdate );
			
			Component::UpdateType compUpdateType;
			if ( compUpdate == "PreTick" )
			{
				compUpdateType = Component::PreTick;
			}
			else
			{
				compUpdateType = Component::PostTick;
			}

			rapidjson::Value& compProps = newComps[k]["properties"];
			auto newComp = mCompSpawnMap.find( compType )->second.mFunc( *actor, compUpdateType, compProps );
			newComp->SetProperties( compProps );
		}

		actor->BeginPlay();
	}
}

// Global helper functions
bool GetFloatFromJSON(const rapidjson::Value& inObject, const char* inProperty, float& outFloat)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsDouble())
	{
		return false;
	}

	outFloat = property.GetDouble();
	return true;
}

bool GetIntFromJSON(const rapidjson::Value& inObject, const char* inProperty, int& outInt)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsInt())
	{
		return false;
	}

	outInt = property.GetInt();
	return true;
}

bool GetStringFromJSON(const rapidjson::Value& inObject, const char* inProperty, std::string& outStr)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsString())
	{
		return false;
	}

	outStr = property.GetString();
	return true;
}

bool GetBoolFromJSON(const rapidjson::Value& inObject, const char* inProperty, bool& outBool)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsBool())
	{
		return false;
	}

	outBool = property.GetBool();
	return true;
}

bool GetVectorFromJSON(const rapidjson::Value& inObject, const char* inProperty, Vector3& outVector)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsArray() || property.Size() != 3)
	{
		return false;
	}

	for (rapidjson::SizeType i = 0; i < 3; i++)
	{
		if (!property[i].IsDouble())
		{
			return false;
		}
	}

	outVector.x = property[0].GetDouble();
	outVector.y = property[1].GetDouble();
	outVector.z = property[2].GetDouble();

	return true;
}

bool GetQuaternionFromJSON(const rapidjson::Value& inObject, const char* inProperty, Quaternion& outQuat)
{
	auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;

	for (rapidjson::SizeType i = 0; i < 4; i++)
	{
		if (!property[i].IsDouble())
		{
			return false;
		}
	}

	outQuat.x = property[0].GetDouble();
	outQuat.y = property[1].GetDouble();
	outQuat.z = property[2].GetDouble();
	outQuat.w = property[3].GetDouble();

	return true;
}
