

下のようなjsonファイルを読み込んで、リソースの読み込みと解放を行うようなシステムを作りたい。


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


TODO: GraphicsResourceCollectionのResource管理を考える
std::unordered_map<std::string, Resource> resources;
std::vector<Resource> resources;


``` c++

MeshRenderingPipeline::Draw() {

/// mesh rendererを持っているentityをかき集める
std::list<Entity*> entities_;
for(auto& entity : entities_) {
    if(entity->HasComponent<MeshRenderer>()) {
        entities_.push_back(entity);
    }
}mes

/// transformをかき集める
std::vector<Transform*> transforms_;

for(auto& entity : entities_) {
    auto transform = entity->GetComponent<Transform>();
    if(transform) {
        transforms_.push_back(transform);
    }
}



```