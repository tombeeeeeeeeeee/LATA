#pragma once
#include "Maths.h"
#include <vector>

struct Node;

struct Edge
{
    Node* target;
    float cost;
    Edge() { target = nullptr; cost = 0.0f; }
    Edge(Node* _target, float _cost) : target(_target), cost(_cost) {}
};

struct Node
{
    glm::vec2 position;
    std::vector<Edge> connections;
    float gScore = 0.0f;
    float fScore = 0.0f;
    Node* previous = nullptr;

    Node(float x, float y)
    {
        position.x = x;
        position.y = y;
    };

    void ConnectTo(Node* other, float cost)
    {
        connections.push_back(Edge(other, cost));
    }

};