using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleManager : MonoScript {

	List<Entity> puzzleStands_ = new List<Entity>();
	uint childCount_ = 0;

	public override void Initialize() {
		/// 子のエンティティがすべてパズル台なので取得しておく
		childCount_ = entity.GetChildCount();
		for (uint i = 0; i < childCount_; i++) {
			Entity child = entity.GetChild(i);
			if (child != null) {
				puzzleStands_.Add(child);
			}
		}
	}

	public override void Update() {

		/// アクティブになっているパズル台があれば、他のパズル台を非アクティブにする
		Entity activePuzzleStand = null;
		for (int i = 0; i < puzzleStands_.Count; i++) {
			PuzzleStartController psc = puzzleStands_[i].GetScript<PuzzleStartController>();
			if (psc.IsStartedPuzzle()) {
				activePuzzleStand = puzzleStands_[i];
				break;
			}
		}

		if (!activePuzzleStand) {
			/// 非アクティブにした可能性があるのでもとに戻す
			for (int i = 0; i < puzzleStands_.Count; i++) {
				puzzleStands_[i].enable = true;
			}
			return;
		}

		/// アクティブなパズル台以外を非アクティブにする
		for (int i = 0; i < puzzleStands_.Count; i++) {
			if (puzzleStands_[i] != activePuzzleStand) {
				puzzleStands_[i].enable = false;
			}
		}

	}
}
