using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Threading;


/// <summary>
/// コマンドスタッカーの状態
/// </summary>
public enum StackerState {
	Running,     /// 実行中
	WaitingInput /// 入力待機中
}

public enum CommandState {
	Executing, /// 実行中
	Undoing,   /// Undo中
	Finished,  /// 完了
	Failed     /// 失敗
}

/// <summary>
/// パズルコマンドの基底クラス
/// </summary>
public class PuzzleCommand {
	/// <summary>
	/// 実行中
	/// </summary>
	/// <returns></returns>
	public virtual CommandState Execution() { return CommandState.Finished; }

	/// <summary>
	/// 戻す
	/// </summary>
	/// <returns></returns>
	public virtual CommandState Undo() { return CommandState.Finished; }
}

/// <summary>
/// コマンドのスタッカー
/// </summary>
public class PuzzleCommandStacker {
	private List<PuzzleCommand> commands_ = new List<PuzzleCommand>(); /// コマンドリスト
	private PuzzleCommand runningCommand_ = null; /// 実行中のコマンド
	private StackerState state_ = StackerState.WaitingInput; /// スタッカーの状態

	public void Update() {
		/// 実行中のコマンドを更新 ( ?.Exe() にすることでNULLチェックを同時に行う )
		if (runningCommand_ != null) {
			CommandState state = runningCommand_.Execution();
			if (state == CommandState.Finished || state == CommandState.Failed) {
				/// 実行完了
				runningCommand_ = null;
				state_ = StackerState.WaitingInput;
			}
		}

		if (state_ == StackerState.WaitingInput) {
			/// Undoの入力を受け付ける
			if (Input.TriggerGamepad(Gamepad.X)) {
				Undo();
			}
		}

	}

	/// <summary>
	/// コマンドの実行
	/// </summary>
	/// <typeparam name="T"></typeparam>
	public T ExecutionCommand<T>() where T : PuzzleCommand {
		if (state_ != StackerState.WaitingInput) {
			Debug.Log("===== Stacker is not waiting input.");
			return null;
		}

		Debug.Log("===== Execute Command: " + typeof(T).ToString());

		// 引数付きでインスタンスを生成
		T t = (T)Activator.CreateInstance(typeof(T));
		if (t == null) {
			Debug.LogError("===== Failed to create command instance: " + typeof(T).ToString());
			return null;
		}

		commands_.Add(t);
		runningCommand_ = t;
		state_ = StackerState.Running;
		return t;
	}


	void Undo() {
		/// 最後に実行したコマンドをUndoする
		var lastCommand = commands_[commands_.Count - 1];
		if (lastCommand != null) {
			lastCommand.Undo();
			commands_.RemoveAt(commands_.Count - 1);
		}
	}


	public bool IsRunning() {
		return state_ == StackerState.Running;
	}
}


/// ↓ 以下パズルのコマンド群 ↓ ///
/*
 * 移動
 *	白 上下左右
 *	黒 上下左右
 * 切り替え
 */


namespace PuzzleCommands {

	struct BlockData {
		public Vector2Int address; /// ブロックのアドレス
		public int prevMapValue; /// 変更前のマップデータの値
	}


	/// <summary>
	/// 白ブロック移動コマンド
	/// </summary>
	public class MoveWhiteBlockCommand : PuzzleCommand {
		Entity puzzle_; /// パズル自体のエンティティ
		Entity operateTarget_; /// 操作対象のエンティティ
		Vector2Int moveDir_;   /// 移動方向
		Vector2Int prevAddress_;  /// 移動前の位置
		Vector2Int nextAddress_;  /// 移動後の位置
		List<BlockData> changedBlocks_ = new List<BlockData>(); /// 移動に伴い変更されるブロックのリスト

		int moveStep_ = 0; /// 移動ステップ数
		int maxMoveStep_ = 1; /// 最大移動ステップ数

		public void Awake(Entity _puzzle, Entity _target, Vector2Int _dir) {
			puzzle_ = _puzzle;
			operateTarget_ = _target;
			moveDir_ = _dir;

			prevAddress_ = operateTarget_.GetScript<PuzzlePlayer>().blockData.address;
			nextAddress_ = CalcNextAddress();

			maxMoveStep_ = Math.Abs(nextAddress_.x - prevAddress_.x) + Math.Abs(nextAddress_.y - prevAddress_.y);
		}

