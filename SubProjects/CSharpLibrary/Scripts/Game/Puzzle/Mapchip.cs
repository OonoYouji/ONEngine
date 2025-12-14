using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;

public enum MAPDATA : int {
	BLOCK_WHTIE = 10,
	BLOCK_BLACK = 11,
	CONSTANT_BLOCK_WHITE = 30,
	CONSTANT_BLOCK_BLACK = 31,
	GOAL_WHITE = 40,
	GOAL_BLACK = 41,
	PLAYER_WHITE = 100,
	PLAYER_BLACK = 101,
}

namespace Stage {
	public class Root {
		public Map map;
		public Player player;
		public Player subPlayer;
		public PartitionList partitionList;
	}

	public class Map {
		public List<List<int>> tiles; // 2次元リストで可変サイズに対応
	}

	public class Player {
		public int column;
		public int row;
		public int type;
		public int goalType;
	}

	public class PartitionList {
		public int max;
		public List<Partition> date;
	}


	/// <summary>
	/// address1とaddress1の間に配置する
	/// </summary>
	public class Partition {
		public Vector2Int address1;
		public Vector2Int address2;
		public int type;
	}
}


public class Mapchip : MonoScript {
	// Mapデータの集まり
	private Stage.Root root_;
	private string loadedText_;

	public override void Initialize() {
	}

	public override void Update() {
		// Debug.Log(loadedText_);
	}

	public void LoadMap(string directory, string filename) {
		Debug.Log(filename);
		loadedText_ = Mathf.LoadFile(directory + filename);
		root_ = JsonConvert.DeserializeObject<Stage.Root>(loadedText_);
		root_.map.tiles.Reverse();

		/// partitionのデバッグ出力
		Debug.Log("---------------------------------------------------------------");
		if (root_.partitionList != null) {
			foreach (var d in root_.partitionList.date) {
				Debug.Log("address1: x=" + d.address1.x + "  y=" + d.address1.y);
				Debug.Log("address1: x=" + d.address2.x + "  y=" + d.address2.y);
				Debug.Log("type=" + d.type);
			}
		}
		Debug.Log("---------------------------------------------------------------");
	}

	public List<List<int>> GetStartMapData() {
		//return new List<List<int>>(root_.map.tiles);
		return root_.map.tiles
			.Select(inner => new List<int>(inner))
			.ToList();
	}

	public Stage.Player GetPlayer() {
		return root_.player;
	}

	public Stage.Player GetSubPlayer() {
		return root_.subPlayer;
	}
}