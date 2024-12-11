#pragma once

/// std
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

/// externals
#include <json.hpp>


/// <summary>
/// jsonからファイルを作成する
/// </summary>
/// <param name="_directoryPath">  : ファイルの作成するディレクトリパス </param>
/// <param name="_outputFileName"> : 作成するファイルの名前           </param>
/// <param name="_jsonItme">       : jsonのデータ                   </param>
inline bool CreateJsonFile(
	const std::string& _directoryPath,
	const std::string& _outputFileName,
	nlohmann::json& _jsonItme) {

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(_directoryPath);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}

	///- File open
	std::string path = _directoryPath + _outputFileName + ".json";
	std::ofstream ofs;
	ofs.open(path);

	if(ofs.fail()) {
		return false;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << _jsonItme << std::endl;
	ofs.close();

	return true;
}