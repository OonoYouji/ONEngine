#pragma once

namespace ONEngine {

/**
 * @brief コンポーネント関連のC++内部関数をC#（Mono）に登録します。
 */
void AddComponentInternalCalls();

/**
 * @brief エンティティ関連およびECSグループ関連のC++内部関数をC#（Mono）に登録します。
 */
void AddEntityInternalCalls();

/**
 * @brief 入力（キーボード、マウス、ゲームパッド）関連のC++内部関数をC#（Mono）に登録します。
 */
void AddInputInternalCalls();

/**
 * @brief シーン遷移関連のC++内部関数をC#（Mono）に登録します。
 */
void AddSceneInternalCalls();

/**
 * @brief Gizmo描画（デバッグ描画ラインなど）関連のC++内部関数をC#（Mono）に登録します。
 */
void AddGizmoInternalCalls();

/**
 * @brief ウィンドウ管理関連のC++内部関数をC#（Mono）に登録します。
 */
void AddWindowInternalCalls();

/**
 * @brief アニメーション再生・制御関連のC++内部関数をC#（Mono）に登録します。
 * @note この関数の実体は AnimationInternalCalls.cpp にあります。
 */
void AddAnimationInternalCalls();

} /// namespace ONEngine