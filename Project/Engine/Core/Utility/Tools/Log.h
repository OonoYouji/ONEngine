#pragma once

/// directX
#include <d3d11.h>

/// std
#include <vector>
#include <string>
#include <format>
#include <tchar.h>
#include <optional>


/// @brief 最大のログバッファサイズ
static const size_t MAX_LOG_BUFFER_SIZE = 2000;


/// ////////////////////////////////////////////////
/// Console Log
/// ////////////////////////////////////////////////
namespace ONEngine {

enum class LogLevel {
	Info,
	Warning,
	Error
};

enum class LogCategory {
	Engine,
	ScriptEngine,
	Application
};

struct LogEntry {
	LogLevel level;
	LogCategory category;
	std::string message;
};

/**
 * @class Console
 * @brief エンジン内のデバッグログ、エラーログ、警告等のコンソール出力およびバッファリングを管理する静的クラス
 */
class Console final {
	/// ----- friend class ----- ///
	friend class GameFramework;

private:
	/// ===================================================
	// private : static members
	/// ===================================================

	/**
	 * @brief ログ出力システムの初期化（出力先のセットアップ等）を行います。
	 */
	static void Initialize();

	/**
	 * @brief ログ出力システムのクリーンアップを行います。
	 */
	static void Finalize();

	/**
	 * @brief メモリバッファに新しいログを追加します。
	 * @param _msg ログメッセージ
	 * @param _level ログ重要度レベル
	 * @param _category ログカテゴリ
	 */
	static void AddToBuffer(const std::string& _msg, LogLevel _level, LogCategory _category);


public:

	/**
	 * @brief デストラクタ
	 */
	~Console();

	/**
	 * @brief 一般ログ（Infoレベル）を出力します。
	 * @param _message メッセージ文字列（UTF-8）
	 * @param _category ログカテゴリ
	 */
	static void Log(const std::string& _message, LogCategory _category = LogCategory::Engine);

	/**
	 * @brief 一般ログ（Infoレベル）を出力します。ワイド文字列版。
	 * @param _message ワイドメッセージ文字列（UTF-16）
	 * @param _category ログカテゴリ
	 */
	static void Log(const std::wstring& _message, LogCategory _category = LogCategory::Engine);

	/**
	 * @brief 情報ログ（Infoレベル）を出力します。
	 * @param _message メッセージ文字列
	 * @param _category ログカテゴリ
	 */
	static void LogInfo(const std::string& _message, LogCategory _category = LogCategory::Engine);

	/**
	 * @brief エラーログ（Errorレベル）を出力します。
	 * @param _message メッセージ文字列
	 * @param _category ログカテゴリ
	 */
	static void LogError(const std::string& _message, LogCategory _category = LogCategory::Engine);

	/**
	 * @brief 警告ログ（Warningレベル）を出力します。
	 * @param _message メッセージ文字列
	 * @param _category ログカテゴリ
	 */
	static void LogWarning(const std::string& _message, LogCategory _category = LogCategory::Engine);

	/**
	 * @brief これまでにバッファに格納されたすべてのログエントリの配列を取得します。
	 * @return ログエントリを格納したstd::vectorの参照
	 */
	static const std::vector<LogEntry>& GetLogVector();

	/**
	 * @brief 格納されているログバッファをクリアします。
	 * @param _category 特定のカテゴリのみクリアしたい場合に指定します。省略時は全クリア。
	 */
	static void ClearLogBuffer(std::optional<LogCategory> _category = std::nullopt);

	/**
	 * @brief ログ出力をシャットダウンし、終了時にログをファイルへ永続化します。
	 */
	static void Shutdown();
};


/**
 * @brief ワイド文字列（std::wstring）をマルチバイト文字列（std::string）に変換します。
 * @param _wstr ソースのワイド文字列
 * @return 変換後のマルチバイト文字列
 */
std::string ConvertString(const std::wstring& _wstr);

/**
 * @brief マルチバイト文字列（std::string）をワイド文字列（std::wstring）に変換します。
 * @param _str ソースのマルチバイト文字列
 * @return 変換後のワイド文字列
 */
std::wstring ConvertString(const std::string& _str);

/**
 * @brief WindowsのTCHAR*（環境に応じた文字列）をマルチバイト文字列（std::string）に変換します。
 * @param _tstr ソースのTCHAR文字列ポインタ
 * @return 変換後のマルチバイト文字列
 */
std::string ConvertTCHARToString(const TCHAR* _tstr);

/**
 * @brief DWORD型（符号なし32bit）の数値を16進数または10進数の文字列に変換します。
 * @param _dw ソースのDWORD値
 * @return 変換後の文字列
 */
std::string ConvertString(DWORD _dw);

/**
 * @brief HRESULTエラーコードに対応するエラー内容の文字列を取得します。
 * @param _hr HRESULTエラーコード
 * @return エラー内容のマルチバイト文字列
 */
std::string HrToString(HRESULT _hr);

} /// ONEngine
