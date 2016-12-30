# TTXCommandLineOptKai

フォーラムでの「[マクロでの文字色と背景色の変更](https://ja.osdn.net/projects/ttssh2/forums/5841/21746/ "マクロでの文字色と背景色の変更")」の議論を参考に、
[TTXCommandLineOpt](https://ja.osdn.net/projects/ttssh2/scm/svn/tree/head/tags/teraterm-4_86/TTXSamples/TTXCommandLineOpt/ "TTXCommandLineOpt") のコードをベースとして。太字や点滅の背景色も変更されるように対応させたものです。

## 使用方法

ttermpro.exe と同じフォルダに TTXCommandLineOpt.dll を配置します。
ホストの指定に `192.168.0.1 /FG=0,255,0 /BG=0,0,0` とすることで文字色、背景色が指定できます。