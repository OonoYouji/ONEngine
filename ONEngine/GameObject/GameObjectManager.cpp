#include "GameObjectManager.h"

#include <ImGuiManager.h>



/// ===================================================
/// 初期化
/// ===================================================
void GameObjectManager::Initialize() {

}

/// ===================================================
/// 終了処理
/// ===================================================
void GameObjectManager::Finalize() {
	objects_.clear();
}


/// ===================================================
/// 更新
/// ===================================================
void GameObjectManager::Update() {

	ReName();

	for(auto& obj : objects_) {
		if(obj->isActive) {
			obj->Update();
			obj->UpdateMatrix();
		}
	}
}

void GameObjectManager::LastUpdate() {
	for(auto& obj : objects_) {
		if(obj->isActive) {
			obj->LastUpdate();
		}
	}

	/// 消去命令の出たオブジェクトを削除
	for(auto& obj : destoryList_) {
		SubGameObject(obj);
	}
	destoryList_.clear();

}


/// ===================================================
/// 描画
/// ===================================================
void GameObjectManager::Draw() {
	for(auto& obj : objects_) {
		if(obj->isDrawActive) {
			obj->Draw();
		}
	}
}

void GameObjectManager::FrontSpriteDraw() {
	for(auto& obj : objects_) {
		if(obj->isDrawActive) {
			obj->FrontSpriteDraw();
		}
	}
}

void GameObjectManager::BackSpriteDraw() {
	for(auto& obj : objects_) {
		if(obj->isDrawActive) {
			obj->BackSpriteDraw();
		}
	}
}

void GameObjectManager::BackSpriteDraw(int layerId) {
	for(auto& obj : objects_) {
		if(!obj->isDrawActive) { continue; }
		if(obj->drawLayerId != layerId) { continue; }
		obj->BackSpriteDraw();
	}
}

void GameObjectManager::Object3dDraw(int layerId) {
	for(auto& obj : objects_) {
		if(!obj->isDrawActive) { continue; }
		if(obj->drawLayerId != layerId) { continue; }
		obj->Draw();
	}
}

void GameObjectManager::FrontSpriteDraw(int layerId) {
	for(auto& obj : objects_) {
		if(!obj->isDrawActive) { continue; }
		if(obj->drawLayerId != layerId) { continue; }
		obj->BackSpriteDraw();
	}
}



/// ===================================================
/// ゲームオブジェクトの追加
/// ===================================================
void GameObjectManager::AddGameObject(BaseGameObject* object) {
	std::unique_ptr<BaseGameObject> newObject(object);
	objects_.push_back(std::move(newObject));
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
		//CollisionManager::GetInstance()->SubGameObject(object);
		objects_.erase(it);
	}

}

/// ===================================================
/// ゲームオブジェクトの消去リストに追加
/// ===================================================
void GameObjectManager::Destory(BaseGameObject* object) {
	destoryList_.push_back(object);
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
	GameObjectManager* insntance = GetInstance();
	BaseGameObject* result = nullptr;
	auto itr = std::find_if(insntance->objects_.begin(), insntance->objects_.end(), [&name, &result](std::unique_ptr<BaseGameObject>& object) {
		if(object->GetName() == name) {
			result = object.get();
			return true;
		}

		return false;
	});

	return result;
}

/// ===================================================
/// すべてのインスタンスを削除する
/// ===================================================
void GameObjectManager::DestoryAll() {
	GameObjectManager* insntance = GetInstance();
	insntance->objects_.clear();
	insntance->selectObject_ = nullptr;
}

std::string GameObjectManager::CreateName(const BaseGameObject* const object) {
	std::string name = typeid(*object).name();
	name = name.substr(std::string("class ").length());
	return name;
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

	ImGui::Checkbox("isActive", &selectObject_->isActive);
	ImGui::Checkbox("isDrawActive", &selectObject_->isDrawActive);
	ImGui::DragInt("drawLayerId", &selectObject_->drawLayerId, 0);

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
