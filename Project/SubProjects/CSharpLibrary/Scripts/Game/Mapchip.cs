using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public enum MAPDATA : int {
	BLOCK_WHTIE = 10,
	BLOCK_BLACK = 11,
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
		public Partition partition;
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

	public class Partition {
		public int max;
		public JArray array;
	}
}


public class Mapchip : MonoBehavior {
	// Mapデータの集まり
	private Stage.Root root_;
	private string loadedText_;

	public override void Initialize() {
	}

	public override void Update() {
		// Debug.Log(loadedText_);
	}

	public void LoadMap(string directory, string filename) {
		loadedText_ = Mathf.LoadFile(directory + filename);
		root_ = JsonConvert.DeserializeObject<Stage.Root>(loadedText_);
		root_.map.tiles.Reverse();
	}

	public List<List<int>> GetStartMapData() {
		return root_.map.tiles;
	}

	public Stage.Player GetPlayer() {
		return root_.player;
	}

	public Stage.Player GetSubPlayer() {
		return root_.subPlayer;
	}
}