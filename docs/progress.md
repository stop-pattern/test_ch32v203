# 現在の作業状態

更新: 2026-09-17 JST。運用は [development-workflow.md](development-workflow.md)、根拠は [work-log.md](work-log.md) を参照。

## 実機書込み確認 / PWM-DO-01

- 基準: `b839afa`。専用ブランチ`test/pwm-do-upload`で作業する。developは変更しない。
- 許可: main.c/main.cppにPA0のPWMランプとPA3の周期反転を実装し、エラー確認・修正後に書き込む。小単位コミット・匿名化記録を継続する。push・保護解除・ドライバ更新は含まない。
- 本体のBIDS/CANopen実装とは別の限定動作確認。本体の未決仕様を解消済みとしない。
- 確認済み: 開始時はクリーン。現コードはPC1点滅のみ。PlatformIO Core 6.2.0起動成功、WindowsでWCH-LinkRVが正常列挙。ターゲットMCU接続・書込みは未確認。
- 承認: PA0/TIM2_CH1で約1 kHz、2秒で0→100%、続く2秒で100→0%を反復する。PA3はLow開始、1秒ごとに反転。周期は定数化する。EVT R0/WCH-Linkと出力可能な負荷条件を含む提案をユーザーが承認した。
- 現状: main.cpp/内部hppへ実装。TDDの失敗→成功確認、Unity 2試験成功、EVT C++20ビルド成功。read-onlyコードレビューは重大/重要指摘なし。詳細は [確認仕様](pwm-do-smoke.md)。
- 次: 全4環境ビルド成功。実装保存単位`add pwm ramp and periodic digital output check`を保存後、evtでupload、verify/reset結果を確認する。実波形観測と書込み成功は区別する。
- 実機操作は未実施。レビュー委任完了。親がソース・ビルド・Git・実機を単独所有する。
