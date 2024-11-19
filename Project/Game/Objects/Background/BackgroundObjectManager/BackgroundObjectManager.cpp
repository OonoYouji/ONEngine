#include "BackgroundObjectManager.h"

/// std
#include <iostream>
#include <fstream>
#include <format>
#include <numbers>

/// externals
#include <nlohmann/json.hpp>
#include <imgui.h>

/// engine
#include "Debugger/Assertion.h"

/// objects
#include "../Building/Building.h"
#include "../Stage/Stage.h"
#include "../SkyDome/SkyDome.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


using namespace nlohmann;


BackgroundObjectManager::BackgroundObjectManager() {
	CreateTag(this);
}

BackgroundObjectManager::~BackgroundObjectManager() {}

void BackgroundObjectManager::Initialize() {


	/// ---------------------------------------------------
	/// io data setting
	/// ---------------------------------------------------

	currentItemIndex_ = 0;

	objectClassNames_ = {
		"Building", "SkyDome", "Stage"
	};

	srcObject_.className = "Building";
	srcObject_.position  = { 0, 0, 0 };
	srcObject_.rotate    = { 0, 0, 0 };
	srcObject_.scale     = Vec3::kOne;


#ifdef _DEBUG
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("axis");
#endif // DEBUG

	Register("Building", []() { return new Building(); });
	Register("SkyDome", []() { return new SkyDome(); });
	Register("Stage", []() { return new Stage(); });

	

	/// ---------------------------------------------------
	/// オブジェクトの読み込み、生成
	/// ---------------------------------------------------

	LoadFile(directoryFile_);
	for(auto& data : objectIOData_) {
		CreateObject(data);
	}


	/// ---------------------------------------------------
	/// object create and initializing
	/// ---------------------------------------------------









}

void BackgroundObjectManager::Update() {

	pTransform_->position = srcObject_.position;
	pTransform_->rotate   = srcObject_.rotate;
	pTransform_->scale    = srcObject_.scale;

	int index = 0;
	for(auto& object : bbObjects_) {
		IOData& data = objectIOData_[index];
		object->SetPosition(data.position);
		object->SetRotate(data.rotate);
		object->SetScale(data.scale);

		index++;
	}

}


#pragma region Debug And IO


