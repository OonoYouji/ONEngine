using System;
using System.Collections.Generic;
using System.IO;

static public class FileSystem {

	public static List<List<int>> LoadCSV(string filePath) {
		var data = new List<List<int>>();

		if (!File.Exists(filePath)) {
			Console.WriteLine($"[error] Mathf.LoadCSV: Could not open file {filePath}");
			return data;
		}

		try {
			foreach (var line in File.ReadLines(filePath)) {
				var row = new List<int>();
				var cells = line.Split(',');

				foreach (var cell in cells) {
					if (int.TryParse(cell, out int value)) {
						row.Add(value);
					} else {
						Console.WriteLine($"[error] Mathf.LoadCSV: Invalid integer in file {filePath}: {cell}");
					}
				}

				data.Add(row);
			}
		} catch (Exception e) {
			Console.WriteLine($"[error] Mathf.LoadCSV: Exception while reading file {filePath}: {e.Message}");
		}

		return data;
	}

}
