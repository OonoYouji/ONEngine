#pragma once

/// std
#include <string>
#include <optional>

/// externals
#include <jit/jit.h>
#include <metadata/assembly.h>
#include <metadata/mono-debug.h>
#include <metadata/debug-helpers.h>
#include <utils/mono-logger.h>

/// engine
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"


/// ///////////////////////////////////////////////////
/// monoを使ったC#スクリプトエンジン
/// ///////////////////////////////////////////////////
namespace ONEngine {

class ECSGroup;

class MonoScriptEngine {
private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/**
	 * @brief プライベートコンストラクタ（シングルトン）
	 */
	MonoScriptEngine();

	/**
	 * @brief デストラクタ
	 */
	~MonoScriptEngine();

	/// 代入演算子の禁止
	MonoScriptEngine(const MonoScriptEngine&) = delete;
	MonoScriptEngine& operator=(const MonoScriptEngine&) = delete;
	MonoScriptEngine(MonoScriptEngine&&) = delete;
	MonoScriptEngine& operator=(MonoScriptEngine&&) = delete;

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief シングルトンインスタンスを取得します。
	 * @return MonoScriptEngineの参照
	 */
	static MonoScriptEngine& GetInstance();

	/**
	 * @brief Monoランタイムを初期化し、アセンブリ（DLL）をロードします。
	 */
	void Initialize();

	/**
	 * @brief Monoランタイムの終了処理を行います。
	 */
	void Finalize();

	/**
	 * @brief C++の各内部コール（Internal Calls）をC#側にバインド登録します。
	 */
	void RegisterFunctions();

	/**
	 * @brief スクリプトDLLのホットリロード（再ロード）を実行します。
	 */
	void HotReload();

	/**
	 * @brief ECSシステムオブジェクトの参照を設定します。
	 * @param _ecs ECS管理クラスのポインタ
	 */
	void SetEcsPtr(class EntityComponentSystem* _ecs);

	/**
	 * @brief 指定ディレクトリ内から最新のビルド済みスクリプトDLL（パス）を検索します。
	 * @param _dirPath 検索先ディレクトリ
	 * @param _baseName DLLの基本名
	 * @return 最新のDLLファイルパス（存在しない場合はnullopt）
	 */
	std::optional<std::string> FindLatestDll(const std::string& _dirPath, const std::string& _baseName);

	/**
	 * @brief C#側のスクリプト状態（アセンブリ、インスタンス等）をリセットします。
	 */
	void ResetCS();

	/**
	 * @brief C#側にある指定された名前のECSグループデータをクリアします。
	 * @param _name 対象のECSグループ名
	 */
	void ClearECSGroup(const std::string& _name);

	/**
	 * @brief C++で生成された初期エンティティ・コンポーネントデータをC#側に同期します。
	 * @param _ecsGroup 同期対象のECSグループ
	 */
	void SyncInitialComponentsToCS(ECSGroup* _ecsGroup);

	/**
	 * @brief C#側で管理されているEntityオブジェクトを取得します。
	 * @param _ecsGroupName 所属するECSグループ名
	 * @param _entityId エンティティID
	 * @return C#のEntityオブジェクトのMonoObjectポインタ
	 */
	MonoObject* GetEntityFromCS(const std::string& _ecsGroupName, int32_t _entityId);

	/**
	 * @brief C#側で管理されている特定のMonoBehaviourオブジェクトを取得します。
	 * @param _ecsGroupName 所属するECSグループ名
	 * @param _entityId エンティティID
	 * @param _behaviorName 取得するスクリプトクラス名
	 * @return MonoBehaviourオブジェクトのMonoObjectポインタ
	 */
	MonoObject* GetMonoBehaviorFromCS(const std::string& _ecsGroupName, int32_t _entityId, const std::string& _behaviorName);

	/**
	 * @brief 指定されたMonoObject（C#オブジェクト）の所有者であるC++側のGameEntityを取得します。
	 * @param _obj C#オブジェクト
	 * @return 所有者であるGameEntityのポインタ
	 */
	class GameEntity* GetOwnerEntity(MonoObject* _obj);

	/**
	 * @brief 指定されたGuidを持つC++側のGameEntityを取得します。
	 * @param _guid 検索対象のGuid
	 * @return GameEntityのポインタ
	 */
	class GameEntity* GetOwnerEntity(const struct Guid& _guid);

	/**
	 * @brief エンティティのGuidから、そのエンティティが所属しているECSグループ名を取得します。
	 * @param _guid 検索対象のGuid
	 * @return 所属グループ名文字列
	 */
	std::string GetGroupNameByEntityGuid(const struct Guid& _guid);

