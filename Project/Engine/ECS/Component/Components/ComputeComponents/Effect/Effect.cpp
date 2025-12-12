#include "Effect.h"

/// std
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"

using namespace ONEngine;

Effect::Effect() {
	isCreateParticle_ = true;
	emitInstanceCount_ = 10;

	SetTexturePath("./Packages/Textures/Effects/Particle.png");
	SetMeshPath("./Packages/Models/primitive/frontToPlane.obj");
	SetMaxEffectCount(1000); // 初期の最大エフェクト数を設定

	SetStartColor(Color::kWhite, Color::kWhite);
	SetStartSize(Vector3::kOne, Vector3::kOne);
	SetStartRotate(Vector3::kZero, Vector3::kZero);
	SetStartSpeed(1.0f, 1.0f);

}

void Effect::CreateElement(const Vector3& _position, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = Vector3::kOne;
	element.transform.rotate = Quaternion::kIdentity;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = mainModule_.lifeLeftTime_;
	element.velocity = Vector3::kZero;
	elements_.push_back(element);
}

void Effect::CreateElement(const Vector3& _position, const Vector3& _velocity, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = Vector3::kOne;
	element.transform.rotate = Quaternion::kIdentity;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = mainModule_.lifeLeftTime_;
	element.velocity = _velocity;
	elements_.push_back(element);
}

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

void Effect::RemoveElement(size_t _index) {
	if (_index < elements_.size()) {
		elements_.erase(elements_.begin() + _index);
	}
}

void Effect::SetMainModule(const EffectMainModule& _module) {
	mainModule_ = _module;
}

void Effect::SetEmitShape(const EffectEmitShape& _shape) {
	emitShape_ = _shape;
}

void Effect::SetEmitType(EmitType _type) {
	emitType_ = _type;
}

void Effect::SetMaxEffectCount(size_t _maxCount) {
	maxEffectCount_ = _maxCount;
	elements_.reserve(maxEffectCount_);
}

void Effect::SetEmitTypeDistance(float _interval, size_t _emitInstanceCount) {
	emitType_ = EmitType::Distance;
	distanceEmitData_.emitDistance = _interval;
	distanceEmitData_.emitInterval = _interval;
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetEmitTypeDistance(const DistanceEmitData& _data) {
	distanceEmitData_ = _data;
}

void Effect::SetEmitTypeTime(const TimeEmitData& _data, size_t _emitInstanceCount) {
	emitType_ = EmitType::Time;
	timeEmitData_ = _data;
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetEmitTypeTime(const TimeEmitData& _data) {
	timeEmitData_ = _data;
}

void Effect::SetEmitInstanceCount(size_t _emitInstanceCount) {
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetLifeLeftTime(float _time) {
	mainModule_.lifeLeftTime_ = _time;
}

void Effect::SetElementUpdateFunc(std::function<void(Element*)> _func) {
	elementUpdateFunc_ = _func;
}

void Effect::SetUseBillboard(bool _use) {
	useBillboard_ = _use;
}

void Effect::SetIsCreateParticle(bool _isCreateParticle) {
	isCreateParticle_ = _isCreateParticle;
}

void Effect::SetBlendMode(BlendMode _blendMode) {
	blendMode_ = _blendMode;
}

void Effect::SetStartSize(const Vector3& _size) {
	mainModule_.SetSizeStartData(_size);
}

void Effect::SetStartSize(const Vector3& _size1, const Vector3& _size2) {
	mainModule_.SetSizeStartData(std::make_pair(_size1, _size2));
}

void Effect::SetStartRotate(const Vector3& _rotate) {
	mainModule_.SetRotateStartData(_rotate);
}

void Effect::SetStartRotate(const Vector3& _rotate1, const Vector3& _rotate2) {
	mainModule_.SetRotateStartData(std::make_pair(_rotate1, _rotate2));
}

void Effect::SetStartColor(const Color& _color) {
	mainModule_.SetColorStartData(_color);
}

void Effect::SetStartColor(const Color& _color1, const Color& _color2) {
	mainModule_.SetColorStartData(std::make_pair(_color1, _color2));
}

void Effect::SetStartSpeed(float _speed) {
	mainModule_.SetSpeedStartData(_speed);
}

void Effect::SetStartSpeed(float _speed1, float _speed2) {
	mainModule_.SetSpeedStartData(std::make_pair(_speed1, _speed2));
}

void Effect::SetEmitShape(const Vector3& _center, float _radius) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Sphere);
	emitShape_.SetSphere(_center, _radius);
}

void Effect::SetEmitShape(const Vector3& _center, const Vector3& _size) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Cube);
	emitShape_.SetCube(_center, _size);
}

void Effect::SetEmitShape(const Vector3& _apex, float _angle, float _radius, float _height) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Cone);
	emitShape_.SetCone(_apex, _angle, _radius, _height);
}

bool Effect::IsCreateParticle() const {
	return isCreateParticle_;
}

size_t Effect::GetMaxEffectCount() const {
	return maxEffectCount_;
}

const std::string& Effect::GetMeshPath() const {
	return meshPath_;
}

const std::string& Effect::GetTexturePath() const {
	return texturePath_;
}

const std::vector<Effect::Element>& Effect::GetElements() const {
	return elements_;
}

Effect::BlendMode Effect::GetBlendMode() const {
	return blendMode_;
}

EffectMainModule* Effect::GetMainModule() {
	return &mainModule_;
}

const EffectMainModule& Effect::GetMainModule() const {
	return mainModule_;
}

EffectEmitShape* Effect::GetEmitShape() {
	return &emitShape_;
}

const EffectEmitShape& Effect::GetEmitShape() const {
	return emitShape_;
}

int Effect::GetEmitType() const {
	return static_cast<int>(emitType_);
}

const Effect::DistanceEmitData& Effect::GetDistanceEmitData() const {
	return distanceEmitData_;
}

const Effect::TimeEmitData& Effect::GetTimeEmitData() const {
	return timeEmitData_;
}

size_t Effect::GetEmitInstanceCount() const {
	return emitInstanceCount_;
}

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
