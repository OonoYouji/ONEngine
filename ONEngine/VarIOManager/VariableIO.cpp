#include "VariableIO.h"

/// std
#include <cassert>
#include <fstream>
#include <sstream>

/// engine
#include <ImGuiManager.h>


void VariableIO::Update() {}

void VariableIO::ImGuiDebug() {}




void VariableIO::SaveFile(const std::string& groupName) {

	///- グループ検索
	std::unordered_map<std::string, Group>::iterator itrGroup = datas_.find(groupName);
	///- 未登録検索
	assert(itrGroup != datas_.end());

	json root;
	root = json::object();

	///- jsonオブジェクト登録
	root[groupName] = json::object();

	///- 各項目
	for(auto& itrItem : itrGroup->second) {

		const std::string& itemName = itrItem.first;
		Item& item = itrItem.second;

		if(std::holds_alternative<int32_t>(item)) {

			root[groupName][itemName] = std::get<int32_t>(item);
		} else if(std::holds_alternative<float>(item)) {

			root[groupName][itemName] = std::get<float>(item);
		} else if(std::holds_alternative<Vec3>(item)) {

			Vec3 value = std::get<Vec3>(item);
			root[groupName][itemName] = json::array({ value.x,value.y,value.z });
		} else if(std::holds_alternative<bool>(item)) {

			root[groupName][itemName] = std::get<bool>(item);
		}


	}

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath_);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}


	///- File open
	std::string filePath = kDirectoryPath_ + groupName + ".json";
	std::ofstream ofs;
	ofs.open(filePath);

	if(ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(false);
		return;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();

}


void VariableIO::LoadFiles() {

	///- 読み込み先がなければ return
	std::filesystem::path dir(kDirectoryPath_);
	if(!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);
	for(const auto& entry : dir_it) {

		///- ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		///- ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		///- .json以外はスキップ
		if(extension.compare(".json") != 0) {
			continue;
		}

		LoadFile(filePath.stem().string());

	}

}


void VariableIO::LoadFile(const std::string& groupName) {

	///- ファイルを開く
	std::string filePath = kDirectoryPath_ + groupName + ".json";
	std::ifstream ifs;
	ifs.open(filePath);

	///- 開けなければメッセージを出す
	if(!ifs.is_open()) {
		std::string message = "File could not be opened.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(false);
		return;
	}

	///- json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();

	///- グループ検索
	json::iterator itGroup = root.find(groupName);

	///- 未登録チェック
	assert(itGroup != root.end());

	///- 各アイテム
	for(json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {

		///- アイテム名を取得
		const std::string& itemName = itItem.key();

		///- int32_t型の値があれば
		if(itItem->is_number_integer()) {
			///- int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);

		} else if(itItem->is_number_float()) {
			///- float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));

		} else if(itItem->is_array() && itItem->size() == 3) {
			///- Vector3型の値を登録
			Vec3f value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);

		} else if(itItem->is_boolean()) {
			///- bool型の値を登録
			bool value = itItem->get<bool>();
			SetValue(groupName, itemName, value);
		}

	}


}
