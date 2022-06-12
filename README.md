以下の改良を予定しています。<br>

・ソースを全面的にutf8に変換<br>
・(DWORD)−1でsegmantation fault する問題に対処(異常終了対策)<br>
・SDL2メイン対応(SDL1もサポート)<br>
・SDL2 Renderer活用 (sdl-gfx不要)<br>
・F11でFullScreenモード対応(SDL2)[SDL1は未]<br>
・SCSIサポート(HDSイメージからのBoot)<br>
・SCSI-IPLの機能もEmu側で実装する？(未)<br>
・MIDI演奏サポート(内蔵音源/MT32エミュ/fluidsynth/実機器)<br>
・変数をstdint形式に変更(なるべく)<br>
・File名のutf8/sjis自動判別表示<br>
・Mac用SDL1/2.frameworkでのbuildスクリプト添付<br>
・Mac用単独アプリ化ラッパー追加[完了]<br>
・PSPサポート削除(32bit-ASM削除につき)<br>
・macOS/Linux/MinGW で動作確認<br>
<br>
$ make       (SDL2でLINK Linux/macOS/MinGW)<br>
$ make SDL=1 (SDL1でLINKの場合)<br>
$ make mac  (macOS only)<br>
$ make FLUID=1 (MIDI出力にfluidsynthを使う)<br>
<br>
-------------追記ここまで---------------<br>
<br>
px68kのフォーク。<br>
x86_64でコンパイルできるように修正しました。<br>
c68kをyabause 0.9.14のものに差し替えました。<br>
ICount周りが未実装です。<br>
とりあえずhumanが立ち上がるところまで。<br>
ゲームは多分ダメ。<br>

