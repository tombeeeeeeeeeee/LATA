#include "Collider.h"

void Collider::setCollisionLayer(int layer)
{
	collisionLayer = layer;
}

CollisionLayers Collider::getCollisionLayer()
{
	return (CollisionLayers)collisionLayer;
}

toml::table Collider::Serialise(unsigned long long GUID) const
{
	return toml::table{
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "isTrigger", isTrigger },
		{ "collisionLayer", collisionLayer }
	};
}

Collider* Collider::Load(toml::table table)
{
	// TODO: Write
	return nullptr;
}
