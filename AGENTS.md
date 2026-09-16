# Repository Guidelines

## Start and Resume

Read [progress](docs/progress.md), [development workflow](docs/development-workflow.md), and the relevant specifications before acting. Check Git status, delegated work, and actual process/device state; a stale checkpoint is not proof that a process is running. Firmware implementation remains gated by the user's implementation instruction and resolved prerequisites in [implementation plan](docs/implementation-plan.md).

## Autonomous Development

For an authorized goal, define scope and measurable acceptance criteria, then autonomously repeat planning, implementation, tests/builds, error analysis, fixes, rebuilds, authorized upload, log capture, and debugging until acceptance is evidenced. Do not ask again for already granted authority. Unresolved product decisions or new scope require user input; continue independent in-scope work meanwhile. Create a tool-managed goal only when explicitly requested, and follow the goal tool's lifecycle rules.

Persist every meaningful finding, failed attempt, command/result, pending operation, and next action promptly in [work log](docs/work-log.md) and the current checkpoint, after removing personal and confidential information. Privacy takes priority over verbatim evidence retention, including historical logs. Checkpoint before long operations, delegation, context handoff, and turn completion. Do not rely on conversation memory or claim required unperformed HIL as complete.

## Parallel Work

Actively delegate bounded read-only investigation, error analysis, and review. Also parallelize implementation, tests, and documentation when responsibilities are independent. Assign file ownership, inputs, deliverables, and acceptance criteria. The coordinator owns shared progress files, integration, and Git index/commits. Serialize shared build directories, source snapshots, boards, probes, and debug/serial sessions; review agent results before integration.

## Project Layout and Commands

The directory contract below is mandatory for AI agents and contributors. It must remain identical to the marked block in [README.md](README.md). Select the correct responsibility before creating a file; check both copies and the architecture when changing the layout. Planned directories are not permission to start implementation or move existing code.

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

Use [ch32v203-platformio](skills/ch32v203-platformio/SKILL.md) for CLI discovery and build/upload procedures. Examples: `pio run -e evt`, or all four environments with `pio run -e generic -e evt -e generic_isp -e evt_isp`. Authorized EVT upload uses `pio run -e evt -t upload`; USB ISP uses `evt_isp`. Always select the actual board explicitly. Embedded `pio test` may flash hardware; no host test environment is currently defined.

## Code and Verification

Follow [coding style](docs/coding-style.md): Google C++ Style Guide with C++20, four-space indentation, and `.cpp`/`.hpp` extensions. Every project function, class, and struct declaration requires Doxygen comments. Project firmware uses no exceptions, RTTI, or heap allocation; vendor SDK/stack code keeps its upstream C conventions. Preserve required ISR attributes, ABI names, and register operations at hardware boundaries. OS/thread-specific and Google-library adoption guidance is outside the firmware scope. Existing code cleanup is deferred; do not reformat or migrate it as part of this documentation work. C++20 toolchain compatibility is a future P-02/T-20 check; no formatter/linter is currently configured.

Use PlatformIO Unity and [test plan](docs/test-plan.md). Target 90% branch coverage for protocol, transformation, configuration, and state-machine logic. Verify affected build environments; record hardware, firmware identity, logs, and measurements for HIL. Documentation-only work needs document/skill checks, not an unrelated firmware rebuild.

## Documentation and Commits

Maintain Japanese human-facing specifications as a coherent current description. On additions or removals, search related requirements, interfaces, tests, decisions, plans, and skills; rewrite affected sections as if the resulting design had always been present. Remove obsolete assertions and references. Keep historical evidence in the work log, decision rationale, and Git, not contradictory correction paragraphs in current specifications.

Never commit personal or confidential information: usernames in absolute paths, private keys, passwords, tokens, credentials, private endpoints, or sensitive identifiers. This applies to code, docs, logs, screenshots, fixtures, skills, filenames, and commit messages. Use repository-relative paths, environment variables, or non-identifying placeholders; sanitize tool/subagent output before saving. Do not dump secrets into reports or commit them for later removal. Review the complete staged content, filenames, and commit message before every commit; inspect binary assets separately. Pattern checks and .gitignore are aids, not proof of safety. Follow the exposure response in [development workflow](docs/development-workflow.md); do not rewrite history or rotate credentials without authorization.

Commit frequently at the smallest meaningful, coherent, independently reviewable and verifiable work unit, using short lowercase imperative messages such as `update development workflow`. Prompt boundaries do not define commits: split independent fixes or investigations within one prompt, but keep inseparable changes together. Stage only the relevant files/hunks; preserve unrelated user changes. PRs explain the outcome, requirements/issues, verification and remaining gaps, with sanitized logs or wiring notes where relevant. Installed skill copies must match their versioned originals.
