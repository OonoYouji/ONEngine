#pragma once

#include <vector>
#include <string>
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Color.h"

namespace ONEngine {

    // --- Common Utilities ---

    enum class MinMaxState : uint8_t {
        Constant,
        RandomBetweenTwoConstants,
        Curve,
        RandomBetweenTwoCurves
    };

    /**
     * @struct MinMaxFloat
     * @brief 定数値、あるいは最小・最大値の範囲からランダムな浮動小数点を生成するための構造体
     */
    struct MinMaxFloat {
        MinMaxState state;
        float constant;
        float minVal;
        float maxVal;
        // AnimationCurve curve; // Future

        /**
         * @brief デフォルトコンストラクタ
         */
        MinMaxFloat() : state(MinMaxState::Constant), constant(0.0f), minVal(0.0f), maxVal(1.0f) {}
        /**
         * @brief 定数値を指定するコンストラクタ
         */
        MinMaxFloat(float _c) : state(MinMaxState::Constant), constant(_c), minVal(0.0f), maxVal(1.0f) {}
        /**
         * @brief 最小・最大値を指定してランダム範囲とするコンストラクタ
         */
        MinMaxFloat(float _min, float _max) : state(MinMaxState::RandomBetweenTwoConstants), constant(0.0f), minVal(_min), maxVal(_max) {}
    };

    /**
     * @struct MinMaxColor
     * @brief 単一のカラー値、あるいは最小・最大カラーの範囲からランダムなカラーを生成するための構造体
     */
    struct MinMaxColor {
        MinMaxState state;
        Color constant;
        Color minVal;
        Color maxVal;

        /**
         * @brief デフォルトコンストラクタ
         */
        MinMaxColor() : state(MinMaxState::Constant), constant(Color::kWhite), minVal(Color::kWhite), maxVal(Color::kWhite) {}
        /**
         * @brief 単一のカラーを指定するコンストラクタ
         */
        MinMaxColor(const Color& _c) : state(MinMaxState::Constant), constant(_c), minVal(Color::kWhite), maxVal(Color::kWhite) {}
        /**
         * @brief 最小・最大カラーを指定してランダム範囲とするコンストラクタ
         */
        MinMaxColor(const Color& _min, const Color& _max) : state(MinMaxState::RandomBetweenTwoConstants), constant(Color::kWhite), minVal(_min), maxVal(_max) {}
    };

    struct GradientColorKey {
        Color color;
        float time;
    };
    struct GradientAlphaKey {
        float alpha;
        float time;
    };

    /**
     * @struct ParticleSystemGradient
     * @brief 時間経過割合（0.0f〜1.0f）におけるカラーとアルファのキーフレームリストを保持し、中間値を補間算出する構造体
     */
    struct ParticleSystemGradient {
        std::vector<GradientColorKey> colorKeys;
        std::vector<GradientAlphaKey> alphaKeys;

        /**
         * @brief 指定時間における補間カラーを算出します。
         */
        Color Evaluate(float time) const;
    };

    struct AnimationCurveKey {
        float time;
        float value;
    };

    /**
     * @struct AnimationCurve
     * @brief 時間に対応するキーフレーム値の配列を保持し、任意の時間における補間値を算出する構造体
     */
    struct AnimationCurve {
        std::vector<AnimationCurveKey> keys;

        /**
         * @brief 指定時間における補間値を算出します。
         */
        float Evaluate(float time) const;
    };

    struct MinMaxGradient {
        MinMaxState state = MinMaxState::Constant;
        ParticleSystemGradient gradient;
        ParticleSystemGradient gradientMin;
        ParticleSystemGradient gradientMax;
    };

    struct MinMaxCurve {
        MinMaxState state = MinMaxState::Constant;
        float constant = 1.0f;
        AnimationCurve curve;
        AnimationCurve curveMin;
        AnimationCurve curveMax;
    };

    // --- Modules ---

    enum class SimulationSpace : uint8_t {
        Local,
        World
    };

    struct ParticleSystemMain {
        float duration = 5.0f;
        bool looping = true;
        bool prewarm = false;
        MinMaxFloat startDelay = MinMaxFloat(0.0f);
        MinMaxFloat startLifetime = MinMaxFloat(5.0f);
        MinMaxFloat startSpeed = MinMaxFloat(5.0f);
        MinMaxFloat startSize = MinMaxFloat(1.0f);
        MinMaxFloat startRotation = MinMaxFloat(0.0f);
        MinMaxColor startColor = MinMaxColor(Color::kWhite);
        float gravityModifier = 0.0f;
        SimulationSpace simulationSpace = SimulationSpace::Local;
        int maxParticles = 1000;
        bool playOnAwake = true;
    };

    struct ParticleSystemEmission {
        bool enabled = true;
        float rateOverTime = 10.0f;
        float rateOverDistance = 0.0f;

        struct Burst {
            float time = 0.0f;
            int count = 30;
            int cycles = 1;
            float interval = 0.01f;
            float probability = 1.0f;
        };
        std::vector<Burst> bursts;
    };

    enum class ParticleSystemShapeType : uint8_t {
        Sphere,
        Hemisphere,
        Cone,
        Box,
        Circle,
        Edge
    };

    struct ParticleSystemShape {
        bool enabled = true;
        ParticleSystemShapeType type = ParticleSystemShapeType::Sphere;
        float radius = 1.0f;
        float radiusThickness = 1.0f; // 0 to 1
        float arc = 360.0f;
        float angle = 25.0f; // For Cone
        Vector3 boxScale = { 1.0f, 1.0f, 1.0f };
    };

    struct ParticleSystemColorOverLifetime {
        bool enabled = false;
        MinMaxGradient color;
    };

    struct ParticleSystemSizeOverLifetime {
        bool enabled = false;
        MinMaxCurve size;
    };

    struct ParticleSystemVelocityOverLifetime {
        bool enabled = false;
        MinMaxCurve x;
        MinMaxCurve y;
        MinMaxCurve z;
        MinMaxCurve speedModifier;
        SimulationSpace space = SimulationSpace::Local;

        /**
         * @brief デフォルトコンストラクタ。速度補正係数を 1.0f に初期化します。
         */
        ParticleSystemVelocityOverLifetime() {
            speedModifier.constant = 1.0f;
        }
    };

    struct ParticleSystemRenderer {
        enum class RenderMode : uint8_t {
            Billboard,
            StretchedBillboard,
            HorizontalBillboard,
            VerticalBillboard,
            Mesh
        };
        RenderMode renderMode = RenderMode::Billboard;

        enum class RenderAlignment : uint8_t {
            View,
            Velocity
        };
        RenderAlignment alignment = RenderAlignment::View;

        enum class BlendMode : uint8_t {
            Normal,
            Add,
            Subtract,
            Multiply,
            Screen,
            None
        };
        BlendMode blendMode = BlendMode::Normal;

        float speedScale = 0.0f;
        float lengthScale = 2.0f;

        std::string materialGuid; // Reference to material asset
        std::string meshGuid;     // For Mesh mode
    };

}
