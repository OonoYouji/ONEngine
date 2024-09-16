#pragma once

#include <list>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>

#include <Component/Transform/Transform.h>
#include "Component/Base/BaseComponent.h"

#include "Collision/BaseCollider.h"
#include <Model.h>


/// <summary>
/// すべてのゲームオブジェクトの基本となるクラス
/// </summary>
class BaseGameObject {
public:

	BaseGameObject();
	virtual ~BaseGameObject() = default;


	virtual void Initialize() = 0;

	virtual void Update() = 0;
	virtual void LastUpdate() {}

	virtual void Draw() {};
	virtual void FrontSpriteDraw() {};
	virtual void BackSpriteDraw() {};

	virtual void Debug() {};

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// 自身のインスタンスを削除する
	/// </summary>
	void Destory();

	/// <summary>
	/// Group内の変数をImGuiでデバッグする
	/// </summary>
	void ImGuiDebug();

	void CreateTag(BaseGameObject* object);

#pragma region Transform
	void SetPositionX(float x) { pTranform_->position.x = x; }
	void SetPositionY(float y) { pTranform_->position.y = y; }
	void SetPositionZ(float z) { pTranform_->position.z = z; }
	void SetPosition(const Vec3& v) { pTranform_->position = v; }

	void SetRotateX(float x) { pTranform_->rotate.x = x; }
	void SetRotateY(float y) { pTranform_->rotate.y = y; }
	void SetRotateZ(float z) { pTranform_->rotate.z = z; }
	void SetRotate(const Vec3& v) { pTranform_->rotate = v; }

	void SetScaleX(float x) { pTranform_->scale.x = x; }
	void SetScaleY(float y) { pTranform_->scale.y = y; }
	void SetScaleZ(float z) { pTranform_->scale.z = z; }
	void SetScale(const Vec3& v) { pTranform_->scale = v; }

	const Vec3 GetPosition() const;
	const Vec3 GetRotate() const { return pTranform_->rotate; }
	const Vec3 GetScale() const { return pTranform_->scale; }

	const Mat4& GetMatTransform() const { return pTranform_->matTransform; };
	const Transform& GetTransform() const { return *pTranform_; }
#pragma endregion

#pragma region Parent Child
	void SetParent(BaseGameObject* parent);
	BaseGameObject* GetParent() const;
	void AddChild(BaseGameObject* child);
	const std::list<BaseGameObject*>& GetChilds() const;
#pragma endregion Parent Child

#pragma region Accesser Tag Name
	void SetTag(const std::string& tag);
	const std::string& GetTag() const;
	void SetName(const std::string& name);
	const std::string& GetName() const;
#pragma endregion Accesser Tag Name

#pragma region Collider 
	virtual void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {}
	virtual void OnCollisionStay([[maybe_unused]] BaseGameObject* const collision) {}
	virtual void OnCollisionExit([[maybe_unused]] BaseGameObject* const collision) {}

	void CreateBoxCollider(Model* model);
	void CreateSphereCollider(Model* model);

	BaseCollider* GetCollider() const { return collider_.get(); }
#pragma endregion Collider



	/// ===================================================
	/// コンポーネントの追加
	/// ===================================================
	template<typename T, typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type* = nullptr>
	T* AddComponent() {
		auto addComponent = std::make_unique<T>();
		addComponent->SetParent(this);
		addComponent->Initialize();
		T* componentPtr = addComponent.get();
		components_.push_back(std::move(addComponent));
		return componentPtr;
	}


	/// ===================================================
	/// コンポーネントのゲッタ
	/// ===================================================
	template<typename T, typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type* = nullptr>
	T* GetComponent() {
		for(auto& component : components_) {
			T* result = dynamic_cast<T*>(component.get());
			/// キャスト可能なら返す
			if(result) {
				return result;
			}
		}
		return nullptr;
	}


	const std::list<std::unique_ptr<BaseComponent>>& GetComponents() const {
		return components_;
	}

private:
	std::string tag_;	//- グループごとの名前 : Enemyなど
	std::string name_;	//- オブジェクトごとに違う名前 : Enemy1, Enemy2など
protected:

	/// components内のtransformへのポインタ
	Transform* pTranform_;

	/// 親
	BaseGameObject* parent_ = nullptr;
	std::list<BaseGameObject*> childs_;


	std::unique_ptr<BaseCollider> collider_ = nullptr;

	std::list<std::unique_ptr<BaseComponent>> components_;

public:

	bool isActive = true;
	bool isDrawActive = true;
	int drawLayerId = 0;

};

