@echo off
: RealSense2ToPC�̃e�X�g

:�l�[�~���O�T�[�r�X�̊m�F
rtls /localhost > nul
if errorlevel 1 (
  echo �l�[�~���O�T�[�o��������܂���
  pause
  exit /b 1
  rem /b�I�v�V�����͐e���I��点�Ȃ����߂ɕK�{
)

set pythonScript=SetCommand

:�R���|�[�l���g�̋N��
call ..\RealSense2ToPC\RealSense2ToPC.bat
call ..\PointCloudViewer\PointCloudViewer.bat
start "%pythonScript%" .\%pythonScript%.pyw

:�R���|�[�l���g����ϐ���
set s=/localhost/RealSense2ToPC0.rtc
set v=/localhost/PointCloudViewer0.rtc
set i=/localhost/%pythonScript%0.rtc

:�R���|�[�l���g�N���҂�
:rtls-s
echo %s%�̋N���҂�
timeout 1 /nobreak > nul
rtls %s% > nul 2>&1
if errorlevel 1 goto rtls-s

:rtls-i
echo %i%�̋N���҂�
timeout 1 /nobreak > nul
rtls %i% > nul 2>&1
if errorlevel 1 goto rtls-i

:�ڑ�
rtcon %s%:pc %v%:pc
rtcon %i%:command %s%:command

:�A�N�e�B�x�[�g
rtact %s% %v% %i%

:loop
set /p ans="�I�����܂����H (y/n)"
if not "%ans%"=="y" goto loop

:�f�B�A�N�e�B�x�[�g
rtdeact %s% %v% %i%

:�I���irtexit�́C����������j
rtexit %s%
rtexit %v%
rtexit %i%

taskkill /fi "WINDOWTITLE eq %pythonScript%"
