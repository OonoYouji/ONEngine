#include "RendererCollection.h"

#include "../Mesh/MeshRenderingPipeline.h"
#include "../Primitive/Line2DRenderingPipeline.h"


RenderingPipelineCollection::RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) 
	: shaderCompiler_(_shaderCompiler), dxDevice_(_dxDevice) {}

RenderingPipelineCollection::~RenderingPipelineCollection() {}

void RenderingPipelineCollection::Initialize() {

	/// generate rendering pipeline
	GenerateRenderingPipeline<Line2DRenderingPipeline>();
	GenerateRenderingPipeline<MeshRenderingPipeline>();

}
