#include "AnimationClipLoader.h"

/// std
#include <fstream>
#include <filesystem>

/// external
#include <nlohmann/json.hpp>

using namespace ONEngine::Asset;

/**
 * @brief ディスクからアニメーションクリップ（.anim）ファイルをJSONとしてロードします。
 * @param _filepath ロード対象のファイルパス
 * @param meta アニメーションクリップのメタデータ
 * @return ロードされたAnimationClipアセット（失敗時はstd::nullopt）
 */
std::optional<AnimationClip> AssetLoader<AnimationClip>::Load(const std::string& _filepath, typename Meta<AnimationClip::MetaData> meta) {
    std::ifstream file(_filepath);
    if (!file.is_open()) return std::nullopt;

    nlohmann::json j;
    try {
        file >> j;
    } catch (...) {
        return std::nullopt;
    }

    AnimationClip clip;
    clip.guid = meta.base.guid; // GUIDをMetaデータから適用
    clip.name = j.value("name", "");
    clip.startFrame = j.value("startFrame", 0);
    
    // 互換性のため、endFrameがなければdurationから計算する
    if (j.contains("endFrame")) {
        clip.endFrame = j.at("endFrame").get<int>();
    } else {
        clip.endFrame = static_cast<int>(j.value("duration", 1.0f) * 60.0f);
    }
    
    clip.duration = clip.endFrame / 60.0f; // durationも一応更新
    clip.isLooping = j.value("loop", false);

    if (j.contains("tracks")) {
        for (const auto& trackJson : j["tracks"]) {
            AnimationTrack track;
            track.componentName = trackJson.value("component", "");
            track.propertyPath = trackJson.value("property", "");

            if (trackJson.contains("keyframes")) {
                for (const auto& keyJson : trackJson["keyframes"]) {
                    AnimationKeyframe key;
                    key.time = keyJson.value("t", 0.0f);
                    key.interpolation = keyJson.value("in", "Linear");

                    auto v = keyJson["v"];
                    if (v.is_number()) {
                        key.value = v.get<float>();
                    } else if (v.is_array()) {
                        if (v.size() == 2) key.value = Vector2(v[0], v[1]);
                        else if (v.size() == 3) key.value = Vector3(v[0], v[1], v[2]);
                        else if (v.size() == 4) key.value = Vector4(v[0], v[1], v[2], v[3]);
                    } else if (v.is_object()) {
                        if (v.contains("w")) {
                            key.value = Vector4(v.value("x", 0.0f), v.value("y", 0.0f), v.value("z", 0.0f), v.value("w", 0.0f));
                        } else if (v.contains("z")) {
                            key.value = Vector3(v.value("x", 0.0f), v.value("y", 0.0f), v.value("z", 0.0f));
                        } else if (v.contains("y")) {
                            key.value = Vector2(v.value("x", 0.0f), v.value("y", 0.0f));
                        }
                    }
                    track.keyframes.push_back(key);
                }
            }
            clip.tracks.push_back(track);
        }
    }

    return clip;
}

/**
 * @brief 既存のアニメーションクリップに対して再ロード（リロード）を実行します。
 * @param _filepath 再ロード対象のファイルパス
 * @param _src 再ロード元のAnimationClipオブジェクトへのポインタ
 * @param meta アニメーションクリップのメタデータ
 * @return 再ロードされたAnimationClipアセット（失敗時はstd::nullopt）
 */
std::optional<AnimationClip> AssetLoader<AnimationClip>::Reload(const std::string& _filepath, AnimationClip* /*_src*/, typename Meta<AnimationClip::MetaData> meta) {
    return Load(_filepath, meta);
}

/**
 * @brief アニメーションクリップに対応するメタデータを取得します。
 * @param _filepath 対象アセットファイルのパス
 * @return 解析・構築されたメタデータオブジェクト
 */
Meta<typename AnimationClip::MetaData> AssetLoader<AnimationClip>::GetMetaData(const std::string& _filepath) {
    std::string metaPath = _filepath + ".meta";
    MetaBase base = LoadOrGenerateMetaBase(metaPath, _filepath);
    return { base, AnimationClip::MetaData{} };
}
