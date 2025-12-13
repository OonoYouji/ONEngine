#include "VectorWidget.h"

/// std
#include <string>
#include <cstdio>
#include <cmath>
#include <type_traits>

/// externals
#include <imgui.h>
#include <imgui_internal.h>

/// editor
#include "Engine/Editor/Manager/EditCommand.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"



using namespace Editor;


namespace {

// ==========================================================
// 内部用テンプレート関数
// ==========================================================
template<typename TVector, typename TValue, int N>
static bool DrawVecControlT(const std::string& _label, TVector& _values, float _speed, TValue _min, TValue _max, float _columnWidth, bool* _unified) {

	constexpr bool kIsFloat = std::is_floating_point_v<TValue>;
	bool* unifiedPtr = kIsFloat ? _unified : nullptr;
	bool valueChanged = false;

	// 制限有効かどうかの判定 (minもmaxも0なら制限なし)
	bool hasLimits = (_min != static_cast<TValue>(0) || _max != static_cast<TValue>(0));

	// --- 定数定義 ---
	constexpr float kSafetyMarginWidth = 1.0f;
	constexpr float kButtonPaddingW = 3.0f;
	constexpr float kFramePaddingScale = 2.0f;
	constexpr float kSpacingScale = 2.0f;
	constexpr float kMinInputWidth = 10.0f;
	constexpr int   kNumColumns = 2;

	const ImVec4 kColorX = ImVec4(0.8f, 0.1f, 0.15f, 1.0f);
	const ImVec4 kColorY = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
	const ImVec4 kColorZ = ImVec4(0.1f, 0.25f, 0.8f, 1.0f);
	const ImVec4 kColorW = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

	constexpr float kColorHoverOffset = 0.1f;
	constexpr float kColorActiveOffset = 0.2f;
	constexpr float kZeroEpsilon = 1e-6f;
	constexpr int   kTextBufferSize = 64;
	const ImVec2    kTextAlignCenter = ImVec2(0.5f, 0.5f);

	static TVector s_startValue;

	ImGui::PushID(_label.c_str());

	TVector beforeValues = _values;

	// リサイズ不可など
	ImGuiTableFlags tableFlags = ImGuiTableFlags_NoSavedSettings;

	if(ImGui::BeginTable("##VecControlTable", kNumColumns, tableFlags)) {
		ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, _columnWidth);
		ImGui::TableSetupColumn("##Values", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		// ラベル列
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", _label.c_str());

		// 値操作列
		ImGui::TableNextColumn();
		ImGui::PushID("##VecVals");

		// レイアウト計算
		float availWidth = ImGui::GetContentRegionAvail().x - kSafetyMarginWidth;
		float itemSpacing = GImGui->Style.ItemSpacing.x;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * kFramePaddingScale;
		ImVec2 buttonSize = { lineHeight + kButtonPaddingW, lineHeight };

		float checkboxWidth = (unifiedPtr != nullptr) ? (ImGui::GetFrameHeight() + itemSpacing) : 0.0f;
		float totalAxesWidth = availWidth - checkboxWidth - (itemSpacing * kSpacingScale);
		float axisWidth = totalAxesWidth / static_cast<float>(N);
		float inputWidth = (std::max)(kMinInputWidth, axisWidth - buttonSize.x);

		// Unified Checkbox
		if(unifiedPtr != nullptr) {
			ImGui::Checkbox("##Unified", unifiedPtr);
			if(ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Uniform Scale (Lock Ratio)");
			}
			ImGui::SameLine();
		}

		// ポインタ構築
		TValue* axisValues[4] = { nullptr, nullptr, nullptr, nullptr };
		TValue beforeAxisValues[4] = { 0, 0, 0, 0 };

		axisValues[0] = &_values.x; beforeAxisValues[0] = beforeValues.x;
		axisValues[1] = &_values.y; beforeAxisValues[1] = beforeValues.y;
		if constexpr(N >= 3) { axisValues[2] = &_values.z; beforeAxisValues[2] = beforeValues.z; }
		if constexpr(N >= 4) { axisValues[3] = &_values.w; beforeAxisValues[3] = beforeValues.w; }

		const char* axisLabels[] = { "X", "Y", "Z", "W" };
		const ImVec4 axisColors[] = { kColorX, kColorY, kColorZ, kColorW };

		ImGuiID nextFocusID = ImGui::GetID("##nextFocusAxis");
		int focusAxisIdx = ImGui::GetStateStorage()->GetInt(nextFocusID, -1);
		if(focusAxisIdx != -1) ImGui::GetStateStorage()->SetInt(nextFocusID, -1);

		// ---------------------------------------------------------------------
		// [修正点1] 全ての軸が0かどうかを判定する
		// ---------------------------------------------------------------------
		bool allZeros = true;
		for(int i = 0; i < N; ++i) {
			bool isAxisZero = false;
			if constexpr(kIsFloat) isAxisZero = fabsf(*axisValues[i]) < kZeroEpsilon;
			else isAxisZero = (*axisValues[i] == 0);

			if(!isAxisZero) {
				allZeros = false;
				break;
			}
		}

		for(int i = 0; i < N; ++i) {
			ImGui::PushID(i);

			bool isZero = false;
			if constexpr(kIsFloat) isZero = fabsf(*axisValues[i]) < kZeroEpsilon;
			else isZero = (*axisValues[i] == 0);

			bool isUnified = (unifiedPtr && *unifiedPtr);

			// ---------------------------------------------------------------------
			// [修正点2] ロック条件を変更
			// Unified有効 かつ 現在の軸が0 かつ 「全ての軸が0」ではない場合のみロックする
			// (全て0の場合はロックせず、1:1:1として扱えるようにする)
			// ---------------------------------------------------------------------
			bool isLocked = isUnified && isZero && !allZeros;

			if(isLocked) ImGui::BeginDisabled(true);

			// --- Button ---
			ImVec4 baseColor = axisColors[i];
			ImVec4 hoverColor = ImVec4(baseColor.x + kColorHoverOffset, baseColor.y + kColorHoverOffset, baseColor.z + kColorHoverOffset, 1.0f);
			ImVec4 activeColor = ImVec4(baseColor.x + kColorActiveOffset, baseColor.y + kColorActiveOffset, baseColor.z + kColorActiveOffset, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);

			ImGui::Button(axisLabels[i], buttonSize);

			if(ImGui::IsItemActivated()) s_startValue = _values;
			if(ImGui::IsItemDeactivated()) {
				// 変更チェック
			}

			bool buttonActive = ImGui::IsItemActive();
			bool buttonDragged = buttonActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			if(!isLocked && (ImGui::IsItemHovered() || buttonActive)) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}

			ImGui::PopStyleColor(3);
			ImGui::SameLine(0, 0);

			// --- Input ---
			ImGui::SetNextItemWidth(inputWidth);
			ImGuiID inputID = ImGui::GetID("##v");
			ImGuiID focusReqID = ImGui::GetID("##req_focus");

			if(focusAxisIdx == i && !isLocked) {
				ImGui::GetStateStorage()->SetBool(inputID, true);
				ImGui::GetStateStorage()->SetBool(focusReqID, true);
			}

			bool isEditing = ImGui::GetStateStorage()->GetBool(inputID, false);
			bool inputChanged = false;

			if(isEditing && !isLocked) {
				if(ImGui::GetStateStorage()->GetBool(focusReqID, false)) {
					ImGui::SetKeyboardFocusHere(0);
					ImGui::GetStateStorage()->SetBool(focusReqID, false);
				}

				if constexpr(kIsFloat) {
					inputChanged = ImGui::InputFloat("##v", (float*)axisValues[i], 0.0f, 0.0f, "%.2f");
				} else {
					inputChanged = ImGui::InputInt("##v", (int*)axisValues[i], 0, 0);
				}

				if(ImGui::IsItemActivated()) s_startValue = _values;
				if(ImGui::IsItemDeactivatedAfterEdit()) {
					// コマンド実行
				}

				if(inputChanged && hasLimits) {
					*axisValues[i] = std::clamp(*axisValues[i], _min, _max);
				}

				bool tabPressed = ImGui::IsKeyPressed(ImGuiKey_Tab);
				bool shiftPressed = ImGui::GetIO().KeyShift;

				if(tabPressed && (ImGui::IsItemActive() || ImGui::IsItemDeactivated())) {
					ImGui::GetStateStorage()->SetBool(inputID, false);
					int direction = shiftPressed ? -1 : 1;
					int targetIdx = i + direction;
					if(targetIdx >= 0 && targetIdx < N) {
						if(direction > 0) focusAxisIdx = targetIdx;
						else ImGui::GetStateStorage()->SetInt(nextFocusID, targetIdx);
					}
				} else if(ImGui::IsItemDeactivated()) {
					ImGui::GetStateStorage()->SetBool(inputID, false);
				}
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, kTextAlignCenter);

				char buf[kTextBufferSize];
				if constexpr(kIsFloat) sprintf_s(buf, kTextBufferSize, "%.2f", (float)*axisValues[i]);
				else sprintf_s(buf, kTextBufferSize, "%d", (int)*axisValues[i]);

				if(ImGui::Button(buf, ImVec2(inputWidth, 0))) {
					ImGui::GetStateStorage()->SetBool(inputID, true);
					ImGui::GetStateStorage()->SetBool(focusReqID, true);
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleColor(3);
			}

			if(isLocked) ImGui::EndDisabled();

			// --- 値の更新 ---
			bool currentAxisChanged = false;

			if(!isLocked) {
				if(buttonDragged) {
					float dragDelta = ImGui::GetIO().MouseDelta.x * _speed;
					if(ImGui::GetIO().KeyShift || ImGui::GetIO().KeyAlt) {
						dragDelta *= (kIsFloat ? 0.01f : 0.1f);
					}

					if constexpr(kIsFloat) {
						*axisValues[i] += dragDelta;
					} else {
						*axisValues[i] += static_cast<int>(dragDelta);
					}

					if(hasLimits) {
						*axisValues[i] = std::clamp(*axisValues[i], _min, _max);
					}

					currentAxisChanged = true;
				}

				if(inputChanged) currentAxisChanged = true;
			}

			if(currentAxisChanged) {
				valueChanged = true;

				// Unified Scale (Floatのみ)
				if constexpr(kIsFloat) {
					if((unifiedPtr && *unifiedPtr) || ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
						float oldVal = beforeAxisValues[i];
						float newVal = *axisValues[i];

						if(fabsf(oldVal) > kZeroEpsilon) {
							// ゼロでない場合は比率計算
							float ratio = newVal / oldVal;
							for(int j = 0; j < N; ++j) {
								if(j == i) continue;
								*axisValues[j] = beforeAxisValues[j] * ratio;
							}
						} else {
							// [修正点3] 元がゼロの場合の処理
							// 既に実装されていたこのelseブロックにより、
							// (0,0,0) -> (1,0,0) と操作した際に (1,1,1) になる挙動が保証されます。
							for(int j = 0; j < N; ++j) {
								*axisValues[j] = newVal;
							}
						}

						if(hasLimits) {
							for(int j = 0; j < N; ++j) {
								*axisValues[j] = std::clamp(*axisValues[j], _min, _max);
							}
						}
					}
				}
			}

			if(i < N - 1) ImGui::SameLine();
			ImGui::PopID();
		}

		ImGui::PopID();
		ImGui::EndTable();
	}
	ImGui::PopID();

