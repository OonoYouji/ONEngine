#pragma once

/// std
#include <list>
#include <string>

/// externals
#include <mono/jit/jit.h>


/// engine
#include "../Interface/ECSISystem.h"

/// /////////////////////////////////////////////////
/// scriptの更新を行うシステム
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ScriptUpdateSystem
 * @brief C# スクリプトコンポーネントの初期化、アセンブリメソッドバインド、毎フレームの更新処理（C#側のUpdate呼び出し）を橋渡しするシステムクラス
 */
class ScriptUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	ScriptUpdateSystem(class ECSGroup* _ecs);

	/**
	 * @brief デストラクタ
	 */
	~ScriptUpdateSystem() override;

	/**
	 * @brief エディタ停止中（非実行時）のスクリプト更新処理を行います。
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時のスクリプト（C#側の全Entity更新ロジック）を実行します。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ECSグループ内のすべてのエンティティおよびアタッチされているコンポーネント情報をC#のスクリプトエンジンに登録します。
	 */
	void AddAllEntitiesAndComponents(class ECSGroup* _ecsGroup);

	/**
	 * @brief 個々のエンティティ情報をC#スクリプトエンジンに新しく登録します。
	 */
	bool AddEntityToScript(class GameEntity* _entity);

	/**
	 * @brief C#側のEcsGroupの更新メソッドを直接呼び出します。
	 */
	void CallUpdateEcsGroup();

	/**
	 * @brief Monoアセンブリイメージおよび対象ECSグループ情報から、対応するC#側のメソッドポインタ（UpdateやAddメソッド等）を取得・準備します。
	 */
	void MakeScriptMethod(MonoImage* _image, const std::string& _ecsGroupName);

	/**
	 * @brief GC（ガベージコレクション）されないようにピン留めしている各種C#オブジェクトのGCHandleを解放します。
	 */
	void ReleaseGCHandle();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	MonoClass* monoClass_;
	uint32_t    gcHandle_;
	MonoMethod* updateEntitiesMethod_;
	MonoMethod* addEntityMethod_;
	MonoMethod* addScriptMethod_;

};



/**
 * @class DebugScriptUpdateSystem
 * @brief デバッグ用途でC#スクリプトエンジンのメソッド呼び出し状況や例外トラッキングを行うためのスクリプト更新システム拡張クラス
 */
class DebugScriptUpdateSystem : public ScriptUpdateSystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	DebugScriptUpdateSystem(class ECSGroup* _ecs);

	/**
	 * @brief デストラクタ
	 */
	~DebugScriptUpdateSystem() override;

	/**
	 * @brief エディタ非実行時のデバッグ用スクリプト更新処理を行います。
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時のデバッグ用スクリプト更新処理を実行します。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;
};

} /// ONEngine
