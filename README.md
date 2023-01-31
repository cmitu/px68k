# ----from TurtleBazooka/px68k----

## From(thanx!)
* fork from kenyahiro(https://github.com/kenyahiro/px68k)
* Original fork from hissorii(https://github.com/hissorii/px68k)

## About
* 単独アプリ形式のエミュレータの改良を目指します。
* ソースコードをutf8に変換
* ソースコードをstdint形式に変数変更
* (DWORD)-1 で頻繁に異常終了する問題に対処
* SDL2のRenderer(GPU)を活用、よって"sdl-gfx"は不要
* Support Full-Screen Mode.(`F11`)
* 正確なScreen描画の再現(表示域と描画域を個別演算)
* SCSI DiskImageサポート(`boot from *.HDS`)
* SCSI-IPLの機能もEmu側で実装する？(未)
* `Support MIDI-Play` (Internal/Munt/fluidsynth/USB-MIDI)
* MIDI音源FileにSoundFont2使用可(fluidsynth)
* Support US-KeyBoard. `keyconf.dat` put into .keropi folder.
* Printer出力をFileに保存
* サイバースティックのサポート追加(Analog3軸-XYZ JoyPad)
* `FileNameのutf8/sjis` 自動判別(iconvではなく専用table変換)
* Mac用SDL1/2.frameworkでのbuildスクリプト添付
* Mac用単独アプリ化ラッパー追加
* macOS/Linux/MinGW64(win) で動作確認

## Need
* SDL2 (still support SDL1)(https://www.libsdl.org/)
* `iplrom.dat` `cgrom.dat` and SCSI-IPL `scsiexrom.dat` in .keropi folder
* Munt/MT-32 emu (if needed)(https://sourceforge.net/projects/munt/)
* fluidsynth and SoundFont2 (if needed)(https://www.fluidsynth.org/)

## Build

```sh
 $ make       (SDL2でLINK Linux/macOS/MinGW)
 $ make SDL=1 (SDL1でLINKの場合)
 $ make mac  (macOS only)
 $ make FLUID=1 (MIDI出力にfluidsynthを使う)

 $ make clean (お掃除)
 $ make SDL=1 clean (SDL1 お掃除)

 $ make cgrom (app for Generate cgrom.tmp)
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

## Limitation
 * SCSI対応は現状IOCSレベル。 よってNetBSD/X68kはbootできません。
 * WinのMIDIはデバイスリストから選択可
 * macのMIDIはCoreMIDIデバイスリストから選択可
 * LinuxのMIDIは現状fluidsynthのみ
 * MIDI-INは暫定対応(WinMM,and CoreMIDI)
 * MIDIの音の割り付け順の変更はMIMPIトーンマップでも変更可
 * SDL1でのkeymapデコードは不完全(¥、ろ)
 * SDL1の描画はCPU依存、よって遅いです。

## How to make `cgrom.tmp`
 * cgrom.datがない場合用に代替ファイルをTrueTypeから生成できます。
 * generate from Japanese-TrueType Font.
 * maked `cgrom.tmp` put into ./keropi folder.

```sh
  $ ./SDL2/tool/mkcgrom
  or
  $ ./SDL2/tool/mkcgrom ./pri.ttf ./sec.ttf
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
