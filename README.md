# Point Cloud Grabber RTC Using Intel RealSense SDK 2

大阪電気通信大学  
升谷 保博  
2018年3月30日

## はじめに

- Intel RealSenseから深度と色の情報を読み取り，RTC:PCLのPointCloud型（`PointCloudTypes::PointCoud`）を
出力するRTコンポーネントです．RealSense SDK 2を使っており，D400シリーズで動作します．R200には対応していません．
- 以下の環境で開発，動作確認しています．
  - Windows 10 64bit版
  - Visual Studio 2015 x64
  - [OpenRTM-aist 1.1.2 64bit版](https://github.com/IntelRealSense/librealsense/)
  - [Point Cloud Library 1.8.1 AllinOne (VS2015 64bit用)](https://github.com/PointCloudLibrary/pcl/releases/tag/pcl-1.8.1)
  - [Intel® RealSense™ SDK 2.0](https://github.com/IntelRealSense/librealsense/)
- [PCLのリポジトリで議論されているPR](https://github.com/PointCloudLibrary/pcl/pull/2214)を少し変更して使っています．
- CMakeでRealSense SDKを見つけるモジュール[`cmake/Modules/FindRealSense2.cmake`]は，
[INRIA Rennes Bretagne Atlantique](https://github.com/lagadic)の[vispに付属のもの](https://github.com/lagadic/visp/blob/master/cmake/FindRealSense2.cmake)を真似して作りました．
- `pointcloud.idl` は，Geoffrey Biggs (gbiggs)氏の
[RT-Components for the Point Cloud Library](https://github.com/gbiggs/rtcpcl/)
に[含まれているもの](https://github.com/gbiggs/rtcpcl/blob/master/pc_type/pointcloud.idl)
をそのまま使っています．
- 出力する点群を表す座標系は，y軸は上向きが正，z軸は後ろ向きが正です（それぞれRealSenseの座標系と逆向き，x軸はどちらも右向きが正）．
Choeonoidの深度センサのモデルに合わせるためにこのようにしています．

## インストール

- [OpenRTM-aist 1.1.2](http://www.openrtm.org/openrtm/ja/node/6034)をインストール．
- [GitHubのpclのRelease](https://github.com/PointCloudLibrary/pcl/releases)の中のWindows用AllInOne`PCL-X.X.X-AllInOne-msvcYYYY-winZZ.exe`をダウンロードし実行．
- [GitHubのlibrealsenseのRelease](https://github.com/IntelRealSense/librealsense/releases)の中のWindows用インストーラ`Intel.RealSense.SDK.exe` をダウンロードし実行．
- 環境変数
  - Pathの値の並びに以下を追加．
    - `%PCL_ROOT%\bin`  
    - `%OPENNI2_REDIST64%`  
    - `C:\Program Files (x86)\Intel RealSense SDK 2.0\bin\x64` （または`C:\Program Files (x86)\Intel RealSense SDK 2.0\bin\x86`）
- [RealSense2ToPC](https://github.com/MasutaniLab/RealSense2ToPC)
をクローンかダウンロードする．
- CMake
  - ビルドディレクトリはトップ直下の`build`
  - ConfigureはVisual Studioのバージョンとプラットフォームに合わせる．
- `build\RealSense2ToPC.sln`をVisual Studioで開く．
- パフォーマンスを出すために，Releaseでビルドがお勧め．

## 使い方

- RealSenseは，必ずUSB 3.0のポートに接続すること．2.0では動作しません．
- 出力されるデータ量が多いので，CORBAのデフォルトの設定ではエラーになります．
rtc.confに`corba.args: -ORBgiopMaxMsgSize`の設定が必要です．
トップディレクトリのrtc.confでは`corba.args: -ORBgiopMaxMsgSize 20971520`
にしています（デフォルト値の10倍）．
- コンポーネントを起動するバッチファイル`RealSense2ToPC.bat`を用意しています．
  - ビルドディレクトリがトップ直下の`build`であることを仮定しています．
  - 環境変数`RTM_VC_CONFIG`を`Debug`か`Release`に設定してください．
- 動作確認のための接続相手として，
[PointCloudViewer](https://github.com/MasutaniLab/PointCloudViewer)
を使ってください．

### コンフィギュレーション
今のところ何もありません．

## 既知の問題・TODO

- type "xyzrgb"しか出力できません．
- 解像度を変更することができません．
- キャリブレーション．
