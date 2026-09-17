# 現在の作業状態

更新: 2026-09-17 JST。運用は [development-workflow.md](development-workflow.md)、根拠は [work-log.md](work-log.md) を参照。

## 実機書込み確認 / PWM-DO-01

- 基準: `b839afa`。専用ブランチ`test/pwm-do-upload`で作業する。developは変更しない。
- 許可: main.c/main.cppにPA0のPWMランプとPA3の周期反転を実装し、エラー確認・修正後に書き込む。小単位コミット・匿名化記録を継続する。push・保護解除・ドライバ更新は含まない。
- 本体のBIDS/CANopen実装とは別の限定動作確認。本体の未決仕様を解消済みとしない。
- 環境: PlatformIO Core 6.2.0、WCH-LinkRV。開始時のPC1点滅サンプルを、承認された確認プログラムへ置換した。
- 承認: PA0/TIM2_CH1で約1 kHz、2秒で0→100%、続く2秒で100→0%を反復する。PA3はLow開始、1秒ごとに反転。周期は定数化する。EVT R0/WCH-Linkと出力可能な負荷条件を含む提案をユーザーが承認した。
- 状態: 依頼された実装・ビルド・書込み確認は完了。実装コミット`25213d8`。全4環境ビルド、Unity 2試験、独立レビューを完了。詳細は [確認仕様](pwm-do-smoke.md)。
- 実機結果: `pio run -e evt -t upload`終了コード0。Programming Finished / Verified OK / Resetting Targetを確認。実際に書いたELFとbinのSHA-256はwork-logへ保存した。
- 残る確認: PA0/PA3の実波形・周波数・2秒ランプ・1秒反転は測定器で未確認。書込み成功と物理出力の合格を混同しない。本体P-01の未決事項は維持する。
- ブランチは専用のまま保持。develop=`b839afa`で変更なし。merge/pushなし。委任とuploadプロセスは終了し、実機はupload後のリセット状態から実行する。追加のデバッグ接続は保持していない。
