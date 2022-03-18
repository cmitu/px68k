以下の改良を予定しています。<br>

・ソースを全面的にutf8に変換[完了]<br>
・(DWORD)−1でsegmantation fault する問題に対処[90%]<br>
・SDL2メイン対応(SDL1もサポート)[80%]<br>
・画面解像度の最適化(sdl-gfx不要)[完了]<br>
・F11でFullScreenモード対応(SDL2)[SDL1が未]<br>
・SCSIサポート(HDSイメージからのBoot)[完了]<br>
・SCSI-IPLの機能もEmu側で実装する？<br>
・変数をstdint形式に変更(なるべく)[完了]<br>
・File名のutf8/sjis表示自動判別[完了]<br>
・Mac用SDL1/2.frameworkでのbuildスクリプト添付[完了]<br>
・Mac用単独アプリ化ラッパー追加[完了]<br>
・macOS/Linux/MinGW で動作確認<br>
<br>
$ make       (SDL2でLINK Linux/macOS/MinGW)<br>
$ make SDL=1 (SDL1でLINKの場合)<br>
$ make mac  (macOS only)<br>
<br>
-------------追記ここまで---------------<br>
<br>
px68kのフォーク。<br>
x86_64でコンパイルできるように修正しました。<br>
c68kをyabause 0.9.14のものに差し替えました。<br>
ICount周りが未実装です。<br>
とりあえずhumanが立ち上がるところまで。<br>
ゲームは多分ダメ。<br>

