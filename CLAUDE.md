# SP31Project — DirectX 11 3D レンダリング学習プロジェクト

## プロジェクト概要
DirectX 11 を使った 3D レンダリング技術の学習プロジェクト。
各種シェーダー（Phong, PBR, Bump, LimLight など）を実装し、ジオラマシーンを表示する。

## ビルド・実行
- Visual Studio でソリューションを開いてビルド・実行
- シェーダーは `.cso` ファイルとして事前コンパイル済み（HLSLから生成）
- 新しいシェーダーを追加する際は `.hlsl` → `.cso` コンパイルが必要

## シーン構成（ジオラマ）

### Game.cpp がシーン全体を管理
- `InitGame` / `FinalizeGame` / `UpdateGame` / `DrawGame` でライフサイクルを制御

### 描画オブジェクト

| オブジェクト | クラス | シェーダー | FBXモデル |
|---|---|---|---|
| ジオラマ床 | `DioramaFloor` | BumpVS/PS | ポリゴン直接生成 |
| 左右のビル | `Building` | PointPixelLightingVS/PS | cube.fbx × 複数（手前→奥に整列） |
| 台座 | `Pedestal` | DisneyPBRVS/PS | cube.fbx |
| オオカミ | `Wolf` | LimLightingVS/PS | Wolf.fbx |

### FBXアセット（asset/model/）
- `cube.fbx` — 汎用キューブ（ビル・台座に使用）
- `Wolf.fbx` — オオカミモデル
- `model.fbx` — 人型モデル
- `sky.fbx` — 空

### テクスチャ（asset/texture/）
- `sura.jpg` — 床のカラーテクスチャ
- `Normal.png` — バンプマッピング用法線マップ
- `Roughness.jpg` — DisneyPBR ラフネス
- `Metalness.jpg` — DisneyPBR メタルネス

## 既存シェーダー一覧（新規追加禁止）
| CSO名 | 用途 |
|---|---|
| `BumpVS/PS` | バンプマッピング（床） |
| `PointPixelLightingVS/PS` | 点光源ピクセルライティング |
| `LimLightingVS/PS` | リムライティング（アウトライン強調） |
| `DisneyPBRVS/PS` | Disney PBR（金属・粗さ） |
| `LimLightingVS/PS` | リムライティング |
| `PixelDirectionalLightingVS/PS` | 平行光源 |
| `PixelLightingBlinnPhongVS/PS` | Blinn-Phong |
| `HemiSphereLightingVS/PS` | 半球ライティング |
| `SpotLightingVS/PS` | スポットライト |
| `CookTorranceVS/PS` | Cook-Torrance |

## コーディング規則
- 各描画オブジェクトは `.h` / `.cpp` の1クラス1ファイル構成
- クラスは `Init / Finalize / Update / Draw` の4メソッドを持つ
- `Update()` に ImGui スライダーを配置してパラメーターを調整可能にする
- シェーダーは既存の `.cso` のみ使用（新規 HLSL は書かない）
- 文字コードは Shift-JIS（既存ファイルに合わせる）

## カメラ操作
- 右クリックドラッグ → マウスで視点回転
- `W/A/S/D` → 前後左右移動
- 実装：`Camera.cpp`

## 座標系
- DirectX 左手系（Y 上、Z 奥）
- モデルローダー（model.cpp）で Blender 座標 → DirectX 座標に変換
  - `x → x`, `y → -z`, `z → y`

## 配置メモ（ジオラマ）
- 床は Y=0 を基準
- オオカミ：(0, 0.35, 0.2)、スケール (0.32, 0.32, 0.32) ※主役として大きめ＋明るめに調整（Ambient 0.55 / Diffuse 1.0）
- 台座：(0, 0.15, 0.2)、スケール (0.6, 0.3, 0.6)
- ビル：`Building.cpp` 冒頭の定数でループ生成（手前(-Z)→奥(+Z)へ片側 `BUILDING_COUNT` 棟）。
  - 列を伸ばす＝`BUILDING_COUNT` を増やすだけ。間隔=`BUILDING_SPACING`、左右位置=`BUILDING_SIDE_X`、幅=`BUILDING_WIDTH`。
  - 高さは `LeftHeights[]` / `RightHeights[]` のパターンを繰り返して起伏を出す。各ビルは Position.y = 高さ/2 で床(Y=0)に接地。
  - 列を長くしたら奥のビルが減衰で暗くならないよう、`Building` の `light.PointLightParam.x`（到達距離）も大きめにする。
- 床：`DioramaFloor.cpp` の `FLOOR_HALF`（現在14.0）で大きさを決定。ビルの列長に合わせて拡張する。`FLOOR_TSIZE` は HALF/2 にしてタイル密度を維持。
- カメラ初期位置：`Camera.cpp` の `InitCamera` で (0, 0.9, -2.6)。シーン全体が近く見えるよう寄せてある。
