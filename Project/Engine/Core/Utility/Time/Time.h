#pragma once


/// /////////////////////////////////////////////////////////////
/// Time
/// /////////////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Time
 * @brief ゲームの実行時間、デルタタイム（フレーム間経過時間）、タイムスケールを管理するクラス
 */
class Time {
	friend class GameFramework;
private:
	/**
	 * @brief プライベートコンストラクタ（静的ユーティリティクラス）
	 */
	Time();

	/**
	 * @brief デストラクタ
	 */
	~Time() ;

	/**
	 * @brief 時間計測システムの初期化を行います。
	 */
	static void Initialize();

	/**
	 * @brief 時間計測システムの終了処理を行います。
	 */
	static void Finalize();

	/**
	 * @brief 毎フレーム呼び出され、経過時間とデルタタイムを更新します。
	 */
	static void Update();

public:
	/// =======================================================
	/// public : methods
	/// =======================================================

	/**
	 * @brief 蓄積されたゲーム経過時間をリセットします。
	 */
	static void ResetTime();

	/**
	 * @brief ゲーム開始からの累計経過時間（秒）を取得します（タイムスケールの影響を受けます）。
	 * @return 累計経過時間（秒）
	 */
	static float GetTime();

	/**
	 * @brief 前フレームからの経過時間（秒）を取得します（タイムスケールの影響を受けます）。
	 * @return デルタタイム（秒）
	 */
	static float DeltaTime();

	/**
	 * @brief タイムスケールによる影響を受けない、実際のフレーム間経過時間（秒）を取得します。
	 * @return 補正前のデルタタイム（秒）
	 */
	static float UnscaledDeltaTime();

	/**
	 * @brief 現在の時間進行スケール率を取得します（1.0が通常速度、0.5がスロー、2.0が倍速など）。
	 * @return タイムスケール値
	 */
	static float TimeScale();

	/**
	 * @brief 時間進行スケール率を設定します。
	 * @param _timeScale 設定するタイムスケール倍率値
	 */
	static void SetTimeScale(float _timeScale);
};



} /// ONEngine
