#include "Effect.h"

/// std
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"

using namespace ONEngine;

/**
 * @brief コンストラクタ
 */
Effect::Effect() {
	isCreateParticle_ = true;
	emitInstanceCount_ = 10;

	SetTexturePath("./Packages/Textures/Effects/Particle.png");
	SetMeshPath("./Packages/Models/primitive/frontToPlane.obj");
	SetMaxEffectCount(1000); // 初期の最大エフェクト数を設定

	SetStartColor(Color::kWhite, Color::kWhite);
	SetStartSize(Vector3::One, Vector3::One);
	SetStartRotate(Vector3::Zero, Vector3::Zero);
	SetStartSpeed(1.0f, 1.0f);

}

/**
 * @brief 新しいエフェクト粒子要素を生成（基本パラメータのみ）します。
 */
void Effect::CreateElement(const Vector3& _position, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = Vector3::One;
	element.transform.rotate = Quaternion::kIdentity;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = mainModule_.lifeLeftTime_;
	element.velocity = Vector3::Zero;
	elements_.push_back(element);
}

/**
 * @brief 新しいエフェクト粒子要素を生成（初期速度付き）します。
 */
void Effect::CreateElement(const Vector3& _position, const Vector3& _velocity, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = Vector3::One;
	element.transform.rotate = Quaternion::kIdentity;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = mainModule_.lifeLeftTime_;
	element.velocity = _velocity;
	elements_.push_back(element);
}

/**
 * @brief 新しいエフェクト粒子要素を詳細指定（サイズ・角度・速度・カラー等）で生成します。
 */
void Effect::CreateElement(const Vector3& _position, const Vector3& _scale, const Vector3& _rotate, const Vector3& _velocity, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = _scale;
	element.transform.rotate = Quaternion::FromEuler(_rotate);
	element.transform.Update();

	element.color = _color;
	element.lifeTime = mainModule_.lifeLeftTime_;
	element.velocity = _velocity;
	elements_.push_back(element);
}

/**
 * @brief 指定インデックスのエフェクト粒子要素を削除します。
 */
void Effect::RemoveElement(size_t _index) {
	if (_index < elements_.size()) {
		elements_.erase(elements_.begin() + _index);
	}
}

/**
 * @brief エフェクトのメインモジュール設定を設定します。
 */
void Effect::SetMainModule(const EffectMainModule& _module) {
	mainModule_ = _module;
}

/**
 * @brief 放出形状（EmitShape）を設定します。
 */
void Effect::SetEmitShape(const EffectEmitShape& _shape) {
	emitShape_ = _shape;
}

/**
 * @brief 放出トリガー判定方式（EmitType）を設定します。
 */
void Effect::SetEmitType(EmitType _type) {
	emitType_ = _type;
}

/**
 * @brief 最大エフェクト要素数を設定します。
 */
void Effect::SetMaxEffectCount(size_t _maxCount) {
	maxEffectCount_ = _maxCount;
	elements_.reserve(maxEffectCount_);
}

/**
 * @brief 距離放出パラメータを設定します。
 */
void Effect::SetEmitTypeDistance(float _interval, size_t _emitInstanceCount) {
	emitType_ = EmitType::Distance;
	distanceEmitData_.emitDistance = _interval;
	distanceEmitData_.emitInterval = _interval;
	emitInstanceCount_ = _emitInstanceCount;
}

/**
 * @brief 距離放出データ構造体を直接設定します。
 */
void Effect::SetEmitTypeDistance(const DistanceEmitData& _data) {
	distanceEmitData_ = _data;
}

/**
 * @brief 時間放出パラメータを設定します。
 */
void Effect::SetEmitTypeTime(const TimeEmitData& _data, size_t _emitInstanceCount) {
	emitType_ = EmitType::Time;
	timeEmitData_ = _data;
	emitInstanceCount_ = _emitInstanceCount;
}

/**
 * @brief 時間放出データ構造体を直接設定します。
 */
