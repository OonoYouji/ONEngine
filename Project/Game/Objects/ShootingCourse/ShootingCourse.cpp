#define NOMINMAX
#include "ShootingCourse.h"

/// std
#include <cassert>
#include <fstream>
#include <sstream>
#include <numbers>

/// externals
#include <json.hpp>

/// engine
#include "ImGuiManager/ImGuiManager.h"

/// components
#include "ComponentManager/SplinePathRenderer/SplinePathRenderer.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

/// using namespace
using namespace nlohmann;


ShootingCourse::ShootingCourse() {
	CreateTag(this);
}

ShootingCourse::~ShootingCourse() {}

void ShootingCourse::Initialize() {
	splinePathRenderer_         = AddComponent<SplinePathRenderer>(6);
	upDirInterpolationRenderer_ = AddComponent<SplinePathRenderer>(6);

#ifdef _DEBUG
	splinePathRenderer_->isActive = true;
	upDirInterpolationRenderer_->isActive = true;
#else
	splinePathRenderer_->isActive = false;
	upDirInterpolationRenderer_->isActive = false;
#endif // _DEBUG


	meshInstancedRenderer_      = AddComponent<MeshInstancingRenderer>(256);
	anchorPointRenderer_        = AddComponent<MeshInstancingRenderer>(48);

	/// レールのモデルをセット
	meshInstancedRenderer_->SetModel("Rail");
	anchorPointRenderer_->SetModel("AnchorPoint");

	/// file input
	LoadFile(filePath_);
	splinePathRenderer_->SetAnchorPointArray(vertices_);

	/// 読み込んだポイントが4個以下であれば
	if(anchorPointArray_.size() < 4) {
		AddAnchorPoint({});
		AddAnchorPoint({});
		AddAnchorPoint({});
		AddAnchorPoint({});
	}


	upDirArray_.clear();

	for(auto& anchorPoint : anchorPointArray_) {
		vertices_.push_back(anchorPoint.position);
		upDirArray_.push_back(anchorPoint.up);
	}

	splinePathRenderer_->SetAnchorPointArray(vertices_);
	splinePathRenderer_->Update();
	CalcuationUpDirctionArray();
	upDirInterpolationRenderer_->Update();


	anchorPointRenderer_->isActive = false;

}

void ShootingCourse::Update() {

#ifdef _DEBUG	/// copy, 重そう、 debugのときだけにする
	std::vector<Vec3> tmpVertices;
	for(auto& anchorPoint : anchorPointArray_) {
		tmpVertices.push_back(anchorPoint.position);
	}
	vertices_ = tmpVertices;
#endif // _DEBUG

	
	CalcuationUpDirctionArray();

	/// tranformの再計算
	CalcuationRailTransform();
	CalcuationAnchorPointArray();

	splinePathRenderer_->SetAnchorPointArray(vertices_);
}

#pragma region debug
void ShootingCourse::Debug() {
	if(ImGui::TreeNodeEx("course editor", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::TreeNodeEx("anchor points", ImGuiTreeNodeFlags_DefaultOpen)) {

			if(ImGui::Button("save file")) {
				SaveFile(filePath_);
			}

			ImGui::Separator();


			ImGui::SliderInt("addIndex", &addIndex_, 0, static_cast<int>(anchorPointArray_.size() - 1));
			if(ImGui::Button("add")) {
				auto itr = anchorPointArray_.begin() + addIndex_;
				anchorPointArray_.insert(itr, (*itr));
			}

			ImGui::Separator();


			ImGui::SliderInt("subtractIndex", &subtractIndex_, 0, static_cast<int>(anchorPointArray_.size() - 1));
			if(ImGui::Button("subtract")) {
				if(subtractIndex_ >= anchorPointArray_.size()) {
					subtractIndex_ = static_cast<int>(anchorPointArray_.size() - 1);
				}
				if(anchorPointArray_.size() > 4) {
					anchorPointArray_.erase(anchorPointArray_.begin() + subtractIndex_);
				}
			}


			ImGui::TreePop();
		}

		ImGui::TreePop();
	}


	ImGui::BeginChild("anchor point array scroll bar", ImVec2(0, 360.0f), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if(ImGui::TreeNodeEx("anchor points", ImGuiTreeNodeFlags_DefaultOpen)) {

		uint32_t index = 0u;
		for(auto& anchorPoint : anchorPointArray_) {

			std::string positionLabel = std::string("position_") + std::to_string(index);
			std::string upLabel       = std::string("up_") + std::to_string(index);

			ImGui::DragFloat3(positionLabel.c_str(), &anchorPoint.position.x, 0.05f);
			ImGui::DragFloat3(upLabel.c_str(),       &anchorPoint.up.x,       0.05f);

			ImGui::Spacing();
			index++;
		}

		ImGui::TreePop();
	}

	ImGui::EndChild();


}
#pragma endregion


#pragma region file input output
void ShootingCourse::SaveFile(const std::string& filePath) {
	json root = json::object();
	for(size_t i = 0; i < anchorPointArray_.size(); ++i) {
		auto& item = root[std::to_string(i)];

		AnchorPoint& point = anchorPointArray_[i];
		item["position"] = json::array({ point.position.x, point.position.y, point.position.z });
		item["up"]       = json::array({ point.up.x,       point.up.y,       point.up.z });
	}

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(filePath);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}


	///- File open
	std::string path = filePath + "ShootingCourse.json";
	std::ofstream ofs;
	ofs.open(path);

	if(ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "ShootingCourse", 0);
		assert(false);
		return;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();

}

