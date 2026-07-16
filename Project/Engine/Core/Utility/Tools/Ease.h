#pragma once

namespace ONEngine {
namespace Ease {

/**
 * @namespace In
 * @brief イージングイン（徐々に加速する）関数群
 */
namespace In {
float Sine(float t);
float Quad(float t);
float Cubic(float t);
float Quart(float t);
float Quint(float t);
float Expo(float t);
float Circ(float t);
float Back(float t);
float Elastic(float t);
float Bounce(float t);
} /// namespace In

/**
 * @namespace Out
 * @brief イージングアウト（徐々に減速する）関数群
 */
namespace Out {
float Sine(float t);
float Quad(float t);
float Cubic(float t);
float Quart(float t);
float Quint(float t);
float Expo(float t);
float Circ(float t);
float Back(float t);
float Elastic(float t);
float Bounce(float t);
} /// namespace Out

/**
 * @namespace InOut
 * @brief イージングイン・アウト（加速した後に減速する）関数群
 */
namespace InOut {
float Sine(float t);
float Quad(float t);
float Cubic(float t);
float Quart(float t);
float Quint(float t);
float Expo(float t);
float Circ(float t);
float Back(float t);
float Elastic(float t);
float Bounce(float t);
} /// namespace InOut

} /// namespace Ease
} /// namespace ONEngine