		public override CommandState Execution() {
			if (prevAddress_ == nextAddress_) {
				Debug.Log("===== MoveWhiteBlockCommand Execution - 移動先が同じため移動しない");
				return CommandState.Finished;
			}

			/// 移動先に移動
			/// とりあえず1フレーム1マスの移動で実装
			moveStep_++;

			PuzzlePlayer pp = operateTarget_.GetScript<PuzzlePlayer>();
			Vector2Int currentAddress = pp.blockData.address;
			/// newAddressの位置にあるブロックを取得し、色を変える
			PuzzleStage puzzleStage = puzzle_.GetScript<PuzzleStage>();
			int mapValue = puzzleStage.GetMapValue(currentAddress);
			if (puzzleStage.CheckIsBlock(mapValue)) {
				/// 変更前のブロックデータを保存
				changedBlocks_.Add(new BlockData {
					address = new Vector2Int(currentAddress.x, currentAddress.y),
					prevMapValue = mapValue
				});

				/// 色を変える
				if (mapValue == (int)MAPDATA.BLOCK_WHTIE) {
					mapValue = (int)MAPDATA.BLOCK_BLACK;
				} else {
					mapValue = (int)MAPDATA.BLOCK_WHTIE;
				}
				puzzleStage.SetMapValue(currentAddress, mapValue);
			}

			/// 移動実行
			pp?.Move(moveDir_);

			if (maxMoveStep_ <= moveStep_) {
				Debug.Log("===== MoveWhiteBlockCommand Execution - 移動完了");
				/// 移動完了
				return CommandState.Finished;
			}

			/// 実行処理
			return CommandState.Executing;
		}

		public override CommandState Undo() {

			/// 移動前の状態に戻す
			/// プレイヤーを移動前の位置に戻す
			PuzzlePlayer pp = operateTarget_.GetScript<PuzzlePlayer>();
			if (pp) {
				pp.SetPosition(prevAddress_);
			}

			/// 変更したブロックの色を元に戻す
			foreach (var blockData in changedBlocks_) {
				PuzzleStage puzzleStage = puzzle_.GetScript<PuzzleStage>();
				puzzleStage.SetMapValue(blockData.address, blockData.prevMapValue);
			}

			// 戻す処理
			return CommandState.Finished;
		}

		Vector2Int CalcNextAddress() {
			/// マップの周囲には超過しないように1つ余裕を持たせているので、その分を考慮して計算する
			Vector2Int address = prevAddress_;

			/// 移動方向に進める限り進む
			PuzzleStage puzzleStage = puzzle_.GetScript<PuzzleStage>();
			List<List<int>> mapData = puzzleStage.GetMapData();

			/// 最大のアドレスを設定し、無制限に進むことができないようにする
			Vector2Int kMaxAddress = new Vector2Int(32, 32);
			/// 進行方向次第でループ条件を変える
			if (moveDir_.x > 0) {
				/// +X方向
				for (int x = address.x + 1; x < kMaxAddress.x; x++) {
					int mapValue = mapData[address.y][x];
					if (!CheckCanMoveBlock(mapValue, new Vector2Int(x, address.y))) {
						break;
					}
					address.x = x;
				}

			} else if (moveDir_.x < 0) {
				/// -X方向
				for (int x = address.x - 1; x >= 0; x--) {
					int mapValue = mapData[address.y][x];
					if (!CheckCanMoveBlock(mapValue, new Vector2Int(x, address.y))) {
						break;
					}
					address.x = x;
				}

			} else if (moveDir_.y > 0) {
				/// +Y方向
				for (int y = address.y + 1; y < kMaxAddress.y; y++) {
					int mapValue = mapData[y][address.x];
					if (!CheckCanMoveBlock(mapValue, new Vector2Int(address.x, y))) {
						break;
					}
					address.y = y;
				}

			} else if (moveDir_.y < 0) {
				/// -Y方向
				for (int y = address.y - 1; y >= 0; y--) {
					int mapValue = mapData[y][address.x];
					if (!CheckCanMoveBlock(mapValue, new Vector2Int(address.x, y))) {
						break;
					}
					address.y = y;
				}

			}

			return address;
		}


		bool CheckCanMoveBlock(int _mapValue, Vector2Int _nextAddress) {
			/// 移動可能なブロックかどうかをチェック
			if (_mapValue == (int)MAPDATA.BLOCK_WHTIE ||
				_mapValue == (int)MAPDATA.CONSTANT_BLOCK_WHITE ||
				_mapValue == (int)MAPDATA.GOAL_WHITE) {

				/// もう１つ条件としてもう一人のプレイヤーがその位置にいないこと
				List<Entity> players = puzzle_.GetScript<PuzzleStage>().GetPlayers();
				foreach (var player in players) {
					if (player != operateTarget_) {
						Vector2Int otherAddress = player.GetScript<PuzzlePlayer>().blockData.address;
						if (otherAddress == _nextAddress) {
							return false;
						}
					}
				}

				return true;
			}
			return false;
		}

	} /// class MoveWhiteBlockCommand



	class MoveBlackBlockCommand : PuzzleCommand {
		Entity puzzle_; /// パズル自体のエンティティ
		Entity operateTarget_; /// 操作対象のエンティティ
		Vector2Int moveDir_;   /// 移動方向
		Vector2Int prevAddress_;  /// 移動前の位置
		Vector2Int nextAddress_;  /// 移動後の位置
		List<BlockData> changedBlocks_ = new List<BlockData>(); /// 移動に伴い変更されるブロックのリスト

		int moveStep_ = 0; /// 移動ステップ数
		int maxMoveStep_ = 1; /// 最大移動ステップ数

