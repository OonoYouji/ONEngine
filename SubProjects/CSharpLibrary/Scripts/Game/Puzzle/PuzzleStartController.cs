using Stage;
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
		MeshRenderer mr = startUI_.AddComponent<MeshRenderer>();
		if (mr) {
			mr.enable = 0;
		}


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

		/// パズルの開始用UIを更新
		UpdateStartUI();


		/// 開始出来る状態かチェック
		if (startPuzzleDistance_ > toPlayerDistance_) {
			/// 入力によってパズルを始める
			if (Input.TriggerKey(KeyCode.Space) ||
				Input.TriggerGamepad(Gamepad.A)) {
				StartPuzzle();
			}
		}


		/// パズルをやめる入力チェック
		if (isStartedPuzzle_) {
			if (Input.TriggerKey(KeyCode.Escape) ||
				Input.TriggerGamepad(Gamepad.Start)) {
				EndPuzzle();
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

	private void StartPuzzle() {
		isStartedPuzzle_ = true;
		for (int i = 0; i < thisScripts_.Count; i++) {
			thisScripts_[i].enable = true;
		}
		/// パズルの状態に合わせてプレイヤーの状態を変更する
		player_.enable = false;
		MeshRenderer playerMR = player_.entity.GetComponent<MeshRenderer>();
		if (playerMR) {
			playerMR.color = Vector4.zero;
		}
	}


	private void EndPuzzle() {
		isStartedPuzzle_ = false;
		for (int i = 0; i < thisScripts_.Count; i++) {
			thisScripts_[i].enable = false;
		}
		/// パズルの状態に合わせてプレイヤーの状態を変更する
		player_.enable = true;
		MeshRenderer playerMR = player_.entity.GetComponent<MeshRenderer>();
		if (playerMR) {
			playerMR.color = Vector4.one;
		}
	}

	private void UpdateStartUI() {
		/// ----- パズル開始UIの表示・非表示を切り替え ----- /// 

		MeshRenderer mr = startUI_.GetComponent<MeshRenderer>();
		if (!mr) {
			/// 制御対象がないなら何もできない
			return;
		}

		/// パズルが開始しているなら非表示
		if (isStartedPuzzle_) {
			mr.color = new Vector4(1, 1, 1, 0);
			return;
		}


		/// パズルスタンドとプレイヤーの距離で表示・非表示を切り替え
		bool enable = (startPuzzleDistance_ > toPlayerDistance_);
		if (enable) {
			mr.color = new Vector4(1, 1, 1, 1);
		} else {
			mr.color = new Vector4(1, 1, 1, 0);
		}
	}

}