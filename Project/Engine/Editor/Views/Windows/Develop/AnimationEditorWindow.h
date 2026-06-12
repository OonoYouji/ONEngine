#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

/// engine
#include "../../EditorViewCollection.h"
#include "Engine/Asset/Assets/Animation/AnimationClip.h"
#include <ImSequencer.h>
#include <ImCurveEdit.h>
#include <memory>

namespace Editor {

/// ImSequencer用のインターフェース実装
struct AnimationSequenceWrapper : public ImSequencer::SequenceInterface {
    ONEngine::Asset::AnimationClip* clip = nullptr;
    int mFrameMin = 0;
    int mFrameMax = 60; // 60fps想定で1秒
    
    // 仮のデータ（Start, End）
    std::vector<std::pair<int, int>> itemFrames;

    AnimationSequenceWrapper() {}

    void SetClip(ONEngine::Asset::AnimationClip* _clip) {
        clip = _clip;
        if (clip) {
            mFrameMax = static_cast<int>(clip->duration * 60.0f);
            itemFrames.resize(clip->tracks.size());
            for (size_t i = 0; i < clip->tracks.size(); ++i) {
                itemFrames[i] = { 0, mFrameMax };
            }
        }
    }

    int GetFrameMin() const override { return mFrameMin; }
    int GetFrameMax() const override { return mFrameMax; }
    int GetItemCount() const override { return clip ? (int)clip->tracks.size() : 0; }

    int GetItemTypeCount() const override { return 0; }
    const char* GetItemTypeName(int typeIndex) const override { return ""; }
    const char* GetItemLabel(int index) const override {
        if (!clip || index < 0 || index >= clip->tracks.size()) return "";
        // キャッシュ用の一時文字列領域はImGuiの仕組みに頼るか工夫が必要
        // 簡易的に直接プロパティパスを返す
        return clip->tracks[index].propertyPath.c_str();
    }

    void Get(int index, int** start, int** end, int* type, unsigned int* color) override {
        if (index < 0 || index >= (int)itemFrames.size()) return;
        if (start) *start = &itemFrames[index].first;
        if (end) *end = &itemFrames[index].second;
        if (type) *type = 0;
        if (color) *color = 0xFFAA8080;
    }

    void Add(int type) override;
    void Del(int index) override;
    void Duplicate(int index) override;
    
    // カスタム描画でキーフレームをプロットする
    size_t GetCustomHeight(int index) override { return 15; }
    void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect) override;

    // --- Interaction State ---
    int draggingTrackIndex = -1;
    int draggingKeyframeIndex = -1;
    int selectedKeyframeIndex = -1;
    int* pSelectedEntry = nullptr; // AnimationEditorWindowのselectedEntryへのポインタ

    // Context menu state
    int contextTrackIndex = -1;
    int contextKeyframeIndex = -1;
    float contextKeyTime = 0.0f;

    // Undo support
    ONEngine::Asset::AnimationClip clipCopy;
};

/// ///////////////////////////////////////////////////
/// アニメーション編集用ウィンドウ
/// ///////////////////////////////////////////////////
class AnimationEditorWindow : public IEditorWindow {
public:
    AnimationEditorWindow();
    ~AnimationEditorWindow() override = default;

    void ShowImGui() override;

private:
    void DrawTimeline();
    void DrawTrackProperties(ONEngine::Asset::AnimationTrack& track);

    std::string windowName_ = "Animation Editor";
    std::string currentClipPath;
    float currentTimelineTime = 0.0f;
    
    AnimationSequenceWrapper sequence;
    int currentFrame = 0;
    bool expanded = true;
    int selectedEntry = -1;
    int firstFrame = 0;

    char addTrackSearchBuf[128] = ""; // New: Search filter for Add Track popup
};

} /// namespace Editor
