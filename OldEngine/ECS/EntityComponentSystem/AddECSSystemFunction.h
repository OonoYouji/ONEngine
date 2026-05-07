#pragma once


namespace ONEngine {
class DxManager;
class ECSGroup;
}

namespace ONEngine::Asset {
class AssetCollection;
}


namespace ONEngine {

/// ゲーム用のECSGroupにシステムを追加する関数
void GameECSGroupAddSystemFunction(ECSGroup* _ecs, DxManager* _dxm, Asset::AssetCollection* _assetCollection);

/// デバッグ用のECSGroupにシステムを追加する関数
void DebugECSGroupAddSystemFunction(ECSGroup* _ecs, DxManager* _dxm, Asset::AssetCollection* _assetCollection);

} /// namespace ONEngine