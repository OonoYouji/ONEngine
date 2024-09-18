#include "BaseGameObject.h"

#include <format>

#include <ImGuiManager.h>
#include <SceneManager.h>
#include <GameObjectManager.h>
#include <SceneManager.h>


#include <CreateName.h>


/// ===================================================
/// コンストラクタ
/// ===================================================
BaseGameObject::BaseGameObject() {
	GameObjectManager::GetInstance()->AddGameObject(this);

	pTranform_ = AddComponent<Transform>();
	UpdateMatrix();
}


/// ===================================================
/// 行列の更新
/// ===================================================
void BaseGameObject::UpdateMatrix() {
	pTranform_->UpdateMatrix();
	if(parent_) {
		pTranform_->matTransform *= parent_->GetMatTransform();
	}
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
		pTranform_->matTransform.m[3][0],
		pTranform_->matTransform.m[3][1],
		pTranform_->matTransform.m[3][2]
	};
}



#pragma region Parent Child
/// ===================================================
/// 親のセット
/// ===================================================
void BaseGameObject::SetParent(BaseGameObject* parent) {
	parent_ = parent;
	parent->AddChild(this); //- 相手の子供に自身を追加
}


/// ===================================================
/// 親のゲット
/// ===================================================
BaseGameObject* BaseGameObject::GetParent() const {
	return parent_;
}


/// ===================================================
/// 子供の追加
/// ===================================================
void BaseGameObject::AddChild(BaseGameObject* child) {
	childs_.push_back(child);
}


/// ===================================================
/// 子供のゲット
/// ===================================================
const std::list<BaseGameObject*>& BaseGameObject::GetChilds() const {
	return childs_;
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
