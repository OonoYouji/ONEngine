#include "BaseGameObject.h"

#include <format>

#include "ImGuiManager/ImGuiManager.h"
#include "Scenes/Manager/SceneManager.h"
#include "GameObjectManager/GameObjectManager.h"
#include <CollisionManager/CollisionManager.h>

#include <Math/CreateName.h>


/// ===================================================
/// コンストラクタ
/// ===================================================
BaseGameObject::BaseGameObject() {
	GameObjectManager::GetInstance()->AddGameObject(this);
	CollisionManager::GetInstance()->AddGameObject(this);

	pTransform_ = AddComponent<Transform>();
	UpdateMatrix();
}


/// ===================================================
/// 行列の更新
/// ===================================================
void BaseGameObject::UpdateMatrix() {
	pTransform_->Update();
}


/// ===================================================
/// 自身を削除
/// ===================================================
void BaseGameObject::Destory() {
	GameObjectManager::GetInstance()->Destory(this);
}


/// ===================================================
/// 座標のゲット
/// ===================================================
const Vec3 BaseGameObject::GetPosition() const {
	return  {
		pTransform_->matTransform.m[3][0],
		pTransform_->matTransform.m[3][1],
		pTransform_->matTransform.m[3][2]
	};
}



#pragma region Parent Child
/// ===================================================
/// 親のセット
/// ===================================================
void BaseGameObject::SetParent(Transform* parent) {
	pTransform_->SetParent(parent);
	//pTransform_->AddChild(this); //- 相手の子供に自身を追加
}


/// ===================================================
/// 親のゲット
/// ===================================================
Transform* BaseGameObject::GetParent() const {
	return pTransform_->GetParent();
}


/// ===================================================
/// 子供のゲット
/// ===================================================
std::list<BaseGameObject*> BaseGameObject::GetChilds() const {
	std::list<BaseGameObject*> childs{};
	for(auto& child : pTransform_->GetChilds()) {
		childs.push_back(child->GetOwner());
	}
	return childs;
}
#pragma endregion


#pragma region Accesser Tag Name
/// ===================================================
/// タグのセット
/// ===================================================
void BaseGameObject::SetTag(const std::string& tag) {
	tag_ = tag;
}


/// ===================================================
/// タグのゲット
/// ===================================================
const std::string& BaseGameObject::GetTag() const {
	return tag_;
}


/// ===================================================
/// 名前のセット
/// ===================================================
void BaseGameObject::SetName(const std::string& name) {
	name_ = name;
}


/// ===================================================
/// 名前のゲット
/// ===================================================
const std::string& BaseGameObject::GetName() const {
	return name_;
}
#pragma endregion 



/// ===================================================
/// ImGuiでデバッグ表示
/// ===================================================
void BaseGameObject::ImGuiDebug() {
#ifdef _DEBUG

	for(auto& component : components_) {
		std::string label = "isActive_" + component->GetName();
		ImGui::Checkbox(label.c_str(), &component->isActive);
		component->Debug();
		ImGui::Separator();
	}

	Debug();

#endif // _DEBUG
}

void BaseGameObject::CreateTag(BaseGameObject* object) {
	std::string name = CreateName(object);
	SetTag(name);
	SetName(name);
}

void BaseGameObject::RenameComponents() {
	for(auto& component : components_) {
		std::string name = CreateName(component.get()) + std::format("##{:p}", reinterpret_cast<void*>(component.get()));
		component->SetName(name);
	}
}