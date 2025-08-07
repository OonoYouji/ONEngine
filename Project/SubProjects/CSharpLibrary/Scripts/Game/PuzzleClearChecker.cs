public class PuzzleClearChecker : MonoBehavior {

	private PuzzleStage puzzleStage_;
	
	public override void Initialize() {
		puzzleStage_ = entity.GetScript<PuzzleStage>();
		if (!puzzleStage_) {
			Debug.LogWarning("puzzle stage is null");
			return;
		}
	}

	public override void Update() {
		/* ----- ステージのクリアチェック ----- */

		if (!puzzleStage_) {
			return;
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