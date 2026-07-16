#pragma once

/// std
#include <vector>

/// engine
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "../IEditCommand.h"

namespace Editor {

/// ///////////////////////////////////////////////
/// Transformの値を変更するコマンド
/// ///////////////////////////////////////////////
/**
 * @class ModifyTransformCommand
 * @brief エディタのインスペクターやギズモ操作でエンティティの位置・回転・拡縮（Transform）を変更した際、複数のオブジェクト選択時も含めて一括して Undo / Redo を可能にするコマンドクラス
 */
class ModifyTransformCommand : public IEditCommand {
public:
    /**
     * @enum Target
     * @brief 編集対象となる Transform の要素（位置、回転、拡縮）を指定する列挙型
     */
    enum class Target { Position, Rotation, Scale };

    /**
     * @struct Data
     * @brief 各オブジェクトごとの Transform 変更前後のパラメータを保持する構造体
     */
    struct Data {
        ONEngine::Transform* pTransform; ///< 変更対象の Transform コンポーネントへのポインタ
        ONEngine::Vector3 oldVal;        ///< 変更前の値 (Position / EulerDegrees / Scale)
        ONEngine::Vector3 newVal;        ///< 変更後の値 (Position / EulerDegrees / Scale)
    };

    /**
     * @brief コンストラクタ
     * @param _target 編集対象の要素（位置/回転/拡縮）
     * @param _data 変更対象となる全 Transform とその値の一覧
     */
    ModifyTransformCommand(Target _target, const std::vector<Data>& _data)
        : target_(_target), data_(_data) {}

    /**
     * @brief コマンド的実行（新しい値に Transform パラメータを更新します）。
     * @return 実行結果ステート
     */
    EDITOR_STATE Execute() override {
        if (data_.empty()) return EDITOR_STATE_FAILED;
        for (const auto& d : data_) {
            ApplyValue(d.pTransform, d.newVal);
        }
        return EDITOR_STATE_FINISH;
    }

    /**
     * @brief コマンドの取り消し処理（元の古い値に Transform パラメータを更新します）。
     * @return 実行結果ステート
     */
    EDITOR_STATE Undo() override {
        if (data_.empty()) return EDITOR_STATE_FAILED;
        for (const auto& d : data_) {
            ApplyValue(d.pTransform, d.oldVal);
        }
        return EDITOR_STATE_FINISH;
    }

private:
    /**
     * @brief 指定した Transform に対して、Position, Rotation, Scale を対象種別に応じて適切に割り当てます。
     * @param _pTransform 対象の Transform コンポーネント
     * @param _val 設定する値ベクトル
     */
    void ApplyValue(ONEngine::Transform* _pTransform, const ONEngine::Vector3& _val) {
        if (!_pTransform) return;

        switch (target_) {
        case Target::Position: 
            _pTransform->position = _val; 
            break;
        case Target::Rotation: 
            // 度数法(Degrees) -> 弧度法(Euler Radian) -> Quaternion の順で変換
            _pTransform->euler = _val; 
            _pTransform->SyncQuaternionFromEuler();
            break;
        case Target::Scale:    
            _pTransform->scale = _val;    
            break;
        }
        _pTransform->Update();
    }

    Target target_;
    std::vector<Data> data_;
};

} /// namespace Editor
