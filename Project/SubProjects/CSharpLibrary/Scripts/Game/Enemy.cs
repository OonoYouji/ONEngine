using System;

public class Enemy : MonoBehavior {

	public override void Initialize() {
		Console.WriteLine("Enemy.Init() called!");
	}

	public override void Update() {
		Console.WriteLine("Enemy.Update() called!");
	}
}