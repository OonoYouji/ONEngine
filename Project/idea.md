

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