void Effect::SetEmitTypeTime(const TimeEmitData& _data) {
	timeEmitData_ = _data;
}

/**
 * @brief 一度に放出する個数（インスタンス数）を設定します。
 */
void Effect::SetEmitInstanceCount(size_t _emitInstanceCount) {
	emitInstanceCount_ = _emitInstanceCount;
}

/**
 * @brief 放出粒子の初期寿命（秒）を設定します。
 */
void Effect::SetLifeLeftTime(float _time) {
	mainModule_.lifeLeftTime_ = _time;
}

/**
 * @brief 粒子要素更新時のカスタムコールバックを設定します。
 */
void Effect::SetElementUpdateFunc(std::function<void(Element*)> _func) {
	elementUpdateFunc_ = _func;
}

/**
 * @brief ビルボード描画（カメラ正対）の有効・無効を設定します。
 */
void Effect::SetUseBillboard(bool _use) {
	useBillboard_ = _use;
}

/**
 * @brief 粒子の自動放出の有効・無効を設定します。
 */
void Effect::SetIsCreateParticle(bool _isCreateParticle) {
	isCreateParticle_ = _isCreateParticle;
}

/**
 * @brief カラーブレンドステート（BlendMode）を設定します。
 */
void Effect::SetBlendMode(BlendMode _blendMode) {
	blendMode_ = _blendMode;
}

/**
 * @brief 開始時の初期サイズを設定します。
 */
void Effect::SetStartSize(const Vector3& _size) {
	mainModule_.SetSizeStartData(_size);
}

/**
 * @brief 開始時の初期サイズ範囲を設定します。
 */
void Effect::SetStartSize(const Vector3& _size1, const Vector3& _size2) {
	mainModule_.SetSizeStartData(std::make_pair(_size1, _size2));
}

/**
 * @brief 開始時の初期回転角度を設定します。
 */
void Effect::SetStartRotate(const Vector3& _rotate) {
	mainModule_.SetRotateStartData(_rotate);
}

/**
 * @brief 開始時の初期回転角度範囲を設定します。
 */
void Effect::SetStartRotate(const Vector3& _rotate1, const Vector3& _rotate2) {
	mainModule_.SetRotateStartData(std::make_pair(_rotate1, _rotate2));
}

/**
 * @brief 開始時の初期カラーを設定します。
 */
void Effect::SetStartColor(const Color& _color) {
	mainModule_.SetColorStartData(_color);
}

/**
 * @brief 開始時の初期カラー範囲を設定します。
 */
void Effect::SetStartColor(const Color& _color1, const Color& _color2) {
	mainModule_.SetColorStartData(std::make_pair(_color1, _color2));
}

/**
 * @brief 開始時の初期速度を設定します。
 */
void Effect::SetStartSpeed(float _speed) {
	mainModule_.SetSpeedStartData(_speed);
}

/**
 * @brief 開始時の初期速度範囲を設定します。
 */
void Effect::SetStartSpeed(float _speed1, float _speed2) {
	mainModule_.SetSpeedStartData(std::make_pair(_speed1, _speed2));
}

/**
 * @brief 放出形状を「球体（Sphere）」に設定します。
 */
void Effect::SetEmitShape(const Vector3& _center, float _radius) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Sphere);
	emitShape_.SetSphere(_center, _radius);
}

/**
 * @brief 放出形状を「箱（Cube）」に設定します。
 */
void Effect::SetEmitShape(const Vector3& _center, const Vector3& _size) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Cube);
	emitShape_.SetCube(_center, _size);
}

/**
 * @brief 放出形状を「コーン（Cone）」に設定します。
 */
void Effect::SetEmitShape(const Vector3& _apex, float _angle, float _radius, float _height) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Cone);
	emitShape_.SetCone(_apex, _angle, _radius, _height);
}

/**
 * @brief 粒子の自動放出が有効かを判定します。
 */
bool Effect::IsCreateParticle() const {
	return isCreateParticle_;
}

