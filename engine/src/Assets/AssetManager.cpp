//
// Created by pointerlost on 10/4/25.
//
#include "Assets/AssetManager.h"
#include "Core/Logger.h"
#include "Graphics/Model.h"

namespace Real {

    void AssetManager::RegisterModel(const Ref<Model>& model) {
        if (!m_Models.contains(model->m_UUID)) {
            m_Models.emplace(model->m_UUID, model);
            if (!m_ModelNameToUUID.contains(model->m_Name))
                m_ModelNameToUUID.emplace(model->m_Name, model->m_UUID);
        }
    }

    Ref<Model> AssetManager::GetModel(const String& name) const {
        const auto it = m_ModelNameToUUID.find(name);
        if (it == m_ModelNameToUUID.end()) {
            Warn("Model not found: " + name);
            return nullptr;
        }
        const auto mit = m_Models.find(it->second);
        return (mit != m_Models.end()) ? mit->second : nullptr;
    }

    bool AssetManager::IsModelExist(const String& name) const {
        return m_ModelNameToUUID.contains(name) && m_Models.contains(m_ModelNameToUUID.at(name));
    }

}
