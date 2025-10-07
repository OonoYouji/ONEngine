#include "River.h"

/// std
#include <fstream>

/// externals
#include <nlohmann/json.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"


Vector3 CatmullRomPosition(const Vector3& _p0, const Vector3& _p1, const Vector3& _p2, const Vector3& _p3, float _t) {
	float t2 = _t * _t;
	float t3 = t2 * _t;

	return 0.5f * (
		(2.0f * _p1) +
		(-_p0 + _p2) * _t +
		(2.0f * _p0 - 5.0f * _p1 + 4.0f * _p2 - _p3) * t2 +
		(-_p0 + 3.0f * _p1 - 3.0f * _p2 + _p3) * t3);
}

RiverControlPoint CatmullRom(const RiverControlPoint& _p0, const RiverControlPoint& _p1, const RiverControlPoint& _p2, const RiverControlPoint& _p3, float _t) {
	RiverControlPoint result;
	result.position = CatmullRomPosition(_p0.position, _p1.position, _p2.position, _p3.position, _t);
	result.width = _p1.width * (1.0f - _t) + _p2.width * _t;
	return result;
}

std::vector<RiverControlPoint> SampleRiverSpline(const std::vector<RiverControlPoint>& _points, int _samplePerSegment) {
	std::vector<RiverControlPoint> result;

	/// コントロールポイントが4個以下なら何もできない
	if (_points.size() < 4) {
		return result;
	}

	for (size_t i = 0; i < _points.size() - 3; i++) {
		for (size_t s = 0; s < _samplePerSegment; s++) {
			float t = static_cast<float>(s) / _samplePerSegment;
			result.push_back(CatmullRom(
				_points[i + 0],
				_points[i + 1],
				_points[i + 2],
				_points[i + 3],
				t
			));
		}
	}

	// 最後のポイントも追加（p2）
	result.push_back(_points[_points.size() - 2]);
	return result;
}



River::River() : samplePerSegment_(10), isCreatedBuffers_(false) , isGenerateMeshRequest_(false) {};
River::~River() = default;

void River::Edit(EntityComponentSystem* _ecs) {
	/// ----- 川の編集 ----- ///

	/// ---------------------------------------------------------------
	/// ポイントの値を表示
	/// ---------------------------------------------------------------
	if (ImGui::CollapsingHeader("River")) {
		ImGui::Text("Control Points:");
		ImGui::Separator();

		for (int i = 0; i < controlPoints_.size(); i++) {
			ImGui::PushID(i);
			auto& point = controlPoints_[i];
			ImGui::DragFloat4("##point", &point.position.x);
			ImGui::PopID();
		}
	}



	/// ---------------------------------------------------------------
	/// imguizmoでコントロールポイントの編集
	/// ---------------------------------------------------------------

	ImGuizmo::SetOrthographic(false); // 透視投影
	const Vector2& pos = Input::GetImGuiImagePos("Scene");
	const Vector2& size = Input::GetImGuiImageSize("Scene");
	ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

	/// コントロールポイントの表示 & 編集
	for (auto& point : controlPoints_) {
		Gizmo::DrawWireSphere(point.position, point.width, Color::kRed);

		/// 操作対象の行列
		Matrix4x4 targetMatrix = Matrix4x4::MakeTranslate(point.position);
		/// カメラの取得
		CameraComponent* camera = _ecs->GetECSGroup("Debug")->GetMainCamera();
		if (camera) {
			ImGuizmo::Manipulate(
				&camera->GetViewMatrix().m[0][0],
				&camera->GetProjectionMatrix().m[0][0],
				ImGuizmo::OPERATION::TRANSLATE,
				ImGuizmo::MODE::WORLD,
				&targetMatrix.m[0][0]
			);

			if (!ImGuizmo::IsOver()) {
				continue;
			}

			/// 操作したならbreak
			if (ImGuizmo::IsUsing()) {
				/// 行列をSRTに分解、エンティティに適応
				float translation[3], rotation[3], scale[3];
				ImGuizmo::DecomposeMatrixToComponents(&targetMatrix.m[0][0], translation, rotation, scale);

				Vector3 translationV = Vector3(translation[0], translation[1], translation[2]);
				point.position = translationV;
				break;
			}
		}
	}


	/// imgui edit
	if (ImGui::Button("Add")) {
		RiverControlPoint add = { Vector3::kZero, 2.0f };
		controlPoints_.push_back(add);
	}

	isGenerateMeshRequest_ = false;
	if (ImGui::Button("Generate Mesh")) {
		isGenerateMeshRequest_ = true;
	}

	if (ImGui::Button("SaveToFile")) {
		SaveToJson("river");
	}

	if (ImGui::Button("LoadFromJson")) {
		LoadFromJson("river");
	}



	DrawSplineCurve();
}

