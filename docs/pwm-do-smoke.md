# PA0 PWM・PA3 DO 書込み確認

作業ID: PWM-DO-01。専用ブランチ`test/pwm-do-upload`で行う限定試験。本体のBIDS/CANopen機能・安全状態機械を実装したものではない。対象は承認済みのCH32V203C8T6 EVT R0とWCH-Link。

## 出力と構成

| 対象 | 動作 |
|---|---|
| PA0 / TIM2_CH1 | 初期0%。2秒で0→100%、続く2秒で100→0%、4秒周期で反復 |
| PA3 | push-pull、初期Low、1秒ごとにHigh/Low反転（周期2秒） |
| 時間基準 | TIM3の1 ms更新割込み。mainループで出力更新、待ち時間による累積誤差を避ける |

この確認用プログラムではPWMの0～65535の論理尺度に対し、ARR=65534、PSC=1を使う。カウンタが0～65534なのでCCR=65535で100%になる。本体仕様のARR=65535の実装や端点問題の解決を示すものではない。EVTの設定上はAPB1タイマ144 MHzから約1098.65 Hzとなるが、実周波数は未測定。CCRはpreloadを使用し、次のPWM周期境界で反映する。

実装は`src/main.cpp`、機器に依存しない波形計算は`src/smoke_waveform.hpp`。後者は内部ヘッダであり、製品全体の共通APIではない。WCH C SDKは改変せず、割込みはCリンケージを維持する。SDKヘッダ内のvolatile複合代入に対するC++20警告だけをincludeの範囲で抑制し、自作コードの警告は維持する。

## 検証手順

1. `pio run -e evt`でビルドする。現在の設定は自作部C++20、SDKはC。PlatformIOがテスト依存のUnityを`.pio/libdeps/evt/`へ用意する。
2. WindowsのVisual Studio C++ toolsでホスト試験を行う。リポジトリルートから`powershell -NoProfile -ExecutionPolicy Bypass -File test/test_pwm_do_smoke/run-tests.ps1`。実行ポリシー指定はこの子プロセス限りで、恒久設定を変更しない。
3. 対象配線・給電とプローブの非占有を確認して`pio run -e evt -t upload`。書込み・verify・resetのログと実際の成果物ハッシュを確認する。
4. PA0とPA3をGND基準で測定し、PWM周波数、0/100%端点、2秒の上昇/下降、DOの1秒反転を確認する。外部負荷の駆動保証はしない。

ホストUnity試験は12個の時刻境界と各1 msの単調増減を検証する。実機レジスタ、割込み、配線、波形の正しさは代替できない。`pio test -e evt`はこのホスト用ランナーの代わりに使用しない。

## 結果と限界

2026-09-17、実装`25213d8`で全4環境ビルドとホストUnity 2試験が成功した。`pio run -e evt -t upload`は終了コード0で、Programming Finished / Verified OK / Resetting Targetを確認した。実行結果・成果物ハッシュは [work-log.md](work-log.md)、現在の状態は [progress.md](progress.md) を参照。

PA0/PA3の実波形は測定器で未確認。書込みと実波形は別判定とし、未実施を合格にしない。ゲートウェイ全体のP-02完了や本体の要求ベース試験の合格も意味しない。
