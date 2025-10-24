#include "FileSsytem.h"

/// std
#include <filesystem>
#include <fstream>

/// externals
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"

namespace fs = std::filesystem;


bool FileSystemf::FindFile(const std::string& _directory, const std::string& _filename) {
	/// ----- 指定したディレクトリ内にファイルが存在するか確認 ----- ///

	fs::path dirPath(_directory);

	try {

		// ディレクトリが存在するか確認
		if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
			return false;
		}

		// ディレクトリ内を走査
		for (const auto& entry : fs::directory_iterator(dirPath)) {
			if (entry.is_regular_file() && entry.path().filename() == _filename) {
				return true; // 見つかった
			}
		}

	} catch (const std::exception& _exc) {
		Console::LogWarning(_exc.what());
		return false;
	}

	return false;
}

bool FileSystemf::FindFile(const std::string& _path) {
	return std::filesystem::exists(_path);
}

std::string FileSystemf::LoadFile(const std::string& _directory, const std::string& _filename) {
	/// ----- ファイルを読み込む ----- ///

	if (!FindFile(_directory, _filename)) {
		return "";
	}

	/// パスをフルパスに変換
	std::filesystem::path dir(_directory);
	std::filesystem::path filename(_filename);
	std::filesystem::path fullPath = dir / filename;

	return LoadFile(fullPath.string());
}

std::string FileSystemf::LoadFile(const std::string& _path) {
	/// ----- ファイルを読み込む ----- ///

	// ファイルストリームで読み込み
	std::ifstream file(_path);
	if (!file.is_open()) {
		return ""; // 開けなかった場合も空文字列
	}

	/// ファイルの中身をテキストに
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}

MonoString* MONO_INTERNAL_METHOD::LoadFile(MonoString* _path) {

	/// スクリプト名をUTF-8に変換
	char* cstr = mono_string_to_utf8(_path);
	std::string pathStr(cstr);
	mono_free(cstr);

	std::string fileText = FileSystemf::LoadFile(pathStr);
	MonoString* monoStr = mono_string_new(mono_domain_get(), fileText.c_str());

	return monoStr;
}
