#include "TerrainVertexEditorCompute.h"

TerrainVertexEditorCompute::TerrainVertexEditorCompute() {}
TerrainVertexEditorCompute::~TerrainVertexEditorCompute() {}

void TerrainVertexEditorCompute::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// Shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/TerrainVertexEditor.cs.hlsl", L"cs_6_6", Shader::Type::cs);

	}

	{	/// Buffer

	}
}

void TerrainVertexEditorCompute::Draw(DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection) {

}
