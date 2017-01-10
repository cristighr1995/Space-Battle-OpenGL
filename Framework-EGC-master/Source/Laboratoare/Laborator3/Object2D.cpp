#include "Object2D.h"

#include <Core/Engine.h>

Mesh* Object2D::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{		
		VertexFormat(corner + glm::vec3(-length/2, -length/2, 0), color),
		VertexFormat(corner + glm::vec3(length/2, -length/2, 0), color),
		VertexFormat(corner + glm::vec3(length/2, length/2, 0), color),
		VertexFormat(corner + glm::vec3(-length/2, length/2, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };
	
	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* Object2D::CreateShip(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner + glm::vec3(-length / 2, -length * sqrt(3) / 2, 0), color), // 0
		VertexFormat(corner, color),                                                    // 1
		VertexFormat(corner + glm::vec3(length / 2, -length * sqrt(3) / 2, 0), color),  // 2
		VertexFormat(corner + glm::vec3(-length / 2, length * sqrt(3) / 2, 0), color),  // 3
		VertexFormat(corner + glm::vec3(length / 2, length * sqrt(3) / 2, 0), color)    // 4
	};

	Mesh* doubletriangle = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 4, 1, 3};

	if (!fill) {
		doubletriangle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		//indices.push_back(1);
		//indices.push_back(4);
	}

	doubletriangle->InitFromData(vertices, indices);
	return doubletriangle;
}

Mesh* Object2D::CreateBullet(std::string name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner + glm::vec3(-width / 2, -height / 2, 0), color),
		VertexFormat(corner + glm::vec3(width / 2, -height / 2, 0), color),
		VertexFormat(corner + glm::vec3(width / 2, height / 2, 0), color),
		VertexFormat(corner + glm::vec3(-width / 2, height / 2, 0), color)
	};

	Mesh* bullet = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	if (!fill) {
		bullet->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	bullet->InitFromData(vertices, indices);
	return bullet;
}