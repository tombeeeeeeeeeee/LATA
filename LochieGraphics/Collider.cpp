#include "Collider.h"

#include "Utilities.h"
#include "EditorGUI.h"

#include <iostream>

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
		{ "collisionLayer", collisionLayer },
		{ "type", (int)getType() }
	};
}

Collider* Collider::Load(toml::table table)
{
	switch ((ColliderType)Serialisation::LoadAsInt(table["type"]))
	{
	case ColliderType::polygon:
		return new PolygonCollider(table);
	case ColliderType::plane:
		return new PlaneCollider(table);
	case ColliderType::directionalPoly:
		return new DirectionalCollider(table);
	default:
		std::cout << "Unsupported Collider attempting to load\n";
		break;
	}
}

void Collider::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::BeginDisabled();
	std::string type = "Error!";
	switch (getType())
	{
	case ColliderType::empty:           type = "Empty";               break;
	case ColliderType::polygon:         type = "Polygon";             break;
	case ColliderType::plane:           type = "Plane";               break;
	case ColliderType::directionalPoly: type = "Directional Polygon"; break;
	}
	ImGui::InputText(("Type##" + tag).c_str(), &type);
	ImGui::EndDisabled();
	ImGui::Checkbox(("Trigger##" + tag).c_str(), &isTrigger);

	// TODO: Better gui to show what layer this is on
	ImGui::InputInt(("Layers##" + tag).c_str(), &collisionLayer);
}

Collider::Collider(toml::table table)
{
	isTrigger = Serialisation::LoadAsBool(table["isTrigger"]);
	collisionLayer = Serialisation::LoadAsInt(table["collisionLayer"]);
}

toml::table PolygonCollider::Serialise(unsigned long long GUID) const
{
	toml::array savedVerts;
	for (size_t i = 0; i < verts.size(); i++)
	{
		savedVerts.push_back(Serialisation::SaveAsVec2(verts[i]));
	}
	toml::table table = Collider::Serialise(GUID);
	table.emplace("verts", savedVerts);
	table.emplace("radius", radius);

	return table;
}

PolygonCollider::PolygonCollider(toml::table table) : Collider(table)
{
	toml::array* loadingVerts = table["verts"].as_array();
	for (size_t i = 0; i < loadingVerts->size(); i++)
	{
		verts.push_back(Serialisation::LoadAsVec2(loadingVerts->at(i).as_array()));
	}
	radius = Serialisation::LoadAsFloat(table["radius"]);
}

void PolygonCollider::GUI()
{
	Collider::GUI();

}

toml::table PlaneCollider::Serialise(unsigned long long GUID) const
{
	toml::table table = Collider::Serialise(GUID);
	table.emplace("normal", Serialisation::SaveAsVec2(normal));
	table.emplace("displacement", displacement);

	return table;
}

PlaneCollider::PlaneCollider(toml::table table) : Collider(table)
{
	normal = Serialisation::LoadAsVec2(table["normal"]);
	displacement = Serialisation::LoadAsFloat(table["displacement"]);
}

void PlaneCollider::GUI()
{
	Collider::GUI();

}

toml::table DirectionalCollider::Serialise(unsigned long long GUID) const
{
	toml::table table = Collider::Serialise(GUID);
	toml::array savedFaces;
	for (size_t i = 0; i < collidingFaces.size(); i++)
	{
		savedFaces.push_back(collidingFaces[i]);
	}
	table.emplace("collidingFaces", savedFaces);
	return table;
}

DirectionalCollider::DirectionalCollider(toml::table table) : PolygonCollider(table)
{
	toml::array* loadingFaces = table["collidingFaces"].as_array();
	for (size_t i = 0; i < loadingFaces->size(); i++)
	{
		collidingFaces.push_back(Serialisation::LoadAsInt(&loadingFaces->at(i)));
	}
}

void DirectionalCollider::GUI()
{
	Collider::GUI();

}
