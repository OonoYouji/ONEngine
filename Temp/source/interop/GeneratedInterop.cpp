#include "EcsInterop.h"
#include "EcsWorld.h"
#include "GeneratedComponents.h"

// 自動生成された型登録コード
#define REGISTER_TYPE(Type) \
    extern "C" EXPORT uint32_t Ecs_GetTypeId_##Type() { return TypeID::get<Type>(); } \
    static inline uint32_t _reg_##Type = []() { \
        uint32_t id = TypeID::get<Type>(); \
        TypeRegistry::Register(id, []() { return std::make_unique<SparseSet<Type>>(); }); \
        return id; \
    }();

REGISTER_TYPE(LocalTransform)
REGISTER_TYPE(WorldMatrix)
REGISTER_TYPE(Parent)
REGISTER_TYPE(LocalRect)
REGISTER_TYPE(CanvasWorldMatrix)
REGISTER_TYPE(Velocity)
REGISTER_TYPE(PhysicsProperties)
REGISTER_TYPE(BoxGeometry)
REGISTER_TYPE(SphereGeometry)
REGISTER_TYPE(CapsuleGeometry)
REGISTER_TYPE(ColliderFilter)
REGISTER_TYPE(CameraData)
REGISTER_TYPE(LightData)
REGISTER_TYPE(RenderMesh)
REGISTER_TYPE(SpriteRenderData)
REGISTER_TYPE(AudioSourceData)
REGISTER_TYPE(AudioPlayState)
REGISTER_TYPE(AudioListenerTag)
REGISTER_TYPE(CanvasData)
REGISTER_TYPE(UIImageData)
REGISTER_TYPE(UITextData)
REGISTER_TYPE(UIButtonState)
REGISTER_TYPE(AnimatorState)
REGISTER_TYPE(NavAgentParams)
REGISTER_TYPE(NavAgentState)
REGISTER_TYPE(ScriptComponent)
REGISTER_TYPE(SceneMask)