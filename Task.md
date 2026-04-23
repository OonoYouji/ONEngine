# やりたいことリスト
- ECS
    - Componentの継承で管理をやめる
    - SystemでRenderer.Queueに追加まで行う
- Rendering
    - ShaderReflectionの実装
    - TextureBindlessの実装
    - Pipelineの外部ファイル化
- Asset
    <!-- - Shaderのアセット化 -->
    - Materialから使用するShaderの選択
    - Sceneのデータ構造の変更
        - object(entity)ごとにファイルを生成
    - metaファイルにアセットごとの設定を組み込む
        <!-- - metaファイルのJSON化 -->
        - metaファイルのエディター
- Animation
    - GPUAnimationの実装
- Editor
    - Engineへの依存度を少なくする
        - Coreなクラスへの依存はよいが、アセットがないとクラッシュするのは良くない
    - Release時にビルドしないようにする
    
    