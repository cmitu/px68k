Part of SDL2 for px68k.

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

11/23/2021 from kameya.

[日本語解説] by kameya
Winにはxm6というスバらしいエミュがありますよね。
しかし、Macにはここ最近、単独アプリのX68Kのエミュはありません。(PowerPCの頃にはあった)
RetroArchのPluginで、PX68Kが出ているのが唯一の救いです。

単独アプリでMacで動くX68Kエミュレータがあったらいいな、と思い
kenyahiroさんが64bit対応されてましたGitから、Forkさせていただきました。(感謝)
Framework版のSDL2をメインに、Mac(mid2009/HighSierra)で開発しています。
主な特徴は以下です。
・SDL2のGPU活用(Texture、Renderer活用)
・フルスクリーンモード追加(F11 SDL2のみ)
・SCSIのDiskImageサポート(*.HDSからBoot可)
・MIDI出力対応(Win/Mac)　※Linuxは未
・File選択時の表示をutf8対応に(X68搭載Fontの範囲で)
・グラフィックモードの再現性向上
・LinuxやMinGW(win)でもコンパイル可能
・Macの単独アプリとしての生成リソースを追加

[実行]
SDL2のFrameWorkを/Library/Frameworksに入れておいてください。

[コンパイル]
ソースからコンパイルする場合は、SDL2のFrameworkはHeaderの相互関係が不完全（2.26.1）
なので、以下のLINKを張ってください。
% sudo ln -s /Library/Frameworks/SDL2.framework/Versions/A/Headers /Library/Frameworks/SDL2.framework/Headers/SDL2

% make
and..
% make mac

欠点もたくさんあります。
Android/iPhoneでの動作確認はしていません。
SCSIはIOCSレベルなのでNetBSD/X68Kやコントローラを
直接操作するドライバ系は動きません。
MIDI-INはサポートしてません。

utf8対応は最初iconvを使ってましたがX68Kの搭載fontとの相性が
イマイチと感じたので、結局Table変換してます。

ネット上でX68000の解析や改造を精力的に行われてる方々が公開
されている情報にホントに助けられました。 このユーザパワーは
素晴らしいですね。　あらためてお礼申し上げます。