/**
 * @brief 最大エフェクト要素数を取得します。
 */
size_t Effect::GetMaxEffectCount() const {
	return maxEffectCount_;
}

/**
 * @brief 粒子メッシュアセットパスを取得します。
 */
const std::string& Effect::GetMeshPath() const {
	return meshPath_;
}

/**
 * @brief 粒子テクスチャアセットパスを取得します。
 */
const std::string& Effect::GetTexturePath() const {
	return texturePath_;
}

/**
 * @brief 現在アクティブな粒子要素の配列を取得します。
 */
const std::vector<Effect::Element>& Effect::GetElements() const {
	return elements_;
}

/**
 * @brief 設定されているブレンドモードを取得します。
 */
Effect::BlendMode Effect::GetBlendMode() const {
	return blendMode_;
}

/**
 * @brief メインモジュールのポインタを取得します。
 */
EffectMainModule* Effect::GetMainModule() {
	return &mainModule_;
}

/**
 * @brief メインモジュールの読み取り専用参照を取得します。
 */
const EffectMainModule& Effect::GetMainModule() const {
	return mainModule_;
}

/**
 * @brief 放出形状モジュールのポインタを取得します。
 */
EffectEmitShape* Effect::GetEmitShape() {
	return &emitShape_;
}

/**
 * @brief 放出形状モジュールの読み取り専用参照を取得します。
 */
const EffectEmitShape& Effect::GetEmitShape() const {
	return emitShape_;
}

/**
 * @brief 放出トリガー判定方式（EmitType）を取得します。
 */
int Effect::GetEmitType() const {
	return static_cast<int>(emitType_);
}

/**
 * @brief 距離放出パラメータの読み取り専用参照を取得します。
 */
const Effect::DistanceEmitData& Effect::GetDistanceEmitData() const {
	return distanceEmitData_;
}

/**
 * @brief 時間放出パラメータの読み取り専用参照を取得します。
 */
const Effect::TimeEmitData& Effect::GetTimeEmitData() const {
	return timeEmitData_;
}

/**
 * @brief 一度放出する個数（インスタンス数）を取得します。
 */
size_t Effect::GetEmitInstanceCount() const {
	return emitInstanceCount_;
}

/**
 * @brief エディタ用：Effectコンポーネントのデバッグ表示（Gui描画等）処理を行います。
 */
