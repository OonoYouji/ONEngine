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
// 汎用テンプレート関数 (内部実装)
// ==========================================================
template<typename TVector, typename TValue, int N>
bool DrawVecControlT(const std::string& _label, TVector& _values, float _speed, float _columnWidth, bool* _unified) {

	// float型かどうかを判定
	constexpr bool kIsFloat = std::is_floating_point_v<TValue>;

	// 整数型の場合、Unified Scale（比率固定）は無意味なので無効化する
	bool* unifiedPtr = kIsFloat ? _unified : nullptr;

	bool valueChanged = false;

	// --- 定数定義 ---
	constexpr float kSafetyMarginWidth = 1.0f;
	constexpr float kButtonPaddingW = 3.0f;
	constexpr float kFramePaddingScale = 2.0f;
	constexpr float kSpacingScale = 2.0f;
	constexpr float kMinInputWidth = 10.0f;
	constexpr int   kNumColumns = 2;
	// N はテンプレート引数から来る (2, 3, 4)

	// 色定義 (X, Y, Z, W)
	const ImVec4 kColorX = ImVec4(0.8f, 0.1f, 0.15f, 1.0f);
	const ImVec4 kColorY = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
	const ImVec4 kColorZ = ImVec4(0.1f, 0.25f, 0.8f, 1.0f);
	const ImVec4 kColorW = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // W軸用 (グレーなど)

	constexpr float kColorHoverOffset = 0.1f;
	constexpr float kColorActiveOffset = 0.2f;
	constexpr float kZeroEpsilon = 1e-6f;
	constexpr int   kTextBufferSize = 64;
	const ImVec2    kTextAlignCenter = ImVec2(0.5f, 0.5f);

	// --- 静的変数 (操作開始時の値) ---
	static TVector s_startValue;

	ImGui::PushID(_label.c_str());

	TVector beforeValues = _values;

	// テーブルフラグ
	ImGuiTableFlags tableFlags = ImGuiTableFlags_NoSavedSettings;

	if(ImGui::BeginTable("##VecControlTable", kNumColumns, tableFlags)) {
		ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, _columnWidth);
		ImGui::TableSetupColumn("##Values", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		// --- 1列目: ラベル ---
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", _label.c_str());

		// --- 2列目: 値の操作 ---
		ImGui::TableNextColumn();
		ImGui::PushID("##VecVals");

		// --- レイアウト計算 ---
		float availWidth = ImGui::GetContentRegionAvail().x - kSafetyMarginWidth;
		float itemSpacing = GImGui->Style.ItemSpacing.x;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * kFramePaddingScale;
		ImVec2 buttonSize = { lineHeight + kButtonPaddingW, lineHeight };

		float checkboxWidth = (unifiedPtr != nullptr) ? (ImGui::GetFrameHeight() + itemSpacing) : 0.0f;
		float totalAxesWidth = availWidth - checkboxWidth - (itemSpacing * kSpacingScale);
		float axisWidth = totalAxesWidth / static_cast<float>(N);
		float inputWidth = (std::max)(kMinInputWidth, axisWidth - buttonSize.x);

		// --- チェックボックス (Floatのみ) ---
		if(unifiedPtr != nullptr) {
			ImGui::Checkbox("##Unified", unifiedPtr);
			if(ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Uniform Scale (Lock Ratio)");
			}
			ImGui::SameLine();
		}

		// --- 値ポインタ配列の構築 ---
		// Vector2/3/4 の構造に合わせてポインタを取得
		TValue* axisValues[4] = { nullptr, nullptr, nullptr, nullptr };
		TValue beforeAxisValues[4] = { 0, 0, 0, 0 }; // Ratio計算用

		axisValues[0] = &_values.x;
		beforeAxisValues[0] = beforeValues.x;

		axisValues[1] = &_values.y;
		beforeAxisValues[1] = beforeValues.y;

		if constexpr(N >= 3) {
			axisValues[2] = &_values.z;
			beforeAxisValues[2] = beforeValues.z;
		}
		if constexpr(N >= 4) {
			axisValues[3] = &_values.w; // Vector4/4Int は .w を持つ前提
			beforeAxisValues[3] = beforeValues.w;
		}

		const char* axisLabels[] = { "X", "Y", "Z", "W" };
		const ImVec4 axisColors[] = { kColorX, kColorY, kColorZ, kColorW };

		// フォーカス制御
		ImGuiID nextFocusID = ImGui::GetID("##nextFocusAxis");
		int focusAxisIdx = ImGui::GetStateStorage()->GetInt(nextFocusID, -1);
		if(focusAxisIdx != -1) {
			ImGui::GetStateStorage()->SetInt(nextFocusID, -1);
		}

		// --- 各軸のループ ---
		for(int i = 0; i < N; ++i) {
			ImGui::PushID(i);

			// ゼロ判定 (Floatの場合はEpsilon考慮)
			bool isZero = false;
			if constexpr(kIsFloat) {
				isZero = fabsf(*axisValues[i]) < kZeroEpsilon;
			} else {
				isZero = (*axisValues[i] == 0);
			}

			bool isUnified = (unifiedPtr && *unifiedPtr);
			bool isLocked = isUnified && isZero;

			if(isLocked) {
				ImGui::BeginDisabled(true);
			}

			// ==========================================================
			// A. 軸ラベルボタン
			// ==========================================================
			ImVec4 baseColor = axisColors[i];
			ImVec4 hoverColor = ImVec4(baseColor.x + kColorHoverOffset, baseColor.y + kColorHoverOffset, baseColor.z + kColorHoverOffset, 1.0f);
			ImVec4 activeColor = ImVec4(baseColor.x + kColorActiveOffset, baseColor.y + kColorActiveOffset, baseColor.z + kColorActiveOffset, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);

			ImGui::Button(axisLabels[i], buttonSize);

			// ボタン操作開始
			if(ImGui::IsItemActivated()) {
				s_startValue = _values;
			}

			// ボタン操作終了 (Undo)
			if(ImGui::IsItemDeactivated()) {
				// 値が変わったか簡易チェック
				// (厳密な比較はType依存だが、メモリ比較または各成分比較でOK)
				if(memcmp(&_values, &s_startValue, sizeof(TVector)) != 0) {
					EditCommand::Execute<ImGuiCommand::ModifyValueCommand<TVector>>(&_values, s_startValue, _values);
				}
			}

			bool buttonActive = ImGui::IsItemActive();
			bool buttonDragged = buttonActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			if(!isLocked && (ImGui::IsItemHovered() || buttonActive)) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}

			ImGui::PopStyleColor(3);
			ImGui::SameLine(0, 0);

			// ==========================================================
			// B. 数値表示/入力エリア
			// ==========================================================
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

				// 型に応じたInputウィジェット
				if constexpr(kIsFloat) {
					inputChanged = ImGui::InputFloat("##v", (float*)axisValues[i], 0.0f, 0.0f, "%.2f");
				} else {
					inputChanged = ImGui::InputInt("##v", (int*)axisValues[i], 0, 0); // Step=0でボタン非表示
				}

				if(ImGui::IsItemActivated()) {
					s_startValue = _values;
				}

				if(ImGui::IsItemDeactivatedAfterEdit()) {
					EditCommand::Execute<ImGuiCommand::ModifyValueCommand<TVector>>(&_values, s_startValue, _values);
				}

				// Tabキー遷移
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
				// 表示モード (Button)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, kTextAlignCenter);

				char buf[kTextBufferSize];
				if constexpr(kIsFloat) {
					sprintf_s(buf, kTextBufferSize, "%.2f", (float)*axisValues[i]);
				} else {
					sprintf_s(buf, kTextBufferSize, "%d", (int)*axisValues[i]);
				}

				if(ImGui::Button(buf, ImVec2(inputWidth, 0))) {
					ImGui::GetStateStorage()->SetBool(inputID, true);
					ImGui::GetStateStorage()->SetBool(focusReqID, true);
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleColor(3);
			}

			if(isLocked) {
				ImGui::EndDisabled();
			}

			// ==========================================================
			// C. 変更反映 (ドラッグ操作など)
			// ==========================================================
			bool currentAxisChanged = false;

			if(!isLocked) {
				if(buttonDragged) {
					float dragDelta = ImGui::GetIO().MouseDelta.x * _speed;

					// Shift/Altキーで減速
					if(ImGui::GetIO().KeyShift || ImGui::GetIO().KeyAlt) {
						dragDelta *= (kIsFloat ? 0.01f : 0.1f); // intの場合は遅すぎると動かなくなるので調整
					}

					if constexpr(kIsFloat) {
						*axisValues[i] += dragDelta;
					} else {
						// intの場合、小数点以下を切り捨てて加算
						*axisValues[i] += static_cast<int>(dragDelta);
					}
					currentAxisChanged = true;
				}

				if(inputChanged) {
					currentAxisChanged = true;
				}
			}

			if(currentAxisChanged) {
				valueChanged = true;

				// Unified Scale (Floatのみ)
				if constexpr(kIsFloat) {
					if((unifiedPtr && *unifiedPtr) || ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
						float oldVal = beforeAxisValues[i];
						float newVal = *axisValues[i];

						if(fabsf(oldVal) > kZeroEpsilon) {
							float ratio = newVal / oldVal;
							// 他の軸全てに比率を適用
							for(int j = 0; j < N; ++j) {
								if(j == i) continue; // 自分自身は既に更新済み
								*axisValues[j] = beforeAxisValues[j] * ratio;
							}
						} else {
							// 元が0の場合は比率計算できないので、同じ値を代入する挙動にする
							for(int j = 0; j < N; ++j) {
								*axisValues[j] = newVal;
							}
						}
					}
				}
			}

			if(i < N - 1) {
				ImGui::SameLine();
			}
			ImGui::PopID();
		} // End For Loop

		ImGui::PopID();
		ImGui::EndTable();
	}
	ImGui::PopID();

	return valueChanged;
}

} /// namespace


