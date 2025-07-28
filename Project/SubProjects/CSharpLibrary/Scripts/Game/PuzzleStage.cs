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
	private Entity activePlayer_; // 

	[SerializeField] private float blockSpace;
	PuzzleCommandStacker puzzleCommandStacker_;

	public override void Initialize() {
		puzzleCommandStacker_ = new PuzzleCommandStacker();

		Entity mapchip = EntityCollection.CreateEntity("Mapchip");
		if (mapchip != null) {
			Mapchip mapchipScript = mapchip.GetScript<Mapchip>();
			if (mapchipScript != null) {
				mapchipScript.LoadMap("./Assets/Game/StageData/", "stage1.json");
			}
		}

		blockData_.height = 2f; // ブロックの高さを設定
		blockData_.blockSpace = 0.22f; // ブロックのアドレスを初期化

		BlockDeploy(); // ブロック配置
		PlayerDeploy(); // プレイヤー配置

		blockPosOffset_ = new Vector3(1f, -1f, 1f);
		blockSpace = 0.22f;
	}


	public override void Update() {
		blockSpace = 0.22f;
		blockData_.blockSpace = blockSpace;

		int width = blocks_[0].Count;
		int height = blocks_.Count;
		blockPosOffset_ = new Vector3(width / 2f, 0, height / 2f) * blockData_.blockSpace;
		Debug.Log("---:" + (int)blockData_.blockSpace);
		Debug.Log("---:" + width);
		Debug.Log("---:" + height);

		for (int r = 0; r < blocks_.Count; r++) {
			for (int c = 0; c < blocks_[r].Count; c++) {
				Entity block = blocks_[r][c];
				if (block == null) {
					continue;
				}

				Transform t = block.transform;
				t.position = new Vector3(c * blockData_.blockSpace, blockData_.height, r * blockData_.blockSpace);
				t.position -= blockPosOffset_;
				
				Block blockScript = block.GetScript<Block>();
				if (blockScript) {
					blockScript.blockData.type = mapData_[r][c];
					Debug.LogInfo("-----: block type setting");
				}
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
		Vector2Int beforeAddress = puzzlePlayer.blockData.address;
		/// 新しいアドレスが移動出来る場所か確認
		if (CheckPlayerMoving(puzzlePlayer.blockData.address, moveDir)) {
			/* ----- プレイヤーの移動を行う ----- */
			puzzlePlayer.blockData.address = puzzlePlayer.blockData.address + moveDir;
			Moved(beforeAddress, puzzlePlayer.blockData.address, puzzlePlayer.blockData.type);
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
	}


	private void BlockDeploy() {
		/* ----- ブロックの配置を行う ----- */

		mapData_ = new List<List<int>> {
			new List<int> { 0, 1, 0, 1, 0 },
			new List<int> { 1, 0, 1, 0, 1 },
			new List<int> { 0, 0, 0, 1, 0 },
			new List<int> { 0, 0, 0, 0, 1 },
		};
		
		mapData_.Reverse();

		Debug.Log("----- BlockDeployed. -----");


		blocks_ = new List<List<Entity>>();

		Vector2 mapSize = new Vector2(mapData_.Count, mapData_[0].Count);

		for (int r = 0; r < mapData_.Count; r++) {
			for (int c = 0; c < mapData_[r].Count; c++) {
				Debug.Log("map[" + r + "][" + c + "] = " + mapData_[r][c]);


				Entity block = EntityCollection.CreateEntity("Block");
				if (block == null) {
					continue;
				}

				/// blockの初期化
				Block blockScript = block.GetScript<Block>();
				if (blockScript) {
					blockScript.blockData.address = new Vector2Int(r, c);
					blockScript.blockData.height = blockData_.height;
					blockScript.blockData.blockSpace = blockData_.blockSpace;
					blockScript.blockData.type = mapData_[r][c];
				}
				
				block.parent = this.entity;
				Transform t = block.transform;

				/// blockのindexで位置を決定
				t.position = new Vector3(r, blockData_.height, c);

				MeshRenderer mr = block.GetComponent<MeshRenderer>();
				if (mr != null) {
					/// 色を黒か白に設定
					Vector4 color = Vector4.one * mapData_[r][c]; // 1なら白、0なら黒
					color.w = 1f;

					mr.color = color;
				}


				if (blocks_.Count <= r) {
					blocks_.Add(new List<Entity>());
				}

				blocks_[r].Add(block); // ブロックをリストに追加
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

		/// 移動方向が自身と違う色なら false
		PuzzlePlayer puzzlePlayer = activePlayer_.GetScript<PuzzlePlayer>();
		if (mapData_[newAddress.y][newAddress.x] != puzzlePlayer.blockData.type) {
			return false;
		}

		return true;
	}


	/// <summary>
	/// 移動後のmap dataの更新を行う
	/// </summary>
	public void Moved(Vector2Int _currentAddress, Vector2Int _movedAddress, int type) {
		
		/// x,zのどちらに移動したのか確認
		
		if (_currentAddress.x != _movedAddress.x) {
			/// x軸に移動した
			int yAddress = _currentAddress.y;
			int subLenght = Mathf.Abs(_movedAddress.x - _currentAddress.x);

			for (int i = 0; i < subLenght; i++) {
				BlockType value = (BlockType)mapData_[yAddress][_currentAddress.x + i];
				if (value == (int)BlockType.Black) {
					mapData_[yAddress][_currentAddress.x + i] = (int)BlockType.White;
				} else {
					mapData_[yAddress][_currentAddress.x + i] = (int)BlockType.Black;
				}
			}
			
		} else if (_currentAddress.y != _movedAddress.y) {
			/// y軸に移動した
			int xAddress = _currentAddress.x;
			int subLenght = Mathf.Abs(_movedAddress.y - _currentAddress.y);

			for (int i = 0; i < subLenght; i++) {
				BlockType value = (BlockType)mapData_[_currentAddress.y + i][xAddress];
				if (value == (int)BlockType.Black) {
					mapData_[_currentAddress.y + i][xAddress] = (int)BlockType.White;
				} else {
					mapData_[_currentAddress.y + i][xAddress] = (int)BlockType.Black;
				}
			}
		}
		
	}
}