void ComponentDebug::EffectDebug(Effect* _effect) {
	if (!_effect) {
		return;
	}

	ImGui::Indent(4);

	if (ImGui::CollapsingHeader("Base")) {

		/// ---------------------------------------------------------
		/// テクスチャとメッシュのパスを設定
		/// ---------------------------------------------------------

		std::string texturePath = _effect->GetTexturePath();
		std::string meshPath = _effect->GetMeshPath();

		ImGui::Text("mesh path");
		Editor::ImMathf::InputText("##mesh path", &meshPath, ImGuiInputTextFlags_EnterReturnsTrue);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

				/// ペイロードが存在する場合
				if (payload->Data) {
					const char* droppedPath = static_cast<const char*>(payload->Data);
					std::string path = std::string(droppedPath);

					/// メッシュのパスが有効な形式か確認
					if (path.find(".obj") != std::string::npos
						|| path.find(".gltf") != std::string::npos) {
						_effect->SetMeshPath(path);

						Console::Log(std::format("Mesh path set to: {}", path));
					} else {
						Console::LogError("Invalid mesh format. Please use .obj or .gltf.");
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		/// texture path
		ImGui::Text("texture path");
		Editor::ImMathf::InputText("##texture path", &texturePath, ImGuiInputTextFlags_EnterReturnsTrue);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

				/// ペイロードが存在する場合
				if (payload->Data) {
					const char* droppedPath = static_cast<const char*>(payload->Data);
					std::string path = std::string(droppedPath);

					/// テクスチャのパスが有効な形式か確認
					if (path.find(".png") != std::string::npos
						|| path.find(".jpg") != std::string::npos
						|| path.find(".jpeg") != std::string::npos) {
						_effect->SetTexturePath(path);

						Console::Log(std::format("Texture path set to: {}", path));
					} else {
						Console::LogError("Invalid texture format. Please use .png, .jpg, or .jpeg.");
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

	}

	/// main module 
	if (ImGui::CollapsingHeader("main module")) {
		EffectMainModule* mainModule = _effect->GetMainModule();
		if (!mainModule) {
			ImGui::Text("no main module");
		} else {

			/// param get
			std::pair<float, float> speed = mainModule->GetSpeedStartData();
			std::pair<Vector3, Vector3> size = mainModule->GetSizeStartData();
			std::pair<Vector3, Vector3> rotate = mainModule->GetRotateStartData();
			std::pair<Color, Color> color = mainModule->GetColorStartData();

			/// スピードの編集
			ImGui::DragFloat("first speed", &speed.first, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat("second speed", &speed.second, 0.1f, 0.0f, FLT_MAX);
			ImGui::Spacing();

			/// サイズの編集
			ImGui::DragFloat3("first size", &size.first.x, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat3("second size", &size.second.x, 0.1f, 0.0f, FLT_MAX);
			ImGui::Spacing();

			/// 回転の編集
			ImGui::DragFloat3("first rotate", &rotate.first.x, 0.1f);
			ImGui::DragFloat3("second rotate", &rotate.second.x, 0.1f);
			ImGui::Spacing();

			/// 色の編集
			ImGui::ColorEdit4("first color", &color.first.r);
			ImGui::ColorEdit4("second color", &color.second.r);


			/// 編集したら値のセット
			mainModule->SetSpeedStartData(speed);
			mainModule->SetSizeStartData(size);
			mainModule->SetRotateStartData(rotate);
			mainModule->SetColorStartData(color);


		}

	}

	/// emit shape
	if (ImGui::CollapsingHeader("shape")) {
		EffectEmitShape* emitShape = _effect->GetEmitShape();
		if (emitShape) {

			/// 形状の選択
			const char* shapeTypes[] = { "Sphere", "Cube", "Cone" };
			int shapeType = static_cast<int>(emitShape->GetType());
			if (ImGui::Combo("shape type", &shapeType, shapeTypes, IM_ARRAYSIZE(shapeTypes))) {
				emitShape->SetShapeType(static_cast<EffectEmitShape::ShapeType>(shapeType));
			}
			ImGui::Spacing();

			/// 形状ごとのパラメータの編集
			switch (emitShape->GetType()) {
			case EffectEmitShape::ShapeType::Sphere:
			{
				Sphere sphere = emitShape->GetSphere();
				ImGui::DragFloat3("center", &sphere.center.x, 0.1f);
				ImGui::DragFloat("radius", &sphere.radius, 0.1f, 0.0f, FLT_MAX);
				emitShape->SetSphere(sphere);
				break;
			}
			case EffectEmitShape::ShapeType::Cube:
			{
				Cube cube = emitShape->GetCube();
				ImGui::DragFloat3("center", &cube.center.x, 0.1f);
				ImGui::DragFloat3("size", &cube.size.x, 0.1f, 0.0f, FLT_MAX);
				emitShape->SetCube(cube);
				break;
			}
			case EffectEmitShape::ShapeType::Cone:
			{
				Cone cone = emitShape->GetCone();
				ImGui::DragFloat3("apex", &cone.center.x, 0.1f);
				ImGui::DragFloat("angle", &cone.angle, 0.1f, 0.0f, 180.0f);
				ImGui::DragFloat("radius", &cone.radius, 0.1f, 0.0f, FLT_MAX);
				ImGui::DragFloat("height", &cone.height, 0.1f, 0.0f, FLT_MAX);
				emitShape->SetCone(cone);
				break;
			}
			default:
				ImGui::Text("Unknown shape type");
				break;
			}


		}

	}

	ImGui::Unindent(4);
}
