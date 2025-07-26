using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Threading;

public class PuzzleCommand {
	public virtual void Execute() {}
	public virtual void Undo() {}
}

/// <summary>
/// コマンドのスタッカー
/// </summary>
public class PuzzleCommandStacker {
	public List<PuzzleCommand> commands;
	
	public void ExecutionCommand<T>() where T : PuzzleCommand {
		T t = Activator.CreateInstance<T>();
		t.Execute();
		commands.Add(t);
	}

	public void UndoCommand<T>() where T : PuzzleCommand {
		var t = commands[commands.Count - 1];
		t.Undo();
		commands.Remove(t);
	}
}