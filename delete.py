from pathlib import Path

def delete_meta_json(root_dir: str):
    root = Path(root_dir)

    for filepath in root.rglob("*.meta.json"):
        try:
            filepath.unlink()
            print(f"Deleted: {filepath}")
        except Exception as e:
            print(f"Failed: {filepath} ({e})")


if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        target_dir = sys.argv[1]
    else:
        target_dir = "./Project"

    delete_meta_json(target_dir)
