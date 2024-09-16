#include "GameObjectManager.h"

#include <ImGuiManager.h>
#include <Collision/CollisionManager.h>

#include <BaseCamera.h>
#include <Light/DirectionalLight.h>

#include <CreateName.h>


/// ===================================================
/// 初期化
/// ===================================================
void GameObjectManager::Initialize() {

}

/// ===================================================
/// 終了処理
/// ===================================================
void GameObjectManager::Finalize() {
	if (!addObjectList_.empty()) {
		for (auto& object : addObjectList_) {
			std::unique_ptr<BaseGameObject> newObject(object);
			objects_.push_back(std::move(newObject));
		}
		addObjectList_.clear();
	}
	objects_.clear();
}

void GameObjectManager::GameObjectInitialize(int sceneId) {
	for(auto& obj : objects_) {
		obj->Initialize();
	}
}


/// ===================================================
/// 更新
/// ===================================================
void GameObjectManager::Update() {

	if(!addObjectList_.empty()) {
		for(auto& object : addObjectList_) {
			std::unique_ptr<BaseGameObject> newObject(object);
			objects_.push_back(std::move(newObject));
		}
		addObjectList_.clear();
	}

	ReName();

	for(auto& obj : objects_) {
		if(!obj->isActive) { continue; }
		obj->Update();
		for(auto& component : obj->GetComponents()) {
			if(!component->isActive) { continue; }
			component->Update();
		}
	}
}

void GameObjectManager::LastUpdate() {
	for(auto& obj : objects_) {
		if(!obj->isActive) { continue; }
		obj->LastUpdate();
		for(auto& component : obj->GetComponents()) {
			if(!component->isActive) { continue; }
			component->LastUpdate();
		}
	}

	/// 消去命令の出たオブジェクトを削除
	for(auto& obj : destoryList_) {
		SubGameObject(obj);
	}
	destoryList_.clear();

}


void GameObjectManager::BackSpriteDraw(int layerId) {
	for(auto& obj : objects_) {
		if(obj->drawLayerId != layerId) { continue; }

		for(auto& component : obj->GetComponents()) {
			if(!component->isActive) { continue; }
			component->BackSpriteDraw();
		}
	}
}

void GameObjectManager::Object3dDraw(int layerId) {
	for(auto& obj : objects_) {
		if(obj->drawLayerId != layerId) { continue; }

		for(auto& component : obj->GetComponents()) {
			if(!component->isActive) { continue; }
			component->Draw();
		}

	}
}

void GameObjectManager::FrontSpriteDraw(int layerId) {
	for(auto& obj : objects_) {
		if(obj->drawLayerId != layerId) { continue; }

		for(auto& component : obj->GetComponents()) {
			if(!component->isActive) { continue; }
			component->FrontSpriteDraw();
		}
	}
}



/// ===================================================
/// ゲームオブジェクトの追加
/// ===================================================
void GameObjectManager::AddGameObject(BaseGameObject* object) {
	addObjectList_.push_back(object);

}


/// ===================================================
/// ゲームオブジェクトの削除
/// ===================================================
void GameObjectManager::SubGameObject(BaseGameObject* object) {
	auto it = std::find_if(objects_.begin(), objects_.end(),
						   [&object](const std::unique_ptr<BaseGameObject>& obj) {
		return obj.get() == object;
	});

	if(it != objects_.end()) {
		if(object == selectObject_) {
			selectObject_ = nullptr;
		}
		CollisionManager::GetInstance()->SubGameObject(object);
		objects_.erase(it);
	}

}

/// ===================================================
/// ゲームオブジェクトの消去リストに追加
/// ===================================================
void GameObjectManager::Destory(BaseGameObject* object) {
	destoryList_.push_back(object);
	for(auto& child : object->GetChilds()) {
		destoryList_.push_back(object);
	}

}



void GameObjectManager::ReName() {
	std::unordered_map<std::string, uint32_t> instanceCounts;
	for(auto& object : objects_) {
		std::string name = CreateName(object.get());
		uint32_t count = instanceCounts[name];
		if(count) {
			object->SetName(name + std::to_string(count));
		}
		++instanceCounts[name];
	}
}


/// ===================================================
/// nameからGameObjectを探索、返す
/// ===================================================
BaseGameObject* GameObjectManager::GetGameObject(const std::string& name) {
	GameObjectManager* instance = GetInstance();
	BaseGameObject* result = nullptr;
	auto itr = std::find_if(instance->objects_.begin(), instance->objects_.end(), [&name, &result](std::unique_ptr<BaseGameObject>& object) {
		if(object->GetName() == name) {
			result = object.get();
			return true;
		}

		return false;
	});

	if(!result) {
		if(!instance->addObjectList_.empty()) {
			auto addObjectListItr = std::find_if(instance->addObjectList_.begin(), instance->addObjectList_.end(), [&name](BaseGameObject* object) {
				if(object->GetName() == name) {
					return true;
				}
				return false;
			});
			if(addObjectListItr != instance->addObjectList_.end()) {
				result = (*addObjectListItr);
			}
		}
	}

	return result;
}

uint32_t GameObjectManager::GetInstanceCount(const std::string& tag) {
	GameObjectManager* instance = GetInstance();
	uint32_t count = 0U;
	for(const auto& object : instance->objects_) {
		if(object->GetTag() == tag) {
			count++;
		}
	}
	return count;
}

