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

using namespace ONEngine;


/**
 * @brief Catmull-Rom スプライン曲線上の点を計算補間します。
 */
RiverControlPoint ONEngine::CatmullRom(const RiverControlPoint& _p0, const RiverControlPoint& _p1, const RiverControlPoint& _p2, const RiverControlPoint& _p3, float _t) {
	RiverControlPoint result;
	result.position = Math::CatmullRomPosition(_p0.position, _p1.position, _p2.position, _p3.position, _t);
	result.width = _p1.width * (1.0f - _t) + _p2.width * _t;
	return result;
}

/**
 * @brief 制御点配列から指定サンプリング密度でスプラインに沿った補間点配列を生成します。
 */
std::vector<RiverControlPoint> ONEngine::SampleRiverSpline(const std::vector<RiverControlPoint>& _points, int _samplePerSegment) {
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



/**
 * @brief コンストラクタ
 */
River::River() : samplePerSegment_(10), isCreatedBuffers_(false), isGenerateMeshRequest_(false) {};

/**
 * @brief デストラクタ
 */
River::~River() = default;

/**
 * @brief エディタ用：川のパラメータや制御点の追加・削除など編集用Guiの描画処理を行います。
 */
void River::Edit(EntityComponentSystem* /*_ecs*/) {
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

		///// 操作対象の行列
		//Matrix4x4 targetMatrix = Matrix4x4::MakeTranslate(point.position);
		///// カメラの取得
		//CameraComponent* camera = _ecs->GetECSGroup("Debug")->GetMainCamera();
		//if (camera) {
		//	ImGuizmo::Manipulate(
		//		&camera->GetViewMatrix().m[0][0],
		//		&camera->GetProjectionMatrix().m[0][0],
		//		ImGuizmo::OPERATION::TRANSLATE,
		//		ImGuizmo::MODE::WORLD,
		//		&targetMatrix.m[0][0]
		//	);

		//	if (!ImGuizmo::IsOver()) {
		//		continue;
		//	}

		//	/// 操作したならbreak
		//	if (ImGuizmo::IsUsing()) {
		//		/// 行列をSRTに分解、エンティティに適応
		//		float translation[3], rotation[3], scale[3];
		//		ImGuizmo::DecomposeMatrixToComponents(&targetMatrix.m[0][0], translation, rotation, scale);

		//		Vector3 translationV = Vector3(translation[0], translation[1], translation[2]);
		//		point.position = translationV;
		//		break;
		//	}
		//}
	}


	/// imgui edit
	if (ImGui::Button("Add")) {
		RiverControlPoint add = { Vector3::Zero, 2.0f };
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

/**
 * @brief 川の制御点設定を外部JSONファイルへ保存します。
 */
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

/**
 * @brief 外部JSONファイルから川の制御点設定を読み込みます。
 */
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

/**
 * @brief シーン上にスプラインのパス線分や制御点用ギズモ（エディタ用）を描画します。
 */
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

/**
 * @brief GPU側で川メッシュを格納・変形するためのUAV/CBVバッファを生成します。
 */
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

/**
 * @brief 制御点バッファ等へCPU側のデータをアップロード・転送します。
 */
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

/**
 * @brief 川用の水面マテリアルパラメータ（描画時のテクスチャインデックスやOwner Entity情報）を設定します。
 */
void River::SetMaterialData(int32_t _entityId, int32_t _texIndex) {
	materialBuffer_.SetMappedData({
		.uvTransform = {
			.position = Vector2(Time::GetTime(), 0.0f),
			.scale = Vector2::One,
			.rotate = 0.0f,
		},
		.baseColor = Vector4::White,
		.postEffectFlags = 0,
		.entityId = _entityId,
		.baseTextureId = _texIndex,
		.normalTextureId = -1
		}
	);
}

/**
 * @brief 計算シェーダ等で変形したバッファを、描画（VBV/IBV経由でのレンダリング）に適したステートへ移行するバリアを生成します。
 */
void River::CreateRenderingBarriers(DxCommand* _dxCommand) {
	rwVertices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		_dxCommand
	);

	rwIndices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		_dxCommand
	);
}

/**
 * @brief 描画用に移行したリソースステートを、計算用の元ステート（UAV）へ復元するバリアを生成します。
 */
void River::RestoreResourceBarriers(DxCommand* _dxCommand) {
	rwVertices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);

	rwIndices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);
}

/**
 * @brief レンダリング呼び出し用の頂点バッファビュー（D3D12_VERTEX_BUFFER_VIEW）を取得します。
 */
D3D12_VERTEX_BUFFER_VIEW River::CreateVBV() {
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = rwVertices_.GetResource().Get()->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(RiverVertex);
	vbv.SizeInBytes = sizeof(RiverVertex) * GetTotalVertices();
	return vbv;
}

/**
 * @brief レンダリング呼び出し用のインデックスバッファビュー（D3D12_INDEX_BUFFER_VIEW）を取得します。
 */
D3D12_INDEX_BUFFER_VIEW River::CreateIBV() {
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = rwIndices_.GetResource().Get()->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R32_UINT;
	ibv.SizeInBytes = static_cast<UINT>(sizeof(uint32_t)) * GetTotalIndices();
	return ibv;
}

/**
 * @brief セグメント辺あたりの補間サンプル数を取得します。
 */
int River::GetSamplePerSegment() const {
	return samplePerSegment_;
}

/**
 * @brief 現在登録されている制御点（コントロールポイント）の数を取得します。
 */
int River::GetNumControlPoint() const {
	return static_cast<int>(controlPoints_.size());
}

/**
 * @brief 川メッシュの動的再生成要求フラグを取得します。
 */
bool River::GetIsGenerateMeshRequest() const {
	return isGenerateMeshRequest_;
}

/**
 * @brief 川メッシュの動的再生成要求フラグを設定します。
 */
void River::SetIsGenerateMeshRequest(bool _request) {
	isGenerateMeshRequest_ = _request;
}

/**
 * @brief 定数バッファ（Param）を取得します。
 */
const ConstantBuffer<River::Param>& River::GetParamBuffer() const {
	return paramBuf_;
}

/**
 * @brief マテリアル定数バッファ（GPUMaterial）を取得します。
 */
const ConstantBuffer<GPUMaterial>& River::GetMaterialBuffer() const {
	return materialBuffer_;
}

/**
 * @brief 生成された川頂点を保持する構造化バッファの読み取り専用参照を取得します。
 */
const StructuredBuffer<RiverVertex>& River::GetRwVertices() const {
	return rwVertices_;
}

/**
 * @brief 生成された川インデックスを保持する構造化バッファの読み取り専用参照を取得します。
 */
const StructuredBuffer<uint32_t>& River::GetRwIndices() const {
	return rwIndices_;
}

/**
 * @brief 制御点配列のGPU構造化バッファを取得します。
 */
const StructuredBuffer<RiverControlPoint>& River::GetControlPointBuffer() const {
	return controlPointBuf_;
}

/**
 * @brief 各バッファが生成済みであるかを取得します。
 */
bool River::GetIsCreatedBuffers() const {
	return isCreatedBuffers_;
}

/**
 * @brief 川インデックスバッファの総要素数を取得します。
 */
UINT River::GetTotalIndices() const {
	return totalIndices_;
}

/**
 * @brief 川頂点バッファの総要素数を取得します。
 */
UINT River::GetTotalVertices() const {
	return totalVertices_;
}
