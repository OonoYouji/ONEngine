using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleStage : MonoBehavior {
	private PuzzleBlockData blockData_;

	// マップの情報、ブロックリスト、プレイヤーリスト
	private List<List<int>> mapData_;
	private Entity blockParent_;
	private List<Entity> blocks_;
	private List<Entity> players_;

	private Vector3 blockPosOffset_; // ブロックの位置オフセット
	private Entity activePlayer_; // 
	private Entity mapchip_;
	[SerializeField] private string stageFilepath_ = "stage1.json";

	PuzzleCommandStacker puzzleCommandStacker_;

	int initCallCount_ = 0; // 初期化の呼び出し回数

	/// <summary>
	/// 移動パラメータ
	/// </summary>
	private Vector2Int moveDir_;

	public override void Initialize() {
		initCallCount_++;
		Debug.Log("====================================================================");
		Debug.Log("PuzzleStage Initialize called. Call count: " + initCallCount_);
		Debug.Log("====================================================================");

		puzzleCommandStacker_ = new PuzzleCommandStacker();

		mapchip_ = ecsGroup.CreateEntity("Mapchip");
		if (mapchip_ != null) {
			mapchip_.parent = entity;
			Mapchip mapchipScript = mapchip_.GetScript<Mapchip>();
			if (mapchipScript != null) {
				mapchipScript.LoadMap("./Assets/Game/StageData/", stageFilepath_);
			}

			mapData_ = mapchipScript.GetStartMapData();
		}

		blockData_.blockSpace = 0.22f; // ブロックのアドレスを初期化

		CreateBlockParent();
		BlockDeploy(); // ブロック配置
		PlayerDeploy(); // プレイヤー配置
		UpdateEntityPosition();
	}


	public override void Update() {
		Game();
	}


	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// 初期化に使用する関数
	/// ///////////////////////////////////////////////////////////////////////////////////////////
	private void CreateBlockParent() {
		blockParent_ = ecsGroup.CreateEntity("GameEntity");
		blockParent_.parent = entity;

		if (mapData_ != null) {
			int width = mapData_[0].Count;
			int height = mapData_.Count;
			float space = blockData_.blockSpace;
			blockPosOffset_ = new Vector3(width / 2f, 0f, height / 2f) * space;
			blockPosOffset_ -= new Vector3(space / 2f, 0f, space / 2f);
			blockPosOffset_ *= -1.0f;
			blockPosOffset_.y = 2f;

			blockParent_.transform.position = blockPosOffset_;
		}
	}

	private void PlayerDeploy() {
		Debug.Log("----- PlayerDeploy. -----");
		/* ----- プレイヤーの配置 ----- */

		Mapchip mapchipScript = mapchip_.GetScript<Mapchip>();
		if (!mapchipScript) {
			return;
		}

		List<Stage.Player> stagePlayers = new List<Stage.Player>();
		stagePlayers.Add(mapchipScript.GetPlayer());
		if (mapchipScript.GetSubPlayer() != null) {
			stagePlayers.Add(mapchipScript.GetSubPlayer());
		}

		List<Vector2Int> playerAddresses = new List<Vector2Int>();
		players_ = new List<Entity>();

		for (int i = 0; i < stagePlayers.Count; i++) {
			playerAddresses.Add(new Vector2Int(stagePlayers[i].column, stagePlayers[i].row));
			players_.Add(ecsGroup.CreateEntity("PuzzlePlayer"));
		}

		for (int i = 0; i < players_.Count; i++) {
			Entity player = players_[i];
			Stage.Player stagePlayer = stagePlayers[i];

			player.parent = blockParent_;
			if (player.parent != null) {
				Debug.LogInfo("player parent setting");
			} else {
				Debug.Log("player parent not set");
			}

			Vector2Int playerAddress = playerAddresses[i];
			/// 座標設定
			Transform t = player.transform;
			t.position = new Vector3(playerAddress.x * blockData_.blockSpace, 0f,
				playerAddress.y * blockData_.blockSpace);

			/// スクリプトの値設定
			PuzzlePlayer puzzlePlayer = player.GetScript<PuzzlePlayer>();
			if (puzzlePlayer != null) {
				puzzlePlayer.blockData.address = playerAddress; // プレイヤーのアドレスを設定
				puzzlePlayer.blockData.blockSpace = blockData_.blockSpace; // プレイヤーの高さを設定

				if (stagePlayer.type == (int)BlockType.Black) {
					puzzlePlayer.blockData.mapValue = (int)MAPDATA.PLAYER_BLACK;
				} else {
					puzzlePlayer.blockData.mapValue = (int)MAPDATA.PLAYER_WHITE;
				}
			}
		}

		/// アクティブなプレイヤーは最初のプレイヤーで
		activePlayer_ = players_[0];

		Debug.Log("----- PlayerDeploy. ended -----");
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
					block = ecsGroup.CreateEntity("Block");
				} else if (CheckIsGoal(mapValue)) {
					block = ecsGroup.CreateEntity("Goal");
				}

				if (block == null) {
					continue;
				}

				/// blockの初期化
				Block blockScript = block.GetScript<Block>();
				if (blockScript) {
					blockScript.blockData.address = new Vector2Int(c, r);
					blockScript.blockData.blockSpace = blockData_.blockSpace;
					blockScript.blockData.mapValue = mapValue;
				}

				block.parent = blockParent_;
				Transform t = block.transform;

				/// blockのindexで位置を決定
				t.position = new Vector3(c, 0f, r);

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
			float space = blockData_.blockSpace;
			blockPosOffset_ = new Vector3(width / 2f, 0f, height / 2f) * space;
			blockPosOffset_ += new Vector3(space / 2f, 0f, space / 2f);
			blockPosOffset_ *= -1.0f;
			blockPosOffset_.y = 2f;

			blockParent_.transform.position = blockPosOffset_;
		}

		/* パズルを行っているときの更新 */
		UpdatePlayer();
		UpdateEntityPosition();
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
			/// 操作対象の切り替え
			if (Input.TriggerGamepad(Gamepad.Y)) {
				ActivePlayerChange();
			}

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

	private void UpdateEntityPosition() {
		Debug.Log("----- UpdateEntityPosition. -----");

		/// ====================================================
		/// このパズルのエンティティの座標を更新する
		/// ====================================================

		for (int i = 0; i < players_.Count; ++i) {
			Entity player = players_[i];
			PuzzlePlayer pp = player.GetScript<PuzzlePlayer>();
			if (pp) {
				pp.UpdatePosition();
			}
		}

		PuzzlePlayer activePlayer = activePlayer_.GetScript<PuzzlePlayer>();
		for (int i = 0; i < blocks_.Count; i++) {
			Entity block = blocks_[i];
			if (block == null) {
				continue;
			}

			Block blockScript = block.GetScript<Block>();
			if (blockScript) {
				Vector2Int address = blockScript.blockData.address;
				blockScript.blockData.mapValue = mapData_[address.y][address.x];
				blockScript.UpdatePosition(activePlayer.blockData.type);
			}
		}

		Debug.Log("----- UpdateEntityPosition. ended -----");
	}


	private void ActivePlayerChange() {
		/// ====================================================
		/// 操作対象のプレイヤーを切り替える
		/// ====================================================

		for (int i = 0; i < players_.Count; ++i) {
			if (activePlayer_ != players_[i]) {
				activePlayer_ = players_[i];
				break;
			}
		}
	}


	/// ///////////////////////////////////////////////////////////////////////////////////////////
	/// アクセッサ
	/// ///////////////////////////////////////////////////////////////////////////////////////////
	public List<List<int>> GetMapData() {
		return mapData_;
	}

	public List<Entity> GetPlayers() {
		return players_;
	}

	public PuzzlePlayer GetActivePlayer() {
		return activePlayer_.GetScript<PuzzlePlayer>();
	}

	public List<Entity> GetBlocks() {
		return blocks_;
	}
}