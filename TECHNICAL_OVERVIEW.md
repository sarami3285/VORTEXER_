# 技術概要

## 構成

- 言語: C++17
- 描画・入力: SDL2
- 画像: SDL2_image
- 音声: SDL2_mixer
- 文字描画: SDL2_ttf
- データ: JSON（nlohmann/json）
- 対象: Windows x64 / Visual Studio 2022

## 設計上の要点

### Actor / Component

ゲーム内オブジェクトを `Actor`、機能を `Component` として分離しています。`Game` がActorの寿命を管理し、各ActorがComponentを排他的に所有します。描画、移動、当たり判定、HP、AI、武器などをComponentとして組み合わせます。

### Scene

タイトル、拠点、カスタマイズ、ショップ、ミッション選択、戦闘、結果を `Scene` 派生クラスとして切り替えます。シーン切替時にUI・Actor・音声・テクスチャの寿命が交差しないよう、解放順序を明示しています。

### データ駆動

ミッション配置、報酬、敵編成、武器性能をJSONから読み込みます。データの構文・必須項目を起動時に検証し、ミッション読込失敗時にはゼロ除算や不正サイズを避ける安全なフォールバックを使用します。

### 敵と戦闘

`EnemyFactory` で敵生成を集約し、敵の行動は状態Componentや攻撃Componentへ分割しています。通常弾は `ObjectPool<Bullet>` で再利用し、派生型のミサイルはサイズ不一致を避けて通常確保へフォールバックします。

