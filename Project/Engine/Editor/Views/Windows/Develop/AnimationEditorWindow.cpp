#include "AnimationEditorWindow.h"

/// std
#include <fstream>
#include <filesystem>
#include <algorithm>

/// external
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <dialog/ImGuiFileDialog.h>
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Asset/Assets/Animation/AnimationClip.h"
#include "Engine/Core/Utility/FileSystem/FileSystem.h"

using namespace Editor;
using namespace ONEngine;

namespace {
    std::string NormalizePath(const std::string& _path) {
        std::string path = _path;
        std::replace(path.begin(), path.end(), '\\', '/');
        // エンジン内部では "./Assets/..." 形式が多いため、それに合わせる
        if (!path.starts_with("./") && !path.starts_with("/") && (path.starts_with("Assets") || path.starts_with("Packages"))) {
            path = "./" + path;
        }
        return path;
    }
}

#include "Engine/Editor/Manager/EditCommand.h"

namespace {
    // AnimationClip全体の変更を記録するコマンド
    class ModifyAnimationClipCommand : public IEditCommand {
    public:
        ModifyAnimationClipCommand(ONEngine::Asset::AnimationClip* _clip, const ONEngine::Asset::AnimationClip& _old, const ONEngine::Asset::AnimationClip& _new)
            : pClip_(_clip), oldClip_(_old), newClip_(_new) {}
        
        EDITOR_STATE Execute() override {
            if (pClip_) *pClip_ = newClip_;
            return EDITOR_STATE_FINISH;
        }
        EDITOR_STATE Undo() override {
            if (pClip_) *pClip_ = oldClip_;
            return EDITOR_STATE_FINISH;
        }
    private:
        ONEngine::Asset::AnimationClip* pClip_;
        ONEngine::Asset::AnimationClip oldClip_, newClip_;
    };
}

// -------------------------------------------------------------
// Sequence Wrapper Interface Implementation
// -------------------------------------------------------------
void AnimationSequenceWrapper::Add(int /*type*/) {
    if (clip) {
        clipCopy = *clip;
        clip->tracks.push_back({ "Transform", "position.x", {} });
        itemFrames.push_back({ 0, mFrameMax });
        EditCommand::Execute<ModifyAnimationClipCommand>(clip, clipCopy, *clip);
    }
}

void AnimationSequenceWrapper::Del(int index) {
    if (clip && index >= 0 && index < (int)clip->tracks.size()) {
        clipCopy = *clip;
        clip->tracks.erase(clip->tracks.begin() + index);
        itemFrames.erase(itemFrames.begin() + index);
        EditCommand::Execute<ModifyAnimationClipCommand>(clip, clipCopy, *clip);
    }
}

void AnimationSequenceWrapper::Duplicate(int index) {
    if (clip && index >= 0 && index < (int)clip->tracks.size()) {
        clipCopy = *clip;
        clip->tracks.push_back(clip->tracks[index]);
        itemFrames.push_back(itemFrames[index]);
        EditCommand::Execute<ModifyAnimationClipCommand>(clip, clipCopy, *clip);
    }
}