std::list<BaseGameObject*> GameObjectManager::GetGameObjectList(const std::string& tag) {
	GameObjectManager* instance = GetInstance();
	std::list<BaseGameObject*> result;
	for(const auto& object : instance->objects_) {
		if(object->GetTag() == tag) {
			result.push_back(object.get());
		}
	}

	for(const auto& object : instance->addObjectList_) {
		if(object->GetTag() == tag) {
			result.push_back(object);
		}
	}


	return result;
}

/// ===================================================
/// すべてのインスタンスを削除する
/// ===================================================
void GameObjectManager::DestoryAll() {
	GameObjectManager* insntance = GetInstance();



	insntance->objects_.remove_if([](const std::unique_ptr<BaseGameObject>& obj) {
		auto IsDelete = [](BaseGameObject* obj) -> bool {
			if(dynamic_cast<DirectionalLight*>(obj) != nullptr) {
				return false;
			}
			if(dynamic_cast<BaseCamera*>(obj) != nullptr) {
				return false;
			}
			return true;
		};

		bool isDelete = true;
		isDelete &= IsDelete(obj.get());
		isDelete &= IsDelete(obj->GetParent());
		for(auto& child : obj->GetChilds()) {
			isDelete &= IsDelete(child);
		}

		if(isDelete) {
			CollisionManager::GetInstance()->SubGameObject(obj.get());
		}

		return isDelete;
	});

	insntance->selectObject_ = nullptr;
}


bool GameObjectManager::IsAliveObject(BaseGameObject* object) {
	GameObjectManager* instance = GetInstance();

	auto CheckAlive = [&object](const std::unique_ptr<BaseGameObject>& obj) { return obj.get() == object; };
	auto itr = std::find_if(instance->objects_.begin(), instance->objects_.end(), CheckAlive);

	if(itr != instance->objects_.end()) {
		return true;
	}

	auto CheckAlivePtr = [&object](const BaseGameObject* obj) { return obj == object; };
	auto addObjItr = std::find_if(instance->addObjectList_.begin(), instance->addObjectList_.end(), CheckAlivePtr);
	if(addObjItr != instance->addObjectList_.end()) {
		return true;
	}

	return false;
}

void GameObjectManager::AddObjectsToObjectsCopy() {
	GameObjectManager* instance = GetInstance();
	if(!instance->addObjectList_.empty()) {
		for(auto& object : instance->addObjectList_) {
			std::unique_ptr<BaseGameObject> newObject(object);
			instance->objects_.push_back(std::move(newObject));
		}
		instance->addObjectList_.clear();
	}
}


/// ===================================================
/// imguiでデバッグ表示
/// ===================================================
void GameObjectManager::ImGuiDebug() {
#ifdef _DEBUG

	/// ===================================================
	///  ヒエラルキー オブジェクトの選択
	/// ===================================================
	ImGui::Begin("Hierarchy");


	/// ------------------------------------------------
	/// BaseGameObject SelecTable
	/// ------------------------------------------------
	for(auto& gameObject : objects_) {

		if(gameObject->GetParent()) { continue; }
		if(ImGui::Selectable(gameObject->GetName().c_str(), selectObject_ == gameObject.get())) {
			selectObject_ = gameObject.get();
		}

		ImGuiSelectChilds(gameObject->GetChilds());

	}

	ImGui::End();


	/// ===================================================
	/// 選択されたオブジェクトのデバッグ表示
	/// ===================================================
	ImGui::Begin("Inspector");

	if(selectObject_) {
		ImGuiSelectObjectDebug();
	}

	ImGui::End();

#endif // _DEBUG
}



/// ===================================================
/// ImGuiのBaseGameObjectの子供をselectableで設定
/// ===================================================
void GameObjectManager::ImGuiSelectChilds([[maybe_unused]] const std::list<BaseGameObject*>& childs) {
#ifdef _DEBUG
	ImGui::Indent();
	for(auto& child : childs) {
		if(ImGui::Selectable(child->GetName().c_str(), selectObject_ == child)) {
			selectObject_ = child;
		}
		ImGuiSelectChilds(child->GetChilds());
	}
	ImGui::Unindent();
#endif // _DEBUG
}


/// ===================================================
/// select objcetのデバッグ
/// ===================================================
void GameObjectManager::ImGuiSelectObjectDebug() {
#ifdef _DEBUG

	/// activeのフラグをデバッグ
	ImGui::Checkbox("isActive", &selectObject_->isActive);
	if(ImGui::IsItemEdited()) {
		for(auto& child : selectObject_->GetChilds()) {
			child->isActive = selectObject_->isActive;
		}
	}

	/// drawLayerIdのフラグをデバッグ
	ImGui::DragInt("drawLayerId", &selectObject_->drawLayerId, 0);
	if(ImGui::IsItemEdited()) {
		for(auto& child : selectObject_->GetChilds()) {
			child->drawLayerId = selectObject_->drawLayerId;
		}
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Always);

	ImGuiTreeNodeFlags_ flags = ImGuiTreeNodeFlags_(ImGuiTreeNodeFlags_DefaultOpen);
	if(!ImGui::TreeNodeEx(selectObject_->GetName().c_str(), flags)) {
		return;
	}

	ImGui::Unindent();


	selectObject_->ImGuiDebug();

	ImGui::TreePop();
#endif // _DEBUG
}
