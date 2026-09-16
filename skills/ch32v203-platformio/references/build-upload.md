# ビルド・書き込み手順

Windows PowerShell用。以下ではSKILL.mdの解決手順で得た `$pioExe` を使い、対象リポジトリのルートで実行する。別の作業ディレクトリからは `run --project-dir '対象の絶対パス'` を指定する。CLIフラグの可否は使用中のCoreの `run --help` で確認できる。

## 環境を選ぶ

`platformio.ini` を正とする。2026-09-17時点のtest_ch32v203は次の構成。

| 環境 | ボード | クロック源 | upload方式 |
|---|---|---|---|
| generic | genericCH32V203C8T6 | HSI + PLL、144 MHz設定 | WCH-Link |
| evt | ch32v203c8t6_evt_r0 | HSE + PLL、144 MHz設定 | WCH-Link |
| generic_isp | genericの派生 | HSI + PLL | USB ISP |
| evt_isp | evtの派生 | HSE + PLL | USB ISP |

`pio run` 単体の既定はgeneric。EVTで外部クロックが使えること、カスタム基板で対応するクロック源があることを確認する。ビルドログの144 MHz表示は実機の周波数計測ではない。

## ビルドと生成物

対象環境だけをビルドする例:

```powershell
& $pioExe run -e evt
if ($LASTEXITCODE -ne 0) { throw 'EVT build failed.' }
```

既存4環境をまとめて確認する例（uploadしない）:

```powershell
& $pioExe run -e generic -e evt -e generic_isp -e evt_isp
if ($LASTEXITCODE -ne 0) { throw 'Build matrix failed.' }
```

生成物は `.pio/build/<環境>/firmware.elf` と `firmware.bin`。今回のプラットフォームでは逆アセンブルの `firmware.lst`、`firmware.debug.lst` も生成される。`.pio/` はコミットしない。

通常は増分ビルドを使う。設定変更後の古い生成物が疑われる場合に限り、対象環境の `run -e evt -t clean` 後に再ビルドする。依存パッケージが不足した場合はCoreのエラーを確認し、手順整備だけを理由にプラットフォーム全体を更新しない。

## WCH-Linkで書き込む

1. 書き込み対象のボード、環境、プローブを特定する。WCH-LinkをCH32V向けのモード・対応ドライバで利用し、GND、SDI信号、給電を基板資料に従って確認する。複数の給電源を無条件に接続しない。
2. 同じプローブを使用中のデバッグセッションを終了する。対象環境がビルド成功していることを確認する。
3. 次のうち対象に一致するコマンドを1つだけ実行する。

```powershell
& $pioExe run -e evt -t upload
# genericボードなら、上記の代わりに:
# & $pioExe run -e generic -t upload
if ($LASTEXITCODE -ne 0) { throw 'WCH-Link upload failed.' }
```

現在のch32v builderはWCH版OpenOCDの `program ... verify reset` を使う。ログで書き込み・検証・リセットの結果を確認し、その後の動作は別途確認する。`--upload-port COM4` 等をWCH-Linkの選択方法と仮定しない。COMの有無だけではSDI接続やターゲットMCUを確認できない。

## USB ISPで書き込む

1. ボード資料でISPへの入り方を確認する。BOOT設定とリセット/電源投入の操作は実基板の回路に従い、未確認のピン値を推測しない。
2. WCH-Link側USBではなく、MCUのUSB ISPにつながるコネクタを接続する。ISPデバイスの列挙と対応ドライバを確認する。
3. 対象のISP環境で実行する。

```powershell
& $pioExe run -e evt_isp -t upload
# genericボードなら、上記の代わりに:
# & $pioExe run -e generic_isp -t upload
if ($LASTEXITCODE -ne 0) { throw 'USB ISP upload failed.' }
```

インストール済みch32v builderは `wchisp flash firmware.elf` を使用し、設定したCOMポートをこのコマンドへ渡していない。USB ISPをUART/CDCのCOM指定手順と混同しない。書き込み結果を確認し、必要なら基板のBOOT設定を通常起動へ戻してリセットし、アプリケーション動作を確認する。

WindowsでDLLやドライバが原因のときは、利用中wchispのエラーと [上流のWindows向け説明](https://github.com/ch32-rs/wchisp#note-for-windows) を確認する。ドライバ差替えは他ツールへの影響があるため、接続先を特定せず実行しない。

## ポート確認・モニタ・テスト

```powershell
& $pioExe device list
# 実際のログ用ポートを特定してから実行する例:
# & $pioExe device monitor --port COM4 -b 115200
```

COM番号は接続状態で変化するため固定しない。`monitor_speed=115200` は現プロジェクトのモニタ設定で、BIDSの19200ラインコーディングとは別である。モニタは対話的に継続するため、必要な場合だけ起動して終了時はCtrl+Cで閉じる。現在の点滅サンプルにログ出力があるとは限らない。

`test/` に試験が追加された後で、試験環境と実行方法を確認する。ホスト用native環境は現状未定義。`pio test -e generic` は実機書き込みを伴い得るため、ホスト専用試験として扱わない。

## 失敗の切り分けと終了条件

| 症状 | 確認すること |
|---|---|
| pioが見つからない | SKILL.mdの実行ファイル解決。Core自体の不存在とPATH未登録を区別 |
| ビルド失敗 | 最初のコンパイラ/リンカエラー、使用環境、パッケージ版 |
| WCH-Linkが開けない | プローブのモード・ドライバ、他デバッガによる占有、USB接続 |
| プローブは開くがMCUに接続できない | 給電、GND、SDI配線、MCU型番と設定 |
| ISPが見つからない | ISP起動、MCU側コネクタ、列挙状態、wchispのDLL/ドライバ |
| 書込み後に動かない | 環境のHSI/HSE差、BOOT設定、リセット、実ピンとLEDジャンパー |

仮説、計測、入力、修正、外部状態の変化を記録して再試行する。診断目的の再試行も認めるが、同じ失敗を無変更で反復しない。ターゲット未特定、配線操作待ち、未承認の保護解除が必要なら、その具体的な条件を報告して依存する書込みを止め、独立した解析を進める。成功時は使用環境、実際の書込み成果物、終了コード、検証ログ、実機動作の確認範囲を記録する。

## 根拠

- [PlatformIOのシェル連携](https://docs.platformio.org/en/latest/core/installation/shell-commands.html): 既存CoreとPATHの関係。
- ローカル `platformio.ini`、`ch32v/boards/*.json`、`ch32v/builder/main.py`: 環境、クロック、OpenOCD/wchispコマンドの根拠。プラットフォーム版変更時に再確認する。
- [wchisp](https://github.com/ch32-rs/wchisp): ISP用ツールの手順とWindows依存。WCH-Link用ツールとは区別する。
