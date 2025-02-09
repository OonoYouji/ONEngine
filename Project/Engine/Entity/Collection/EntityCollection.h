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
	IEntity* GenerateEntity() requires std::is_base_of_v<IEntity, T>;

	/// @brief 新しい camera を生成する
	/// @return cameraへのポインタ
	Camera* GenerateCamera();

private:

	/// other class pointer
	class DxManager* pDxManager_;

	std::vector<std::unique_ptr<IEntity>> entities_;

};


/// ===================================================
/// inline methods
/// ===================================================

template<class T>
inline IEntity* EntityCollection::GenerateEntity() requires std::is_base_of_v<IEntity, T> {
	std::unique_ptr<T> entity = std::make_unique<T>();
	entity->Initialize();

	IEntity* entityPtr = entity.get();
	entities_.push_back(std::move(entity));

	return entityPtr;
}
