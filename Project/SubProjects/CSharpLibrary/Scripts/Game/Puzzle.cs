using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Puzzle : MonoBehavior {

	bool isStartPuzzle = false; // パズルが開始されているかどうか


	[SerializeField] float blockSpace = 0.22f; // ブロック間のスペース 0.22f
	[SerializeField] float blockHeight = 2f; // ブロックの高さ 2f
	List<List<int>> mapData;
	List<List<Entity>> blocks;
	Vector3 blockPosOffset; // ブロックの位置オフセット

	public override void Initialize() {
		/*
		 [ mapのデータ詳細 ]
			0: 黒
			1: 白
		*/

		Vector2 playerAddress = new Vector2(1, 1);
		Entity player = EntityCollection.CreateEntity("PuzzlePlayer");
		if (player != null) {
			Transform t = player.transform;
			t.position = new Vector3(
				playerAddress.x * blockSpace,
				0f,
				playerAddress.y * blockSpace
			);
		}



		mapData = new List<List<int>> {
			new List<int> { 0, 1, 0, 1, 0 },
			new List<int> { 1, 0, 1, 0, 1 },
			new List<int> { 0, 1, 0, 1, 0 },
			new List<int> { 1, 0, 1, 0, 1 },
		};


		blocks = new List<List<Entity>>();

		Vector2 mapSize = new Vector2(mapData.Count, mapData[0].Count);
		blockPosOffset = new Vector3(
			-(mapSize.x - 1) * blockSpace / 2f,
			0f,
			-(mapSize.y - 1) * blockSpace / 2f
		);

		//Log.WriteLine("this Id: " + this.entity.Id);
		for (int i = 0; i < mapData.Count; i++) {
			for (int j = 0; j < mapData[i].Count; j++) {
				Log.WriteLine("map[" + i + "][" + j + "] = " + mapData[i][j]);


				Entity block = EntityCollection.CreateEntity("Block");
				if (block == null) {
					continue;
				}

				/// blockの初期化

				block.parent = this.entity;
				Transform t = block.transform;

				/// blockのindexで位置を決定

				t.position = new Vector3(i, blockHeight, j);
				t.position -= blockPosOffset;

				MeshRenderer mr = block.GetComponent<MeshRenderer>();
				if (mr != null) {
					/// 色を黒か白に設定
					Vector4 color = Vector4.one * mapData[i][j]; // 1なら白、0なら黒
					color.w = 1f;

					mr.color = color;
				}


				if (blocks.Count <= i) {
					blocks.Add(new List<Entity>());
				}

				blocks[i].Add(block); // ブロックをリストに追加


			}
		}


	}

	public override void Update() {

		for (int i = 0; i < blocks.Count; i++) {
			for (int j = 0; j < blocks[i].Count; j++) {
				Entity block = blocks[i][j];
				if (block == null) {
					continue;
				}
				Transform t = block.transform;
				t.position = new Vector3(i * blockSpace, blockHeight, j * blockSpace);
				t.position += blockPosOffset;
			}
		}

		/// パズルが開始されていない場合は何もしない
		if (!isStartPuzzle) {
			return;
		}

		Game();

	}


	void Game() {
		/* パズルを行っているときの更新 */

		/// パズルを終了する
		if (Input.TriggerGamepad(Gamepad.B)) {
			/// パズルを終了する
			isStartPuzzle = false;
			Log.WriteLine("Puzzle ended.");
		}

		/// パズルのロジックをここに記述


	}


	public override void OnCollisionEnter(Entity collision) {
	}

	public override void OnCollisionStay(Entity collision) {

		/// 衝突相手がプレイヤーでない場合は何もしない
		if (collision.name != "Player") {
			return;
		}

		/// 
		if (Input.TriggerGamepad(Gamepad.A)) {
			isStartPuzzle = true;
		}

	}



}
