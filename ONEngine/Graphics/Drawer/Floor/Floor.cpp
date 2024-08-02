#include <Floor.h>

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxDescriptor.h>
#include <DxResourceCreator.h>

#include <CameraManager.h>

namespace {

	struct Matrix3x3 {
		float m[3][3];
		
		Matrix3x3() = default;
		Matrix3x3(float aa, float ab, float ac, float ba, float bb, float bc, float ca, float cb, float cc) {

			this->m[0][0] = aa;
			this->m[0][1] = ab;
			this->m[0][2] = ac;

			this->m[1][0] = ba;
			this->m[1][1] = bb;
			this->m[1][2] = bc;

			this->m[2][0] = ca;
			this->m[2][1] = cb;
			this->m[2][2] = cc;

		}

		Matrix3x3 operator*(const Matrix3x3& other) const {
			Matrix3x3 result{};
			for(int r = 0; r < 3; ++r) {
				for(int c = 0; c < 3; ++c) {
					for(int i = 0; i < 3; ++i) {
						result.m[r][c] += this->m[r][i] * other.m[i][c];
					}
				}
			}

			return result;
		}
	};

	Matrix3x3 MakeScale(const Vec2& scale) {
		return Matrix3x3(
			scale.x, 0.0f, 0.0f,
			0.0f, scale.y, 0.0f,
			0.0f, 0.0f, 1.0f
		);
	}

	Matrix3x3 MakeRotate(float rotate) {
		return Matrix3x3(
			std::cos(rotate), std::sin(rotate), 0.0f,
			-std::sin(rotate), std::cos(rotate), 0.0f,
			0.0f, 0.0f, 1.0f
		);
	}

	Matrix3x3 MakeTranslate(const Vec2& translate) {
		return Matrix3x3(
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			translate.x, translate.y, 1.0f
		);
	}

	Matrix3x3 MakeAffine(const Vec2& scale, float rotate, const Vec2& translate) {
		Matrix3x3 S = MakeScale(scale);
		Matrix3x3 R = MakeRotate(rotate);
		Matrix3x3 T = MakeTranslate(translate);
		return S * R * T;
	}

	Matrix4x4 CopyMatrix(const Matrix3x3& m) {
		return {
			{m.m[0][0],m.m[0][1],m.m[0][2],0.0f},
			{m.m[1][0],m.m[1][1],m.m[1][2],0.0f},
			{m.m[2][0],m.m[2][1],m.m[2][2],0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f}
		};
	}
}


Floor::Floor() {}
Floor::~Floor() {}


/// ===================================================
/// 初期化
/// ===================================================
void Floor::Initialize() {

	pipeline_.reset(new PipelineState());
	shader_.ShaderCompile(
		L"Floor/Floor.VS.hlsl", L"vs_6_0",
		L"Floor/Floor.PS.hlsl", L"ps_6_0"
	);

	pipeline_->SetFillMode(kSolid);

	pipeline_->SetShader(&shader_);

	pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// view projection
	pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// transform
	pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);  /// uvTransform

	pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);	/// texture range
	pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);		/// texture table
	pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);			/// sampler

	pipeline_->Initialize();

	transform_.Initialize();
	transform_.scale = Vec3(100, 1, 100);
	transform_.UpdateMatrix();

	material_.SetFilePath("Floor.png");
	material_.SetTextureName("Floor");
	material_.Create();


	mesh_.AddVertex({ { -0.5f, 0.0f, -0.5f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh_.AddVertex({ { -0.5f, 0.0f,  0.5f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh_.AddVertex({ {  0.5f, 0.0f,  0.5f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh_.AddVertex({ {  0.5f, 0.0f, -0.5f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } });

	mesh_.AddIndex(0);
	mesh_.AddIndex(1);
	mesh_.AddIndex(2);
	mesh_.AddIndex(0);
	mesh_.AddIndex(2);
	mesh_.AddIndex(3);

	mesh_.Create();

	uvTransformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	uvTransformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matUvTransform_));

	scale_ = Vec2(100, 100);
	Matrix3x3 matrix = MakeAffine(scale_, rotate_, translate_);
	*matUvTransform_ = CopyMatrix(matrix);

}


/// ===================================================
/// 描画
/// ===================================================
void Floor::Draw() {
	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	ID3D12Resource* viewBuffer = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();

	pipeline_->SetPipelineState();

	ONE::DxCommon::GetInstance()->GetDxDescriptor()->SetSRVHeap(commandList);

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());
	transform_.BindTransform(commandList, 1);
	commandList->SetGraphicsRootConstantBufferView(2, uvTransformBuffer_->GetGPUVirtualAddress());
	material_.BindTexture(commandList, 3);

	mesh_.Draw(commandList, true);

}