// -------------------------------------------------------------
// Sequence Wrapper Custom Draw for Keyframes
// -------------------------------------------------------------
void AnimationSequenceWrapper::CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& /*legendRect*/, const ImRect& clippingRect, const ImRect& /*legendClippingRect*/) {
    if (!clip || index < 0 || index >= (int)clip->tracks.size()) return;
    
    auto& track = clip->tracks[index];
    
    // X軸のピクセルからフレーム/時間への変換
    float framesPerPixel = (float)(mFrameMax - mFrameMin) / rc.GetWidth();
    
    draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
    
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mousePos = io.MousePos;

    bool anyKeyframeHovered = false;

    for (int i = 0; i < (int)track.keyframes.size(); ++i) {
        auto& key = track.keyframes[i];
        // time to frame
        int frame = static_cast<int>(key.time * 60.0f); // 60fps
        float pX = rc.Min.x + (frame - mFrameMin) / framesPerPixel;
        
        if (pX >= clippingRect.Min.x - 5.0f && pX <= clippingRect.Max.x + 5.0f) {
            // Draw diamond for keyframe
            ImVec2 center(pX, rc.Min.y + rc.GetHeight() * 0.5f);
            float s = 4.0f;
            
            // Hit Test
            bool isHovered = false;
            if (clippingRect.Contains(mousePos)) {
                float dx = mousePos.x - center.x;
                float dy = mousePos.y - center.y;
                if (std::abs(dx) < s + 3.0f && std::abs(dy) < s + 3.0f) {
                    isHovered = true;
                    anyKeyframeHovered = true;
                }
            }

            if (isHovered) {
                if (ImGui::IsMouseClicked(0)) {
                    draggingTrackIndex = index;
                    draggingKeyframeIndex = i;
                    selectedKeyframeIndex = i;
                    if (pSelectedEntry) *pSelectedEntry = index;
                    clipCopy = *clip; // Start drag: save state
                } else if (ImGui::IsMouseClicked(1)) {
                    // Right-click to select and open menu
                    selectedKeyframeIndex = i;
                    if (pSelectedEntry) *pSelectedEntry = index;
                    contextTrackIndex = index;
                    contextKeyframeIndex = i;
                    contextKeyTime = key.time;
                    ImGui::OpenPopup("KeyframeContextMenu");
                }
            }

            ImVec2 pts[4] = {
                {center.x, center.y - s},
                {center.x + s, center.y},
                {center.x, center.y + s},
                {center.x - s, center.y}
            };
            
            ImU32 color = IM_COL32(255, 255, 0, 255);
            if (pSelectedEntry && *pSelectedEntry == index && selectedKeyframeIndex == i) {
                color = IM_COL32(255, 255, 255, 255);
                s += 1.5f;
                pts[0] = {center.x, center.y - s};
                pts[1] = {center.x + s, center.y};
                pts[2] = {center.x, center.y + s};
                pts[3] = {center.x - s, center.y};
                
                // Blender-like 'X' key deletion for selected keyframe
                if (ImGui::IsKeyPressed(ImGuiKey_X)) {
                    clipCopy = *clip;
                    track.keyframes.erase(track.keyframes.begin() + i);
                    EditCommand::Execute<ModifyAnimationClipCommand>(clip, clipCopy, *clip);
                    selectedKeyframeIndex = -1;
                    continue;
                }
            } else if (isHovered) {
                color = IM_COL32(255, 200, 0, 255);
            }

            draw_list->AddConvexPolyFilled(pts, 4, color);
        }
    }

    // --- Add Keyframe (K key or Double Click) ---
    if (clippingRect.Contains(mousePos) && !anyKeyframeHovered) {
        bool doubleClicked = ImGui::IsMouseDoubleClicked(0);
        bool kPressed = ImGui::IsKeyPressed(ImGuiKey_K);
        
        if (doubleClicked || kPressed) {
            float newFrame = (mousePos.x - rc.Min.x) * framesPerPixel + mFrameMin;
            float newTime = std::clamp(newFrame / 60.0f, 0.0f, (float)mFrameMax / 60.0f);
            
            // Check for existing key at this time
            bool found = false;
            for (auto& k : track.keyframes) {
                if (std::abs(k.time - newTime) < 0.001f) {
                    found = true; break;
                }
            }
            
            if (!found) {
                clipCopy = *clip;
                std::variant<float, Vector2, Vector3, Vector4> defaultValue = 0.0f;
                if (!track.keyframes.empty()) defaultValue = track.keyframes[0].value;
                
                track.keyframes.push_back({newTime, defaultValue, "Linear"});
                std::sort(track.keyframes.begin(), track.keyframes.end(), [](const auto& a, const auto& b) {
                    return a.time < b.time;
                });
                
                EditCommand::Execute<ModifyAnimationClipCommand>(clip, clipCopy, *clip);

                // Select the new keyframe
                for (int i = 0; i < (int)track.keyframes.size(); ++i) {
                    if (std::abs(track.keyframes[i].time - newTime) < 0.0001f) {
                        selectedKeyframeIndex = i;
                        if (pSelectedEntry) *pSelectedEntry = index;
                        break;
                    }
                }
            }
        }
    }
    
    // --- Context Menu Content ---
    // Note: This logic should ideally be called once per frame outside this track loop,
    // but to keep it simple, we only allow the track matching contextTrackIndex to draw it.
    if (contextTrackIndex == index && ImGui::BeginPopup("KeyframeContextMenu")) {
        if (ImGui::MenuItem("Delete Keyframe")) {
            if (contextKeyframeIndex != -1) {
                clipCopy = *clip;
                // Since index might have changed due to sorting, find key by time
                auto it = std::find_if(track.keyframes.begin(), track.keyframes.end(), [this](const auto& k) {
                    return std::abs(k.time - contextKeyTime) < 0.0001f;
                });
                if (it != track.keyframes.end()) {
                    track.keyframes.erase(it);
                    EditCommand::Execute<ModifyAnimationClipCommand>(clip, clipCopy, *clip);
                }
                selectedKeyframeIndex = -1;
                contextKeyframeIndex = -1;
                contextTrackIndex = -1;
            }
        }
        ImGui::EndPopup();
    }
    
    // Dragging Logic
    if (draggingTrackIndex == index && draggingKeyframeIndex != -1) {
        if (ImGui::IsMouseDragging(0)) {
            float newFrame = (mousePos.x - rc.Min.x) * framesPerPixel + mFrameMin;
            track.keyframes[draggingKeyframeIndex].time = std::clamp(newFrame / 60.0f, 0.0f, (float)mFrameMax / 60.0f);
        }
        
        if (ImGui::IsMouseReleased(0)) {
            // Sort and update selectedKeyframeIndex
            auto selectedKeyTime = track.keyframes[draggingKeyframeIndex].time;
            std::sort(track.keyframes.begin(), track.keyframes.end(), [](const auto& a, const auto& b) {
                return a.time < b.time;
            });
            
            EditCommand::Execute<ModifyAnimationClipCommand>(clip, clipCopy, *clip);

            // Find new index of selected keyframe
            for (int i = 0; i < (int)track.keyframes.size(); ++i) {
                if (std::abs(track.keyframes[i].time - selectedKeyTime) < 0.0001f) {
                    selectedKeyframeIndex = i;
                    break;
                }
            }
            
            draggingTrackIndex = -1;
            draggingKeyframeIndex = -1;
        }
    }
    
    draw_list->PopClipRect();
}

