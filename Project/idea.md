

���̂悤��json�t�@�C����ǂݍ���ŁA���\�[�X�̓ǂݍ��݂Ɖ�����s���悤�ȃV�X�e������肽���B


``` json
{
    "load": {
        "white2x2.png": "texture",
        "uvChecker.png": "texture",
        "cube.obj": "model"
    },
    "unload": {
        "white2x2.png": "texture"
    }
}

```


TODO: GraphicsResourceCollection��Resource�Ǘ����l����
std::unordered_map<std::string, Resource> resources;
std::vector<Resource> resources;


``` c++

MeshRenderingPipeline::Draw() {

/// mesh renderer�������Ă���entity�������W�߂�
std::list<Entity*> entities_;
for(auto& entity : entities_) {
    if(entity->HasComponent<MeshRenderer>()) {
        entities_.push_back(entity);
    }
}mes

/// transform�������W�߂�
std::vector<Transform*> transforms_;

for(auto& entity : entities_) {
    auto transform = entity->GetComponent<Transform>();
    if(transform) {
        transforms_.push_back(transform);
    }
}



```