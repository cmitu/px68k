以下の改良を予定しています。

・ソースを全面的にutf8に変換<br>
・(DWORD)−1でsegmantation fault する問題に対処<br>
・SDL2メイン対応(SDL1もサポート)<br>
・画面解像度の最適化(sdl-gfx不要)<br>
・F11でFullScreenモード対応(SDL2)<br>
・SCSIサポート(HDSイメージからのBoot)<br>
・変数をstdint形式に変更(なるべく)<br>
・File名のutf8/sjis表示自動判別<br>
・Mac用SDL1/2.frameworkでのbuildスクリプト添付<br>
・Mac用単独アプリ化ラッパー追加<br>
<br>
$ make.  (Linux/macOS)<br>
$ make mac (macOS only)<br>
<br>
-------------追記ここまで---------------<br>
<br>
px68kのフォーク。<br>
x86_64でコンパイルできるように修正しました。<br>
c68kをyabause 0.9.14のものに差し替えました。<br>
ICount周りが未実装です。<br>
とりあえずhumanが立ち上がるところまで。<br>
ゲームは多分ダメ。<br>

