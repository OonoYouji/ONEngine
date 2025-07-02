using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Puzzle : MonoBehavior {

	bool isStartPuzzle = false; // パズルが開始されているかどうか

	//Entity blockPrefab = new Block(0,0);

	[SerializeField] float blockSpace = 0.22f; // ブロック間のスペース 0.22f
	[SerializeField] float blockHeight = 2f; // ブロックの高さ 2f
	int[][] map;
	List<List<Entity>> blocks;

	public override void Initialize() {
		/*
		 [ mapのデータ詳細 ]
			0: 黒
			1: 白
		*/

		map = new int[][] {
			new int[] { 0, 1, 0 },
			new int[] { 0, 1, 0 },
			new int[] { 0, 1, 0 }
		};

		blocks = new List<List<Entity>>();


		//Log.WriteLine("this Id: " + this.entity.Id);
		for (int i = 0; i < map.Length; i++) {
			for (int j = 0; j < map[i].Length; j++) {
				Log.WriteLine("map[" + i + "][" + j + "] = " + map[i][j]);


				Entity block = EntityCollection.CreateEntity("Block");
				if (block == null) {
					continue;
				}

				/// blockの初期化

				block.parent = this.entity;
				Transform t = block.transform;

				/// blockのindexで位置を決定

				t.position = new Vector3(i, blockHeight, j);

				MeshRenderer mr = block.GetComponent<MeshRenderer>();
				if (mr != null) {
					/// 色を黒か白に設定
					Vector4 color = Vector4.one * map[i][j]; // 1なら白、0なら黒
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
