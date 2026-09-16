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
