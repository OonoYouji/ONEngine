#include "IEntity.h"

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Collection/ComponentCollection.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"

IEntity::IEntity() {
	parent_ = nullptr;
}

void IEntity::CommonInitialize() {
	className_ = typeid(*this).name();
	className_.erase(0, 6);
	name_ = className_;
	prefabName_ = "";

	pEntityComponentSystem_->LoadComponent(this);

	transform_ = AddComponent<Transform>();
	variables_ = AddComponent<Variables>();
	variables_->LoadJson("./Assets/Jsons/" + name_ + ".json");
}

IComponent* IEntity::AddComponent(const std::string& _name) {

	size_t hash = GetComponentHash(_name);
	auto it = components_.find(hash);
	if (it != components_.end()) { ///< すでに同じコンポーネントが存在している場合
		it->second->SetOwner(this);
		return it->second;
	}

	/// component の生成, 追加
	IComponent* component = pEntityComponentSystem_->AddComponent(_name);
	if (!component) {
		return nullptr;
	}

	component->SetOwner(this);
	components_[hash] = component;

	return component;
}

IComponent* IEntity::GetComponent(const std::string& _compName) const {

	/// stringをhashに変換
	size_t hash = GetComponentHash(_compName);

	/// hashからコンポーネントを取得
	auto itr = components_.find(hash);
	if (itr != components_.end()) {
		return itr->second;
	}

	/// コンポーネントが見つからない場合はnullptrを返す
	return nullptr;
}

void IEntity::RemoveComponent(const std::string& _compName) {
	size_t hash = GetComponentHash(_compName);
	auto it = components_.find(hash);
	if (it != components_.end()) {
		pEntityComponentSystem_->RemoveComponent(hash, it->second->id); ///< コンポーネントを削除
		components_.erase(it); ///< コンポーネントのマップから削除
	}

	if (_compName == "Transform") {
		transform_ = nullptr; ///< Transformコンポーネントを削除した場合はnullptrに設定
	} else if (_compName == "Variables") {
		variables_ = nullptr; ///< Variablesコンポーネントを削除した場合はnullptrに設定
	}
}

void IEntity::RemoveComponentAll() {
	pEntityComponentSystem_->RemoveComponentAll(this); ///< 全てのコンポーネントを削除
	components_.clear();
}

void IEntity::UpdateTransform() {
	transform_->Update();

	if (parent_) {

		if ((transform_->matrixCalcFlags & Transform::kAll) == Transform::kAll) {
			transform_->matWorld *= parent_->transform_->GetMatWorld();
			return;
		}

		Matrix4x4 matCancel = Matrix4x4::kIdentity;
		if (transform_->matrixCalcFlags & Transform::kScale) {
			matCancel = Matrix4x4::MakeScale(parent_->transform_->scale);
		}

		if (transform_->matrixCalcFlags & Transform::kRotate) {
			matCancel *= Matrix4x4::MakeRotate(parent_->transform_->rotate);
		}

		if (transform_->matrixCalcFlags & Transform::kPosition) {
			matCancel *= Matrix4x4::MakeTranslate(parent_->transform_->position);
		}

		transform_->matWorld *= matCancel;
	}
}

void IEntity::Destroy() {
	pEntityComponentSystem_->RemoveEntity(this);
}

void IEntity::SetPosition(const Vector3& _v) {
	transform_->position = _v;
}

void IEntity::SetPositionX(float _x) {
	transform_->position.x = _x;
}

void IEntity::SetPositionY(float _y) {
	transform_->position.y = _y;
}

void IEntity::SetPositionZ(float _z) {
	transform_->position.z = _z;
}

void IEntity::SetRotate(const Vector3& _v) {
	transform_->rotate = Quaternion::FromEuler(_v);
}

void IEntity::SetRotate(const Quaternion& _q) {
	transform_->rotate = _q;
}

void IEntity::SetScale(const Vector3& _v) {
	transform_->scale = _v;
}

void IEntity::SetScaleX(float _x) {
	transform_->scale.x = _x;
}

void IEntity::SetScaleY(float _y) {
	transform_->scale.y = _y;
}

void IEntity::SetScaleZ(float _z) {
	transform_->scale.z = _z;
}

void IEntity::SetParent(IEntity* _parent) {
	_parent->children_.push_back(this);
	parent_ = _parent;
}

void IEntity::RemoveParent() {
	if (parent_) {
		auto itr = std::remove_if(parent_->children_.begin(), parent_->children_.end(),
			[this](IEntity* child) {
				return child == this;
			}
		);
		parent_->children_.erase(itr, parent_->children_.end());
		parent_ = nullptr;
	}
}

void IEntity::SetName(const std::string& _name) {
	name_ = _name;
}

void IEntity::SetPrefabName(const std::string& _name) {
	prefabName_ = _name;
}

void IEntity::SetActive(bool _active) {
	active_ = _active;
}

const Vector3& IEntity::GetLocalPosition() const {
	return transform_->position;
}

Vector3 IEntity::GetLocalRotate() const {
	return Quaternion::ToEuler(transform_->rotate);
}

const Quaternion& IEntity::GetLocalRotateQuaternion() const {
	return transform_->rotate;
}

const Vector3& IEntity::GetLocalScale() const {
	return transform_->scale;
}

Vector3 IEntity::GetPosition() {
	return Matrix4x4::Transform(Vector3::kZero, transform_->GetMatWorld());
}

Vector3 IEntity::GetRotate() {
	if (!parent_) {
		return Quaternion::ToEuler(transform_->rotate);
	}

	// 自身のローカル回転を加算  
	return Quaternion::ToEuler(parent_->GetRotateQuaternion() * transform_->rotate);
}

Quaternion IEntity::GetRotateQuaternion() {
	if (!parent_) {
		return transform_->rotate;
	}

	return parent_->GetRotateQuaternion() * transform_->rotate;
}

Vector3 IEntity::GetScale() {
	return transform_->scale;
}

Transform* IEntity::GetTransform() const {
	return transform_;
}

const IEntity* IEntity::GetParent() const {
	return parent_;
}

IEntity* IEntity::GetParent() {
	return parent_;
}

const std::vector<IEntity*>& IEntity::GetChildren() const {
	return children_;
}

IEntity* IEntity::GetChild(size_t _index) {
	return children_[_index];
}

const std::unordered_map<size_t, IComponent*>& IEntity::GetComponents() const {
	return components_;
}

std::unordered_map<size_t, IComponent*>& IEntity::GetComponents() {
	return components_;
}

const std::string& IEntity::GetName() const {
	return name_;
}

const std::string& IEntity::GetEntityClassName() const {
	return className_;
}

const std::string& IEntity::GetPrefabName() const {
	return prefabName_;
}

bool IEntity::ContainsPrefab() const {
	/// 空文字列でないかチェック
	return prefabName_ != "";
}

bool IEntity::GetActive() const {
	return active_;
}

int32_t IEntity::GetId() const {
	return id_;
}


