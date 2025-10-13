//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/MeshFactory.h"
#include "Graphics/Mesh.h"
#include <glm/ext.hpp>

namespace Real {

    std::pair<std::vector<Graphics::Vertex>, std::vector<unsigned int>> MeshFactory::CreateTriangle() {
        std::vector<Graphics::Vertex> vertices = {
            { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f) }
        };

        std::vector<unsigned int> indices = { 0, 1, 2 };

        return std::make_pair(vertices, indices);
    }

    std::pair<std::vector<Graphics::Vertex>, std::vector<unsigned int>> MeshFactory::CreateCube() {
        std::vector<Graphics::Vertex> vertices =
		{	// Vertices, Normal, UV

			// Front face (+Z)
			{ glm::vec3(-0.5f, -0.5f, 0.5f),   glm::vec3 {0.0,  0.0, 1.0},  {0.0f, 0.0f}  },
			{ glm::vec3(0.5f,  -0.5f, 0.5f),   glm::vec3 {0.0,  0.0, 1.0},  {1.0f, 0.0f}  },
			{ glm::vec3(0.5f,   0.5f, 0.5f),   glm::vec3 {0.0,  0.0, 1.0},  {1.0f, 1.0f}  },
			{ glm::vec3(-0.5f,  0.5f, 0.5f),   glm::vec3 {0.0,  0.0, 1.0},  {0.0f, 1.0f}  },

			// Back face  (-Z)
			{ glm::vec3(0.5f,  -0.5f, -0.5f),  glm::vec3 {0.0,  0.0, -1.0}, {0.0f, 0.0f}  },
			{ glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3 {0.0,  0.0, -1.0}, {1.0f, 0.0f}  },
			{ glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3 {0.0,  0.0, -1.0}, {1.0f, 1.0f}  },
			{ glm::vec3(0.5f,   0.5f, -0.5f),  glm::vec3 {0.0,  0.0, -1.0}, {0.0f, 1.0f}  },

			// Left face  (-X)
			{ glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3 {-1.0,  0.0, 0.0}, {0.0f, 0.0f}  },
			{ glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3 {-1.0,  0.0, 0.0}, {1.0f, 0.0f}  },
			{ glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec3 {-1.0,  0.0, 0.0}, {1.0f, 1.0f}  },
			{ glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3 {-1.0,  0.0, 0.0}, {0.0f, 1.0f}  },

			// Right face (+X)
			{ glm::vec3(0.5f, -0.5f,  0.5f),   glm::vec3 {1.0,  0.0, 0.0},  {0.0f, 0.0f}  },
			{ glm::vec3(0.5f, -0.5f, -0.5f),   glm::vec3 {1.0,  0.0, 0.0},  {1.0f, 0.0f}  },
			{ glm::vec3(0.5f,  0.5f, -0.5f),   glm::vec3 {1.0,  0.0, 0.0},  {1.0f, 1.0f}  },
			{ glm::vec3(0.5f,  0.5f,  0.5f),   glm::vec3 {1.0,  0.0, 0.0},  {0.0f, 1.0f}  },

			// Top face (+Y)
			{ glm::vec3(-0.5f, 0.5f,  0.5f),   glm::vec3 {0.0,  1.0, 0.0},  {0.0f, 0.0f}  },
			{ glm::vec3(0.5f,  0.5f,  0.5f),   glm::vec3 {0.0,  1.0, 0.0},  {1.0f, 0.0f}  },
			{ glm::vec3(0.5f,  0.5f, -0.5f),   glm::vec3 {0.0,  1.0, 0.0},  {1.0f, 1.0f}  },
			{ glm::vec3(-0.5f, 0.5f, -0.5f),   glm::vec3 {0.0,  1.0, 0.0},  {0.0f, 1.0f}  },

			// Bottom face (-Y)
			{ glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3 {0.0, -1.0, 0.0},  {0.0f, 0.0f}  },
			{ glm::vec3(0.5f,  -0.5f, -0.5f),  glm::vec3 {0.0, -1.0, 0.0},  {1.0f, 0.0f}  },
			{ glm::vec3(0.5f,  -0.5f,  0.5f),  glm::vec3 {0.0, -1.0, 0.0},  {1.0f, 1.0f}  },
			{ glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3 {0.0, -1.0, 0.0},  {0.0f, 1.0f}  },
		};

		std::vector<unsigned int> indices =
		{
			// Front face
			0, 1, 2,      2, 3, 0,

			// Back face
			4, 5, 6,      6, 7, 4,

			// Left face
			8, 9, 10,     10, 11, 8,

			// Right face
			12, 13, 14,   14, 15, 12,

			// Top face
			16, 17, 18,   18, 19, 16,

			// Bottom face
			20, 21, 22,   22, 23, 20,
		};

    	return std::make_pair(vertices, indices);
    }

}
