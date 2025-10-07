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

	public override void Initialize() {
		/// playerを検索
		Entity ePlayer = ecsGroup.FindEntity("Player");
		player_ = ePlayer.GetScript<Player>();
		if (!player_) {
			/// 見つからなかったログを出力する
			Debug.LogError("PuzzleStartController.Initialize - Player script not found.");
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
	}

	public override void Update() {

		/// プレイヤーとパズルの距離を計算
		toPlayerDistance_ = Vector3.Distance(transform.position, player_.transform.position);
		if (startPuzzleDistance_ > toPlayerDistance_) {

			/// 開始出来る
			if (Input.TriggerKey(KeyCode.Space) ||
				Input.TriggerGamepad(Gamepad.A)) {
				ToggleScriptEnable();
			}
		}
	}

	private void ToggleScriptEnable() {
		for (int i = 0; i < thisScripts_.Count; i++) {
			thisScripts_[i].enable = !thisScripts_[i].enable;
		}

		Debug.Log("[ToggleScriptEnable] ToggleScriptEnable");
	}

}