	return valueChanged;
}

} /// namespace


bool ImMathf::DrawVec2Control(const std::string& _label, ONEngine::Vector2& _values, float _speed, float _min, float _max, float _columnWidth, bool* _unified) {
	return DrawVecControlT<ONEngine::Vector2, float, 2>(_label, _values, _speed, _min, _max, _columnWidth, _unified);
}

bool ImMathf::DrawVec3Control(const std::string& _label, ONEngine::Vector3& _values, float _speed, float _min, float _max, float _columnWidth, bool* _unified) {
	return DrawVecControlT<ONEngine::Vector3, float, 3>(_label, _values, _speed, _min, _max, _columnWidth, _unified);
}

bool ImMathf::DrawVec4Control(const std::string& _label, ONEngine::Vector4& _values, float _speed, float _min, float _max, float _columnWidth, bool* _unified) {
	return DrawVecControlT<ONEngine::Vector4, float, 4>(_label, _values, _speed, _min, _max, _columnWidth, _unified);
}

bool ImMathf::DrawVec2IntControl(const std::string& _label, ONEngine::Vector2Int& _values, float _speed, int _min, int _max, float _columnWidth) {
	return DrawVecControlT<ONEngine::Vector2Int, int32_t, 2>(_label, _values, _speed, _min, _max, _columnWidth, nullptr);
}

bool ImMathf::DrawVec3IntControl(const std::string& _label, ONEngine::Vector3Int& _values, float _speed, int _min, int _max, float _columnWidth) {
	return DrawVecControlT<ONEngine::Vector3Int, int32_t, 3>(_label, _values, _speed, _min, _max, _columnWidth, nullptr);
}

bool ImMathf::DrawVec4IntControl(const std::string& _label, ONEngine::Vector4Int& _values, float _speed, int _min, int _max, float _columnWidth) {
	return DrawVecControlT<ONEngine::Vector4Int, int32_t, 4>(_label, _values, _speed, _min, _max, _columnWidth, nullptr);
}