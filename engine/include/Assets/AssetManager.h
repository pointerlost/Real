//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include "Core/Utils.h"
#include "Core/UUID.h"

namespace Real { struct Model; }

namespace Real {

    class AssetManager {
    public:
        // Models
        void                     RegisterModel(const Ref<Model>& model);
        [[nodiscard]] Ref<Model> GetModel(const String& name) const;
        [[nodiscard]] bool       IsModelExist(const String& name) const;

    private:
        std::unordered_map<UUID, Ref<Model>>  m_Models;
        std::unordered_map<String, UUID>      m_ModelNameToUUID;
    };
}
