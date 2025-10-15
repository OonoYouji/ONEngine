using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleStartController : MonoBehavior {

	private List<MonoBehavior> thisScripts_;
	private Player player_;
	[SerializeField] float toPlayerDistance_ = 0.0f; /// プレイヤーとの距離
	[SerializeField] float startPuzzleDistance_ = 4.0f; /// パズルを開始する距離

	private bool isStartedPuzzle_ = false; /// パズルが開始しているか
	private Entity startUI_;

	public override void Initialize() {

		/// ----------------------------------
		/// startUIを生成
		/// ----------------------------------
		startUI_ = ecsGroup.CreateEntity("PuzzleStartUI");
		startUI_.parent = entity;


		/// playerを検索
		Entity ePlayer = ecsGroup.FindEntity("Player");
		player_ = ePlayer.GetScript<Player>();
		if (!player_) {
			/// 見つからなかったログを出力する
			Debug.LogError("PuzzleStartController.Initialize - Player script not found.");
		}

		Billboard uiBillboard = startUI_.GetScript<Billboard>();
		if (uiBillboard) {
			uiBillboard.target = ePlayer;
		}


		thisScripts_ = entity.GetScripts();

		int thisIndex = 0;
		for (int i = 0; i < thisScripts_.Count; i++) {
			if (thisScripts_[i] == this) {
				thisIndex = i;
			}
		}

		/// このスクリプトは管理しないので削除しておく
		thisScripts_.Remove(thisScripts_[thisIndex]);

		ToggleScriptEnable();

		/// パラメータの初期化
		isStartedPuzzle_ = false;
		player_.enable = true;
	}

	public override void Update() {

		/// プレイヤーとパズルの距離を計算
		toPlayerDistance_ = Vector3.Distance(transform.position, player_.transform.position);

		/// 開始出来る状態かチェック
		if (startPuzzleDistance_ > toPlayerDistance_) {

			/// 入力によってパズルを始める
			if (Input.TriggerKey(KeyCode.Space) ||
				Input.TriggerGamepad(Gamepad.A)) {
				ToggleScriptEnable();
			}
		}
	}

	private void ToggleScriptEnable() {
		isStartedPuzzle_ = !isStartedPuzzle_;
		for (int i = 0; i < thisScripts_.Count; i++) {
			thisScripts_[i].enable = !thisScripts_[i].enable;
		}

		/// パズルの状態に合わせてプレイヤーの状態を変更する
		player_.enable = !isStartedPuzzle_;
	}

}