void ShootingCourse::LoadFile(const std::string& filePath) {
	///- ファイルを開く
	std::string path = filePath + "ShootingCourse.json";
	std::ifstream ifs;
	ifs.open(path);

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


	size_t anchorPointSize = 0;
	for(auto& item : root.items()) {
		size_t value = std::stoi(item.key().c_str());
		if(anchorPointSize < value) {
			anchorPointSize = value;
		}
	}

	anchorPointArray_.resize(anchorPointSize + 1);

	for(auto& item : root.items()) {
		size_t index = std::stoi(item.key().c_str());
		auto jsonPos = item.value()["position"];
		auto jsonUp  = item.value()["up"];
		AnchorPoint anchorPoint{
			.position = {jsonPos.at(0), jsonPos.at(1), jsonPos.at(2)},
			.up       = {jsonUp.at(0),  jsonUp.at(1),  jsonUp.at(2)}
		};

		anchorPointArray_[index] = anchorPoint;
	}


}
#pragma endregion file input output



void ShootingCourse::AddAnchorPoint(const Vec3& point) {
	anchorPointArray_.emplace_back(point);
}

void ShootingCourse::CalcuationRailTransform() {
	/// スプライン曲線の補完された点をゲット
	const std::vector<Vec3>& segmentPointArray = splinePathRenderer_->GetSegmentPointArray();
	const std::vector<Vec3>& upDirArray        = upDirInterpolationRenderer_->GetSegmentPointArray();

	/// 上でゲットした配列のどちらかが空だと早期リターン
	if(segmentPointArray.empty() || upDirArray.empty()) {
		return;
	}

	/// mesh instanced rendererのtransformをリセット
	meshInstancedRenderer_->ResetTransformArray();
	transformList_.clear();



	
	/// レールのtransformを計算

	for(size_t i = 0; i < segmentPointArray.size() - 1; ++i) {

		float t = static_cast<float>(i) / static_cast<float>(segmentPointArray.size());
		float next = static_cast<float>(i + 1) / static_cast<float>(segmentPointArray.size());

		AnchorPoint currentAP = SplinePosition(anchorPointArray_, t);
		AnchorPoint nextAP    = SplinePosition(anchorPointArray_, next);

		/// 現在の座標と次の座標を得る
		const Vec3& currentPoint = segmentPointArray[i];
		const Vec3& nextPoint    = segmentPointArray[i + 1];

		/// 進行方向、ローカル上方向、ローカル左方向のベクトル計算
		Vec3 moveDir  = Vec3(nextAP.position - currentAP.position).Normalize();
		Vec3 upDir    = Vec3::Lerp(nextAP.up, currentAP.up, 0.5f);
		//Vec3 upDir    = upDirArray[i];
		Vec3 rightDir = Vec3::Cross(moveDir, upDir);


		/// transformを計算する
		Transform transform;
		//transform.rotateOrder = QUATERNION;
		transform.position = currentPoint;

		transform.rotate = {
			std::asin(-moveDir.y),
			std::atan2(moveDir.x, moveDir.z),
			0.0f
		};

		transform.UpdateMatrix();
		transformList_.push_back(std::move(transform));

	}


	for(auto& transform : transformList_) {
		meshInstancedRenderer_->AddTransform(&transform);
	}
}

