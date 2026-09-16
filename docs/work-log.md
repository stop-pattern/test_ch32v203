# 作業・検証記録

現状の正本は [progress.md](progress.md)、記録方法は [development-workflow.md](development-workflow.md)。ここには試行時点の事実、判断の要旨、根拠、結果を時系列で保存する。現在仕様は対応する仕様文書へ反映し、本書の過去記録を現在仕様の代用にしない。

## 2026-09-17 / DOC-LONGRUN-01 / 運用規約の整備

### 開始と分担

- 入力: ユーザーから、将来のgoalに向けた自律開発サイクル、細かな進捗のファイル保存、全文書の整合性、read-only調査と編集の積極的な並列分担を指示された。
- 基準: `d0b0ca2`、開始時の作業ツリーはクリーン。Goalツールの照会結果は未設定。
- 調査: AGENTS、要求、実装計画、判断台帳、既存スキルを再読。古いC限定/coverage規定なし/AGENTS不変/委任非推奨の記述を統合対象とした。
- 分担: `workflow_consistency`へread-onlyレビュー、`workflow_verification_docs`へtest-planとtooling-environmentのみの編集を委譲。親がその他の文書とGit操作を所有する。
- 実機操作: なし。ファームウェアの実装開始を意味しない。

### 検証・引継ぎ

- 実施: AGENTSを現行C++17/coverage/自律運用へ再構成し、development-workflow、progress、work-logを作成。R-29～31、D-33～35、T-25を追加して関連文書とスキルへ反映した。
- レビュー: `workflow_consistency`が独立に既存矛盾を抽出し、最終差分をread-onlyで再確認。重大な残件なし。`workflow_verification_docs`の2文書を親が再読し統合した。
- スキル検証: リポジトリルートで`python -X utf8 "${env:USERPROFILE}\.codex\skills\.system\skill-creator\scripts\quick_validate.py" skills\ch32v203-platformio`を実行し終了コード0。導入先`${env:USERPROFILE}\.codex\skills\ch32v203-platformio`にも同じ検証を実行し成功。
- 同期: SKILL.mdのSHA-256は両配置とも`FC163E29657C65EBBE9BC9E79D9540DD7EE2813C1AB3E9BB8BFF48B9E1F47D79`、参照文書は両配置とも`FC9C2D5B285970DE3A2B9F34FEF41672AD1372BFADEC12045020AA083FC49FBF`。
- 文書検査: AGENTS、docs、skillsのMarkdown全13件をディスクから読み、相対リンク/アンカー、R/O-ID、コードフェンスを照合。31要求すべてが25試験項目に対応し、未決21件を参照。エラー0。
- 差分検査: `git diff --check`成功。旧C限定、coverage規定なし、AGENTS更新禁止、委任を否定する表現が現行規則から除かれたことを検索で確認。LF/CRLFのGit警告は既存設定によるもの。
- 保存単位: `document autonomous development workflow`。ソースとplatformio.iniは変更しない。ビルド、書込み、実機デバッグは文書変更に不要なため未実施。
- 限界: 文書の構造・整合性と分担編集を確認した範囲であり、T-25の実地再開、実エラー修正、無人実機サイクルは未実施。ファームウェアの未決事項を解消したとは扱わない。
- 引継ぎ: 子エージェントの作業とファイル所有は終了。ビルド/モニタ/デバッグの継続プロセスなし。次の仕様検討と実装開始条件はprogressを参照する。

## 2026-09-17 / DOC-STYLE-01 / コーディング規約の照合