		public void Awake(Entity _puzzle, Entity _target, Vector2Int _dir) {
			puzzle_ = _puzzle;
			operateTarget_ = _target;
			moveDir_ = _dir;
			prevAddress_ = operateTarget_.GetScript<PuzzlePlayer>().blockData.address;
			nextAddress_ = CalcNextAddress();
			maxMoveStep_ = Math.Abs(nextAddress_.x - prevAddress_.x) + Math.Abs(nextAddress_.y - prevAddress_.y);
		}

		public override CommandState Execution() {
			if (prevAddress_ == nextAddress_) {
				Debug.Log("===== MoveBlackBlockCommand Execution - 移動先が同じため移動しない");
				return CommandState.Finished;
			}
			/// 移動先に移動
			/// とりあえず1フレーム1マスの移動で実装
			moveStep_++;
			PuzzlePlayer pp = operateTarget_.GetScript<PuzzlePlayer>();
			Vector2Int currentAddress = pp.blockData.address;
			/// newAddressの位置にあるブロックを取得し、色を変える
			PuzzleStage puzzleStage = puzzle_.GetScript<PuzzleStage>();
			int mapValue = puzzleStage.GetMapValue(currentAddress);
			if (puzzleStage.CheckIsBlock(mapValue)) {
				/// 変更前の状態を保存
				changedBlocks_.Add(new BlockData {
					address = new Vector2Int(currentAddress.x, currentAddress.y),
					prevMapValue = mapValue
				});

				/// 色を変える
				if (mapValue == (int)MAPDATA.BLOCK_BLACK) {
					mapValue = (int)MAPDATA.BLOCK_WHTIE;
				} else {
					mapValue = (int)MAPDATA.BLOCK_BLACK;
				}
				puzzleStage.SetMapValue(currentAddress, mapValue);
			}
			/// 移動実行
			pp?.Move(moveDir_);
			if (maxMoveStep_ <= moveStep_) {
				Debug.Log("===== MoveBlackBlockCommand Execution - 移動完了");
				/// 移動完了
				return CommandState.Finished;
			}
			/// 実行処理
			return CommandState.Executing;
		}


		public override CommandState Undo() {

			/// 移動前の状態に戻す
			/// プレイヤーを移動前の位置に戻す
			PuzzlePlayer pp = operateTarget_.GetScript<PuzzlePlayer>();
			if (pp) {
				pp.SetPosition(prevAddress_);
			}

			/// 変更したブロックの色を元に戻す
			foreach (var blockData in changedBlocks_) {
				PuzzleStage puzzleStage = puzzle_.GetScript<PuzzleStage>();
				puzzleStage.SetMapValue(blockData.address, blockData.prevMapValue);
			}

			// 戻す処理
			return CommandState.Finished;
		}

		Vector2Int CalcNextAddress() {
			/// マップの周囲には超過しないように1つ余裕を持たせているので、その分を考慮して計算する
			Vector2Int address = prevAddress_;
			/// 移動方向に進める限り進む
			PuzzleStage puzzleStage = puzzle_.GetScript<PuzzleStage>();
			List<List<int>> mapData = puzzleStage.GetMapData();
			/// 進行方向次第でループ条件を変える
			address += moveDir_;
			int mapValue = mapData[address.y][address.x];
			if (!CheckCanMoveBlock(mapValue, address)) {
				/// 移動できない場合はNextAddressを移動前の位置にし、移動できないようにする
				address = prevAddress_;
			}

			return address;
		}

		bool CheckCanMoveBlock(int _mapValue, Vector2Int _nextAddress) {
			/// 移動可能なブロックかどうかをチェック
			if (_mapValue == (int)MAPDATA.BLOCK_BLACK ||
				_mapValue == (int)MAPDATA.CONSTANT_BLOCK_BLACK ||
				_mapValue == (int)MAPDATA.GOAL_BLACK) {
				/// もう１つ条件としてもう一人のプレイヤーがその位置にいないこと
				List<Entity> players = puzzle_.GetScript<PuzzleStage>().GetPlayers();
				foreach (var player in players) {
					if (player != operateTarget_) {
						Vector2Int otherAddress = player.GetScript<PuzzlePlayer>().blockData.address;
						if (otherAddress == _nextAddress) {
							return false;
						}
					}
				}
				return true;
			}
			return false;
		}

	} /// class MoveBlackBlockCommand


	class SwitchActivePlayerCommand : PuzzleCommand {
		Entity puzzle_; /// パズル自体のエンティティ

		public void Awake(Entity _puzzle) {
			puzzle_ = _puzzle;
		}

		public override CommandState Execution() {
			/// アクティブブロックの切り替え処理
			PuzzleStage puzzleStage = puzzle_.GetScript<PuzzleStage>();
			puzzleStage.SwitchActivePlayer();
			return CommandState.Finished;
		}

		public override CommandState Undo() {
			// 戻す処理
			return Execution();
		}


	} /// class SwitchActivePlayerCommand

} /// namespace PuzzleCommands