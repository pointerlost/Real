//
// Created by pointerlost on 2/20/26.
//
#include "scenes/SandboxScene.h"

#include "Assets/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Real::Editor {

    void SandboxScene::Load(Scene &scene) {
        const auto am = Services::GetAssetManager();

        auto& cube = scene.CreateEntity("RightWall");
        cube.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(26.0, 1.5, 0.0));
        cube.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(45.0, 20.0, 1.0));
        (void)cube.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        auto& cube2 = scene.CreateEntity("LeftWall");
        cube2.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(-26.0, 1.5, 0.0));
        cube2.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(45.0, 20.0, 1.0));
        (void)cube2.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        auto& cube3 = scene.CreateEntity("Floor");
        cube3.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(97.0, 1.5, 98.0));
        (void)cube3.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );
        cube3.AddComponent<ColliderComponent>().shape = physics::ColliderShape::Box;
        cube3.AddComponent<RigidbodyComponent>().type = physics::BodyType::Static;

        auto& cube4 = scene.CreateEntity("Roof");
        cube4.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 13.5, 0.0));
        cube4.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(97.0, 4.0, 1.0));
        (void)cube4.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        auto& cube5 = scene.CreateEntity("Container");
        cube5.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 0.0, 12.0));
        cube5.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(8.0, 8.0, 8.0));
        (void)cube5.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );
        cube5.AddComponent<ColliderComponent>().shape = physics::ColliderShape::Box;
        cube5.AddComponent<RigidbodyComponent>().type = physics::BodyType::Static;

        auto& fordCar = scene.CreateEntity("FordCar");
        fordCar.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        fordCar.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)fordCar.AddComponent<ModelComponent>(am->GetModel("Ford_raptor"));

        auto& island_tree = scene.CreateEntity("Island Tree");
        island_tree.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        island_tree.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)island_tree.AddComponent<ModelComponent>(am->GetModel("island_tree"));

        auto& mountain_road = scene.CreateEntity("Mountain road");
        mountain_road.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        mountain_road.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)mountain_road.AddComponent<ModelComponent>(am->GetModel("mountain_road"));

        auto& porsche_car = scene.CreateEntity("Porsche");
        porsche_car.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        porsche_car.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)porsche_car.AddComponent<ModelComponent>(am->GetModel("porsche_turbo"));

        auto& city_road = scene.CreateEntity("City Road");
        city_road.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        city_road.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)city_road.AddComponent<ModelComponent>(am->GetModel("city_road"));

        auto& light = scene.CreateEntity("Light");
        light.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(-10.0, 10.0, -10.0));
        (void)light.AddComponent<LightComponent>(Light::Mode::DIRECTIONAL);
        (void)light.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        Info("Game resources loaded successfully!");
    }
}
