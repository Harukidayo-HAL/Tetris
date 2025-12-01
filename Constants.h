#pragma once

// どこからでも使えるように共通定義
constexpr int STAGE_X = 10;
constexpr int STAGE_Y = 20;
constexpr int BLOCK_SIZE = 22;

// ゴースト描画やAI用でも使える定数
constexpr int kW = STAGE_X;
constexpr int kH = STAGE_Y + 3;