void ShootingCourse::CalcuationAnchorPointArray() {



	anchorPointRenderer_->ResetTransformArray();
	anchorPointTransformList_.clear();
	for(auto& anchorPoint : anchorPointArray_) {
		Transform transform;
		transform.position = anchorPoint.position;
		transform.rotate = {
			std::atan2(-anchorPoint.up.y, Vec3::Length({ anchorPoint.up.x, 0.0f, anchorPoint.up.z })),
			std::atan2(anchorPoint.up.x, anchorPoint.up.z),
			0.0f,
		};

		transform.rotate.x += std::numbers::pi_v<float> * 0.5f;

		transform.UpdateMatrix();

		anchorPointTransformList_.push_back(std::move(transform));
	}


	for(auto& transform : anchorPointTransformList_) {
		anchorPointRenderer_->AddTransform(&transform);
	}
}

void ShootingCourse::CalcuationUpDirctionArray() {
	/// 上方向ベクトルの正規化
	upDirArray_.clear();
	for(auto& anchorPoint : anchorPointArray_) {
		anchorPoint.up = anchorPoint.up.Normalize();
		upDirArray_.push_back(anchorPoint.up);
	}

	upDirInterpolationRenderer_->SetAnchorPointArray(upDirArray_);

}





AnchorPoint SplinePosition(const std::vector<AnchorPoint>& anchorPointArray, float t) {
	assert(anchorPointArray.size() >= 4 && "制御点は4点以上必要です");

	size_t division = anchorPointArray.size() - 1;
	float areaWidth = 1.0f / static_cast<float>(division);

	///- 区間内の始点を0.0f, 終点を1.0f としたときの現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	///- 区画番号
	size_t index = static_cast<size_t>(t / areaWidth);
	index = std::min(index, anchorPointArray.size() - 1);

	size_t indices[4]{
		index - 1,
		index,
		index + 1,
		index + 2
	};

	if(index == 0) {
		indices[0] = indices[1];
	}

	if(indices[2] >= anchorPointArray.size()) {
		indices[2] = indices[1];
		indices[3] = indices[1];
	}

	if(indices[3] >= anchorPointArray.size()) {
		indices[3] = indices[2];
	}

	const AnchorPoint& p0 = anchorPointArray[indices[0]];
	const AnchorPoint& p1 = anchorPointArray[indices[1]];
	const AnchorPoint& p2 = anchorPointArray[indices[2]];
	const AnchorPoint& p3 = anchorPointArray[indices[3]];

	return SplineInterpolation({ p0, p1, p2, p3 }, t_2);
}

AnchorPoint SplineInterpolation(const std::array<AnchorPoint, 4>& anchorPointArray, float t) {

	const float s = 0.5f;

	float t2 = t * t;  // t^2
	float t3 = t2 * t; // t^3

	/// positionの補完
	std::array<Vec3, 4> p{
		anchorPointArray[0].position,
		anchorPointArray[1].position,
		anchorPointArray[2].position,
		anchorPointArray[3].position
	};

	/// position element-> pe
	Vec3 pe3 = -p[0] + (p[1] * 3.0f) - (p[2] * 3.0f) + p[3];
	Vec3 pe2 = (p[0] * 2.0f) - (p[1] * 5.0f) + (p[2] * 4.0f) - p[3];
	Vec3 pe1 = -p[0] + p[2];
	Vec3 pe0 = p[1] * 2.0f;



	/// twistsの補完
	std::array<Vec3, 4> twists{
		anchorPointArray[0].up,
		anchorPointArray[1].up,
		anchorPointArray[2].up,
		anchorPointArray[3].up
	};

	/// twist element-> te
	Vec3 te3 = -twists[0] + (twists[1] * 3.0f) - (twists[2] * 3.0f) + twists[3];
	Vec3 te2 = (twists[0] * 2.0f) - (twists[1] * 5.0f) + (twists[2] * 4.0f) - twists[3];
	Vec3 te1 = -twists[0] + twists[2];
	Vec3 te0 = twists[1] * 2.0f;

	/// result
	return {
		((pe3 * t3) + (pe2 * t2) + (pe1 * t) + pe0) * s,
		((te3 * t3) + (te2 * t2) + (te1 * t) + te0) * s
	};
}
