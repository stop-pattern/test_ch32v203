---
name: ch32v203-platformio
description: Check PlatformIO CLI availability and build, inspect, or upload CH32V203 noneos-sdk firmware on Windows PowerShell, including generic/EVT profiles, WCH-Link and USB ISP. Use for pio setup, build or flashing tasks in test_ch32v203 and matching projects.
---

# CH32V203 PlatformIO

対象プロジェクトの `platformio.ini`、`AGENTS.md`、関連する `docs/` を読み、現在の環境名と作業範囲を確認する。既定環境を接続ボードと同一だと仮定しない。

長期開発では、対象プロジェクトの `docs/development-workflow.md` と `docs/progress.md` があれば先に参照する。許可済みgoalのビルド・解析・修正・書込み・デバッグは同じ許可を求め直さず継続する。具体的な完了条件、記録先、担当と実機資源を復元してから操作する。

## CLIの解決

PowerShellではPATH上の既存Coreを優先し、見つからない場合はユーザーのPlatformIO仮想環境を利用する。PATH未登録を理由に別のCoreを重複インストールしない。

```powershell
$pioCommand = Get-Command pio,platformio -CommandType Application -ErrorAction SilentlyContinue | Select-Object -First 1
if ($pioCommand) {
    $pioExe = $pioCommand.Source
} else {
    $pioCoreDir = if ($env:PLATFORMIO_CORE_DIR) { $env:PLATFORMIO_CORE_DIR } else { Join-Path $env:USERPROFILE '.platformio' }
    $pioExe = Join-Path $pioCoreDir 'penv\Scripts\pio.exe'
    if (-not (Test-Path -LiteralPath $pioExe)) {
        $pioExe = Join-Path $pioCoreDir 'penv\Scripts\platformio.exe'
    }
    if (-not (Test-Path -LiteralPath $pioExe)) {
        throw 'PlatformIO Core executable not found. Check the installed Core location.'
    }
}
& $pioExe --version
if ($LASTEXITCODE -ne 0) { throw 'PlatformIO Core failed to start.' }
```

ツール呼出しごとに新しいPowerShellが起動する場合、この変数は引き継がれない。解決済みフルパスを次の呼出しで使うか、解決ブロックを同じ呼出し内で実行する。恒久的なPATHやPowerShellプロファイルの変更は、このスキル利用の前提にしない。

## ビルド・書き込み

実行前に [references/build-upload.md](references/build-upload.md) の該当節を読む。そこに環境の選択、ビルド、WCH-Link、USB ISP、失敗時の切り分けを記載している。

- CLI確認・既存コードのビルドと、ファームウェア実装・実機書き込みを区別する。「手順をまとめる」という依頼だけではuploadしない。
- 書き込みが既に依頼され、対象が明確なら確認を繰り返さず進める。対象ボードや複数プローブの識別が不明な場合だけ確認する。
- genericとEVTは使用クロック源が異なる。uploadには常に明示的な `-e` を付ける。
- read-onlyのビルドエラー/ログ解析は独立したサブエージェントへ委譲できる。ソース、生成物、プローブ、シリアル/デバッグ接続の共有資源は担当を固定し、競合操作を直列化する。
- `pio device list` のCOMポートをWCH-LinkやUSB ISPのupload先と決めつけない。
- 失敗時は原因を絞って再試行する。同じ失敗を無変更で反復せず、erase、保護解除、プローブ更新、ドライバ差替えへ自動的に拡大しない。
- `test/` がREADMEだけなら、テスト合格と報告しない。組込み `pio test` は書き込み・実機実行を伴い得るため、ビルド確認の代わりに実行しない。

## 記録

開始前に予定・期待値・対象を、開始後に継続プロセスの識別子を、終了時に結果をファイルへ保存する。結果にはCore版、ソース版/差分、環境、コマンド、終了コード、サイズ、生成物のSHA-256、ログ所在、実機検証の有無、次の行動を残す。`run -t upload`が再ビルドする場合は実際に書いた最終成果物を識別する。ビルド成功を仕様適合や実機動作の成功と同一視しない。手順・環境の変更は関連文書全体と整合させ、ユーザーのコミット方針に従う。

このスキルのGit管理原本は対象リポジトリの `skills/ch32v203-platformio/`。ユーザー領域に導入したコピーを更新するときは原本と同期し、片側だけを独立に変更しない。
