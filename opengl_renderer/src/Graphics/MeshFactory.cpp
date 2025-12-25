//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/MeshFactory.h"
#include <glm/ext.hpp>
#include "Common/RealTypes.h"

namespace Real {

    std::pair<std::vector<Vertex>, std::vector<uint32_t>> MeshFactory::CreateTriangle() {
        std::vector<Vertex> vertices = {
            { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
            { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
            { glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f) }
        };

        std::vector<uint32_t> indices = { 0, 1, 2 };

        return std::make_pair(vertices, indices);
    }

    std::pair<std::vector<Vertex>, std::vector<uint32_t>> MeshFactory::CreateCube() {
        std::vector<Vertex> vertices =
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

		std::vector<uint32_t> indices =
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
