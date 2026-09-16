# CH32V203 シミュレータ用I/Oゲートウェイ

BVE系鉄道シミュレータと操作器・表示器をつなぐ、CH32V203向けファームウェアの開発プロジェクトです。PCのBIDS通信または別MCUのCANopenマスターを通じて、デジタルI/O・PWM・ADCを扱います。EVT基板での試作からカスタム基板への移植を想定しています。

## 開発状況

現在は仕様策定段階です。実装されているのは`src/main.c`のPC1点滅サンプルで、下記のゲートウェイ機能は未実装です。C++20は採用方針であり、ツールチェーン適合・移行はこれから検証します。

既存サンプルはgeneric/EVTと各ISP派生環境の4環境でビルド成功を確認済みです。書き込みや機能・タイミングの合格を意味しません。確認時点の版・サイズ・検証範囲は [環境確認記録](docs/tooling-environment.md)、最新の作業状態は [progress](docs/progress.md) を参照してください。

## 目的と予定する機能

| 分野 | 予定する機能 |
|---|---|
| I/O | DI・DO・PWM・ADCを各1ch、モード入力、RUN/ERROR LED |
| PC接続 | USB Device CDCでBIDSクライアントとしてテキスト/B64Eを処理 |
| CAN接続 | Classical CANのCANopen slave、LSS、NMT、SDO、PDO、Heartbeat、EMCY |
| 操作権 | GPIO選択またはUSB優先方針で操作経路を選び、監視と操作を分離 |
| 変換 | 速度→PWM、ドア状態→DO、DI→Horn、ADC→ブレーキをデモとして構成 |
| 設定・診断 | 揮発設定、入力フィルタ、通信監視、安全出力、LED/ログによる診断 |

