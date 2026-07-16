#include "Pivot.h"

/// externals
#include <imgui.h>
#include <ImGuizmo.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

/// editor
#include "Engine/Editor/Commands/IEditCommand.h"
#include "Engine/Editor/Manager/EditCommand.h"


using namespace Editor;

namespace {

struct CommandTransform {
	ONEngine::Vector3 position;
	ONEngine::Quaternion rotation;
	ONEngine::Vector3 scale;
};

class ModifyPivotCommand : public IEditCommand {
public:

	ModifyPivotCommand(
		const ONEngine::Guid& _entityGuid,
		const CommandTransform& _before,
		const CommandTransform& _after,
		ONEngine::EntityComponentSystem* _ecs,
		const std::string& _sceneName)
		: entityGuid_(_entityGuid),
		before_(_before),
		after_(_after),
		pEcs_(_ecs),
		sceneName_(_sceneName) {

	}

	~ModifyPivotCommand() override = default;

	EDITOR_STATE Execute() override {
		ONEngine::ECSGroup* ecsGroup = pEcs_->GetECSGroup(sceneName_);
		if(!ecsGroup) {
			return EDITOR_STATE_FAILED;
		}

		ONEngine::GameEntity* entity = ecsGroup->GetEntityFromGuid(entityGuid_);
		if(!entity) {
			return EDITOR_STATE_FAILED;
		}

		ONEngine::Transform* transform = entity->GetComponent<ONEngine::Transform>();
		if(!transform) {
			return EDITOR_STATE_FAILED;
		}

		transform->position = after_.position;
		transform->SetRotate(after_.rotation);
		transform->scale = after_.scale;

		return EDITOR_STATE_FINISH;
	}

	EDITOR_STATE Undo() override {
		ONEngine::ECSGroup* ecsGroup = pEcs_->GetECSGroup(sceneName_);
		if(!ecsGroup) {
			return EDITOR_STATE_FAILED;
		}
		ONEngine::GameEntity* entity = ecsGroup->GetEntityFromGuid(entityGuid_);
		if(!entity) {
			return EDITOR_STATE_FAILED;
		}
		ONEngine::Transform* transform = entity->GetComponent<ONEngine::Transform>();
		if(!transform) {
			return EDITOR_STATE_FAILED;
		}

		transform->position = before_.position;
		transform->SetRotate(before_.rotation);
		transform->scale = before_.scale;

		return EDITOR_STATE_FINISH;
	}

private:
	const ONEngine::Guid entityGuid_;
	const CommandTransform before_;
	const CommandTransform after_;
	ONEngine::EntityComponentSystem* pEcs_ = nullptr;
	const std::string sceneName_;
};


/// ///////////////////////////////////////////////////
/// ピボット操作の処理を行うインスタンス
/// ヘッダー側のPivotから操作される
/// ///////////////////////////////////////////////////
struct PivotInstance {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	PivotInstance() = default;
	~PivotInstance() = default;


	/// ===================================================
	/// public : objects
	/// ===================================================

	ONEngine::Vector2 drawRectPos_ = ONEngine::Vector2::Zero;
	ONEngine::Vector2 drawRectSize_ = ONEngine::Vector2::Zero;

	ONEngine::Guid selectedEntityGuid_ = ONEngine::Guid::kInvalid;
	ONEngine::Matrix4x4 pivotMatrix_ = ONEngine::Matrix4x4::kIdentity;

	bool is2DMode_ = false;

};


/// @brief 
PivotInstance gPivotInstance;

} /// unnamed namespace


