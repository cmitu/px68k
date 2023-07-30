/*
  単独アプリケーション用のmain  by kameya
[実行]
% create_cgrom "./TrueType-URL(16x16)" "./Truetype-URL(24x24)"

*/

#include "common.h"
#include "dosio.h"
#include "mkcgrom.h"

#include "kanjiconv.c"

int
main(int argc, char *argv[]){

  uint8_t FONT[0xc0000+1000];
  char FONT1[MAX_PATH];
  char FONT2[MAX_PATH];
  static const char MKFONTFILE[] = "cgrom.tmp";

  //引数指定
  switch(argc)
  {
   case 3:
    strcpy((char *)FONT2, argv[2]);
    strcpy((char *)FONT1, argv[1]);
    break;
   case 2:
    strcpy((char *)FONT2, argv[1]);
    strcpy((char *)FONT1, argv[1]);
    break;
  }

  if(make_cgromdat(FONT, FONT1, FONT2, 0 ) != FALSE)
  {

   // Saving Fonts File
   FILE *fp;
   fp = File_CreateCurDir((char *)MKFONTFILE, FTYPE_BETA);
   if(fp)
   {
    File_Write(fp, FONT, 0xc0000);
    File_Close(fp);
   }

  }

return 1;
}
