//
// Created by pointerlost on 2/20/26.
//
#include "scenes/SandboxScene.h"
#include "Assets/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Assets/MeshManager.h"
#include "Assets/MaterialManager.h"
#include "Assets/ModelManager.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Real::Editor {

    void SandboxScene::Load(Scene &scene) {
        auto& modelManager    = Services::GetModelManager();
        auto& meshManager     = Services::GetMeshManager();
        auto& materialManager = Services::GetMaterialManager();

        auto& cube = scene.CreateEntity("RightWall");
        cube.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(26.0, 1.5, 0.0));
        cube.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(45.0, 20.0, 1.0));
        cube.AddComponent<MeshRendererComponent>(
            meshManager.GetPrimitiveUUID(graphics::PrimitiveType::Cube),
            materialManager.CreateInstance("Marble009")
        );

        auto& cube2 = scene.CreateEntity("LeftWall");
        cube2.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(-26.0, 1.5, 0.0));
        cube2.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(45.0, 20.0, 1.0));
        cube2.AddComponent<MeshRendererComponent>(
            meshManager.GetPrimitiveUUID(graphics::PrimitiveType::Cube),
            materialManager.CreateInstance("Marble009")
        );

        auto& cube3 = scene.CreateEntity("Floor");
        cube3.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(97.0, 1.5, 98.0));
        cube3.AddComponent<MeshRendererComponent>(
            meshManager.GetPrimitiveUUID(graphics::PrimitiveType::Cube),
            materialManager.CreateInstance("Marble009")
        );
        cube3.AddComponent<ColliderComponent>().desc.shape = core::ShapeDesc::Shape::Box;
        cube3.AddComponent<RigidbodyComponent>().desc.type = core::BodyDesc::Type::Static;

        auto& cube4 = scene.CreateEntity("Roof");
        cube4.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(0.0, 13.5, 0.0));
        cube4.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(97.0, 4.0, 1.0));
        cube4.AddComponent<MeshRendererComponent>(
            meshManager.GetPrimitiveUUID(graphics::PrimitiveType::Cube),
            materialManager.CreateInstance("Marble009")
        );

        auto& cube5 = scene.CreateEntity("Container");
        cube5.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(0.0, 0.0, 12.0));
        cube5.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(8.0, 8.0, 8.0));
        cube5.AddComponent<MeshRendererComponent>(
            meshManager.GetPrimitiveUUID(graphics::PrimitiveType::Cube),
            materialManager.CreateInstance("Marble009")
        );
        cube5.AddComponent<ColliderComponent>().desc.shape = core::ShapeDesc::Shape::Box;
        cube5.AddComponent<RigidbodyComponent>().desc.type = core::BodyDesc::Type::Static;

        auto& fordCar = scene.CreateEntity("FordCar");
        fordCar.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(0.0, 10.0, 0.0));
        fordCar.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(1.0, 1.0, 1.0));
        fordCar.AddComponent<ModelComponent>(modelManager.GetModel("Ford_raptor"));

        auto& island_tree = scene.CreateEntity("Island Tree");
        island_tree.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(0.0, 10.0, 0.0));
        island_tree.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(1.0, 1.0, 1.0));
        island_tree.AddComponent<ModelComponent>(modelManager.GetModel("island_tree"));

        auto& mountain_road = scene.CreateEntity("Mountain road");
        mountain_road.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(0.0, 10.0, 0.0));
        mountain_road.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(1.0, 1.0, 1.0));
        mountain_road.AddComponent<ModelComponent>(modelManager.GetModel("mountain_road"));

        auto& porsche_car = scene.CreateEntity("Porsche");
        porsche_car.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(0.0, 10.0, 0.0));
        porsche_car.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(1.0, 1.0, 1.0));
        porsche_car.AddComponent<ModelComponent>(modelManager.GetModel("porsche_turbo"));

        auto& city_road = scene.CreateEntity("City Road");
        city_road.GetComponentForModification<TransformComponent>()->transform.SetWorldPosition(math::Vec3(0.0, 10.0, 0.0));
        city_road.GetComponentForModification<TransformComponent>()->transform.SetLocalScale(math::Vec3(1.0, 1.0, 1.0));
        city_road.AddComponent<ModelComponent>(modelManager.GetModel("city_road"));

        auto& light = scene.CreateEntity("Light");
        light.GetComponentForModification<TransformComponent>()->transform.SetLocalPosition(math::Vec3(-10.0, 10.0, -10.0));
        light.AddComponent<LightComponent>(Light::Mode::DIRECTIONAL);
        light.AddComponent<MeshRendererComponent>(
            meshManager.GetPrimitiveUUID(graphics::PrimitiveType::Cube),
            materialManager.CreateInstance("Marble009")
        );

        Info("Game resources loaded successfully!");
    }
}
