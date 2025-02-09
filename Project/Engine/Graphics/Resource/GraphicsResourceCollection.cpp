#include "GraphicsResourceCollection.h"

GraphicsResourceCollection::GraphicsResourceCollection() {}
GraphicsResourceCollection::~GraphicsResourceCollection() {}

void GraphicsResourceCollection::Initialize(DxManager* _dxManager) {
	resourceLoader_ = std::make_unique<GraphicsResourceLoader>(_dxManager);
	resourceLoader_->Initialize();

	Load("Assets/Models/cube/cube.obj", type::model);
}

void GraphicsResourceCollection::Load(const std::string& _filePath, type _type) {

	switch (_type) {
	case GraphicsResourceCollection::type::texture:
		resourceLoader_->LoadTexture(_filePath);
		break;
	case GraphicsResourceCollection::type::model:
		resourceLoader_->LoadModelObj(_filePath);
		break;
	}

}
