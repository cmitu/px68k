# ----from TurtleBazooka/px68k----

## From(thanx!)
* fork from kenyahiro(https://github.com/kenyahiro/px68k)
* Original fork from hissorii(https://github.com/hissorii/px68k)

## About
* 単独アプリ形式のエミュレータの改良を目指します。
* (DWORD)-1 で頻繁に異常終了する問題に対処
* SDL2のRenderer(GPU)を活用、よって"sdl-gfx"は不要
* Support Full-Screen Mode.(`F11`)
* ソフトキーボード復活(Right click on `F12`Menu mode)
* SCSI DiskImageサポート(`Can boot from *.HDS`)
* 最終画面出力を24bitに変更(RGB565→RGBA8888)
* `Support MIDI-Play` (Internal/Munt/fluidsynth/USB-MIDI)
* Support US-KeyBoard. `keyconf.dat` put into .keropi folder
* Printer出力はFileに保存
* Can use XBOX like USB-GamePad (hot-pluggable)
* Add support CyberStick! (DIGITAL/ANALOG mode)
* `FileNameのutf8/sjis` 自動判別日本語表示(専用table変換)
* 日本語TrueTypeから cgrom.tmp 生成可能
* macOS/Linux/MinGW64(win) で動作確認

## for macOS
* Mac用単独アプリ化ラッパー追加
* Mac用SDL2.frameworkでのbuildスクリプト添付
* pngからiconリソース生成機能搭載(% make icon)

## Need
* SDL2 (or test drive SDL3)(https://www.libsdl.org/)
* `iplrom.dat` `cgrom.dat` and SCSI-IPL `scsiexrom.dat` into .keropi folder
* `gamecontrollerdb.txt` into .keropi folder  (https://github.com/gabomdq/SDL_GameControllerDB)
* cmake and pkg-config build system (if needed)(https://cmake.org)
* Munt/MT-32 emu (if needed)(https://sourceforge.net/projects/munt/)
* fluidsynth and SoundFont2 (if needed)(https://www.fluidsynth.org/)

## Build (with make)

```sh
 $ make      (SDL2でLINK Linux/macOS/MinGW)
 $ make mac  (macOS only)

 $ make NO_MIDI=1 (No MIDI Support)
 $ make FLUID=1 (use fluidsynth for MIDI)

 $ make clean (お掃除)
 $ make cgrom (app for Generate cgrom.tmp)
```
## Build (need cmake)

```sh
 $ mkdir build
 $ cd build

 $ cmake ..
 $ cmake --build .
or
 $ cmake -D SDL3=ON .. (SDL3 test drive!)
 $ cmake --build .
```

## Run on command.

 ```sh
  $ ./px68k.sdl2
  $ ./px68k.sdl2 hoge0.xdf hoge1.xdf hame0.hds hame1.hds
 ```
 Support image file (mount and boot)

      FDD-image         D88,88D,HDM,DUP,2HD,DIM,XDF,IMG
      HDD-image(SASI)   HDF(10/20/40MB)
      HDD-image(SCSI)   HDS(max.900MB)
      MO -image(SCSI)   MOS(128/230/540/640MB) at ID=5

## Limitation
 * SCSI対応は現状IOCSレベル。よってNetBSD/X68kはboot不可
 * SCSI-IPLの機能もEmu側で実装予定 (未)
 * WinのMIDIはデバイスリストから選択可
 * macのMIDIはCoreMIDIデバイスリストから選択可
 * LinuxのMIDIは現状fluidsynthのみ
 * MIDI-INは暫定対応(WinMM,and CoreMIDI)
 * MIDIの音の割り付け順の変更はMIMPIトーンマップでも変更可
 * CyberStickのアナログモードは確認継続中(認識不完全アリ)
 * SDL1のサポートは削除しました。
 * SDL3はまだ準備／テスト段階です。(SDL2推奨)
 * BigEndianのCPUでは動きません。(例:Wii/U,Mac/G4)

## How to make `cgrom.tmp`
 * cgrom.datがない場合用に代替ファイルをTrueTypeから生成できます。
 * generate from Japanese-TrueType Font.(need `SDL2_ttf`)
 * maked `cgrom.tmp` put into ./keropi folder.

```sh
  $ make cgrom
  $ ./SDL3/tool/mkcgrom
  or
  $ ./SDL3/tool/mkcgrom ./pri.ttf ./sec.ttf
```

## How to make SCSI Disk image (*.HDS)
 * まず空のFileを作ります。(例：200MB)
 * (MacならDiskUtilitiesでdmg作って*.HDSにリネームするのが簡単)

```sh
  $ dd if=/dev/zero of=TEST.HDS bs=1m count=200
```
 * Run px68k.sdl2 with FDD0:Human68k 3.02, HDD0:TEST.HDS
 * on human68k run `format` `SCSI装置` `0 ハードディスク 199Mバイト` `装置初期化` `Y`
 * `領域確保` `容量 199` `ボリューム名 hoge` `システム転送 する` `実行` `Y`
 * `終了` `Y`　return to human68k prompt.

```sh
  A>copyall a:¥*.* c:
```

# ----from kenyahiro/px68k----

 px68kのフォーク。
 x86_64でコンパイルできるように修正しました。
 c68kをyabause 0.9.14のものに差し替えました。
 ICount周りが未実装です。
 とりあえずhumanが立ち上がるところまで。
 ゲームは多分ダメ。

# ----from hissorii/px68k----

[Please read original readme.txt](./readme.txt)