// -------------------------------------------------------------
// Animation Editor Window
// -------------------------------------------------------------
AnimationEditorWindow::AnimationEditorWindow() {
    sequence.pSelectedEntry = &selectedEntry;
}

void AnimationEditorWindow::ShowImGui() {
    if (!ImGui::Begin(windowName_.c_str())) {
        ImGui::End();
        return;
    }

    // ツールバー
    if (ImGui::Button("Open Clip")) {
        std::filesystem::path animPath = std::filesystem::absolute("./Assets/Anims");
        std::filesystem::create_directories(animPath);

        IGFD::FileDialogConfig config;
        config.path = animPath.string();
        ImGuiFileDialog::Instance()->OpenDialog("OpenAnimDialog", "Choose AnimationClip", ".anim", config);
    }
    ImGui::SameLine();
    if (ImGui::Button("New Clip")) {
        std::filesystem::path animPath = std::filesystem::absolute("./Assets/Anims");
        std::filesystem::create_directories(animPath);

        IGFD::FileDialogConfig config;
        config.path = animPath.string();
        ImGuiFileDialog::Instance()->OpenDialog("NewAnimDialog", "Create New AnimationClip", ".anim", config);
    }

    ImGui::Text("Current Path: %s", currentClipPath.c_str());

    auto* ac = ONEngine::Asset::AssetCollection::GetInstance();

    // ダイアログ処理
    if (ImGuiFileDialog::Instance()->Display("OpenAnimDialog")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string fullPath = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string relative = std::filesystem::relative(fullPath, std::filesystem::current_path()).string();
            currentClipPath = NormalizePath(relative);
            ac->ReloadAsset(currentClipPath);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("NewAnimDialog")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string fullPath = ImGuiFileDialog::Instance()->GetFilePathName();
            if (std::filesystem::path(fullPath).extension() != ".anim") {
                fullPath += ".anim";
            }
            std::string relative = std::filesystem::relative(fullPath, std::filesystem::current_path()).string();
            currentClipPath = NormalizePath(relative);

            std::filesystem::path fsPath(currentClipPath);
            if (fsPath.has_parent_path()) {
                std::filesystem::create_directories(fsPath.parent_path());
            }

            nlohmann::json j;
            j["name"] = fsPath.stem().string();
            j["startFrame"] = 0;
            j["endFrame"] = 60;
            j["loop"] = true;
            j["tracks"] = nlohmann::json::array();
            j["tracks"].push_back({ {"component", "Transform"}, {"property", "position"}, {"keyframes", nlohmann::json::array()} });

            std::ofstream ofs(currentClipPath);
            if (ofs.is_open()) {
                ofs << j.dump(4);
                ofs.close();
                ac->ReloadAsset(currentClipPath);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    auto* clip = ac->GetAnimationClip(currentClipPath);

    if (clip) {
        ONEngine::Asset::AnimationClip* mutableClip = const_cast<ONEngine::Asset::AnimationClip*>(clip);
        
        // クリップが切り替わった、またはトラック数・範囲が変わった時だけ初期化
        if (sequence.clip != mutableClip || sequence.GetItemCount() != (int)mutableClip->tracks.size() || 
            sequence.mFrameMin != mutableClip->startFrame || sequence.mFrameMax != mutableClip->endFrame) {
            sequence.mFrameMin = mutableClip->startFrame;
            sequence.mFrameMax = mutableClip->endFrame;
            sequence.SetClip(mutableClip);
        }

        // Control Panel
        ImGui::Separator();
        ImGui::Text("Clip: %s", mutableClip->name.c_str());

        ImGui::BeginGroup();
        bool rangeChanged = false;
        
        rangeChanged |= ImGui::DragInt("Start Frame", &mutableClip->startFrame, 1, 0, mutableClip->endFrame - 1);
        if (ImGui::IsItemActivated()) sequence.clipCopy = *mutableClip;
        if (ImGui::IsItemDeactivatedAfterEdit()) EditCommand::Execute<ModifyAnimationClipCommand>(mutableClip, sequence.clipCopy, *mutableClip);

        rangeChanged |= ImGui::DragInt("End Frame", &mutableClip->endFrame, 1, mutableClip->startFrame + 1, 10000);
        if (ImGui::IsItemActivated()) sequence.clipCopy = *mutableClip;
        if (ImGui::IsItemDeactivatedAfterEdit()) EditCommand::Execute<ModifyAnimationClipCommand>(mutableClip, sequence.clipCopy, *mutableClip);

        if (rangeChanged) {
            mutableClip->duration = mutableClip->endFrame / 60.0f;
            sequence.mFrameMin = mutableClip->startFrame;
            sequence.mFrameMax = mutableClip->endFrame;
            sequence.SetClip(mutableClip);
        }
        
        bool loopChanged = ImGui::Checkbox("Looping", &mutableClip->isLooping);
        if (loopChanged) {
            auto oldClip = *mutableClip;
            oldClip.isLooping = !mutableClip->isLooping; // checkbox already flipped it
            EditCommand::Execute<ModifyAnimationClipCommand>(mutableClip, oldClip, *mutableClip);
        }
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 150);
        if (ImGui::Button("Add Track...", ImVec2(130, 40))) {
            ImGui::OpenPopup("AddTrackPopup");
        }

        if (ImGui::BeginPopup("AddTrackPopup")) {
            ImGui::SetNextItemWidth(200);
            if (ImGui::InputText("Search", addTrackSearchBuf, sizeof(addTrackSearchBuf))) {
                // 自動で小文字にするなどの処理が必要ならここで行う
            }
            ImGui::Separator();

            auto addTrack = [&](const std::string& comp, const std::string& prop, const std::variant<float, Vector2, Vector3, Vector4>& val) {
                sequence.clipCopy = *mutableClip;
                mutableClip->tracks.push_back({ comp, prop, { {mutableClip->startFrame / 60.0f, val, "Linear"} } });
                sequence.SetClip(mutableClip);
                selectedEntry = (int)mutableClip->tracks.size() - 1;
                EditCommand::Execute<ModifyAnimationClipCommand>(mutableClip, sequence.clipCopy, *mutableClip);
                addTrackSearchBuf[0] = '\0'; // 成功したらクリア
                ImGui::CloseCurrentPopup();
            };

            std::string search = addTrackSearchBuf;
            auto matches = [&](const std::string& text) {
                if (search.empty()) return true;
                std::string t = text;
                std::transform(t.begin(), t.end(), t.begin(), ::tolower);
                std::string s = search;
                std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                return t.find(s) != std::string::npos;
            };

            // --- Define all possible tracks ---
            struct TrackTemplate { std::string label; std::string comp; std::string prop; std::variant<float, Vector2, Vector3, Vector4> val; };
            std::vector<TrackTemplate> templates = {
                { "Transform/Position (Vec3)", "Transform", "position", Vector3(0,0,0) },
                { "Transform/Rotation (Euler)", "Transform", "rotation", Vector3(0,0,0) },
                { "Transform/Scale (Vec3)", "Transform", "scale", Vector3(1,1,1) },
                { "MeshRenderer/UV Offset (Vec2)", "MeshRenderer", "uvOffset", Vector2(0,0) },
                { "MeshRenderer/UV Scale (Vec2)", "MeshRenderer", "uvScale", Vector2(1,1) },
                { "MeshRenderer/UV Rotation (Float)", "MeshRenderer", "uvRotation", 0.0f },
                { "MeshRenderer/Color (Vec4)", "MeshRenderer", "color", Vector4(1,1,1,1) },
                { "DissolveMesh/Threshold (Float)", "DissolveMeshRenderer", "threshold", 1.0f },
                { "DissolveMesh/Edge Width (Float)", "DissolveMeshRenderer", "edgeWidth", 0.05f },
                { "DissolveMesh/Edge Color (Vec4)", "DissolveMeshRenderer", "edgeColor", Vector4(1,0.5f,0,1) },
                { "DissolveMesh/UV Offset (Vec2)", "DissolveMeshRenderer", "uvOffset", Vector2(0,0) },
                { "SpriteRenderer/UV Offset (Vec2)", "SpriteRenderer", "uvOffset", Vector2(0,0) },
                { "SpriteRenderer/Color (Vec4)", "SpriteRenderer", "color", Vector4(1,1,1,1) },
                { "Particle/Emission Rate (Float)", "ParticleSystem", "emission.rateOverTime", 10.0f },
                { "Particle/Emission Enabled (Bool)", "ParticleSystem", "emission.enabled", 1.0f },
                { "Particle/Start Color (Vec4)", "ParticleSystem", "main.startColor", Vector4(1,1,1,1) },
                { "Particle/Start Speed (Float)", "ParticleSystem", "main.startSpeed", 5.0f },
                { "Particle/Start Size (Float)", "ParticleSystem", "main.startSize", 1.0f },
                { "Light/Intensity (Float)", "Light", "intensity", 1.0f },
                { "Light/Color (Vec4)", "Light", "color", Vector4(1,1,1,1) },
                { "Custom (Float)", "Transform", "position.x", 0.0f }
            };

            std::string lastGroup = "";
            for (const auto& t : templates) {
                if (matches(t.label)) {
                    std::string group = t.label.substr(0, t.label.find('/'));
                    if (lastGroup != "" && group != lastGroup) ImGui::Separator();
                    lastGroup = group;

                    if (ImGui::MenuItem(t.label.c_str())) addTrack(t.comp, t.prop, t.val);
                }
            }
            
            ImGui::EndPopup();
        }

        if (ImGui::Button("Save Clip")) {
            nlohmann::json j;
            j["name"] = mutableClip->name;
            j["startFrame"] = mutableClip->startFrame;
            j["endFrame"] = mutableClip->endFrame;
            j["duration"] = mutableClip->duration;
            j["loop"] = mutableClip->isLooping;
            j["tracks"] = nlohmann::json::array();
            for (const auto& track : mutableClip->tracks) {
                nlohmann::json t;
                t["component"] = track.componentName;
                t["property"] = track.propertyPath;
                t["keyframes"] = nlohmann::json::array();
                for (const auto& key : track.keyframes) {
                    nlohmann::json k;
                    k["t"] = key.time;
                    k["in"] = key.interpolation;
                    std::visit([&k](auto&& arg) { k["v"] = arg; }, key.value);
                    t["keyframes"].push_back(k);
                }
                j["tracks"].push_back(t);
            }
            std::ofstream ofs(currentClipPath);
            if (ofs.is_open()) {
                ofs << j.dump(4);
                ofs.close();
                ONEngine::Console::Log("Saved AnimationClip to: " + currentClipPath);
                ac->ReloadAsset(currentClipPath);
            }
        }

        ImGui::Separator();
        
        // --- トラック選択とタイムラインを横並びにする ---
        ImGui::BeginChild("SequencerRegion", ImVec2(0, 300), true);
        {
            // 左側：トラック名リスト
            ImGui::BeginGroup();
            ImGui::Text("Tracks");
            ImGui::BeginChild("TrackList", ImVec2(150, 0), true);
            for (int i = 0; i < (int)mutableClip->tracks.size(); ++i) {
                bool isSelected = (selectedEntry == i);
                const std::string& propName = mutableClip->tracks[i].propertyPath;
                std::string displayName = propName.empty() ? "(Empty Property)" : propName;
                
                if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                    if (selectedEntry != i) {
                        selectedEntry = i;
                        sequence.selectedKeyframeIndex = -1;
                    }
                }

                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete Track")) {
                        sequence.Del(i);
                        selectedEntry = -1;
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::EndChild();
            ImGui::EndGroup();

            ImGui::SameLine();

            // 右側：タイムライン本体
            ImGui::BeginGroup();
            DrawTimeline();
            ImGui::EndGroup();
        }
        ImGui::EndChild();

        // 下部：詳細編集領域
        if (selectedEntry >= 0 && selectedEntry < (int)mutableClip->tracks.size()) {
            ImGui::Separator();

            if (ImGui::Button("Add Keyframe at Current Time", ImVec2(-1, 35))) {
                sequence.clipCopy = *mutableClip;
                auto& track = mutableClip->tracks[selectedEntry];
                std::variant<float, Vector2, Vector3, Vector4> defaultValue = 0.0f;
                if (!track.keyframes.empty()) defaultValue = track.keyframes[0].value;

                bool found = false;
                for (auto& k : track.keyframes) {
                    if (std::abs(k.time - currentTimelineTime) < 0.001f) {
                        found = true; break;
                    }
                }
                if (!found) {
                    track.keyframes.push_back({currentTimelineTime, defaultValue, "Linear"});
                    std::sort(track.keyframes.begin(), track.keyframes.end(), [](const ONEngine::Asset::AnimationKeyframe& a, const ONEngine::Asset::AnimationKeyframe& b) {
                        return a.time < b.time;
                    });
                    EditCommand::Execute<ModifyAnimationClipCommand>(mutableClip, sequence.clipCopy, *mutableClip);
                }
            }

            ImGui::Text("Track Details: %d (%s)", selectedEntry, mutableClip->tracks[selectedEntry].propertyPath.c_str());
            
            ImGui::BeginChild("DetailsRegion", ImVec2(0, 0), false);
            DrawTrackProperties(mutableClip->tracks[selectedEntry]);
            ImGui::EndChild();
        }

    } else if (!currentClipPath.empty()) {
        ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "File not found in Collection: %s", currentClipPath.c_str());
        if (ImGui::Button("Try Load Manually")) {
            ac->ReloadAsset(currentClipPath);
        }
    }

    ImGui::End();
}

