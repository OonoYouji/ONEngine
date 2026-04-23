# アセットmetaファイルの構成

### 共通設定
- **guid** 64bit整数
- **name**
- **hash** 再インポート用
- **importedPath** 
- **dependencies** 依存しているファイルのGUIDをまとめたやつ。
- **path**
- **type** Texture/Audio/Model

### Texture
- **format** 読み込み時のformat
- **colorSpace** sRGB / Linear


### Mesh
- **scale** モデルの拡縮率
- **lod** 
    - { "level", 0, "path", lod0.gltf }
    - { "level", 1, "path", lod1.obj }

### Material
- **shader** 使用するshaderのguid (HLSLではなく、pipelineの方)
- **parameters**
    - albedo
    - metallic
    - roughness
- **textures** 
    - albedo texture guid
    - normal texture guid
    - metallic roughness guid

### Shader
- **stage** vs/ps/cs/as/ms
- **entryPoint** "main"
- **profile** 6_6
- **compile option**
    - "debug"
    - "optimization"

### audio
- **format** wav/mp3


### prefab
- **component data** 
    - transform
    - camera
    - light
    - collider


### Load phase
```
AssetManager::Load(guid)
    ↓
① キャッシュ確認
    ↓
② Meta読込（共通）
    ↓
③ 依存解決（再帰）
    ↓
④ Loader選択（type）
    ↓
⑤ アセット別Load処理
    ↓
⑥ キャッシュ登録
    ↓
⑦ Handle返却
```