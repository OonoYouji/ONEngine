#pragma once

/// std
#include <string>

/// externals
#include <mono/metadata/object.h>

/// @brief ファイルの処理関数
namespace FileSystemf {

	bool FindFile(const std::string& _directory, const std::string& _filename);
	bool FindFile(const std::string& _path);

	std::string LoadFile(const std::string& _directory, const std::string& _filename);
	std::string LoadFile(const std::string& _path);

}

namespace MONO_INTERNAL_METHOD {

	MonoString* LoadFile(MonoString* _path);
}
