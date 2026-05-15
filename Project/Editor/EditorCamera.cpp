#include "EditorCamera.h"
#include <Windows.h>
#include "imgui.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "EditorContext.h"
#include <algorithm>

namespace Engine::Editor {

EditorCamera::EditorCamera() {
    position_ = { 0, 50, -150 };
    rotation_ = { 0.3f, 0, 0 };
    
    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    aspect_ = (float)graphics.GetWindowSize().x / (float)graphics.GetWindowSize().y;
}

void EditorCamera::Update(float dt) {
    auto& context = EditorContext::GetInstance();
    auto& actions = context.GetActionMap();
    
    if (context.IsSceneFocused()) {
        // 回転
        if (actions.IsPressed("Rotate")) {
            // 回転だけはマウスのデルタ値を直接使いたい場合が多いので Raw から取る
            // (ActionMap に MouseDelta アクションを定義しても良い)
            ImGuiIO& io = ImGui::GetIO();
            rotation_.x += io.MouseDelta.y * rotateSpeed_ * dt;
            rotation_.y += io.MouseDelta.x * rotateSpeed_ * dt;
            
            // ジンバルロック防止
            rotation_.x = std::clamp(rotation_.x, -1.5f, 1.5f);
        }

        // 移動方向の計算
        Math::Vector3 forward = Math::Vector3{
            sinf(rotation_.y) * cosf(rotation_.x),
            -sinf(rotation_.x),
            cosf(rotation_.y) * cosf(rotation_.x)
        };
        forward = Math::Vector3::Normalize(forward);

        Math::Vector3 right = Math::Vector3::Cross(Math::Vector3{0, 1, 0}, forward);
        right = Math::Vector3::Normalize(right);

        Math::Vector3 up = Math::Vector3{ 0, 1, 0 };

        // キー入力による移動 (論理アクション名を使用)
        float speed = moveSpeed_;
        if (actions.IsPressed("SpeedUp")) speed *= 3.0f;

        if (actions.IsPressed("Rotate")) {
            if (actions.IsPressed("MoveForward"))  position_ += forward * speed * dt;
            if (actions.IsPressed("MoveBackward")) position_ -= forward * speed * dt;
            if (actions.IsPressed("MoveLeft"))     position_ -= right * speed * dt;
            if (actions.IsPressed("MoveRight"))    position_ += right * speed * dt;
            if (actions.IsPressed("MoveUp"))       position_ += up * speed * dt;
            if (actions.IsPressed("MoveDown"))     position_ -= up * speed * dt;
        }

        // マウスホイールによる前後移動
        float zoom = actions.GetValue("Zoom");
        if (zoom != 0.0f) {
            position_ += forward * zoom * speed * 0.5f;
        }
    }

    // 行列の更新 (入力に関わらず現在の position_/rotation_ から計算)
    Math::Vector3 forward = Math::Vector3{
        sinf(rotation_.y) * cosf(rotation_.x),
        -sinf(rotation_.x),
        cosf(rotation_.y) * cosf(rotation_.x)
    };
    viewMatrix_ = Math::Matrix4x4::MakeLookAtLH(position_, position_ + forward, Math::Vector3{ 0, 1, 0 });
    projMatrix_ = Math::Matrix4x4::MakePerspectiveFovLH(fov_ * (3.141592f / 180.0f), aspect_, nearZ_, farZ_);
    viewProjMatrix_ = viewMatrix_ * projMatrix_;
}

} // namespace Engine::Editor
