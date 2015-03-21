// **************************************************
// * CDKColors.cpp: generate a visual map of the CDK color identifiers
// *
// * Lucas Gonze <address@hidden>
// * 10/20/02
// **************************************************

extern "C"
{
#include <cdk.h>
}

char *txt[] = {
  "</0>Color pair 0 looks like this.",
  "</1>Color pair 1 looks like this.",
  "</2>Color pair 2 looks like this.",
  "</3>Color pair 3 looks like this.",
  "</4>Color pair 4 looks like this.",
  "</5>Color pair 5 looks like this.",
  "</6>Color pair 6 looks like this.",
  "</7>Color pair 7 looks like this.",
  "</8>Color pair 8 looks like this.",
  "</9>Color pair 9 looks like this.",
  "</10>Color pair 10 looks like this.",
  "</11>Color pair 11 looks like this.",
  "</12>Color pair 12 looks like this.",
  "</13>Color pair 13 looks like this.",
  "</14>Color pair 14 looks like this.",
  "</15>Color pair 15 looks like this.",
  "</16>Color pair 16 looks like this.",
  "</17>Color pair 17 looks like this.",
  "</18>Color pair 18 looks like this.",
  "</19>Color pair 19 looks like this.",
  "</20>Color pair 20 looks like this.",
  "</21>Color pair 21 looks like this.",
  "</22>Color pair 22 looks like this.",
  "</23>Color pair 23 looks like this.",
  "</24>Color pair 24 looks like this.",
  "</25>Color pair 25 looks like this.",
  "</26>Color pair 26 looks like this.",
  "</27>Color pair 27 looks like this.",
  "</28>Color pair 28 looks like this.",
  "</29>Color pair 29 looks like this.",
  "</30>Color pair 30 looks like this.",
  "</31>Color pair 31 looks like this.",
  "</32>Color pair 32 looks like this."
};

char *txt2[] = {
"</32>Color pair 32 looks like this.",
"</33>Color pair 33 looks like this.",
"</34>Color pair 34 looks like this.",
"</35>Color pair 35 looks like this.",
"</36>Color pair 36 looks like this.",
"</37>Color pair 37 looks like this.",
"</38>Color pair 38 looks like this.",
"</39>Color pair 39 looks like this.",
"</40>Color pair 40 looks like this.",
"</41>Color pair 41 looks like this.",
"</42>Color pair 42 looks like this.",
"</43>Color pair 43 looks like this.",
"</44>Color pair 44 looks like this.",
"</45>Color pair 45 looks like this.",
"</46>Color pair 46 looks like this.",
"</47>Color pair 47 looks like this.",
"</48>Color pair 48 looks like this.",
"</49>Color pair 49 looks like this.",
"</50>Color pair 50 looks like this.",
"</51>Color pair 51 looks like this.",
"</52>Color pair 52 looks like this.",
"</53>Color pair 53 looks like this.",
"</54>Color pair 54 looks like this.",
"</55>Color pair 55 looks like this.",
"</56>Color pair 56 looks like this.",
"</57>Color pair 57 looks like this.",
"</58>Color pair 58 looks like this.",
"</59>Color pair 59 looks like this.",
"</60>Color pair 60 looks like this.",
"</61>Color pair 61 looks like this.",
"</62>Color pair 62 looks like this.",
"</63>Color pair 63 looks like this.",
"</64>Color pair 64 looks like this."
};


class CDKColors {
 protected:
  WINDOW *cursesWin;
  CDKSCREEN *cdkscreen;

  void init_screen(){

    cdkscreen = initCDKScreen(cursesWin);
    initCDKColor();

    newCDKLabel (cdkscreen, 10, TOP, txt, 32, FALSE, FALSE);
    newCDKLabel (cdkscreen, 50, TOP, txt2, 32, FALSE, FALSE);

  }

 public:
  CDKSWINDOW *swindow; 

  CDKColors(){
    cursesWin = initscr();
    init_screen();
    refreshCDKScreen (cdkscreen);
  }

  void recalc_window(){

    if( !cdkscreen )
      return;

    // reinitialize the screen to match new coordinates
    destroyCDKScreen (cdkscreen);
    endCDK();
    init_screen();

    // redraw to pickup changes
    refreshCDKScreen(cdkscreen);

  }

  void wait_for_any_key(){

    while(true){
      switch( wgetch (cursesWin) ){
      case KEY_RESIZE:
        recalc_window();
        break;
      default:
        return;
      }
    }
  }

  ~CDKColors(){
    destroyCDKScreen(cdkscreen);
    delwin (cursesWin);
    endCDK();
  }

};

CDKColors *testWin = NULL;

int main (int argc, char **argv){

  testWin = new CDKColors();
  testWin->wait_for_any_key();  
  delete testWin;
}
