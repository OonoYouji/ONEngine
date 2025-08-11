enum BlockType {
	White = 0, // 白ブロック
	Black = 1, // 黒ブロック
}

public struct PuzzleBlockData {
	[SerializeField] public int type; // 色 0:黒, 1:白
	[SerializeField] public int mapValue; // mapDataに入れる値
	public Vector2Int address; // ブロックのアドレス
	public float height; // ブロックの高さ
	public float blockSpace; // ブロック間のスペース
}