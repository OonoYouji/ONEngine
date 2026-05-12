#pragma once
#include "Engine/Core/Math/Math.h"

namespace Engine::Editor {

class EditorCamera {
public:
    EditorCamera();
    ~EditorCamera() = default;

    void Update(float dt);

    const Math::Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
    const Math::Matrix4x4& GetProjMatrix() const { return projMatrix_; }
    const Math::Matrix4x4& GetViewProjMatrix() const { return viewProjMatrix_; }
    const Math::Vector3& GetPosition() const { return position_; }
    float GetNearZ() const { return nearZ_; }
    float GetFarZ() const { return farZ_; }

private:
    Math::Vector3 position_;
    Math::Vector3 rotation_; // Euler angles (pitch, yaw, roll)

    Math::Matrix4x4 viewMatrix_;
    Math::Matrix4x4 projMatrix_;
    Math::Matrix4x4 viewProjMatrix_;

    float fov_ = 45.0f;
    float aspect_ = 16.0f / 9.0f;
    float nearZ_ = 0.1f;
    float farZ_ = 1000.0f;

    float moveSpeed_ = 50.0f;
    float rotateSpeed_ = 1.0f;
};

} // namespace Engine::Editor
