#pragma once

#include "../IAssetLoader.h"
#include "../../Meta/MetaFile.h"
#include "AudioClip.h"

namespace ONEngine::Asset {

/**
 * @class AssetLoader<AudioClip>
 * @brief AudioClipアセット専用のアセットローダーの特殊化クラス
 */
template<>
class AssetLoader<AudioClip> : public IAssetLoader {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ
	 */
	AssetLoader() = default;

	/**
	 * @brief デストラクタ
	 */
	~AssetLoader() override = default;

	/**
	 * @brief ディスクからWAV形式の音声ファイルをロードします。
	 * @param _filepath ロード対象のファイルパス
	 * @param meta 音声のメタデータ
	 * @return ロードされたAudioClipアセット（失敗時はstd::nullopt）
	 */
	std::optional<AudioClip> Load(const std::string& _filepath, Meta<typename AudioClip::MetaData> meta);

	/**
	 * @brief 既存のAudioClipアセットに対して再ロード（リロード）を実行します。
	 * @param _filepath 再ロード対象のファイルパス
	 * @param _src 再ロード元のAudioClipオブジェクトへのポインタ
	 * @param meta 音声のメタデータ
	 * @return 再ロードされたAudioClipアセット（失敗時はstd::nullopt）
	 */
	std::optional<AudioClip> Reload(const std::string& _filepath, AudioClip* _src = nullptr, Meta<typename AudioClip::MetaData> meta = {});

	/**
	 * @brief 音声アセットに対応するメタデータを取得します。
	 * @param _filepath 対象アセットファイルのパス
	 * @return 解析・構築されたメタデータオブジェクト
	 */
	Meta<typename AudioClip::MetaData> GetMetaData(const std::string& _filepath);

};

} /// namespace ONEngine::Asset