#pragma once


/// ゲーム用のECSGroupにシステムを追加する関数
void GameECSGroupAddSystemFunction(class ECSGroup* _ecs, class DxManager* _dxManager, class AssetCollection* _assetCollection);

/// デバッグ用のECSGroupにシステムを追加する関数
void DebugECSGroupAddSystemFunction(class ECSGroup* _ecs, class DxManager* _dxManager, class AssetCollection* _assetCollection);

