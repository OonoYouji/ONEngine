using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleClearEffect : MonoScript {
	
	private PuzzleStage puzzleStage_;
	private PuzzleClearChecker puzzleClearChecker_;
	[SerializeField] private float time_;
	[SerializeField] private float repeatTime_;
	[SerializeField] private int repeatCount_ = 0;
	
	public override void Initialize() {
		puzzleStage_ = entity.GetScript<PuzzleStage>();
		puzzleClearChecker_ = entity.GetScript<PuzzleClearChecker>();
		if (!puzzleStage_) {
			Debug.LogError("No puzzle stage found");
			return;
		}
	}

	public override void Update() {
		if (!puzzleClearChecker_.GetIsClear()) {
			return;
		}
		
		time_ += Time.deltaTime;
		if (time_ >= repeatTime_) {
			repeatCount_++;
			time_ = 0;

			var blocks = puzzleStage_.GetBlocks();
			for (int i = 0; i < blocks.Count; i++) {
				Block block = blocks[i].GetScript<Block>();
				if (block) {
					Vector2Int address = block.blockData.address;
					if (address.x + address.y == repeatCount_) {
						block.StartClearEffect();
					}
				}

			}

		}
	}

}