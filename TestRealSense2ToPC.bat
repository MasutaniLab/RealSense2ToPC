@echo off
: RealSense2ToPCのテスト

:ネーミングサービスの確認
rtls /localhost > nul
if errorlevel 1 (
  echo ネーミングサーバが見つかりません
  pause
  exit /b 1
  rem /bオプションは親を終わらせないために必須
)

set pythonScript=SetCommand

:コンポーネントの起動
call ..\RealSense2ToPC\RealSense2ToPC.bat
call ..\PointCloudViewer\PointCloudViewer.bat
start "%pythonScript%" .\%pythonScript%.pyw

:コンポーネント名を変数化
set s=/localhost/RealSense2ToPC0.rtc
set v=/localhost/PointCloudViewer0.rtc
set i=/localhost/%pythonScript%0.rtc

:コンポーネント起動待ち
:rtls-s
echo %s%の起動待ち
timeout 1 /nobreak > nul
rtls %s% > nul 2>&1
if errorlevel 1 goto rtls-s

:rtls-i
echo %i%の起動待ち
timeout 1 /nobreak > nul
rtls %i% > nul 2>&1
if errorlevel 1 goto rtls-i

:接続
rtcon %s%:pc %v%:pc
rtcon %i%:command %s%:command

:アクティベート
rtact %s% %v% %i%

:loop
set /p ans="終了しますか？ (y/n)"
if not "%ans%"=="y" goto loop

:ディアクティベート
rtdeact %s% %v% %i%

:終了（rtexitは，引数を一つずつ）
rtexit %s%
rtexit %v%
rtexit %i%

taskkill /fi "WINDOWTITLE eq %pythonScript%"