bool ImMathf::DrawVec2Control(const std::string& _label, ONEngine::Vector2& _values, float _speed, float _columnWidth, bool* _unified) {
	return DrawVecControlT<ONEngine::Vector2, float, 2>(_label, _values, _speed, _columnWidth, _unified);
}

bool ImMathf::DrawVec3Control(const std::string& _label, ONEngine::Vector3& _values, float _speed, float _columnWidth, bool* _unified) {
	return DrawVecControlT<ONEngine::Vector3, float, 3>(_label, _values, _speed, _columnWidth, _unified);
}

bool ImMathf::DrawVec4Control(const std::string& _label, ONEngine::Vector4& _values, float _speed, float _columnWidth, bool* _unified) {
	return DrawVecControlT<ONEngine::Vector4, float, 4>(_label, _values, _speed, _columnWidth, _unified);
}

bool ImMathf::DrawVec2IntControl(const std::string& _label, ONEngine::Vector2Int& _values, float _speed, float _columnWidth) {
	return DrawVecControlT<ONEngine::Vector2Int, int32_t, 2>(_label, _values, _speed, _columnWidth, nullptr);
}

bool ImMathf::DrawVec3IntControl(const std::string& _label, ONEngine::Vector3Int& _values, float _speed, float _columnWidth) {
	return DrawVecControlT<ONEngine::Vector3Int, int32_t, 3>(_label, _values, _speed, _columnWidth, nullptr);
}

bool ImMathf::DrawVec4IntControl(const std::string& _label, ONEngine::Vector4Int& _values, float _speed, float _columnWidth) {
	return DrawVecControlT<ONEngine::Vector4Int, int32_t, 4>(_label, _values, _speed, _columnWidth, nullptr);
}