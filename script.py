import json
from pathlib import Path

def parse_meta_file(filepath: Path):
    data = {}

    with filepath.open('r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            if ':' in line:
                key, value = line.split(':', 1)
                key = key.strip()
                value = value.strip()

                if value.isdigit():
                    value = int(value)

                data[key] = value

    return data


def convert_meta_file(filepath: Path):
    data = parse_meta_file(filepath)

    # 同じパスに上書き
    with filepath.open('w', encoding='utf-8') as f:
        json.dump(data, f, indent=4)

    print(f"Converted (overwrite): {filepath}")


def convert_directory(root_dir: str):
    root = Path(root_dir)

    for filepath in root.rglob("*.meta"):
        convert_meta_file(filepath)


if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        target_dir = sys.argv[1]
    else:
        target_dir = "./Project"

    convert_directory(target_dir)