	/**
	 * @brief C#（アセンブリ）からメソッドポインタを取得します。オーバーロード解決用の引数カウントを指定します。
	 * @param _namespace クラスの名前空間
	 * @param _className クラス名
	 * @param _methodName メソッド名
	 * @param _argsCount 引数の数
	 * @return MonoMethodへのポインタ
	 */
	MonoMethod* GetMethodFromCS(const std::string& _namespace, const std::string& _className, const std::string& _methodName, int _argsCount);

	/**
	 * @brief ホットリロード用の新しいMonoDomainを生成します。
	 * @return 生成されたMonoDomainのポインタ
	 */
	MonoDomain* CreateReloadDomain();
	
	/**
	 * @brief AIエージェントの意図（Intents）の更新処理をC#側に中継します。
	 * @param data AIデータの配列ポインタ
	 * @param count 要素数
	 * @param deltaTime 経過時間
	 * @param groupName 所属するグループ名
	 */
	void UpdateAiIntents(void* data, int count, float deltaTime, const std::string& groupName);

	/**
	 * @brief C#側のBlackboardManagerに指定エンティティの特定イベントが完了したことを通知します。
	 * @param entityId 対象のエンティティID
	 * @param eventName 完了したイベント名
	 */
	void NotifyEventCompleted(int32_t entityId, const std::string& eventName);

	/// @struct NodeClassInfo
	/// @brief 動作ツリーのノードクラス情報（C#側のクラス判定用）
	struct NodeClassInfo {
		std::string fullName;     ///< クラスのフルネーム
		bool isDecorator = false; ///< デコレーターであるかどうか
	};

	/**
	 * @brief BehaviorNodeを継承するすべてのC#クラスの情報を取得します（エディタ連携用）。
	 * @return ノードクラス情報の配列
	 */
	std::vector<NodeClassInfo> GetBehaviorNodeClasses();

	/**
	 * @brief BehaviorDecorator または BehaviorService を継承するすべてのC#クラスの情報を取得します。
	 * @return モジュールクラス情報の配列
	 */
	std::vector<NodeClassInfo> GetBehaviorModuleClasses();

	/// @struct FieldInfo
	/// @brief C#スクリプトクラスのフィールド公開情報
	struct FieldInfo {
		std::string name;       ///< フィールド名
		std::string typeName;   ///< フィールドの型名
		bool isBBKey = false;   ///< Blackboardキー属性がついているかどうか
	};

	/**
	 * @brief 指定されたクラス名に含まれる公開フィールド情報を取得します（エディタのリフレクションUI用）。
	 * @param className 検索対象のクラス名
	 * @return フィールド情報の配列
	 */
	std::vector<FieldInfo> GetClassFields(const std::string& className);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string currentDllPath_;

	MonoDomain* domain_;
	MonoImage* image_;
	MonoAssembly* assembly_ = nullptr;

	bool isHotReloadRequest_;
	int32_t domainReloadCounter_; /// domainのリロード回数

	/// C#側のメソッドポインタ
	MonoMethod* receiveAllBatchesMethod_ = nullptr;
	MonoMethod* getEcsGroupMethod_ = nullptr;
	MonoMethod* addEcsGroupMethod_ = nullptr;
	MonoMethod* clearEcsGroupMethod_ = nullptr;
	MonoMethod* addEntityMethod_ = nullptr;
	MonoMethod* fetchInitialDataMethod_ = nullptr;
	MonoClassField* getComponentCollectionField_ = nullptr;
	MonoMethod* updateAiIntentsMethod_ = nullptr;
	MonoMethod* notifyEventCompletedMethod_ = nullptr;

	/// SceneManager
	MonoClassField* sceneNameField_ = nullptr;

	EntityComponentSystem* pEcs_ = nullptr;

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	MonoDomain* Domain() const;
	MonoImage* Image() const;
	MonoAssembly* Assembly() const;

	void SetIsHotReloadRequest(bool _request);
	bool GetIsHotReloadRequest() const;

};


namespace MonoScriptEngineUtils {
	/**
	 * @brief 指定されたクラスおよびその親クラスを再帰的に走査し、メソッドを取得します。
	 * @param _class 対象クラスのMonoClassポインタ
	 * @param _methodName メソッド名
	 * @param _paramCount 引数の数
	 * @return メソッドのポインタ（見つからない場合はnullptr）
	 */
	MonoMethod* FindMethodInClassOrParents(MonoClass* _class, const char* _methodName, int _paramCount);

	/**
	 * @brief 指定されたクラスおよびその親クラスを再帰的に走査し、メンバ変数（フィールド）を取得します。
	 * @param _class 対象クラスのMonoClassポインタ
	 * @param _name フィールド名
	 * @return フィールドのポインタ（見つからない場合はnullptr）
	 */
	MonoClassField* FindFieldRecursive(MonoClass* _class, const char* _name);
} // namespace MonoScriptEngineUtils

} /// ONEngine