シミュレータ用途の試作を対象とします。負荷駆動回路、ESP32マスターのソフトウェア、CAN FD、Flashへの設定保存、独自更新ブートローダは今回の実装範囲に含めません。電気的な対象はMCUの3.3 V論理信号までです。ピン割当て等の検証待ちを含むため、接続前に [インターフェース仕様](docs/interface-spec.md) と [未決事項](docs/decisions.md#未決事項台帳) を確認してください。

## フォルダ構成

下記は人間とAIの共通配置規則です。`AGENTS.md`にも同じ内容を記載し、配置変更時は両方を更新します。

<!-- BEGIN REPOSITORY LAYOUT -->
### フォルダ構造と状態

「計画」は実装時に必要になってから作る配置先で、現時点の実装済み機能を表さない。

```text
test_ch32v203/
├── README.md                 人間向けの入口・構成案内
├── AGENTS.md                 AI/開発者の作業規則
├── platformio.ini            ビルド・ボード・書込み環境
├── LICENSE                   このリポジトリのライセンス
├── src/                      アプリケーション固有の実装
│   ├── main.c                現在の点滅サンプル
│   ├── app/                  [計画] 起動・構成の組立て・協調実行
│   ├── hal/                  [計画] ボード/SDK依存の周辺機能操作
│   ├── io/                   [計画] DI/ADC取得・フィルタ・入力診断
│   ├── protocol/             [計画] 通信プロトコル別の処理
│   │   ├── bids/             [計画] BIDSの解析・要求・通信状態
│   │   └── canopen/          [計画] CANopen接続・OD・サービス
│   ├── control/              [計画] 経路選択・変換・実行時設定・安全出力
│   └── diagnostics/          [計画] エラー集約・LED・ログ
├── include/                  モジュール間で共有する自作ヘッダ
├── config/                   [計画] ビルド時の既定値・ボード設定
├── lib/                      再利用する独立ライブラリ
├── test/                     試験（現在は案内READMEのみ）
├── docs/                     仕様・設計・計画・進捗・検証記録
├── skills/                   スキルのGit管理原本
├── .vscode/                  エディタ設定（追跡対象は.gitignoreに従う）
└── .pio/                     [生成・Git対象外] ビルド成果物・キャッシュ等
```

### 配置の必須ルール

| 配置先 | 入れるもの・境界 |
|---|---|
| `src/app/` | エントリポイントから呼ぶ初期化、各モジュールの組立て、スケジューラ。プロトコル解析やレジスタ操作を混在させない |
| `src/hal/` | クロック、GPIO、タイマ、ADC、USB/CANドライバ、WDT等のハードウェア境界。WCH属性やSDK依存を局所化する |
| `src/io/` | DIデバウンス、ADC平滑化・校正・入力診断。通信フレームを解釈しない |
| `src/protocol/bids/`、`src/protocol/canopen/` | 各プロトコルのフレーミング・解析・状態とアプリケーション接続。低層ドライバはhal、上流スタックは依存ライブラリとして分離する |
| `src/control/` | 操作権限、論理マッピング、変換、実行時のshadow設定/検証/反映、安全状態と最終出力の調停 |
| `src/diagnostics/` | 診断状態、エラー通知の調停、LEDパターン、ログ。実際の周辺機能アクセスはhalへ委譲する |
| `include/<module>/` | 複数モジュールから使用する宣言・型・必要なinline/template定義。モジュール内部だけのヘッダは対応するsrc配下に置く |
| `config/` | ビルド時のボード・通信・タイミング・I/O既定値を役割別.hppに分割する。実行時の状態・設定処理は置かない |
| `lib/<library_name>/` | 独立再利用できる自作ライブラリの実装とヘッダ。自作C++実装は.cpp、公開ヘッダは原則そのライブラリのinclude配下の.hppに置き、ルートincludeへ複製しない |
| `test/test_<behavior>/` | 振る舞い単位のテストと専用fixture。ホスト/実機の依存を明記し、製品コードへ試験用実装を混入させない |
| `docs/` | 日本語の現行仕様・設計・試験計画・判断・進捗。図等は必要時にdocs/assets、完了ログは必要時にdocs/work-logへ置き、案内リンクを保つ |
| `skills/<skill_name>/` | SKILL.mdと、そのスキル固有のreferences/scripts/assets。ユーザー領域へ導入するコピーと同期する |
| `.pio/` | 再生成可能な成果物・キャッシュ・一時的な詳細ログ。Git管理せず、重要な根拠はdocsの記録や保持先にも残す |

1. ファイル作成前に責務から配置先を選ぶ。上記に収まらない新しい用途・トップレベルフォルダは、先に配置規則と関連設計を更新する。
2. 自作C++は.cpp/.hppとし、宣言コメント・命名・書式はdocs/coding-style.mdに従う。上流ライブラリの配置・名前・書式をこの規則のために改変しない。
3. 現在のsrc/main.cや既存ファイルを文書整備だけで移動・改名しない。計画フォルダは必要な実装を追加するときに作り、空フォルダや仮コードだけを先行作成しない。
4. ルートconfigのヘッダ検索パスと、各ライブラリのビルド統合はP-02で明示設定・検証する。フォルダを作ればPlatformIOがすべて自動認識するとは仮定しない。
5. include/lib/test内の既存READMEはPlatformIOの初期案内であり、この配置規則や現在のC++規約より優先しない。
6. README.mdとAGENTS.mdのこの配置ブロックは同一内容を維持する。変更時は両方とarchitecture/implementation-planへの影響を同じ変更単位で確認する。
<!-- END REPOSITORY LAYOUT -->

## ビルド環境とクイックスタート

PlatformIO Coreと、このプロジェクトの`ch32v`プラットフォーム/`noneos-sdk`が必要です。Windows PowerShellでの既存Core検出、環境別ビルド、書き込みの詳細は [PlatformIOスキル](skills/ch32v203-platformio/SKILL.md) と [操作手順](skills/ch32v203-platformio/references/build-upload.md) にまとめています。

| 環境 | 対象 | 設定されているクロック源 | 書き込み方式 |
|---|---|---|---|
| generic | Generic CH32V203C8T6（既定） | HSI + PLL | WCH-Link |
| evt | CH32V203C8T6 EVT R0 | HSE + PLL | WCH-Link |
| generic_isp | genericの派生 | HSI + PLL | USB ISP |
| evt_isp | evtの派生 | HSE + PLL | USB ISP |

リポジトリのルートで、まずビルドだけを行う例です。

```powershell
pio --version
pio run -e evt
# 4環境をまとめてビルドする場合:
pio run -e generic -e evt -e generic_isp -e evt_isp
```

このPCではCoreがインストール済みでもPATHに`pio`がありません。既存の実行ファイルを直接呼び出せます。

```powershell
$pioExe = Join-Path $env:USERPROFILE '.platformio\penv\Scripts\pio.exe'
& $pioExe --version
& $pioExe run -e evt
```

インストール先が異なる場合は、スキルの検出手順を使用してください。Coreや依存版の確認をせずに別のPlatformIOを追加インストールする必要はありません。

### 書き込みと動作確認

対象ボード・配線・書き込み方式を確認したうえで、対象に合うコマンドを1つ使用します。`upload`は実機を書き換えます。

```powershell
# EVTへWCH-Linkで書き込む:
& $pioExe run -e evt -t upload
# EVTをUSB ISPモードにして書き込む場合は、上記の代わりに:
# & $pioExe run -e evt_isp -t upload
```

USB ISPへの入り方は実基板の資料に従います。COMポートの列挙だけで書き込み対象を特定せず、書き込み・検証・リセットと、その後の機能動作を別々に確認してください。デバッグログの取得経路や現在のコードで観測できる内容も操作手順で確認します。

### テスト

`test/`には現在案内READMEのみがあり、Unityのホスト試験環境は未構築です。予定する検証は [試験計画](docs/test-plan.md) を参照してください。組み込み環境の`pio test`は書き込みを伴う場合があるため、ビルドだけの確認として実行しません。

## 文書の読み方

| 知りたいこと | 文書 |
|---|---|
| 何を、なぜ作るか | [requirements.md](docs/requirements.md) |
| モジュール責務とデータフロー | [architecture.md](docs/architecture.md) |
| ピン・通信・データ型・タイミング・異常動作 | [interface-spec.md](docs/interface-spec.md) |
| 要求をどう検証するか | [test-plan.md](docs/test-plan.md) |
| 実装順序と依存関係 | [implementation-plan.md](docs/implementation-plan.md) |
| 設計判断の理由・却下案・未決事項 | [decisions.md](docs/decisions.md) |
| C++とDoxygenの書き方 | [coding-style.md](docs/coding-style.md) |
| 自律開発・分担・ログ保存・再開方法 | [development-workflow.md](docs/development-workflow.md) |
| 現在の到達点と次の作業 | [progress.md](docs/progress.md) |
| 過去の試行と検証の根拠 | [work-log.md](docs/work-log.md) |
| CLI・スキル配置と確認済みの環境 | [tooling-environment.md](docs/tooling-environment.md) |

## 開発・貢献時のルール

自作部はC++20・Google C++ Style Guideを基準とし、4スペース、.cpp/.hpp、関数・クラス・構造体の宣言へのDoxygen必須を上書きします。ファームウェアでは例外・RTTI・ヒープ確保を使用しません。詳細と組み込み例外はコーディング規約を参照してください。

作業前に`AGENTS.md`とprogressを読み、関連仕様とGit差分を確認します。仕様・配置の変更はREADME、AGENTS、関連文書の整合を保ち、必要な進捗や検証根拠を随時ファイルへ残します。意味のまとまりごとに小さく検証・コミットし、メッセージは`add gpio tests`のような短い小文字の命令形にします。

機能実装は仕様の前提を確定し、ユーザーの実装開始指示を受けてから行います。既存コードの規約適合化も別作業です。PRには目的、変更内容、関連要求/課題、検証環境と結果、未実施事項を記載し、ハードウェア関連なら配線・計測条件も添えます。

## ライセンス

このリポジトリは [MIT License](LICENSE) です。導入するSDK・CANopenNode等の外部ライブラリには、それぞれのライセンスが適用されます。上流の通知を保持し、導入時に固定版とライセンス条件を確認します。
