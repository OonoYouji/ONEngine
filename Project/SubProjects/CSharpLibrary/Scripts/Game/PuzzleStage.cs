using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleStage : MonoBehavior {
	private PuzzleBlockData blockData_;
	private bool isStartPuzzle_ = false; // パズルが開始されているかどうか

	private List<List<int>> mapData_;

	// private List<List<Entity>> blocks_;
	private List<Entity> blocks_;

	private Vector3 blockPosOffset_; // ブロックの位置オフセット
	private Entity activePlayer_; // 
	private Entity mapchip_;

	PuzzleCommandStacker puzzleCommandStacker_;

	/// <summary>
	/// 移動パラメータ
	/// </summary>
	private Vector2Int moveDir_;

	public override void Initialize() {
		puzzleCommandStacker_ = new PuzzleCommandStacker();

		mapchip_ = EntityCollection.CreateEntity("Mapchip");
		if (mapchip_ != null) {
			Mapchip mapchipScript = mapchip_.GetScript<Mapchip>();
			if (mapchipScript != null) {
				mapchipScript.LoadMap("./Assets/Game/StageData/", "stage3.json");
			}

			mapData_ = mapchipScript.GetStartMapData();
		}

		blockData_.height = 2f; // ブロックの高さを設定
		blockData_.blockSpace = 0.22f; // ブロックのアドレスを初期化


		BlockDeploy(); // ブロック配置
		PlayerDeploy(); // プレイヤー配置

		blockPosOffset_ = new Vector3(1f, -1f, 1f);
		blockData_.blockSpace = 0.22f;
	}


	public override void Update() {
		/// パズルが開始されていない場合は何もしない
		// if (!isStartPuzzle_) {
		// 	return;
		// }

		Game();
	}


	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// 初期化に使用する関数
	/// ///////////////////////////////////////////////////////////////////////////////////////////
	private void PlayerDeploy() {
		/* ----- プレイヤーの配置 ----- */

		Mapchip mapchipScript = mapchip_.GetScript<Mapchip>();
		if (!mapchipScript) {
			return;
		}

		Stage.Player startPlayer = mapchipScript.GetPlayer();
		Vector2Int playerAddress = new Vector2Int(startPlayer.column, startPlayer.row);

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

				if (startPlayer.type == (int)BlockType.Black) {
					puzzlePlayer.blockData.mapValue = (int)MAPDATA.PLAYER_BLACK;
				} else {
					puzzlePlayer.blockData.mapValue = (int)MAPDATA.PLAYER_WHITE;
				}
			}
		}
	}

	private void BlockDeploy() {
		/* ----- ブロックの配置を行う ----- */

		Debug.Log("----- BlockDeployed. -----");


		blocks_ = new List<Entity>();

		for (int r = 0; r < mapData_.Count; r++) {
			for (int c = 0; c < mapData_[r].Count; c++) {
				Debug.Log("map[" + r + "][" + c + "] = " + mapData_[r][c]);

				/// マップデータがブロックでは無ければ配置しない
				Entity block = null;
				int mapValue = mapData_[r][c];
				if (CheckIsBlock(mapValue)) {
					block = EntityCollection.CreateEntity("Block");
				} else if (CheckIsGoal(mapValue)) {
					block = EntityCollection.CreateEntity("Goal");
				}

				if (block == null) {
					continue;
				}

				/// blockの初期化
				Block blockScript = block.GetScript<Block>();
				if (blockScript) {
					blockScript.blockData.address = new Vector2Int(c, r);
					blockScript.blockData.height = blockData_.height;
					blockScript.blockData.blockSpace = blockData_.blockSpace;
					blockScript.blockData.mapValue = mapValue;
				}

				block.parent = this.entity;
				Transform t = block.transform;

				/// blockのindexで位置を決定
				t.position = new Vector3(c, blockData_.height, r);

				MeshRenderer mr = block.GetComponent<MeshRenderer>();
				if (mr != null) {
					/// 色を黒か白に設定
					Vector4 color = Vector4.one * mapData_[r][c]; // 1なら白、0なら黒
					color.w = 1f;

					mr.color = color;
				}

				blocks_.Add(block); // ブロックをリストに追加
			}
		}

		Debug.Log("----- BlockDeployed. ended -----");
	}


	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// 更新に使用する関数
	/// ///////////////////////////////////////////////////////////////////////////////////////////
	void Game() {
		if (mapData_ != null) {
			int width = mapData_[0].Count;
			int height = mapData_.Count;
			blockPosOffset_ = new Vector3(width / 2f, 0, height / 2f) * blockData_.blockSpace;
		}

		PuzzlePlayer puzzlePlayer = null;
		if (activePlayer_ != null && activePlayer_.transform != null) {
			puzzlePlayer = activePlayer_.GetScript<PuzzlePlayer>();
			puzzlePlayer.UpdatePosition();

			Transform playerTransform = activePlayer_.transform;
			playerTransform.position -= blockPosOffset_;
		}

		for (int i = 0; i < blocks_.Count; i++) {
			Entity block = blocks_[i];
			if (block == null) {
				continue;
			}

			Block blockScript = block.GetScript<Block>();
			if (blockScript) {
				Vector2Int address = blockScript.blockData.address;

				blockScript.blockData.mapValue = mapData_[address.y][address.x];
				if (puzzlePlayer) {
					if (blockScript.blockData.type == puzzlePlayer.blockData.type) {
						blockScript.blockData.height = blockData_.height;
					} else {
						blockScript.blockData.height = blockData_.height - 0.05f;
					}
				}

				Transform t = block.transform;
				t.position = new Vector3(address.x * blockData_.blockSpace, blockScript.blockData.height,
					address.y * blockData_.blockSpace);
				t.position -= blockPosOffset_;
			}
		}


		/* パズルを行っているときの更新 */

		UpdatePlayer();


		if (CheckIsGoaled(puzzlePlayer)) {
			puzzlePlayer.isGoaled.Set(true);
		}

		/// パズルを終了する
		if (Input.TriggerGamepad(Gamepad.B)) {
			/// パズルを終了する
			isStartPuzzle_ = false;

			/// 他のエンティティの更新を停止
			Debug.Log("Puzzle ended.");
		}
	}


	private void UpdatePlayer() {
		/* ----- プレイヤーの更新 ----- */

		if (activePlayer_ == null) {
			return;
		}

		/* ----- プレイヤーの移動を行う ----- */

		PuzzlePlayer player = activePlayer_.GetScript<PuzzlePlayer>();
		if (player) {
			if (player.blockData.type == (int)BlockType.Black) {
				moveDir_ = Vector2Int.zero;
			}
		}

		if (moveDir_ == Vector2Int.zero) {
			/// 移動方向の決定
			if (Input.TriggerGamepad(Gamepad.DPadUp)) {
				moveDir_ = Vector2Int.up;
			}

			if (Input.TriggerGamepad(Gamepad.DPadDown)) {
				moveDir_ = Vector2Int.down;
			}

			if (Input.TriggerGamepad(Gamepad.DPadLeft)) {
				moveDir_ = Vector2Int.left;
			}

			if (Input.TriggerGamepad(Gamepad.DPadRight)) {
				moveDir_ = Vector2Int.right;
			}
		}

		PuzzlePlayer puzzlePlayer = activePlayer_.GetScript<PuzzlePlayer>();
		if (!puzzlePlayer) {
			return;
		}


		Vector2Int beforeAddress = puzzlePlayer.blockData.address;
		/// 新しいアドレスが移動出来る場所か確認
		Debug.Log("-----: puzzle player move direction .x" + moveDir_.x + ": .y" + moveDir_.y);
		if (CheckPlayerMoving(puzzlePlayer.blockData.address, moveDir_)) {
			/* ----- プレイヤーの移動を行う ----- */
			puzzlePlayer.blockData.address = puzzlePlayer.blockData.address + moveDir_;
			Moved(beforeAddress, puzzlePlayer.blockData.address);

			puzzlePlayer.UpdateRotateY(moveDir_);
		} else {
			/// 移動できない場合の処理を追加
			moveDir_ = Vector2Int.zero;
		}
	}

	/// <summary>
	/// プレイヤーが移動可能かどうかをチェックする
	/// </summary>
	private bool CheckPlayerMoving(Vector2Int _currentAddress, Vector2Int _moveDir) {
		/// 移動しないなら false
		if (_moveDir == Vector2Int.zero) {
			return false;
		}

		PuzzlePlayer puzzlePlayer = activePlayer_.GetScript<PuzzlePlayer>();
		if (!puzzlePlayer) {
			return false; //!< スクリプトを取得出来ないならfalseを返す
		}

		/// 移動方向が範囲外なら false
		Vector2Int newAddress = _currentAddress + _moveDir;
		if (newAddress.x < 0 || newAddress.x >= mapData_[0].Count || newAddress.y < 0
		    || newAddress.y >= mapData_.Count) {
			return false;
		}

		/// 移動方向がブロックじゃないなら
		if (!CheckIsBlock(mapData_[newAddress.y][newAddress.x]) && !CheckIsGoal(mapData_[newAddress.y][newAddress.x])) {
			return false;
		}

		/// 移動方向が自身と違う色なら false
		if (mapData_[newAddress.y][newAddress.x] % 10 != puzzlePlayer.blockData.type) {
			return false;
		}

		return true;
	}


	/// <summary>
	/// 移動後のmap dataの更新を行う
	/// </summary>
	public void Moved(Vector2Int _currentAddress, Vector2Int _movedAddress) {
		/// x,zのどちらに移動したのか確認

		if (_currentAddress.x != _movedAddress.x) {
			/// x軸に移動した
			int yAddress = _currentAddress.y;
			int subLenght = Mathf.Abs(_movedAddress.x - _currentAddress.x);

			for (int i = 0; i < subLenght; i++) {
				int value = mapData_[yAddress][_currentAddress.x + i];
				if (value == (int)MAPDATA.BLOCK_BLACK) {
					mapData_[yAddress][_currentAddress.x + i] = (int)MAPDATA.BLOCK_WHTIE;
				} else if (value == (int)MAPDATA.BLOCK_WHTIE) {
					mapData_[yAddress][_currentAddress.x + i] = (int)MAPDATA.BLOCK_BLACK;
				}
			}
		} else if (_currentAddress.y != _movedAddress.y) {
			/// y軸に移動した
			int xAddress = _currentAddress.x;
			int subLenght = Mathf.Abs(_movedAddress.y - _currentAddress.y);

			for (int i = 0; i < subLenght; i++) {
				int value = mapData_[_currentAddress.y + i][xAddress];
				if (value == (int)MAPDATA.BLOCK_BLACK) {
					mapData_[_currentAddress.y + i][xAddress] = (int)MAPDATA.BLOCK_WHTIE;
				} else if (value == (int)MAPDATA.BLOCK_WHTIE) {
					mapData_[_currentAddress.y + i][xAddress] = (int)MAPDATA.BLOCK_BLACK;
				}
			}
		}
	}

	private bool CheckIsBlock(int _mapValue) {
		if (_mapValue == (int)MAPDATA.BLOCK_BLACK || _mapValue == (int)MAPDATA.BLOCK_WHTIE) {
			return true;
		}

		return false;
	}

	private bool CheckIsGoal(int _mapValue) {
		if (_mapValue == (int)MAPDATA.GOAL_BLACK || _mapValue == (int)MAPDATA.GOAL_WHITE) {
			return true;
		}

		return false;
	}

	private bool CheckIsGoaled(PuzzlePlayer _puzzlePlayer) {
		/// プレイヤーのアドレスを確認
		if (!_puzzlePlayer) {
			return false; //!< puzzle playerが null
		}

		Vector2Int address = _puzzlePlayer.blockData.address;
		if (CheckIsGoal(mapData_[address.y][address.x])) {
			return true; //!< 現在いる場所がゴールだったら
		}

		return false;
	}


	private void Reset() {
		activePlayer_.Destroy();
		for (int i = 0; i < blocks_.Count; i++) {
			blocks_[i].Destroy();
		}

		BlockDeploy();
		PlayerDeploy();
	}
}