# Point Cloud Grabber for Intel RealSense

大阪電気通信大学  
升谷 保博  
2017年4月22日

## はじめに

- Intel RealSenseから深度と色の情報を読み取り，RTC:PCLのPointCloud型（`PointCloudTypes::PointCoud`）を
出力するRTコンポーネントです．
- 以下の環境で開発，動作確認しています．
  - Windows 10 64bit版
  - Visual Studio 2012
  - OpenRTM-aist 1.1.2 64bit版
  - Point Cloud Library 1.7.2 AllinOne (VS2012 64bit用)
  - Intel RealSense Developer Kit (R200)
  - Intel® RealSense SDK Essentials for Windows Version 11.0
  - R200 Camera Driver Build Number: 2.1.27.2853
- Sergey Alexandrov (taketwo) 氏の
[PCL grabber for RealSense devices](https://github.com/taketwo/rs)のコードを
そのまま利用しています．それらはディレクトリrsに置いています．
- CMakeでRealSense SDKを見つけるモジュールもSergey Alexandrov氏の
[`FindRSSDK.cmake`](https://github.com/taketwo/rs/blob/master/cmake/Modules/FindRSSDK.cmake)
をそのまま使っています．
- `pointcloud.idl` は，Geoffrey Biggs (gbiggs)氏の
[RT-Components for the Point Cloud Library](https://github.com/gbiggs/rtcpcl/)
に[含まれているもの](https://github.com/gbiggs/rtcpcl/blob/master/pc_type/pointcloud.idl)
をそのまま使っています．

## インストール

- [OpenRTM-aist 1.1.2](http://www.openrtm.org/openrtm/ja/node/6034)をインストール．
- [PCL-1.7.2のWindows用AllInOne](https://onedrive.live.com/?authkey=!ACFnPNzPhXJ0FtU&id=EC9EBB2646FF189A!49089&cid=EC9EBB2646FF189A)
をインストール（2017年3月下旬に削除されたようです）．
- [Intel RealSense Camera Driver Software](https://software.intel.com/en-us/intel-realsense-sdk/download)
をインストール．
- [Intel RealSense SDK Essentials for  Windows](https://registrationcenter.intel.com/en/forms/?productid=2797)
をインストール．
- [RealSenseToPC](https://github.com/MasutaniLab/RealSenseToPC)
をクローンかダウンロードする．
- CMake
  - ビルドディレクトリはトップ直下の`build`
  - ConfigureはVisual Studio 64bit
- `build\RealSenseToPC.sln`をVisual Studioで開く．
- パフォーマンスを出すために，Releaseでビルドがお勧め．

## 使い方

- RealSenseは，必ずUSB 3.0のポートに接続すること．2.0では動作しません．
- 出力されるデータ量が多いので，628×468の場合，CORBAのデフォルトの設定ではエラーになります．
rtc.confに`corba.args: -ORBgiopMaxMsgSize`の設定が必要です．
トップディレクトリのrtc.confでは`corba.args: -ORBgiopMaxMsgSize 20971520`
にしています（デフォルト値の10倍）．
- コンポーネントを起動するバッチファイル`RealSenseToPCComp.bat`を用意しています．
  - ビルドディレクトリがトップ直下の`build`であることを仮定しています．
  - 環境変数`RTM_VC_CONFIG`を`Debug`か`Release`に設定してください．
- 動作確認のための接続相手として，
[PointCloudViewer](https://github.com/MasutaniLab/PointCloudViewer)
を使ってください．

### コンフィギュレーション
- window: 時間的フィルタのウィンドウサイズ
- threshold: 信頼度しきい値
- temporal_filtering: 時間的フィルタの種類（0:なし，1:メジアン, 2:平均）
- with_bilateral: バイラテラルフィルタ（0:なし，1:あり）
- bilateral_sigma_s: バイラテラルフィルタの空間の標準偏差
- bilateral_sigma_r: バイラテラルフィルタのレンジの標準偏差
- mode: RealSenseの動作モード
- device_id: シリアル番号（例:231400041-03）かデバイスインデックス（例:#2ならば2番目に接続したもの）

## 既知の問題・TODO

- type "xyzrgb"しか出力できません．
- 1点のサイズは16byte，320×240の場合1回に送出するデータ量は1.2MB．もっと圧縮すべきかもしれません．
- Windowsでしか動作しません．
- [PCL grabber for RealSense devices](https://github.com/taketwo/rs)のコードは
PCL 1.8に取り込まれたそうですので，PCL 1.8では，もっと簡単に作ることができるはずです．
