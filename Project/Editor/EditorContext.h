#pragma once
#include "Engine/ECS/Entity.h"

namespace Engine::Editor {

namespace ECS = Engine::ECS;

class EditorContext {
public:
    static EditorContext& GetInstance() {
        static EditorContext instance;
        return instance;
    }

    void SetSelectedEntity(ECS::Entity entity) { selectedEntity_ = entity; }
    ECS::Entity GetSelectedEntity() const { return selectedEntity_; }

private:
    EditorContext() : selectedEntity_(0) {}
    ECS::Entity selectedEntity_;
};

} // namespace Engine::Editor
