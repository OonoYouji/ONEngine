#include "EntityPrefabCollection.h"

#include "Engine/Core/Utility/Math/Mathf.h"

void EntityPrefabCollection::LoadPrefabAll() {
	/// Assets/Prefabs フォルダから全てのプレハブを読み込む
	std::string prefabPath = "./Assets/Prefabs/";

	std::vector<File> prefabFiles = Mathf::FindFiles(prefabPath, ".prefab");

	if (prefabFiles.empty()) {
		Console::LogError("No prefab files found in: " + prefabPath);
		return;
	}

	/// directoryを探索
	for (const auto& file : prefabFiles) {
		prefabs_[file.second] = std::make_unique<EntityPrefab>(file.first);
	}
}

void EntityPrefabCollection::ReloadPrefab(const std::string& _prefabName) {
	auto itr = prefabs_.find(_prefabName);
	if (itr == prefabs_.end()) {
		/// もう一度Fileを探索して確認
		File file = Mathf::FindFile("./Assets/Prefabs/", _prefabName);

		if (file.first.empty()) {
			Console::LogWarning("Prefab not found: " + _prefabName);
			return;
		}

		///!< 複数あった場合は最初に見つかったものを使用する
		prefabs_[file.second] = std::make_unique<EntityPrefab>(file.first);

		itr = prefabs_.find(_prefabName);
	}

	/// prefabを再読み込み
	itr->second->Reload();
}

EntityPrefab* EntityPrefabCollection::GetPrefab(const std::string& _prefabName) {
	auto itr = prefabs_.find(_prefabName);
	if (itr != prefabs_.end()) {
		return itr->second.get();
	}

	Console::LogWarning("Prefab not found: " + _prefabName);
	return nullptr;
}
