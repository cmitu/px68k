Part of SDL2 for px68k.
written by kameya  2021/11/23

folder [Contents]
for macOSⅩ build for double clickable aplication resource.

script [sdl2-config-mac]
install SDL2 for FrameWorks, but not include sdl2-config.
this script for --lib, --cflags LINKER option, for FrameWorks SDL2.

px68k build for mac.

Caution!
SDL2-Framework(2.26.1) have header problem.
make LINK,,,
% sudo ln -s /Library/Frameworks/SDL2.framework/Versions/A/Headers /Library/Frameworks/SDL2.framework/Headers/SDL2

$ make
and...
$ make mac

build for GNU/Linux/MinGW(win)
$ make

joy for your Mac life!



[日本語解説] by kameya
Winにはxm6というスバらしいエミュがありますよね。
しかし、Macにはここ最近、単独アプリのX68Kのエミュはありません。(PowerPCの頃にはあった)
RetroArchのPluginで、PX68Kが出ているのが唯一の救いです。

単独アプリでMacで動くX68Kエミュレータがあったらいいな、と思い
kenyahiroさんが64bit対応されてましたGitから、Forkさせていただきました。(感謝)
Framework版のSDL2をメインに、Mac(mid2009/HighSierra)で開発しています。
主な特徴は以下です。
・SDL2のGPU活用(Texture、Renderer活用)
・フルスクリーンモード追加(F11)
・24bitカラーで最終出力(RGB565→RGBA8888に変更)
・SDL1もサポート継続中(FullScreen、正確な画面描画もサポート)
・SCSIのDiskImageサポート(*.HDSからBoot可)
・MOイメージサポート(128/230/540/640MBの*.MOSをマウント可)
・MIDI出力対応(Win/Mac)　※LinuxはFluidsynthのみ
・MIDI-INも一応対応(Win/Mac)
・PrintOutをFileに出力
・USキーボードのサポート(keyconf.datを./keropiに入れてください)
・cgrom.tmpを自前生成可能(cgrom.datは配布されてない)
・ソフトキーボードサポート復活(Menu上で右クリック)
・File選択時のリスト表示をutf8対応に(X68搭載Fontの範囲で)
・グラフィックモードの再現性向上(横スクロール、部分表示)
・XBOXコンパチのGamwControllerをサポート
・サイバースティックのアナログ入力にも対応
・LinuxやMinGW(win)でもコンパイル可能
・Macの単独アプリとしての生成リソースを追加

[実行]
SDL2のFrameWorkを/Library/Frameworksに入れておいてください。
cgrom.tmpを自前生成する場合はSDL2_ttf.frameworkも必要です。

[コンパイル]
ソースからコンパイルする場合は、SDL2のFrameworkはHeaderの相互関係が不完全（2.26.1）
なので、以下のLINKを張ってください。 （ホントはxCodeを使えばいいのでしょうけど・・・）
% sudo ln -s /Library/Frameworks/SDL2.framework/Versions/A/Headers /Library/Frameworks/SDL2.framework/Headers/SDL2

% make
and..
% make mac

[cgrom.tmpの生成機能]
配布されていないcgromをOSのTrueTypeから生成する場合は、
% make cgrom
% ./SDL2/tool/mkcgrom で実行、生成。
できあがったcgrom.tmpを./keropiに入れておいてください。

[GameController対応]
SDL2のGameController対応は、対応したGamePad記述のあるgamecontrollerdb.txt
を./keropiに入れておいてください。

欠点もたくさんあります。
動作実績のあったAndroid/iPhoneでの動作確認はしていません。
SCSIサポートはIOCSレベルなのでNetBSD/X68KやSCSIコントローラを
直接操作するドライバ系は動きません。
インジケータ類(HDDのアクセスLEDなど)は皆無。でもシンプルでいいよね？(笑)

utf8対応は最初iconvを使ってみましたがX68Kの搭載fontとの相性が
イマイチと感じたので、結局Table変換してます。utf8-macでも濁点表示可能です。

ネット上でX68000の解析や改造を精力的に行われてる方々が公開
されている情報にホントに助けられました。 このユーザパワーは
素晴らしいですね。　あらためてお礼申し上げます。
