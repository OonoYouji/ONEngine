#pragma once
#include<random>
// ランダム数生成器の初期化
std::random_device rd;  
std::mt19937 gen(rd()); 

// 各軸の速度範囲を設定
std::uniform_real_distribution<float> distTheta(-1.0f, 1.0f); // x軸の範囲
std::uniform_real_distribution<float> distPhi(-1.0f, 1.0f); // y軸の範囲

