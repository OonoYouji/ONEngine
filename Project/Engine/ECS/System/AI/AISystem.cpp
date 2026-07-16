#include "AISystem.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Agent/AgentIntentComponent.h"
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"

namespace ONEngine {

/**
 * @brief コンストラクタ
 */
AISystem::AISystem() {}

/**
 * @brief AIの意思決定および思考処理の更新処理を毎フレーム実行します。
 */
void AISystem::RuntimeUpdate(ECSGroup* _ecs) {
    if (!_ecs) {
        return;
    }

    auto* compArray = _ecs->GetComponentArray<AgentIntentComponent>();
    if (!compArray) {
        return;
    }

    auto& usedComponents = compArray->GetUsedComponents();
    if (usedComponents.empty()) {
        return;
    }

    // 1. Create a temporary vector for batch data. C# will write to this.
    // We only need to fill the IDs so C# knows which component is which.
    // Ensure all fields are zero-initialized to prevent garbage movement directions.
    std::vector<AgentIntentComponent::BatchData> batchData(usedComponents.size(), { 0 });
    for (size_t i = 0; i < usedComponents.size(); ++i) {
        batchData[i].compId = usedComponents[i]->id;
    }

    // 2. Call C# to let it fill the batchData buffer
    MonoScriptEngine::GetInstance().UpdateAiIntents(batchData.data(), static_cast<int>(batchData.size()), Time::DeltaTime(), _ecs->GetGroupName());

    // 3. Apply the results from the buffer back to the main component array
    for (size_t i = 0; i < usedComponents.size(); ++i) {
        // Assuming the order is preserved and C# doesn't reorder the data.
        // We add an assertion to be safe.
        if (usedComponents[i]->id == batchData[i].compId) {
            usedComponents[i]->desiredMoveDirection = batchData[i].desiredMoveDirection;
            usedComponents[i]->desiredRotation = batchData[i].desiredRotation;
            usedComponents[i]->rotationSpeed = batchData[i].rotationSpeed;
            usedComponents[i]->useDesiredRotation = (batchData[i].useDesiredRotation != 0);
            usedComponents[i]->isAttacking = (batchData[i].isAttacking != 0);
            usedComponents[i]->targetEntityId = batchData[i].targetEntityId;
        }
        else {
            // The order was not preserved. This indicates a problem.
            Console::LogError("AISystem: Component order mismatch after C# call. Aborting apply.", LogCategory::ScriptEngine);
            break; 
        }
    }
}

}
