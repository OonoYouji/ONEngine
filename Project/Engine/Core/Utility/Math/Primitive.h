#pragma once

/// std
#include <array>

/// engine
#include "Vector3.h"

/// @brief 球
namespace ONEngine {

/**
 * @struct Sphere
 * @brief 3D空間の球体を表現する構造体
 */
struct Sphere {
	Vector3 center; ///< 中心座標
	float radius;   ///< 半径
};

/// @brief 箱
/**
 * @struct Cube
 * @brief 3D空間のAABB（軸平行境界ボックス）または立方体を表現する構造体
 */
struct Cube {
	Vector3 center; ///< 中心座標
	Vector3 size;   ///< 各軸方向のサイズ（幅、高さ、奥行き）
};

/// @brief 線分
/**
 * @struct Line
 * @brief 3D空間の線分（開始点と終了点）を表現する構造体
 */
struct Line {
	Vector3 start; ///< 開始点座標
	Vector3 end;   ///< 終了点座標
};

/// @brief レイ(光)
/**
 * @struct Ray
 * @brief 3D空間の光線（位置と方向）を表現する構造体
 */
struct Ray {
	Vector3 origin;    ///< 始点（原点）
	Vector3 direction; ///< 正規化された方向ベクトル
};

/// @brief 平面
/**
 * @struct Plane
 * @brief 3D空間の無限平面を表現する構造体
 */
struct Plane {
	/// @brief 面の法線
	Vector3 normal; ///< 平面の法線ベクトル（面から外を向く正規化ベクトル）
	/// @brief 面から原点までの距離
	float d;        ///< 平面方程式 ax + by + cz + d = 0 における d （原点から平面までの符号付き距離）
};

/// @brief 視錐台
/**
 * @struct Frustum
 * @brief 視錐台を表現し、6つの平面で構成される構造体。クリッピングやカリング処理に使用されます。
 */
struct Frustum {
	static constexpr size_t kPlaneCount = 6; ///< 平面の総数
	std::array<Plane, kPlaneCount> planes;   ///< 視錐台を構成する平面の配列
};


/// @brief 円錐
/**
 * @struct Cone
 * @brief 円錐を表現する構造体
 */
struct Cone {
	Vector3 center; ///< 底面の中心座標
	float angle;    ///< 円錐の開き角（ラジアン）
	float radius;   ///< 底面の半径
	float height;   ///< 円錐の高さ
};

/// ///////////////////////////////////////////////////
/// 以下より上の構造体のJson変換関数
/// ///////////////////////////////////////////////////

/**
 * @brief jsonオブジェクトからSphere構造体へのデシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _s 変換先のSphere構造体の参照
 */
void from_json(const nlohmann::json& _j, Sphere& _s);
/**
 * @brief Sphere構造体からjsonオブジェクトへのシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _s 変換元のSphere構造体
 */
void to_json(nlohmann::json& _j, const Sphere& _s);

/**
 * @brief jsonオブジェクトからCube構造体へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& _j, Cube& _c);
void to_json(nlohmann::json& _j, const Cube& _c);

/**
 * @brief jsonオブジェクトからLine構造体へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& _j, Line& _l);
void to_json(nlohmann::json& _j, const Line& _l);

/**
 * @brief jsonオブジェクトからRay構造体へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& _j, Ray& _r);
void to_json(nlohmann::json& _j, const Ray& _r);

/**
 * @brief jsonオブジェクトからPlane構造体へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& _j, Plane& _p);
void to_json(nlohmann::json& _j, const Plane& _p);

/**
 * @brief jsonオブジェクトからFrustum構造体へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& _j, Frustum& _f);
void to_json(nlohmann::json& _j, const Frustum& _f);

/**
 * @brief jsonオブジェクトからCone構造体へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& _j, Cone& _c);
void to_json(nlohmann::json& _j, const Cone& _c);

} /// ONEngine
