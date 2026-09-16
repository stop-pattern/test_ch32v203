# PlatformIO・スキル環境の確認記録

確認日: 2026-09-17。ビルド確認対象: `960f4f3`時点の既存点滅サンプル。スキルの利用可能一覧への掲載も同日確認した。新しいファームウェア機能の実装・実機書き込みは行っていない。

## pioの利用可否

環境整備時のPowerShellでは`Get-Command pio,platformio`で実行ファイルを解決できなかった。Coreはインストール済みで、ユーザー領域の実行ファイルから起動・ビルドできることを確認した。記録内の個人を含むパスは環境変数で表す。作業再開時にはスキルのCLI検出手順で、その時点の環境を再確認する。

```powershell
& "${env:USERPROFILE}\.platformio\penv\Scripts\pio.exe" --version
```

| 項目 | 確認結果 |
|---|---|
| PlatformIO Core | 6.2.0 |
| Core内Python | 3.11.7 |
| Coreディレクトリ | `${env:USERPROFILE}\.platformio` |
| 実行ファイル | `penv\Scripts\pio.exe`、`penv\Scripts\platformio.exe` |
| ch32v platform | `1.1.0+sha.ce2095c` |
| WCH noneos SDK | `2.30000.0+sha.5e8ed2b` |
| RISC-V toolchain | `1.120200.220829+sha.d283639` |
| WCH OpenOCD | `2.1100.260118` |
| wchisp | `0.23.240914` |

PATHやPowerShellプロファイルの恒久変更、Coreの追加インストールはしていない。スキルはPATH検索後に既存仮想環境へフォールバックするため、現環境でそのまま利用できる。シェルから短い`pio`名を常用する場合のPATH設定は [PlatformIO公式手順](https://docs.platformio.org/en/latest/core/installation/shell-commands.html) を参照する。

## スキルの配置と使い方

- Git管理原本: [skills/ch32v203-platformio/SKILL.md](../skills/ch32v203-platformio/SKILL.md)
- 手順本文: [references/build-upload.md](../skills/ch32v203-platformio/references/build-upload.md)
- この環境への導入先: `${env:USERPROFILE}\.codex\skills\ch32v203-platformio\`
- 明示呼出し名: `$ch32v203-platformio`

`CODEX_HOME`が未設定なのでユーザー既定のスキル領域へ配置した。現在の利用可能スキル一覧に`ch32v203-platformio`と導入先の`SKILL.md`が掲載されている。別セッションで一覧に出ない場合は原本と配置先を確認し、原本の`SKILL.md`を指定して手順を参照する。

原本と導入先は同じ内容とし、変更時には両者を同期して検証する。別環境では、その環境の`CODEX_HOME/skills`、未設定ならユーザーの`.codex/skills`へスキルフォルダを配置する。既存の同名スキルがある場合は内容を確認してから更新する。

`skill-creator`の方針に従い、入口にはCLI解決と作業範囲、参照文書には環境別のビルド・WCH-Link・USB ISP手順を分離した。対象未特定時の扱いと失敗時の終了条件も記載した。長期開発の目標管理、許可範囲、失敗解析、再試行、subagentの分担は [development-workflow.md](development-workflow.md) を正本とし、スキルは各操作の手順として利用する。

原本・導入先の両方で`skill-creator/scripts/quick_validate.py`が成功した。両ファイルのSHA-256一致を確認し、SKILL.md内のCLI解決ブロックを実行してCore 6.2.0と`run --help`の正常終了を確認した。Windowsの文字コード差を避けるため、Python検証は`python -X utf8`で実行した。

## 実施したビルド

```powershell
& "${env:USERPROFILE}\.platformio\penv\Scripts\pio.exe" run -e generic -e evt -e generic_isp -e evt_isp
```

終了コード0、4環境すべて成功。

| 環境 | Flash使用量 | RAM表示値 | ビルドログのクロック設定 |
|---|---:|---:|---|
| generic | 1364 byte | 2076 byte | HSI + PLL、144 MHz |
| evt | 1428 byte | 2076 byte | HSE + PLL、144 MHz |
| generic_isp | 1364 byte | 2076 byte | HSI + PLL、144 MHz |
| evt_isp | 1428 byte | 2076 byte | HSE + PLL、144 MHz |

これらは既存サンプルに対するPlatformIOの表示値であり、将来ファームウェアの容量見積りや実行中のスタック高水位ではない。生成物は`.pio/build/<環境>/`にありGit管理対象外。ISP環境のビルド成功は、ISP書き込み成功を意味しない。

## 継続開発での記録と共有資源

各操作の実行条件・終了状態・ログ、ソースと生成物の識別、書込み先の確認は [development-workflow.md](development-workflow.md) に従って記録する。現在の担当、次の操作、稼働プロセスと資源所有権は [progress.md](progress.md)、反復ごとの観測と結果は [work-log.md](work-log.md) を参照する。詳細ログの保存場所と保持方針も開発手順へ集約する。

ビルド出力、WCH-Link/OpenOCD、USB ISP、シリアルモニター、ボードは共有資源として扱う。並列作業では開発手順の所有権ルールに従い、別agentが使用中の出力領域へ書き込んだり、同一ポート・プローブを同時に開いたりしない。過去のCOM番号やビルド結果を現在の接続先・生成物の識別に流用しない。

## 実機・試験の確認範囲

`pio device list`でCOM1とUSBシリアルCOM4を列挙した。ポート番号は一時的な観測結果であり、接続MCUや書き込み先の識別には使用していない。WCH-Link/ISPによる接続試験、upload、消去、保護解除、デバッグ開始は行っていない。

`test/`にはREADMEのみで、ホストUnity環境も未定義。単体試験の合格や仕様試験の完了とは扱わない。要求ベースの検証は引き続き [test-plan.md](test-plan.md) に従う。実機ピン・タイマクロック・書き込み動作等の未決事項も未解決のままとする。
