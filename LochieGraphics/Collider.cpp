#include "Collider.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"
#include "Transform.h"
#include "RigidBody.h"
#include "RenderSystem.h"
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
	int colliderTypeInt = Serialisation::LoadAsInt(table["type"]);
	switch ((ColliderType)colliderTypeInt)
	{
	case ColliderType::polygon:
		return new PolygonCollider(table);
	case ColliderType::plane:
		return new PlaneCollider(table);
	case ColliderType::directionalPoly:
		return new DirectionalCollider(table);
	default:
		std::cout << "Unsupported Collider attempting to load\n";
		return nullptr;
	}
}

void Collider::GUI()
{
	ImGui::Indent();
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

	const char* layers[] = {
		"Base", "Enemy", "Reflective", 
		"Sync", "Ecco", "Trigger",
		"Enemy Projectile", "Soft Cover", "Half Cover"
	};
	const char* currType = layers[(int)log2f(collisionLayer)];
	ImGui::PushItemWidth(180);
	if (ImGui::BeginCombo(("Collision Layer##" + tag).c_str(), currType))
	{
		for (int i = 0; i < (int)log2f((int)CollisionLayers::count); i++)
		{
			bool isSelected = pow(2, i) == collisionLayer;
			if (ImGui::Selectable(layers[i], isSelected))
			{
				collisionLayer = (int)pow(2, i);
			}
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Unindent();
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
	ImGui::Indent();
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Verts##" + tag).c_str())) {
		for (size_t i = 0; i < verts.size(); i++)
		{
			ImGui::DragFloat2(("Vert " + std::to_string(i) + "##" + tag).c_str(), &verts[i].x);
		}
	}
	ImGui::DragFloat(("Radius##" + tag).c_str(), &radius);
	if (ImGui::Button("Remove Vert"))
	{
		verts.pop_back();
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Vert"))
	{
		verts.push_back({0.0f, 0.0f});
	}
	ImGui::Unindent();
}

void PolygonCollider::DebugDraw(Transform* transform)
{
	if (verts.size() == 1)
	{
		glm::vec2 p = RigidBody::Transform2Din3DSpace(transform->getGlobalMatrix(), verts[0]);
		float height = 0.1f;
		if (collisionLayer & (int)CollisionLayers::reflectiveSurface) height = 70.0f;
		RenderSystem::debugLines.DrawCircle({p.x, height, p.y}, radius, { 0.0f,1.0f,0.0f });
	}
	else
	{
		glm::vec2 prevVert = RigidBody::Transform2Din3DSpace(transform->getGlobalMatrix(), verts[verts.size() - 1]);
		for (int i = 0; i < verts.size(); i++)
		{
			glm::vec2 curVert = RigidBody::Transform2Din3DSpace(transform->getGlobalMatrix(), verts[i]);
			RenderSystem::debugLines.DrawLineSegement2D(prevVert, curVert, { 0.0f,1.0f,0.0f });
			prevVert = curVert;
		}
	}
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
	ImGui::Indent();
	std::string tag = Utilities::PointerToString(this);
	// TODO: Better gui option for normal vectors
	ImGui::DragFloat2(("Normal##" + tag).c_str(), &normal.x);
	ImGui::DragFloat(("Displacement##" + tag).c_str(), &displacement);
	ImGui::Unindent();
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
	PolygonCollider::GUI();
	ImGui::Indent();
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Colliding Faces##" + tag).c_str())) {
		for (size_t i = 0; i < collidingFaces.size(); i++)
		{
			ImGui::DragInt(("##" + std::to_string(i) + tag).c_str(), &collidingFaces[i]);
		}
	}
	ImGui::Unindent();
}