void Editor::UpdatePivot(ONEngine::EntityComponentSystem* _ecs) {

	ONEngine::GameEntity* entity = nullptr;
	if(gPivotInstance.selectedEntityGuid_ != ONEngine::Guid::kInvalid) {
		ONEngine::ECSGroup* currentGroup = _ecs->GetCurrentGroup();
		entity = currentGroup->GetEntityFromGuid(gPivotInstance.selectedEntityGuid_);
	}

	if(!entity) {
		return;
	}

	ONEngine::CameraComponent* cameraComp = _ecs->GetECSGroup("Debug")->GetMainCamera();
	if(!cameraComp) {
		return;
	}


	ImGuizmo::SetOrthographic(gPivotInstance.is2DMode_);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(
		gPivotInstance.drawRectPos_.x,
		gPivotInstance.drawRectPos_.y,
		gPivotInstance.drawRectSize_.x,
		gPivotInstance.drawRectSize_.y
	);

	static int manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE;
	static int manipulateMode_ = ImGuizmo::MODE::WORLD;

	{
		/// 操作モードの選択
		if(ONEngine::Input::TriggerKey(DIK_W)) {
			manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 移動
		} else if(ONEngine::Input::TriggerKey(DIK_E)) {
			manipulateOperation_ = ImGuizmo::OPERATION::ROTATE; // 回転
		} else if(ONEngine::Input::TriggerKey(DIK_R)) {
			manipulateOperation_ = ImGuizmo::OPERATION::SCALE; // 拡縮
		} else if(ONEngine::Input::TriggerKey(DIK_Q)) {
			manipulateOperation_ = 0; // 操作なし
		}

		/// モードの選択
		if(ONEngine::Input::TriggerKey(DIK_1)) {
			manipulateMode_ = ImGuizmo::MODE::WORLD; // ワールド座標
		} else if(ONEngine::Input::TriggerKey(DIK_2)) {
			manipulateMode_ = ImGuizmo::MODE::LOCAL; // ローカル座標
		}
	}


	ONEngine::Transform* transform = entity->GetComponent<ONEngine::Transform>();
	gPivotInstance.pivotMatrix_ = transform->matWorld;

	ONEngine::Matrix4x4 viewMatrix = cameraComp->GetViewMatrix();
	ONEngine::Matrix4x4 projectionMatrix = cameraComp->GetProjectionMatrix();

	if (gPivotInstance.is2DMode_) {
		/// 2Dモード用の行列を作成
		/// ビュー行列：カメラの平行移動(X, Y)のみを反映。Zは固定して手前に配置し、正面(Z+)を向く。
		ONEngine::Vector3 camPos = cameraComp->GetOwner()->GetPosition();
		viewMatrix = ONEngine::Matrix4x4::MakeTranslate({ -camPos.x, -camPos.y, 1000.0f });

		/// 投影行列：平行投影。Z範囲を広く取り、クリッピングを防ぐ。
		ONEngine::Vector2 orthoSize = ONEngine::EngineConfig::kWindowSize;

		projectionMatrix = ONEngine::CameraMath::MakeOrthographicMatrix(
			-orthoSize.x / 2.0f, orthoSize.x / 2.0f,
			-orthoSize.y / 2.0f, orthoSize.y / 2.0f,
			0.0f, 2000.0f
		);
	}

	int currentOperation = manipulateOperation_;
	if (gPivotInstance.is2DMode_) {
		if (currentOperation == ImGuizmo::OPERATION::TRANSLATE) {
			currentOperation = ImGuizmo::OPERATION::TRANSLATE_X | ImGuizmo::OPERATION::TRANSLATE_Y;
		} else if (currentOperation == ImGuizmo::OPERATION::ROTATE) {
			currentOperation = ImGuizmo::OPERATION::ROTATE_Z;
		} else if (currentOperation == ImGuizmo::OPERATION::SCALE) {
			currentOperation = ImGuizmo::OPERATION::SCALE_X | ImGuizmo::OPERATION::SCALE_Y;
		}
	}

	ImGuizmo::Manipulate(
		&viewMatrix.m[0][0],
		&projectionMatrix.m[0][0],
		(ImGuizmo::OPERATION)currentOperation, // TRANSLATE, ROTATE, SCALE
		(ImGuizmo::MODE)manipulateMode_, // WORLD or LOCAL
		&gPivotInstance.pivotMatrix_.m[0][0]
	);

	/// --------------------------------------------------------------------------
	/// Undo/Redo 対応のための操作検知ロジック
	/// --------------------------------------------------------------------------

	static CommandTransform before;
	static bool wasUsing = false; // 前フレームの操作状態
	bool isUsing = ImGuizmo::IsUsing(); // 現在のフレームの操作状態

	if(isUsing && !wasUsing) {
		before.position = transform->position;
		before.rotation = transform->rotate;
		before.scale = transform->scale;
	}

	if(isUsing && ImGuizmo::IsOver()) {
		/// 行列をSRTに分解、エンティティに適応
		float translation[3], rotation[3], scale[3];
		ImGuizmo::DecomposeMatrixToComponents(&gPivotInstance.pivotMatrix_.m[0][0], translation, rotation, scale);

		ONEngine::Vector3 translationV = ONEngine::Vector3(translation[0], translation[1], translation[2]);
		ONEngine::Vector3 eulerRotation = ONEngine::Vector3(rotation[0] * ONEngine::Math::Deg2Rad, rotation[1] * ONEngine::Math::Deg2Rad, rotation[2] * ONEngine::Math::Deg2Rad);
		ONEngine::Vector3 scaleV = ONEngine::Vector3(scale[0], scale[1], scale[2]);

		if(ONEngine::GameEntity* owner = transform->GetOwner()) {
			if(ONEngine::GameEntity* parent = owner->GetParent()) {
				translationV = ONEngine::Matrix4x4::Transform(translationV, parent->GetTransform()->GetMatWorld().Inverse());

				{	/// 拡縮度を親の影響を除去
					ONEngine::Matrix4x4 matParent = parent->GetTransform()->GetMatWorld();
					ONEngine::Vector3 parentScale = {
						ONEngine::Vector3::Length({ matParent.m[0][0], matParent.m[0][1], matParent.m[0][2] }),
						ONEngine::Vector3::Length({ matParent.m[1][0], matParent.m[1][1], matParent.m[1][2] }),
						ONEngine::Vector3::Length({ matParent.m[2][0], matParent.m[2][1], matParent.m[2][2] })
					};

					if (parentScale.x != 0.0f) scaleV.x /= parentScale.x;
					if (parentScale.y != 0.0f) scaleV.y /= parentScale.y;
					if (parentScale.z != 0.0f) scaleV.z /= parentScale.z;
				}
			}
		}

		transform->SetPosition(translationV);
		transform->SetRotate(eulerRotation);
		transform->SetScale(scaleV);

		transform->Update();
	}

	if(!isUsing && wasUsing) {
		CommandTransform after;
		after.position = transform->position;
		after.rotation = transform->rotate;
		after.scale = transform->scale;

		EditCommand::Execute<ModifyPivotCommand>(
			gPivotInstance.selectedEntityGuid_,
			before, after,
			_ecs, _ecs->GetCurrentGroupName()
		);
	}

	wasUsing = isUsing;
}

void Editor::SetEntity(const ONEngine::Guid& _guid) {
	gPivotInstance.selectedEntityGuid_ = _guid;
}
void Editor::ClearEntity() {
	gPivotInstance.selectedEntityGuid_ = ONEngine::Guid::kInvalid;
}

void Editor::SetDrawRect(const ONEngine::Vector2& _pos, const ONEngine::Vector2& _size) {
	gPivotInstance.drawRectPos_ = _pos;
	gPivotInstance.drawRectSize_ = _size;
}

void Editor::Set2DMode(bool _is2DMode) {
	gPivotInstance.is2DMode_ = _is2DMode;
}

bool Editor::Is2DMode() {
	return gPivotInstance.is2DMode_;
}

