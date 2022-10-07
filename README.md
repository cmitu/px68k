# ----from TurtleBazooka/px68k----

## From(thanx!)
* fork from kenyahiro(https://github.com/kenyahiro/px68k)
* Original fork from hissorii(https://github.com/hissorii/px68k)

## About
* 単独アプリ形式のエミュレータの改良を目指します。
* ソースコードをutf8に変換
* (DWORD)-1 で頻繁に異常終了する問題に対処
* SDL2のRenderer(GPU)を活用、よって"sdl-gfx"は不要
* Support Full-Screen Mode.(`F11`)
* 正確なScreen描画の再現(表示域と描画域を個別演算)
* SCSI DiskImageサポート(`boot from *.HDS`)
* SCSI-IPLの機能もEmu側で実装する？(未)
* `Support MIDI-Play` (Internal/Munt/fluidsynth/USB-MIDI)
* MIDI音源FileにSoundFont2使用可(CoreAudio,fluidsynth)
* Support US-KeyBoard. `keyconf.dat` put into .keropi folder.
* stdint 形式に変数を変更
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
 ```

 ## Limitation
 * SCSI対応は現状IOCSレベル。 よってNetBSD/X68kはbootできません。
 * WinのMIDIはMIDI_MAPPERとデバイスリストから選択可
 * macのMIDIは内蔵シンセ(CoreAudio)、MuntやUSB-MIDIはCoreMIDIで
 * LinuxのMIDIは現状fluidsynthのみ
 * MIDI-INには非対応
 * MIDIの音の割り付け順の変更はMIMPIトーンマップでも変更可
 * SDL1でのkeymapデコードは不完全(¥、ろ)
 * SDL1の描画はCPU依存、よって遅いです。


# ----from kenyahiro/px68k----

 px68kのフォーク。
 x86_64でコンパイルできるように修正しました。
 c68kをyabause 0.9.14のものに差し替えました。
 ICount周りが未実装です。
 とりあえずhumanが立ち上がるところまで。
 ゲームは多分ダメ。

# ----from hissorii/px68k----

[Please read original readme.txt](./readme.txt)