- 入力: Google C++ Style Guide採用、4スペース、関数・クラス・構造体の宣言へのDoxygen必須、組み込み不要部分の除外。既存コードの適合化は後日とし、衝突点は採用する側を確認してから文書へ反映する指示。
- 基準: `ab0dffb`、開始時の作業ツリーはクリーン。
- 調査方法: Google公式ページを参照し、`rg`でAGENTS/docs/skillsと現コードの規約関連記述を検索。`coding_rules_audit`へ既存文書のread-only監査を委譲し、完了報告を受領した。
- 外部根拠: [Google C++ Version](https://google.github.io/styleguide/cppguide.html#C++_Version)は現在C++20、[Self-contained Headers](https://google.github.io/styleguide/cppguide.html#Self_contained_Headers)は.hを指定。既存R-22等のC++17とarchitectureの設定ヘッダ.hppには採用判断が必要。
- 文書上の競合: AGENTSの`existing local formatting`はGoogle基準へ整理が必要。O-21は命名・整形・コメントの詳細を未決としている。
- 記述の拡張: 現在のDoxygen要件は関数のみ。クラス・構造体の宣言も対象にすることはユーザーが明示済み。
- 互換な制約と確認点: ヒープ・例外・RTTI禁止は組み込み向け制約として維持可能。上流SDK/CANopenNodeのC・無改変を維持し、ISR属性やABI名等を例外化する範囲を提案した。Google固有ライブラリ/OS向け機能の推奨を対象外にする案であり、既存コード削除の提案ではない。
- 質問: 言語版、拡張子と書式、組み込み適用範囲の3点を提示。回答前に現行の規範文書を採用変更しない。
- 結果/次の行動: 調査完了・回答待ち。今回はprogressと本記録のみ保存。回答後に関連文書を統合更新し、O-21の確定部分とツール導入等の残件を分ける。
- 操作範囲: コード・設定の変更、ビルド、書込み、デバッグ接続は実施していない。

### 回答の反映

- ユーザー回答: C++20へ方針変更し、ツールチェーン適合を後の実装作業で確認する。.cpp/.hppをGoogleに対する例外とし、その他の書式・命名はGoogleに従う。自作部への適用と組み込み除外の提案を採用する。
- 確定: 4スペース、関数/クラス/構造体の宣言へのDoxygen必須、既存コードの修正延期を含め、coding-style.mdを作成した。AGENTS、R-22/R-28、architecture、P-02、D-02/D-36、T-20/T-24を統合的に更新した。
- 未決の整理: O-21は整形/検査ツールの版・導入方法に限定。O-17とP-02にC++20コンパイラ/必要な標準ライブラリ/C ABI/ISR属性の適合確認を追加し、確認済みとは扱わない。
- 分担: `coding_rules_audit`がtest-planのみ編集し、親が差分を確認。`workflow_consistency`へ最終read-onlyレビューを依頼した。
- 対象外: ソースコード、platformio.ini、既存ファイル名、formatter設定、実機は変更しない。スキルのCLI/書込み手順には規約と衝突する記述がなく、スキル変更は不要。
- 検証: 独立レビューでT-24の対象が新規コードだけになっていた点を指摘され、「新規・今後変更する自作コード」へ修正した。規約本文と関連する14文書をディスクから読み、相対リンク/アンカー、R/O/D-ID、コードフェンスを検査しエラー0。31要求すべてが試験に対応し、判断36件、未決21件を確認した。
- 旧規則の確認: C++17と既存ローカル書式優先の残存は、日時付きの調査履歴と却下案だけであり、現行規則には残っていない。
- 変更範囲の検証: `git diff --check`と`git diff --exit-code -- src include lib test platformio.ini`が終了コード0。コードとビルド設定は変更なし。C++20適合試験、ビルド、HILは未実施。
- 保存単位: `adopt google cpp style with embedded overrides`。子エージェントの担当は完了し、継続プロセス・実機所有なし。今回の3点に回答待ちは残っていない。

## 2026-09-17 / DOC-README-01 / READMEと配置規約

- 入力: 目的・機能以外の必要情報も含むREADME、理解しやすいフォルダ構造の定義、同じ内容のAGENTSへの記載とAIの遵守を指示された。
- 基準: `1ea4b67`、開始時の作業ツリーはクリーン。
- 調査: `rg --files`と既存文書を確認。README.md、config、機能別ソースサブフォルダは未作成。既存コードはsrc/main.cの点滅サンプルで、仕様の機能は未実装。
- 分担: `workflow_consistency`へread-onlyで構成・README項目の確認を委譲。親が文書編集と統合を所有する。
- 方針: 既存/計画/生成物を区別し、READMEとAGENTSに同一の配置規則を載せる。フォルダ移動や空の実装フォルダ作成は行わない。ルートconfigのビルド統合はP-02で行う。
- 成果: READMEへ目的/予定機能/現状/環境別コマンド/試験状況/文書案内/貢献規則/ライセンスを記載。AGENTSに同一の配置契約とAIの遵守義務を記載し、architecture/P-02/開発手順/試験計画/D-37と整合させた。
- レビュー: `workflow_consistency`が同一ブロック、現状/計画の区別、CLI例、責務対応を確認。D-37直前の空行が表を分断する指摘を受け、削除した。
- 検証手順の調整: PowerShellの既定文字コードでは日本語の読取りが文字化けしたためUTF8を明示。全文一括のJSON出力は上限で切れたため、ファイルごとに再読して検証した。ファイル内容はこれらの読取り操作では変更していない。
- 検証: 関連15文書とLICENSEをディスクから再読し、相対リンク100件/アンカー、R/D/O-ID、コードフェンスを検査してエラー0。31要求すべてが試験に対応、判断37件、未決21件を確認。README/AGENTSの配置ブロックは完全一致した。
- 変更範囲: `git diff --check`と`git diff --exit-code -- src include lib test platformio.ini`は終了コード0。コード、ビルド設定、既存配置は変更なし。ビルド/書き込み/HILは文書変更の対象外として実施していない。
- 保存単位: `add readme and repository layout rules`。分担レビュー完了、継続プロセス・実機所有なし。未決仕様は維持し、実装は開始していない。
