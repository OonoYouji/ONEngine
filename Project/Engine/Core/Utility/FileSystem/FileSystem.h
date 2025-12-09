#pragma once

/// std
#include <string>

/// externals
#include <mono/metadata/object.h>

namespace ONEngine {

/// //////////////////////////////////////////////////////
/// ファイルの処理関数
/// //////////////////////////////////////////////////////
namespace FileSystemf {

/// @brief ファイルを探す
/// @param _directory ファイルディレクトリ
/// @param _filename ファイル名
/// @return true: 見つかった / false: 見つからなかった
bool FindFile(const std::string& _directory, const std::string& _filename);

/// @brief ファイルを探す
/// @param _path 探索するファイルのパス
/// @return true: 見つかった / false: 見つからなかった
bool FindFile(const std::string& _path);


/// @brief ファイルを読み込む
/// @param _directory ファイルディレクト
/// @param _filename ファイル名
/// @return 読み込んだファイルのテキスト内容、失敗したら空文字列
std::string LoadFile(const std::string& _directory, const std::string& _filename);

/// @brief ファイルを読み込む
/// @param _path 読み込むファイルのパス
/// @return 読み込んだファイルのテキスト内容、失敗したら空文字列
std::string LoadFile(const std::string& _path);

}


/// //////////////////////////////////////////////////////
/// Monoから呼び出すための内部関数群
/// //////////////////////////////////////////////////////
namespace MONO_INTERNAL_METHOD {

/// @brief ファイルを読み込む
MonoString* LoadFile(MonoString* _path);
}

} /// namespace ONEngine