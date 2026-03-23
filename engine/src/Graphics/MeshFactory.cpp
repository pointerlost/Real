//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/MeshFactory.h"
#include "Graphics/RenderTypes.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"

namespace Real {

    std::pair<Vector<graphics::Vertex>, Vector<u32>> MeshFactory::CreateTriangle() {
        Vector<graphics::Vertex> vertices = {
            { math::Vec3(-0.5f, -0.5f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f), math::Vec2(0.0f, 0.0f) },
            { math::Vec3( 0.5f, -0.5f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f), math::Vec2(1.0f, 0.0f) },
            { math::Vec3( 0.0f,  0.5f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f), math::Vec2(0.5f, 1.0f) }
        };

        Vector<u32> indices = { 0, 1, 2 };

        return std::make_pair(vertices, indices);
    }

    std::pair<Vector<graphics::Vertex>, Vector<u32>> MeshFactory::CreateCube() {
        Vector<graphics::Vertex> vertices =
		{	// Vertices, Normal, UV

			// Front face (+Z)
			{ math::Vec3(-0.5f, -0.5f, 0.5f),   math::Vec3 {0.0,  0.0, 1.0},  {0.0f, 0.0f}  },
			{ math::Vec3(0.5f,  -0.5f, 0.5f),   math::Vec3 {0.0,  0.0, 1.0},  {1.0f, 0.0f}  },
			{ math::Vec3(0.5f,   0.5f, 0.5f),   math::Vec3 {0.0,  0.0, 1.0},  {1.0f, 1.0f}  },
			{ math::Vec3(-0.5f,  0.5f, 0.5f),   math::Vec3 {0.0,  0.0, 1.0},  {0.0f, 1.0f}  },

			// Back face  (-Z)
			{ math::Vec3(0.5f,  -0.5f, -0.5f),  math::Vec3 {0.0,  0.0, -1.0}, {0.0f, 0.0f}  },
			{ math::Vec3(-0.5f, -0.5f, -0.5f),  math::Vec3 {0.0,  0.0, -1.0}, {1.0f, 0.0f}  },
			{ math::Vec3(-0.5f,  0.5f, -0.5f),  math::Vec3 {0.0,  0.0, -1.0}, {1.0f, 1.0f}  },
			{ math::Vec3(0.5f,   0.5f, -0.5f),  math::Vec3 {0.0,  0.0, -1.0}, {0.0f, 1.0f}  },

			// Left face  (-X)
			{ math::Vec3(-0.5f, -0.5f, -0.5f),  math::Vec3 {-1.0,  0.0, 0.0}, {0.0f, 0.0f}  },
			{ math::Vec3(-0.5f, -0.5f,  0.5f),  math::Vec3 {-1.0,  0.0, 0.0}, {1.0f, 0.0f}  },
			{ math::Vec3(-0.5f,  0.5f,  0.5f),  math::Vec3 {-1.0,  0.0, 0.0}, {1.0f, 1.0f}  },
			{ math::Vec3(-0.5f,  0.5f, -0.5f),  math::Vec3 {-1.0,  0.0, 0.0}, {0.0f, 1.0f}  },

			// Right face (+X)
			{ math::Vec3(0.5f, -0.5f,  0.5f),   math::Vec3 {1.0,  0.0, 0.0},  {0.0f, 0.0f}  },
			{ math::Vec3(0.5f, -0.5f, -0.5f),   math::Vec3 {1.0,  0.0, 0.0},  {1.0f, 0.0f}  },
			{ math::Vec3(0.5f,  0.5f, -0.5f),   math::Vec3 {1.0,  0.0, 0.0},  {1.0f, 1.0f}  },
			{ math::Vec3(0.5f,  0.5f,  0.5f),   math::Vec3 {1.0,  0.0, 0.0},  {0.0f, 1.0f}  },

			// Top face (+Y)
			{ math::Vec3(-0.5f, 0.5f,  0.5f),   math::Vec3 {0.0,  1.0, 0.0},  {0.0f, 0.0f}  },
			{ math::Vec3(0.5f,  0.5f,  0.5f),   math::Vec3 {0.0,  1.0, 0.0},  {1.0f, 0.0f}  },
			{ math::Vec3(0.5f,  0.5f, -0.5f),   math::Vec3 {0.0,  1.0, 0.0},  {1.0f, 1.0f}  },
			{ math::Vec3(-0.5f, 0.5f, -0.5f),   math::Vec3 {0.0,  1.0, 0.0},  {0.0f, 1.0f}  },

			// Bottom face (-Y)
			{ math::Vec3(-0.5f, -0.5f, -0.5f),  math::Vec3 {0.0, -1.0, 0.0},  {0.0f, 0.0f}  },
			{ math::Vec3(0.5f,  -0.5f, -0.5f),  math::Vec3 {0.0, -1.0, 0.0},  {1.0f, 0.0f}  },
			{ math::Vec3(0.5f,  -0.5f,  0.5f),  math::Vec3 {0.0, -1.0, 0.0},  {1.0f, 1.0f}  },
			{ math::Vec3(-0.5f, -0.5f,  0.5f),  math::Vec3 {0.0, -1.0, 0.0},  {0.0f, 1.0f}  },
		};

		Vector<u32> indices =
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
