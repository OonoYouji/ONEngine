using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Puzzle : MonoBehavior {

	bool isStartPuzzle = false; // パズルが開始されているかどうか

	//Entity blockPrefab = new Block(0,0);

	int[][] map;

	public override void Initialize() {

		/*
		 [ mapのデータ詳細 ]
			0: 白
			1: 黒
		*/

		map = new int[][] {
			new int[] { 0, 1, 0 },
			new int[] { 0, 1, 0 },
			new int[] { 0, 1, 0 }
		};

		foreach (var row in map) {
			foreach (var cell in row) {
				Log.WriteLine("Cell: " + cell);
			}
		}

	}

	public override void Update() {

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
