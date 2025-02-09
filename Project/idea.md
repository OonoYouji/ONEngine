
``` c++

class Pipeline {
public:
	struct RenderingData {

	};

private:
	std::list<>
};

class MeshPipeline : public Pipeline {
	struct RenderingData {

	};

	void PushBackRenderingData(RenderingData* _data);
private:
	std::list<RenderingData*> renderingDataPtrList_;

};


class MeshRenderer {
public:

	void PushBackRenderingData(PipelineManager& _pipelineManager) {
		MeshPipeline* mesh = _pipelineManager->GetPipeline<MeshPipeline>():
		mesh->PushBackRenderingData(&data_);
	}

private:
	MeshPipeline::RenderingData data_;
};


class PipelineManager {
public:

	void DrawEntities() {

		auto commandList = dxManager->GetDxCommand()->GetCommandList();

		for(auto& pipline : pipelines_) {
			pipeline->PreDraw(commandList);
		} 

		for(auto& entity : pEntityManager->GetEntities()) {
			entity->PushBackRenderingData(this);
		}
		
		for(auto& pipeline : pipelines_) {
			pipeline->PostDraw(commandList);
		}
	}

	class EntityManager* pEntityManager_;
};



```