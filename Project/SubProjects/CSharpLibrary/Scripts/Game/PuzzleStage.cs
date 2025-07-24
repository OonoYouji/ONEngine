using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleStage : MonoBehavior {
	private PuzzleBlockData blockData_;
	private bool isStartPuzzle_ = false; // パズルが開始されているかどうか

	private List<List<int>> mapData_;

	private List<List<Entity>> blocks_;

	private Vector3 blockPosOffset_; // ブロックの位置オフセット
	private List<Entity> players_; // 配置されているPlayer
	private Entity activePlayer_; // 

	[SerializeField] private float blockSpace;

	public override void Initialize() {
		Entity mapchip = EntityCollection.CreateEntity("Mapchip");
		if (mapchip != null) {
			Mapchip mapchipScript = mapchip.GetScript<Mapchip>();
			if (mapchipScript != null) {
				mapchipScript.LoadMap("./Assets/Game/StageData/", "stage1.json");
			}
		}

		blockData_.height = 2f; // ブロックの高さを設定
		blockData_.blockSpace = 0.22f; // ブロックのアドレスを初期化
		Debug.Log("----- Initialize:" + (int)blockData_.blockSpace);

		BlockDeploy(); // ブロック配置
		PlayerDeploy(); // プレイヤー配置

		blockPosOffset_ = new Vector3(1f, -1f, 1f);
		blockSpace = 0.22f;
	}

	private bool toggleFlag = false;

	public override void Update() {
		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr != null) {
			toggleFlag = !toggleFlag;
			if (toggleFlag) {
				mr.postEffectFlags = (uint)(PostEffectFlags.Grayscale | PostEffectFlags.Lighting);
			} else {
				// mr.postEffectFlags = (uint)PostEffectFlags.Lighting;
			}
		}

		blockSpace = 0.22f;
		blockData_.blockSpace = blockSpace;

		int width = blocks_[0].Count;
		int height = blocks_.Count;
		blockPosOffset_ = new Vector3(width / 2f, 0, height / 2f) * blockData_.blockSpace;
		Debug.Log("---:" + (int)blockData_.blockSpace);
		Debug.Log("---:" + width);
		Debug.Log("---:" + height);

		for (int i = 0; i < blocks_.Count; i++) {
			for (int j = 0; j < blocks_[i].Count; j++) {
				Entity block = blocks_[i][j];
				if (block == null) {
					continue;
				}

				Transform t = block.transform;
				t.position = new Vector3(j * blockData_.blockSpace, blockData_.height, i * blockData_.blockSpace);
				t.position -= blockPosOffset_;
			}
		}

		if (activePlayer_ != null && activePlayer_.transform != null) {
			PuzzlePlayer pp = activePlayer_.GetScript<PuzzlePlayer>();
			pp.UpdatePosition();

			Transform playerTransform = activePlayer_.transform;
			playerTransform.position -= blockPosOffset_;
			Debug.Log(" ----- Player Position Setting ----- ");
			Debug.Log("---:" + (int)blockPosOffset_.x);
			Debug.Log("---:" + (int)blockPosOffset_.y);
		}


		/// パズルが開始されていない場合は何もしない
		// if (!isStartPuzzle_) {
		// 	return;
		// }

		Game();
	}


	void Game() {
		/* パズルを行っているときの更新 */

		UpdatePlayer();

		/// パズルを終了する
		if (Input.TriggerGamepad(Gamepad.B)) {
			/// パズルを終了する
			isStartPuzzle_ = false;

			/// 他のエンティティの更新を停止
			SetPuzzleEnable(false);
			Debug.Log("Puzzle ended.");
		}
	}


	void SetPuzzleEnable(bool _enable) {
		/// ブロックの更新を有効/無効にする
		for (int r = 0; r < blocks_.Count; ++r) {
			for (int c = 0; c < blocks_[r].Count; ++c) {
				if (blocks_[r][c] != null) {
					Block block = blocks_[r][c].GetScript<Block>();
					if (block != null) {
						block.enable = _enable;
					}
				}
			}
		}
	}


	private void UpdatePlayer() {
		/* ----- プレイヤーの更新 ----- */

		if (activePlayer_ == null) {
			return;
		}

		/* ----- プレイヤーの移動を行う ----- */

		Vector2Int moveDir = Vector2Int.zero;
		if (Input.TriggerGamepad(Gamepad.DPadUp)) {
			moveDir.y += 1;
		}

		if (Input.TriggerGamepad(Gamepad.DPadDown)) {
			moveDir.y -= 1;
		}

		if (Input.TriggerGamepad(Gamepad.DPadLeft)) {
			moveDir.x -= 1;
		}

		if (Input.TriggerGamepad(Gamepad.DPadRight)) {
			moveDir.x += 1;
		}

		PuzzlePlayer puzzlePlayer = activePlayer_.GetScript<PuzzlePlayer>();
		/// 新しいアドレスが移動出来る場所か確認
		if (CheckPlayerMoving(puzzlePlayer.blockData.address, moveDir)) {
			/* ----- プレイヤーの移動を行う ----- */
			puzzlePlayer.blockData.address = puzzlePlayer.blockData.address + moveDir;
		}
	}


	private void PlayerDeploy() {
		/* ----- プレイヤーの配置 ----- */

		///TODO: ファイルからよみこんで配置する
		Vector2Int playerAddress = new Vector2Int(1, 1);

		activePlayer_ = EntityCollection.CreateEntity("PuzzlePlayer");
		if (activePlayer_ != null) {
			activePlayer_.parent = this.entity; // プレイヤーをPuzzleの子にする
			if (activePlayer_.parent != null) {
				Debug.LogInfo("player parent setting");
			} else {
				Debug.Log("player parent not set");
			}

			/// 座標設定
			Transform t = activePlayer_.transform;
			t.position = new Vector3(playerAddress.x * blockData_.blockSpace, 0f,
				playerAddress.y * blockData_.blockSpace);

			/// スクリプトの値設定
			PuzzlePlayer puzzlePlayer = activePlayer_.GetScript<PuzzlePlayer>();
			if (puzzlePlayer != null) {
				puzzlePlayer.blockData.address = playerAddress; // プレイヤーのアドレスを設定
				puzzlePlayer.blockData.height = blockData_.height; // プレイヤーの高さを設定
				puzzlePlayer.blockData.blockSpace = blockData_.blockSpace; // プレイヤーの高さを設定
				// puzzlePlayer.isIdle = false;

				Debug.Log("activePlayer_ blockSpace " + puzzlePlayer.blockData.blockSpace);
			} else {
				Debug.Log("PuzzlePlayer script not found on activePlayer_ entity.");
			}
		} else {
			Debug.LogError("activePlayer_ is null");
		}

		/* ----- プレイヤーリスト ----- */
		players_.Add(activePlayer_);
	}


	private void BlockDeploy() {
		/* ----- ブロックの配置を行う ----- */

		mapData_ = new List<List<int>> {
			new List<int> { 0, 1, 0, 1, 0 },
			new List<int> { 1, 0, 1, 0, 1 },
			new List<int> { 0, 1, 0, 1, 0 },
			new List<int> { 1, 0, 1, 0, 1 },
		};

		Debug.Log("----- BlockDeployed. -----");


		blocks_ = new List<List<Entity>>();

		Vector2 mapSize = new Vector2(mapData_.Count, mapData_[0].Count);

		for (int i = 0; i < mapData_.Count; i++) {
			for (int j = 0; j < mapData_[i].Count; j++) {
				Debug.Log("map[" + i + "][" + j + "] = " + mapData_[i][j]);


				Entity block = EntityCollection.CreateEntity("Block");
				if (block == null) {
					continue;
				}

				/// blockの初期化

				block.parent = this.entity;
				Transform t = block.transform;

				/// blockのindexで位置を決定
				t.position = new Vector3(i, blockData_.height, j);

				MeshRenderer mr = block.GetComponent<MeshRenderer>();
				if (mr != null) {
					/// 色を黒か白に設定
					Vector4 color = Vector4.one * mapData_[i][j]; // 1なら白、0なら黒
					color.w = 1f;

					mr.color = color;
				}


				if (blocks_.Count <= i) {
					blocks_.Add(new List<Entity>());
				}

				blocks_[i].Add(block); // ブロックをリストに追加
			}
		}
	}


	/// <summary>
	/// プレイヤーが移動可能かどうかをチェックする
	/// </summary>
	/// <param name="_currentAddress"> 移動前のアドレス      </param>
	/// <param name="_moveDir">        移動方向の単位ベクトル </param>
	/// <returns> 移動可能かチェック </returns>
	public bool CheckPlayerMoving(Vector2Int _currentAddress, Vector2Int _moveDir) {
		/// 移動しないなら false
		if (_moveDir == Vector2Int.zero) {
			return false;
		}

		/// 移動方向が範囲外なら false
		Vector2Int newAddress = _currentAddress + _moveDir;
		if (newAddress.x < 0 || newAddress.x >= mapData_[0].Count || newAddress.y < 0
		    || newAddress.y >= mapData_.Count) {
			return false;
		}

		/// 移動方向が自身と同じ色なら false
		if (mapData_[newAddress.y][newAddress.x] == mapData_[_currentAddress.y][_currentAddress.x]) {
			return false;
		}

		return true;
	}
}