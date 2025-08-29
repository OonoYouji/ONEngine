using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

public class PuzzleClearChecker : MonoBehavior {
	private PuzzleStage puzzleStage_;
	private List<Vector2Int> goalAddresses_ = new  List<Vector2Int>();
	private bool isClear_;

	public override void Awake() {
		isClear_ = false;
	}
	
	public override void Initialize() {
		puzzleStage_ = entity.GetScript<PuzzleStage>();
		if (!puzzleStage_) {
			Debug.LogWarning("===== puzzle stage is null");
			return;
		} 
		
		/// ゴールのアドレスを確保
		List<List<int>> mapData = puzzleStage_.GetMapData();
		if (mapData == null || mapData.Count == 0) {
			Debug.LogWarning("===== map data is null");
			return;
		}
		
		for (int y = 0; y < mapData.Count; y++) {
			for (int x = 0; x < mapData[y].Count; x++) {
				int value =  mapData[y][x];
				if (CheckIsGoal(value)) {
					goalAddresses_.Add(new Vector2Int(x, y));
				}
			}
		}
	}

	public override void Update() {
		/* ----- ステージのクリアチェック ----- */
		if (!puzzleStage_) {
			return;
		}
		
		List<Entity> players = puzzleStage_.GetPlayers();
		for (int goalIndex = 0; goalIndex < goalAddresses_.Count; goalIndex++) {
			Vector2Int goalAddress = goalAddresses_[goalIndex];

			bool playerIsGoaled = false;
			for (int playerIndex = 0; playerIndex < players.Count; playerIndex++) {
				PuzzlePlayer pp = players[playerIndex].GetScript<PuzzlePlayer>();
				Vector2Int playerAddress = pp.blockData.address;
				if (goalAddress == playerAddress) {
					/// ゴールしている
					playerIsGoaled = true;
					break;
				}
			}

			/// ゴールしていないなら次をチェックしても無駄なので処理をやめる
			if (!playerIsGoaled) {
				break;
			}

			/// 全てのゴールにゴールしている
			if (goalIndex == goalAddresses_.Count - 1) {
				isClear_ = true;
			}
			
		}

		
		/// クリアしたときの処理を記載
		if (isClear_) {
			/// PuzzleStageの処理をとめる
			PuzzleStage puzzleStage = entity.GetScript<PuzzleStage>();
			if (puzzleStage) {
				puzzleStage.enable = false;
			}
		}
		

		MeshRenderer mr = puzzleStage_.entity.GetComponent<MeshRenderer>();
		if (mr) {
			if (isClear_) {
				mr.color = Vector4.one;
			} else {
				mr.color = Vector4.red;
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


	public bool GetIsClear() {
		return isClear_;
	}
}