void BackgroundObjectManager::Debug() {

	/// ---------------------------------------------------
	/// jsonの input output
	/// ---------------------------------------------------

	if(ImGui::Button("save")) {
		SaveFile(directoryFile_);
		MessageBoxA(nullptr, "SaveFile Success", "Success", MB_OK | MB_ICONINFORMATION);
	}

	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();

	if(ImGui::Button("load")) {
		LoadFile(directoryFile_);
		MessageBoxA(nullptr, "LoadFile Success", "Success", MB_OK | MB_ICONINFORMATION);
	}

	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();

	if(ImGui::Button("recreate")) {
		for(auto& obj : bbObjects_) {
			obj->Destory();
		}
		bbObjects_.clear();

		for(auto& data : objectIOData_) {
			CreateObject(data);
		}
	}


	/// ---------------------------------------------------
	/// srcのオブジェクトデバッグ
	/// ---------------------------------------------------

	ImGui::Spacing();
	ImGui::SeparatorText("source object");

	ImGui::DragFloat3("position", &srcObject_.position.x, 0.1f);
	ImGui::DragFloat3("rotate",   &srcObject_.rotate.x,   std::numbers::pi_v<float> * 0.1f);
	ImGui::DragFloat3("scale",    &srcObject_.scale.x,    0.25f);

	std::vector<const char*> cStrings;
	for(const auto& item : objectClassNames_) {
		cStrings.push_back(item.c_str());
	}

	if(ImGui::Combo("Select an option", &currentItemIndex_, cStrings.data(), static_cast<int>(cStrings.size()))) {
		srcObject_.className = objectClassNames_[currentItemIndex_];
	}


	/// ---------------------------------------------------
	/// add, sub objects
	/// ---------------------------------------------------
	
	ImGui::Spacing();
	ImGui::SeparatorText("io data");

	if(objectIOData_.size()) {
		selectIOData_ = std::clamp(selectIOData_, 0, static_cast<int>(objectIOData_.size() - 1));
	} 

	ImGui::SliderInt("io data select index", &selectIOData_, 0, static_cast<int>(objectIOData_.size() - 1));
	
	ImGui::Spacing();


	if(ImGui::Button("add")) {
		objectIOData_.push_back(IOData(srcObject_));
	}

	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();

	if(ImGui::Button("sub")) {
		if(!objectIOData_.empty()) {
			objectIOData_.erase(objectIOData_.begin() + selectIOData_);
		}
	}


	/// ---------------------------------------------------
	/// object debug
	/// ---------------------------------------------------

	ImGui::Spacing();
	ImGui::SeparatorText("io data objects");

	ImGui::BeginChild("io datas", ImVec2(0.0f, 320.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	for(size_t i = 0; i < objectIOData_.size(); ++i) {
		IOData& data = objectIOData_[i];

		ImGui::Text(std::format("IOData({})", i).c_str());
		ImGui::DragFloat3(std::format("position##{:p}", reinterpret_cast<void*>(&data)).c_str(), &data.position.x, 0.1f);
		ImGui::DragFloat3(std::format("rotate##{:p}",   reinterpret_cast<void*>(&data)).c_str(), &data.rotate.x,   std::numbers::pi_v<float> * 0.1f);
		ImGui::DragFloat3(std::format("scale##{:p}",    reinterpret_cast<void*>(&data)).c_str(), &data.scale.x,    0.25f);

		std::vector<char> buffer(data.className.begin(), data.className.end());
		buffer.resize(512, '\0');
		ImGui::InputText(std::format("clasName{:p}", reinterpret_cast<void*>(&data)).c_str(), buffer.data(), buffer.size(), ImGuiInputTextFlags_ReadOnly);


		ImGui::Spacing();
	}

	ImGui::EndChild();

}


void BackgroundObjectManager::SaveFile(const std::string& _filePath) {
	json root = json::object();
	for(size_t i = 0; i < objectIOData_.size(); ++i) {
		auto& item = root[std::to_string(i)];

		IOData& data = objectIOData_[i];
		item["position"]  = json::array({ data.position.x, data.position.y, data.position.z });
		item["rotate"]    = json::array({ data.rotate.x, data.rotate.y, data.rotate.z });
		item["scale"]     = json::array({ data.scale.x, data.scale.y, data.scale.z });
		item["className"] = data.className;
	}

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(_filePath);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}

	///- File open
	std::string path = _filePath + "BackgroundObjectManager.json";
	std::ofstream ofs;
	ofs.open(path);

	if(ofs.fail()) {
		Assert(false, "Failed open data file for write.");
		return;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}



void BackgroundObjectManager::LoadFile(const std::string& _filePath) {

	///- ファイルを開く
	std::string path = _filePath + "BackgroundObjectManager.json";
	std::ifstream ifs;
	ifs.open(path);

	///- 開けなければメッセージを出す
	if(!ifs.is_open()) {
		Assert(false, "File could not be opened.");
		return;
	}

	///- json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();


	size_t ObjectDataArraySize = 0;
	for(auto& item : root.items()) {
		size_t value = std::stoi(item.key().c_str());
		if(ObjectDataArraySize < value) {
			ObjectDataArraySize = value;
		}
	}

	objectIOData_.resize(ObjectDataArraySize + 1);


	for(auto& item : root.items()) {

		size_t index = std::stoi(item.key().c_str());
		auto   jsonPos       = item.value()["position"];
		auto   jsonRotatae   = item.value()["rotate"];
		auto   jsonScale     = item.value()["scale"];
		auto   jsonClassName = item.value()["className"];

		IOData data = {
			.position  = { jsonPos.at(0), jsonPos.at(1), jsonPos.at(2) },
			.rotate    = { jsonRotatae.at(0), jsonRotatae.at(1), jsonRotatae.at(2) },
			.scale     = { jsonScale.at(0), jsonScale.at(1), jsonScale.at(2) },
			.className = jsonClassName,
		};

		objectIOData_[index] = data;
	}
}


#pragma endregion


void BackgroundObjectManager::CreateObject(const IOData& _ioData) {
	BaseGameObject* createObj = factoryMap_.at(_ioData.className)();
	createObj->Initialize();

	createObj->SetPosition(_ioData.position);
	createObj->SetRotate(_ioData.rotate);
	createObj->SetScale(_ioData.scale);

	bbObjects_.push_back(createObj);
}

void BackgroundObjectManager::Register(const std::string& _className, FactoryFunction _factoryFunction) {
	factoryMap_[_className] = _factoryFunction;
}
