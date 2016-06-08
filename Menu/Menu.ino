#include "TicTacToe.cpp"
#include "CarGame.cpp"
#include "ScreenSaver.cpp"
#include "AvoidGame.cpp"
#include "StackIt.cpp"

int screen_map[4][3]={
  {13,12,11},
  {10,9,8},
  {7,6,5},
  {4,3,2}
};

int cur_screen=0;
int total_screens=5;

//Define controls
int in_state[2]={0,0};
int in_map[2]={A0,A1};

int screens[5][4][3] = {
  { //One
    {0,1,1},
    {0,0,1},
    {0,0,1},
    {0,0,1}
  },
  { //Two
    {1,1,1},
    {0,1,0},
    {1,0,0},
    {1,1,1}
  },
  { //Three
    {1,1,1},
    {0,1,1},
    {0,0,1},
    {1,1,1}
  },
  { //Four
    {1,0,1},
    {1,1,1},
    {0,0,1},
    {0,0,1}
  },
  { //Five
    {1,1,1},
    {1,0,0},
    {0,1,1},
    {1,1,1}
  }
};

void setup() {
  //Set all screen pins to OUTPUT mode
  for (int i=0; i<4; i++){
    for (int j=0; j<3; j++){
      pinMode(screen_map[i][j], OUTPUT);
      digitalWrite(screen_map[i][j], true); //True is off (Yes I know fucking weird)
    }
  }
  //Set all button(/input) pins to input
  for (int i=0; i<2; i++){
    pinMode(in_map[i], INPUT);
  }
}

void updateInput(){
  //Check input change for both buttons
  for (int i=0; i<2; i++){
    if (digitalRead(in_map[i]) && in_state[i] == 0){
      //New press
      in_state[i]=1;
    }else if (digitalRead(in_map[i]) && in_state[i] == 1){
      //Holding (Held)
      in_state[i]=2;
    }else if (!digitalRead(in_map[i]) && (in_state[i] == 1 || in_state[i] == 2)){
      //Released
      in_state[i]=3;
    }else if (!digitalRead(in_map[i])){
      in_state[i]=0;
    }
  }
}

void render(){
  for (int i=0; i<4; i++){
    for (int j=0; j<3; j++){
      digitalWrite(screen_map[i][j], !(bool)screens[cur_screen][i][j]); //Render current menu choice (number) to screen
    }
  }
}

void clearScreen(){
  for (int i=0; i<4; i++){
    for (int j=0; j<3; j++){
      digitalWrite(screen_map[i][j], true);
    }
  }
}

void loop() {
  updateInput();
  if (in_state[0] == 1){
    cur_screen+=1;
    if (cur_screen >= total_screens){
      cur_screen=0;
    }
  }
  if (in_state[1] == 1){
    if (cur_screen == 0){
      clearScreen(); //Clear current screen (Not always needed)
      CarGame cargame; //Init game instance
      cargame.startCarGame(); //Start game
      in_state[0]=4;
      in_state[1]=4; //Avoid going to next menu option automatically after exiting game
    }else if (cur_screen == 1){
      clearScreen();
      TicTacToe ttt;
      ttt.startTicTacToe();
      in_state[0]=4;
      in_state[1]=4;
    }else if (cur_screen == 2){
      clearScreen();
      ScreenSaver ss;
      ss.startSS();
      in_state[0]=4;
      in_state[1]=4;
    }else if (cur_screen == 3){
      clearScreen();
      AvoidGame ag;
      ag.startAG();
      in_state[0]=4;
      in_state[1]=4;
    }else if (cur_screen == 4){
      clearScreen();
      StackIt si;
      si.startSI();
      in_state[0]=4;
      in_state[1]=4;
    }
  }
  render();
  delay(16);
}
