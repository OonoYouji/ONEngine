#include "Pivot.h"

/// externals
#include <imgui.h>
#include <ImGuizmo.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
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
		if (!ecsGroup) {
			return EDITOR_STATE_FAILED;
		}

		ONEngine::GameEntity* entity = ecsGroup->GetEntityFromGuid(entityGuid_);
		if (!entity) {
			return EDITOR_STATE_FAILED;
		}

		ONEngine::Transform* transform = entity->GetComponent<ONEngine::Transform>();
		if (!transform) {
			return EDITOR_STATE_FAILED;
		}

		transform->position = after_.position;
		transform->SetRotate(after_.rotation);
		transform->scale = after_.scale;

		return EDITOR_STATE_FINISH;
	}

	EDITOR_STATE Undo() override {
		ONEngine::ECSGroup* ecsGroup = pEcs_->GetECSGroup(sceneName_);
		if (!ecsGroup) {
			return EDITOR_STATE_FAILED;
		}
		ONEngine::GameEntity* entity = ecsGroup->GetEntityFromGuid(entityGuid_);
		if (!entity) {
			return EDITOR_STATE_FAILED;
		}
		ONEngine::Transform* transform = entity->GetComponent<ONEngine::Transform>();
		if (!transform) {
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

};


/// @brief 
PivotInstance gPivotInstance;

} /// unnamed namespace


void Editor::UpdatePivot(ONEngine::EntityComponentSystem* _ecs) {

	ONEngine::GameEntity* entity = nullptr;
	if (gPivotInstance.selectedEntityGuid_ != ONEngine::Guid::kInvalid) {
		ONEngine::ECSGroup* currentGroup = _ecs->GetCurrentGroup();
		entity = currentGroup->GetEntityFromGuid(gPivotInstance.selectedEntityGuid_);
	}

	if (!entity) {
		return;
	}

	ONEngine::CameraComponent* cameraComp = _ecs->GetECSGroup("Debug")->GetMainCamera();
	if (!cameraComp) {
		return;
	}




	ImGuizmo::SetOrthographic(false);
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
		if (ONEngine::Input::TriggerKey(DIK_W)) {
			manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 移動
		} else if (ONEngine::Input::TriggerKey(DIK_E)) {
			manipulateOperation_ = ImGuizmo::OPERATION::ROTATE; // 回転
		} else if (ONEngine::Input::TriggerKey(DIK_R)) {
			manipulateOperation_ = ImGuizmo::OPERATION::SCALE; // 拡縮
		} else if (ONEngine::Input::TriggerKey(DIK_Q)) {
			manipulateOperation_ = 0; // 操作なし
		}

		/// モードの選択
		if (ONEngine::Input::TriggerKey(DIK_1)) {
			manipulateMode_ = ImGuizmo::MODE::WORLD; // ワールド座標
		} else if (ONEngine::Input::TriggerKey(DIK_2)) {
			manipulateMode_ = ImGuizmo::MODE::LOCAL; // ローカル座標
		}
	}



	ONEngine::Transform* transform = entity->GetComponent<ONEngine::Transform>();
	gPivotInstance.pivotMatrix_ = transform->matWorld;

	ImGuizmo::Manipulate(
		&cameraComp->GetViewMatrix().m[0][0],
		&cameraComp->GetProjectionMatrix().m[0][0],
		(ImGuizmo::OPERATION)manipulateOperation_, // TRANSLATE, ROTATE, SCALE
		(ImGuizmo::MODE)manipulateMode_, // WORLD or LOCAL
		&gPivotInstance.pivotMatrix_.m[0][0]
	);

	/// --------------------------------------------------------------------------
	/// Undo/Redo 対応のための操作検知ロジック
	/// --------------------------------------------------------------------------

	static CommandTransform before;
	static bool wasUsing = false; // 前フレームの操作状態
	bool isUsing = ImGuizmo::IsUsing(); // 現在のフレームの操作状態

	// [CASE 1] 操作開始 (マウスをクリックして操作を始めた瞬間)
	if (isUsing && !wasUsing) {
		// TODO: ここでコマンド作成のために「変更前のTransform情報」を保存してください
		// 例: UndoStack::PushStart(transform);
		before.position = transform->position;
		before.rotation = transform->rotate;
		before.scale = transform->scale;
	}

	// [CASE 2] 操作中 (ドラッグ中)
	// IsUsing()がtrueであれば、エンティティの見た目を更新し続ける
	if (isUsing) {
		/// 行列をSRTに分解、エンティティに適応
		float translation[3], rotation[3], scale[3];
		ImGuizmo::DecomposeMatrixToComponents(&gPivotInstance.pivotMatrix_.m[0][0], translation, rotation, scale);

		ONEngine::Vector3 translationV = ONEngine::Vector3(translation[0], translation[1], translation[2]);
		if (ONEngine::GameEntity* owner = transform->GetOwner()) {
			if (ONEngine::GameEntity* parent = owner->GetParent()) {
				translationV = ONEngine::Matrix4x4::Transform(translationV, parent->GetTransform()->GetMatWorld().Inverse());
			}
		}
		transform->SetPosition(translationV);

		ONEngine::Vector3 eulerRotation = ONEngine::Vector3(rotation[0] * ONEngine::Math::Deg2Rad, rotation[1] * ONEngine::Math::Deg2Rad, rotation[2] * ONEngine::Math::Deg2Rad);
		transform->SetRotate(eulerRotation);
		transform->SetScale(ONEngine::Vector3(scale[0], scale[1], scale[2]));

		transform->Update();
	}

	// [CASE 3] 操作終了 (マウスを離して操作を確定した瞬間)
	if (!isUsing && wasUsing) {
		// TODO: ここで「変更後のTransform情報」と「開始時の情報」を使ってコマンドを生成・登録してください
		// 例: UndoStack::PushEnd(transform); または CommandSystem::AddCommand(new TransformCommand(...));
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

	// 次のフレームのために状態を更新
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