void River::SaveToJson(const std::string& _name) {

	/// ファイルに保存
	const std::string filepath = "./Packages/Jsons/Terrain/" + _name + ".json";
	std::ifstream ifs(filepath);
	if (!ifs.is_open()) {
		/// ファイルが開けない場合未生成チェック
		std::filesystem::path path(filepath);
		std::filesystem::path parentDir = path.parent_path();

		/// ディレクトリの作成
		if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
			if (!std::filesystem::create_directories(parentDir)) {
				/// 作成に失敗したら
				Console::LogError("failed error create directories: \"" + filepath + "\"");
				return;
			}
		}

		/// ファイルの作成
		std::ofstream ofs(filepath);
		if (!ofs) {
			Console::LogError("failed error create: \" " + _name + "\"");
			return;
		}

		/// 生成に成功したら
		Console::Log("succeeded create file: \"" + filepath + "\"");
	}


	/// 保存するデータの作成
	nlohmann::json river;
	for (auto& point : controlPoints_) {
		nlohmann::json j = nlohmann::json{
			{ "position", point.position },
			{ "width", point.width }
		};
		river.push_back(j);
	}


	/// ファイルにデータを書き込む
	std::ofstream outputFile(filepath);
	if (!outputFile.is_open()) {
		Console::LogError("failed error open file: \"" + filepath + "\"");
		return;
	}

	outputFile << river.dump(4);
	outputFile.close();
}

void River::LoadFromJson(const std::string& _name) {
	/// ファイルを読み込む
	const std::string filepath = "./Packages/Jsons/Terrain/" + _name + ".json";
	std::ifstream ifs(filepath);
	if (!ifs.is_open()) {
		Console::LogError("failed error open file: \"" + filepath + "\"");
		return;
	}


	/// ファイルの内容をJsonに変換
	nlohmann::json json;
	ifs >> json;

	/// コントロールポイントをクリア
	controlPoints_.clear();

	for (auto& point : json) {
		RiverControlPoint add = {
			point.value("position", Vector3{}),
			point.value("width", 2.0f)
		};
		controlPoints_.push_back(add);
	}

}

void River::DrawSplineCurve() {
	/// spline曲線をGizmoで描画する
	createdPoints_ = SampleRiverSpline(controlPoints_, samplePerSegment_);
	if (createdPoints_.empty()) {
		return;
	}
	for (size_t i = 0; i < createdPoints_.size() - 1; i++) {
		RiverControlPoint& front = createdPoints_[i + 0];
		RiverControlPoint& back = createdPoints_[i + 1];
		Gizmo::DrawLine(front.position, back.position, Vector4(0.98f, 1.0f, 0.1f, 1.0f));
	}
}

void River::CreateBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, DxCommand* _dxCommand) {
	uint32_t totalSegments = static_cast<uint32_t>(createdPoints_.size() - 3);
	uint32_t totalSamples = static_cast<uint32_t>(totalSegments * samplePerSegment_);
	totalVertices_ = totalSamples * 2; /// 頂点数はサンプル数の2倍
	totalIndices_ = totalVertices_ * 6 / 2 - 6;

	paramBuf_.Create(_dxDevice);
	materialBuffer_.Create(_dxDevice);
	controlPointBuf_.Create(100, _dxDevice, _dxSRVHeap);
	rwVertices_.CreateUAV(totalVertices_, _dxDevice, _dxCommand, _dxSRVHeap);
	rwIndices_.CreateUAV(totalIndices_, _dxDevice, _dxCommand, _dxSRVHeap);
	isCreatedBuffers_ = true;
}

void River::SetBufferData() {
	for (size_t i = 0; i < controlPoints_.size(); i++) {
		controlPointBuf_.SetMappedData(i, controlPoints_[i]);
	}

	uint32_t totalSegments = static_cast<uint32_t>(controlPoints_.size() - 3);
	uint32_t totalSamples = static_cast<uint32_t>(totalSegments * samplePerSegment_);
	totalVertices_ = totalSamples * 2; /// 頂点数はサンプル数の2倍
	paramBuf_.SetMappedData({
		.totalSegments = totalSegments,
		.totalVertices = totalVertices_,
		.totalSamples = totalSamples,
		.samplePerSegment = static_cast<uint32_t>(samplePerSegment_)
		});
}

void River::SetMaterialData(int32_t _entityId, int32_t _texIndex) {
	materialBuffer_.SetMappedData({
		.uvTransform = {
			.position = Vector2(Time::GetTime(), 0.0f),
			.scale = Vector2::kOne,
			.rotate = 0.0f,
		},
		.baseColor = Vector4::kWhite,
		.postEffectFlags = 0,
		.entityId = _entityId,
		.baseTextureId = _texIndex,
		.normalTextureId = -1
		}
	);
}

int River::GetSamplePerSegment() const {
	return samplePerSegment_;
}

int River::GetNumControlPoint() const {
	return static_cast<int>(controlPoints_.size());
}

bool River::GetIsGenerateMeshRequest() const {
	return isGenerateMeshRequest_;
}

void River::SetIsGenerateMeshRequest(bool _request) {
	isGenerateMeshRequest_ = _request;
}

ConstantBuffer<River::Param>& River::GetParamBufRef() {
	return paramBuf_;
}

ConstantBuffer<Material>& River::GetMaterialBufRef() {
	return materialBuffer_;
}

StructuredBuffer<RiverVertex>& River::GetRwVerticesRef() {
	return rwVertices_;
}

StructuredBuffer<uint32_t>& River::GetRwIndicesRef() {
	return rwIndices_;
}

StructuredBuffer<RiverControlPoint>& River::GetControlPointBufRef() {
	return controlPointBuf_;
}

bool River::GetIsCreatedBuffers() const {
	return isCreatedBuffers_;
}

UINT River::GetTotalIndices() const {
	return totalIndices_;
}

UINT River::GetTotalVertices() const {
	return totalVertices_;
}
