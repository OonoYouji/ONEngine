namespace PuzzlePlayerCommands {
	
	/// 黒色の場合
	namespace Black {
		/// <summary>
		/// 左に移動
		/// </summary>
		public class RightMoveCmd : PuzzleCommand {
			private PuzzlePlayer player;
			private Vector2Int beforeAddress;

			public RightMoveCmd(PuzzlePlayer _player) {
				player = _player;
			}

			public override void Execute() {
				beforeAddress = player.blockData.address;
				player.MoveBlack(Vector2Int.right);
			}

			public override void Undo() {
			}
		}


		/// <summary>
		/// 右に移動
		/// </summary>
		public class LeftMoveCmd : PuzzleCommand {
			private PuzzlePlayer player;
			private Vector2Int beforeAddress;

			public LeftMoveCmd(PuzzlePlayer _player) {
				player = _player;
			}

			public override void Execute() {
				beforeAddress = player.blockData.address;
				player.MoveBlack(Vector2Int.left);
			}

			public override void Undo() {
			}
		}


		/// <summary>
		/// 上に移動
		/// </summary>
		public class UpMoveCmd : PuzzleCommand {
			private PuzzlePlayer player;
			private Vector2Int beforeAddress;

			public UpMoveCmd(PuzzlePlayer _player) {
				player = _player;
			}

			public override void Execute() {
				beforeAddress = player.blockData.address;
				player.MoveBlack(Vector2Int.up);
			}

			public override void Undo() {
			}
		}

		/// <summary>
		/// 下に移動する
		/// </summary>
		public class DownMoveCmd : PuzzleCommand {
			private PuzzlePlayer player;
			private Vector2Int beforeAddress;

			public DownMoveCmd(PuzzlePlayer _player) {
				player = _player;
			}

			public override void Execute() {
				beforeAddress = player.blockData.address;
				player.MoveBlack(Vector2Int.down);
			}

			public override void Undo() {
			}
		}
	}
}