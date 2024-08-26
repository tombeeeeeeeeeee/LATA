#include "Collider.h"

void Collider::setCollisionLayer(int layer)
{
	collisionLayer = layer;
}

CollisionLayers Collider::getCollisionLayer()
{
	return (CollisionLayers)collisionLayer;
}
