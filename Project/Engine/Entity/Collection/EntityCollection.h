#pragma once

/// std
#include <type_traits>
#include <memory>
#include <vector>

/// engine
#include "../Interface/IEntity.h"
#include "../Camera/Camera.h"


/// ===================================================
/// entity collection
/// ===================================================
class EntityCollection final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityCollection(class DxManager* _dxManager);
	~EntityCollection();

	/// @brief 初期化関数
	void Initialize();

	/// @brief 更新関数
	void Update();

	/// @brief 新しい entity を生成する
	/// @tparam T entityを継承したクラス
	/// @return 
	template <class T>
	T* GenerateEntity() requires std::is_base_of_v<IEntity, T>;

	/// @brief 新しい camera を生成する
	/// @return cameraへのポインタ
	Camera* GenerateCamera();

	/// @brief 新しい　2D camera を生成する
	/// @return 2D cameraへのポインタ
	Camera2D* GenerateCamera2D();

	/// @brief entity の削除
	/// @param _entity 
	void RemoveEntity(IEntity* _entity);

private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// other class pointer
	class DxManager* pDxManager_;

	std::vector<std::unique_ptr<IEntity>> entities_;
	std::vector<Camera*>                  cameras_;   ///< カメラのリスト、本体はentities_に格納されている
	std::vector<Camera2D*>                camera2ds_; ///< カメラのリスト、本体はentities_に格納されている

	Camera* mainCamera_ = nullptr; ///< メインカメラ


#ifdef _DEBUG ///< デバッグモードのみ
	IEntity* selectedEntity_ = nullptr;
#endif // _DEBUG


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief main cameraの設定
	/// @param _camera Cameraクラスへのポインタ
	void SetMainCamera(Camera* _camera) { mainCamera_ = _camera; }

	/// @brief main cameraの設定
	/// @param _index cameraのインデックス
	void SetMainCamera(size_t _index) { mainCamera_ = cameras_[_index]; }


	/// @brief entities の取得
	/// @return entities
	const std::vector<std::unique_ptr<IEntity>>& GetEntities() { return entities_; }

	/// @brief cameras の取得
	/// @return cameras
	const std::vector<Camera*>& GetCameras() { return cameras_; }

	/// @brief 2d cameras の取得
	/// @return camera2ds
	const std::vector<Camera2D*>& GetCamera2Ds() { return camera2ds_; }

	/// @brief main cameraの取得
	/// @return Cameraクラスへのポインタ
	const Camera* GetMainCamera() const { return mainCamera_; }
	Camera* GetMainCamera() { return mainCamera_; }


#ifdef _DEBUG
public:

	/// ===================================================
	/// public : debug methods
	/// ===================================================

	void ImGui();

#endif // _DEBUG
};


/// ===================================================
/// inline methods
/// ===================================================

template<class T>
inline T* EntityCollection::GenerateEntity() requires std::is_base_of_v<IEntity, T> {
	std::unique_ptr<T> entity = std::make_unique<T>();
	entity->pEntityCollection_ = this;
	entity->Initialize();

	T* entityPtr = entity.get();
	entities_.push_back(std::move(entity));

	return entityPtr;
}
