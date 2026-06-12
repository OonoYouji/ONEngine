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

	MonoScriptEngine();
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

	/// インスタンスの取得
	static MonoScriptEngine& GetInstance();

	/// Monoの初期化
	void Initialize();

	/// @brief monoの終了処理
	void Finalize();

	/// CSの関数を登録
	void RegisterFunctions();

	/// CSのHotReloadを行う
	void HotReload();

	void SetEcsPtr(class EntityComponentSystem* _ecs);

	/// DLLのパスを探す
	std::optional<std::string> FindLatestDll(const std::string& _dirPath, const std::string& _baseName);

	/// C#側のリセット
	void ResetCS();

	/// @brief C#側の特定のECSGroupをクリアする
	void ClearECSGroup(const std::string& _name);

	/// @brief C++で初期化したコンポーネントデータをCS側に同期する
	void SyncInitialComponentsToCS(ECSGroup* _ecsGroup);

	/// C#側のEntityを取得
	MonoObject* GetEntityFromCS(const std::string& _ecsGroupName, int32_t _entityId);
	MonoObject* GetMonoBehaviorFromCS(const std::string& _ecsGroupName, int32_t _entityId, const std::string& _behaviorName);

	/// @brief MonoObjectの所有者であるGameEntityを取得する
	/// @param _obj 
	/// @return 
	class GameEntity* GetOwnerEntity(MonoObject* _obj);

	/// @brief GuidからGameEntityを取得する
	/// @param _guid 
	/// @return 
	class GameEntity* GetOwnerEntity(const struct Guid& _guid);

	/// @brief エンティティのGuidから所属しているECSグループ名を取得する
	/// @param _guid 
	/// @return 
	std::string GetGroupNameByEntityGuid(const struct Guid& _guid);

	/// @brief C#側のメソッドを取得する
	/// @param _namespace 名前空間
	/// @param _className クラス名
	/// @param _methodName 関数名
	/// @param _argsCount 引数の数
	/// @return 関数へのポインタ
	MonoMethod* GetMethodFromCS(const std::string& _namespace, const std::string& _className, const std::string& _methodName, int _argsCount);

	/// @brief Reload用のDomainを作成する
	/// @return 作成したDomainへのポインタ
	MonoDomain* CreateReloadDomain();
	
	void UpdateAiIntents(void* data, int count, float deltaTime, const std::string& groupName);

	/// @brief C#のBlackboardManagerにイベント完了を通知する
	/// @param entityId 対象のエンティティID
	/// @param eventName 完了したイベント名
	void NotifyEventCompleted(int32_t entityId, const std::string& eventName);

	struct NodeClassInfo {
		std::string fullName;
		bool isDecorator = false;
	};
	/// @brief BehaviorNodeを継承する全クラス情報を取得する
	std::vector<NodeClassInfo> GetBehaviorNodeClasses();

	/// @brief BehaviorDecorator/Serviceを継承する全クラス情報を取得する
	std::vector<NodeClassInfo> GetBehaviorModuleClasses();

	struct FieldInfo {
		std::string name;
		std::string typeName;
		bool isBBKey = false;
	};
	/// @brief 指定したクラスの公開フィールド情報を取得する
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
	MonoMethod* FindMethodInClassOrParents(MonoClass* _class, const char* _methodName, int _paramCount);
	MonoClassField* FindFieldRecursive(MonoClass* _class, const char* _name);
} // namespace MonoScriptEngineUtils

} /// ONEngine
