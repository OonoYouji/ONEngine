
enum BlockType {
	Black = 0, // 黒ブロック
	White = 1, // 白ブロック
}

public struct PuzzleBlockData {
	public int type; // 色 0:黒, 1:白
	public Vector2Int address; // ブロックのアドレス
	public float height; // ブロックの高さ
	public float blockSpace; // ブロック間のスペース
}