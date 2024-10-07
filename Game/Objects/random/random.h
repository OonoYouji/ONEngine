#pragma once
#include<random>
// ランダム数生成器の初期化
std::random_device rd;  // 非決定的な乱数生成器
std::mt19937 gen(rd()); // メルセンヌ・ツイスタ法による乱数生成器（シードとしてランダムデバイスを使用）

// 各軸の速度範囲を設定
std::uniform_real_distribution<float> distVx(-1.0f, 1.0f); // x軸の範囲
std::uniform_real_distribution<float> distVy(-1.0f, 1.0f); // y軸の範囲
std::uniform_real_distribution<float> distVz(-1.0f, 1.0f); // z軸の範囲
