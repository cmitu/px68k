# -------from TurtleBazoola/px68k-------

## From(thanx!)
* fork from kenyahiro(https://github.com/kenyahiro/px68k)
* Original fork from hissorii(https://github.com/hissorii/px68k)


## About
* 単独アプリ形式のエミュレータの改良を目指します。
* ソースコードをutf8に変換
* (DWORD)-1 で頻繁に異常終了する問題に対処
* SDL2のRenderer(GPU)を活用
*  よって"sdl-gfx"は不要
* フルスクリーンモード・サポート(SDL2:`F11`)
* 正確なScreen描画の再現(表示域と描画域を個別演算)
* SCSI DiskImageサポート(`boot from *.HDS`)
* SCSI-IPLの機能もEmu側で実装する？(未)
* `Support MIDI-Play` (Internal/Munt/fluidsynth/USB-MIDI)
* MIDI音源FileにSoundFont2使用可(CoreAudio,fluidsynth)
* keyconf.datを.keropiに置くことでUS-KeyBoardをサポート
* stdint 形式に変数を変更
* `FileNameのutf8/sjis` 自動判別(iconvではなく専用table変換)
* Mac用SDL1/2.frameworkでのbuildスクリプト添付
* Mac用単独アプリ化ラッパー追加
* macOS/Linux/MinGW64(win) で動作確認

## Need
* SDL2 (still support SDL1)(https://www.libsdl.org/)
* Munt/MT-32 emu.(https://sourceforge.net/projects/munt/)
* fluidsynth and SoundFont2 (if needed)(https://www.fluidsynth.org/)
* SCSI-IPL `scsiexrom.dat` in ./keropi folder

## Build

```sh
 $ make       (SDL2でLINK Linux/macOS/MinGW)
 $ make SDL=1 (SDL1でLINKの場合)
 $ make mac  (macOS only)
 $ make FLUID=1 (MIDI出力にfluidsynthを使う)
 ```

 ## Limitation
 * SCSI対応は現状IOCSレベル。 よってNetBSD/68kはbootできません。
 * WinのMIDIはMIDI_MAPPER固定(あとはOS任せ)
 * macのMIDIは内蔵シンセ(CoreAudio)、MuntやUSB-MIDIはCoreMIDIで
 * Linuxは現状fluidsynthのみ
 * 音の割り付け順の変更はMIMPIトーンマップでも変更可
 * SDL1でのkeymap.confは


# -------from kenyahiro/px68k-------

 px68kのフォーク。
 x86_64でコンパイルできるように修正しました。
 c68kをyabause 0.9.14のものに差し替えました。
 ICount周りが未実装です。
 とりあえずhumanが立ち上がるところまで。
 ゲームは多分ダメ。

# -------from hissorii/px68k-------

[Please read original readme.txt](./readme.txt)