void AnimationEditorWindow::DrawTimeline() {
    int sequenceOptions = ImSequencer::SEQUENCER_EDIT_ALL | ImSequencer::SEQUENCER_CHANGE_FRAME;
    
    // Timeline GUI 描画
    ImSequencer::Sequencer(&sequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, sequenceOptions);

    // 時間の同期 (Frame -> Time)
    currentTimelineTime = (float)currentFrame / 60.0f;
}

void AnimationEditorWindow::DrawTrackProperties(ONEngine::Asset::AnimationTrack& track) {
    auto* clip = sequence.clip;
    if (!clip) return;

    char compBuf[64], propBuf[64];
    strncpy_s(compBuf, track.componentName.c_str(), sizeof(compBuf));
    strncpy_s(propBuf, track.propertyPath.c_str(), sizeof(propBuf));

    if (ImGui::InputText("Component", compBuf, sizeof(compBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        sequence.clipCopy = *clip;
        track.componentName = compBuf;
        EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        // Handle case where they clicked away without pressing Enter
        if (track.componentName != compBuf) {
            sequence.clipCopy = *clip;
            track.componentName = compBuf;
            EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
        }
    }

    if (ImGui::InputText("Property", propBuf, sizeof(propBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        sequence.clipCopy = *clip;
        track.propertyPath = propBuf;
        EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (track.propertyPath != propBuf) {
            sequence.clipCopy = *clip;
            track.propertyPath = propBuf;
            EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
        }
    }

    // 型の切り替え機能
    const char* typeItems[] = { "Float", "Vector2", "Vector3", "Vector4" };
    int currentType = 0;
    if (!track.keyframes.empty()) {
        if (std::holds_alternative<float>(track.keyframes[0].value)) currentType = 0;
        else if (std::holds_alternative<Vector2>(track.keyframes[0].value)) currentType = 1;
        else if (std::holds_alternative<Vector3>(track.keyframes[0].value)) currentType = 2;
        else if (std::holds_alternative<Vector4>(track.keyframes[0].value)) currentType = 3;
    }

    if (ImGui::Combo("Value Type", &currentType, typeItems, IM_ARRAYSIZE(typeItems))) {
        sequence.clipCopy = *clip;
        for (auto& key : track.keyframes) {
            if (currentType == 0) key.value = 0.0f;
            else if (currentType == 1) key.value = Vector2(0, 0);
            else if (currentType == 2) key.value = Vector3(0, 0, 0);
            else if (currentType == 3) key.value = Vector4(0, 0, 0, 1);
        }
        EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
    }

    if (ImGui::TreeNodeEx("Keyframes Detail", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (int i = 0; i < (int)track.keyframes.size(); ++i) {
            ImGui::PushID(i);
            auto& key = track.keyframes[i];
            
            bool isSelected = (sequence.selectedKeyframeIndex == i);
            char keyLabel[64];
            sprintf_s(keyLabel, "Keyframe %d (Frame: %d)", i, static_cast<int>(key.time * 60.0f));
            
            if (ImGui::Selectable(keyLabel, isSelected)) {
                sequence.selectedKeyframeIndex = i;
            }
            if (isSelected) ImGui::Separator();

            int frame = static_cast<int>(key.time * 60.0f);
            if (ImGui::DragInt("Frame", &frame, 1.0f, 0, sequence.mFrameMax)) {
                key.time = (float)frame / 60.0f;
            }
            if (ImGui::IsItemActivated()) sequence.clipCopy = *clip;
            if (ImGui::IsItemDeactivatedAfterEdit()) EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
            
            bool valueChanged = false;
            if (std::holds_alternative<float>(key.value)) {
                float v = std::get<float>(key.value);
                if (ImGui::DragFloat("Value", &v, 0.1f)) { key.value = v; valueChanged = true; }
            } else if (std::holds_alternative<Vector3>(key.value)) {
                Vector3 v = std::get<Vector3>(key.value);
                if (ImGui::DragFloat3("Value", &v.x, 0.1f)) { key.value = v; valueChanged = true; }
            } else if (std::holds_alternative<Vector2>(key.value)) {
                Vector2 v = std::get<Vector2>(key.value);
                if (ImGui::DragFloat2("Value", &v.x, 0.1f)) { key.value = v; valueChanged = true; }
            } else if (std::holds_alternative<Vector4>(key.value)) {
                Vector4 v = std::get<Vector4>(key.value);
                std::string propLower = track.propertyPath;
                std::transform(propLower.begin(), propLower.end(), propLower.begin(), ::tolower);
                
                if (propLower.find("color") != std::string::npos) {
                    if (ImGui::ColorEdit4("Color", &v.x)) { key.value = v; valueChanged = true; }
                } else {
                    if (ImGui::DragFloat4("Value", &v.x, 0.1f)) { key.value = v; valueChanged = true; }
                }
            }
            if (ImGui::IsItemActivated()) sequence.clipCopy = *clip;
            if (ImGui::IsItemDeactivatedAfterEdit()) EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
            
            const char* items[] = { "Linear", "Step" };
            int current_item = (key.interpolation == "Step") ? 1 : 0;
            if (ImGui::Combo("Interpolation", &current_item, items, IM_ARRAYSIZE(items))) {
                sequence.clipCopy = *clip;
                key.interpolation = items[current_item];
                EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
            }

            if (ImGui::Button("Remove")) {
                sequence.clipCopy = *clip;
                track.keyframes.erase(track.keyframes.begin() + i);
                EditCommand::Execute<ModifyAnimationClipCommand>(clip, sequence.clipCopy, *clip);
                ImGui::PopID();
                break;
            }
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}
