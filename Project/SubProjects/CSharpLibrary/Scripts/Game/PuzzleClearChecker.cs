using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleClearChecker : MonoBehavior {
	private PuzzleStage puzzleStage_;

	public override void Initialize() {
		puzzleStage_ = entity.GetScript<PuzzleStage>();
		if (!puzzleStage_) {
			Debug.LogWarning("===== puzzle stage is null");
			return;
		}
	}

	public override void Update() {
		/* ----- ステージのクリアチェック ----- */
		if (!puzzleStage_) {
			return;
		}

		Debug.LogWarning("=========================================================");
		Debug.LogWarning("call update puzzle clear checker");
		Debug.LogWarning("=========================================================");

		List<Entity> players = puzzleStage_.GetPlayers();
		bool isClear = false;
		for (int i = 0; i < players.Count; i++) {
			PuzzlePlayer pp = players[i].GetScript<PuzzlePlayer>();
			if (!pp) {
				break;
			}

			if (!CheckIsGoaled(pp)) {
				break;
			}

			/// 全てのplayerを確認してokならクリア
			if (i == players.Count - 1) {
				isClear = true;
			}
		}


		MeshRenderer mr = puzzleStage_.entity.GetComponent<MeshRenderer>();
		if (mr) {
			if (isClear) {
				mr.color = new Vector4(1, 1, 1, 1);
				Debug.LogWarning("=========================================================");
				Debug.LogWarning("puzzle stage cleared");
				Debug.LogWarning("=========================================================");
			} else {
				mr.color = new Vector4(1, 0, 0, 1);
			}
		}
	}

	private bool CheckIsGoaled(PuzzlePlayer _puzzlePlayer) {
		/// プレイヤーのアドレスを確認
		if (!_puzzlePlayer) {
			return false; //!< puzzle playerが null
		}

		var mapData = puzzleStage_.GetMapData();
		Vector2Int address = _puzzlePlayer.blockData.address;
		if (CheckIsGoal(mapData[address.y][address.x])) {
			return true; //!< 現在いる場所がゴールだったら
		}

		return false;
	}

	private bool CheckIsGoal(int _mapValue) {
		if (_mapValue == (int)MAPDATA.GOAL_BLACK || _mapValue == (int)MAPDATA.GOAL_WHITE) {
			return true;
		}

		return false;
	}
}