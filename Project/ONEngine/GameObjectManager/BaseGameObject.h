#pragma once

#include <list>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>

#include "ComponentManager/Transform/Transform.h"
#include "ComponentManager/Base/BaseComponent.h"

#include "GraphicManager/ModelManager/Model.h"


/// <summary>
/// すべてのゲームオブジェクトの基本となるクラス
/// </summary>
class BaseGameObject {
public:

	BaseGameObject();
	virtual ~BaseGameObject() = default;

	/// ===================================================
	/// public : pure virtual methods
	/// ===================================================

	virtual void Initialize() = 0;
	virtual void Update()     = 0;


	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual void LastUpdate() {}
	virtual void Debug()      {};

	virtual void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {}
	virtual void OnCollisionStay( [[maybe_unused]] BaseGameObject* const collision) {}
	virtual void OnCollisionExit( [[maybe_unused]] BaseGameObject* const collision) {}


	/// ===================================================
	/// public : methods
	/// ===================================================

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

	void RenameComponent(BaseComponent* component);


	/// ---------------------------------------------------
	/// コンポーネントの追加
	/// ---------------------------------------------------
	template<typename T, typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type* = nullptr, typename... Args>
	T* AddComponent(Args&&... args) {
		auto addComponent = std::make_unique<T>(std::forward<Args>(args)...);
		addComponent->SetOwner(this);
		addComponent->Initialize();
		T* componentPtr = addComponent.get();
		RenameComponent(componentPtr);
		components_.push_back(std::move(addComponent));
		return componentPtr;
	}


	/// ---------------------------------------------------
	/// コンポーネントのゲッタ
	/// ---------------------------------------------------
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



	/// ===================================================
	/// public : accessor
	/// ===================================================


	/// ---------------------------------------------------
	/// public : tarnsform accessor
	/// ---------------------------------------------------

	void SetPositionX(float x) { pTransform_->position.x = x; }
	void SetPositionY(float y) { pTransform_->position.y = y; }
	void SetPositionZ(float z) { pTransform_->position.z = z; }
	void SetPosition(const Vec3& v) { pTransform_->position = v; }

	void SetRotateX(float x)   { pTransform_->rotate.x = x; }
	void SetRotateY(float y)   { pTransform_->rotate.y = y; }
	void SetRotateZ(float z)   { pTransform_->rotate.z = z; }
	void SetRotate(const Vec3& v) { pTransform_->rotate = v; }
	void SetQuaternion(const Quaternion& q) { pTransform_->quaternion = q; }
	

	void SetScaleX(float x)    { pTransform_->scale.x = x; }
	void SetScaleY(float y)    { pTransform_->scale.y = y; }
	void SetScaleZ(float z)    { pTransform_->scale.z = z; }
	void SetScale(const Vec3& v) { pTransform_->scale = v; }

	const Vec3  GetPosition()         const;
	const Vec3& GetRotate()           const { return pTransform_->rotate; }
	const Vec3& GetScale()            const { return pTransform_->scale; }
	const Quaternion& GetQuaternion() const { return pTransform_->quaternion; }

	const Mat4& GetMatTransform()   const { return pTransform_->matTransform; };
	Transform* GetTransform() { return pTransform_; }



	/// ---------------------------------------------------
	/// public : parent accessor
	/// ---------------------------------------------------

	void SetParent(Transform* parent);
	Transform* GetParent() const;

	void ParentCancel(bool isLocalToWorld);

	std::list<BaseGameObject*> GetChilds() const;



	/// ---------------------------------------------------
	/// public : tag, name  accessor
	/// ---------------------------------------------------

	void SetTag(const std::string& tag);
	const std::string& GetTag() const;

	void SetName(const std::string& name);
	const std::string& GetName() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string tag_; 
	std::string name_;


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	Transform*                                pTransform_ = nullptr;
	std::list<std::unique_ptr<BaseComponent>> components_;

public:
	/// ===================================================
	/// public : objects
	/// ===================================================

	bool isActive = true;
	int drawLayerId = 0;


};

