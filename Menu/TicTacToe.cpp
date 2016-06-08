#include "Arduino.h"

class TicTacToe{
  public:
    void startTicTacToe(){
      setupTicTacToe();
      loopTicTacToe();
    }
  private:
    int board[9]={0,0,0,0,0,0,0,0,0}; //TicTacToe Board data
    int suc_board[9]={0,0,0,0,0,0,0,0,0}; //Board shown when game is won
    int board_map[9]={13,12,11,10,9,8,7,6,5}; //Maps LED pins for rendering board
    
    int in_state[2]={4,4};  //0=Not Pressed, 1=Pressed, 2=Held, 3=Released
                            //Read as 4 to avoid confusion due to menu button press before game launch
    int in_map[2]={A0,A1};  //Maps button inputs
    
    //For blinking lights (circle)
    int blinkstate=0;
    int blinktimer=0;
    //For faster blinking lights (cursor)
    int fastblinkstate=0;
    int fastblinktimer=0;
    
    int selected=4;
    
    int state=0; //0=Game, 1=Game ended in 1, 2=Tie, 3=Show current player
    
    int whoami_timer=0; //Timer for "Who am I?" render
    
    int cur_player=0; //0=Cross, 1=Circle

    int exit_timer=0;
    int exit_timer_max=180; //~3 sec

    void setupTicTacToe() {
      //Set output mode for board pins
      for (int i=0;i<9;i++){
        pinMode(board_map[i], OUTPUT);
      }
      //Set input mode for buttons
      for (int i=0;i<2;i++){
        pinMode(in_map[i], INPUT);
      }
    }
    
    void render(){
      if (state == 0 || state == 2){
        //Render board
        for (int i=0;i<9;i++){
          switch(board[i]){
            case 0: //Empty
              digitalWrite(board_map[i], true);
              break;
            case 1: //Cross
              digitalWrite(board_map[i], false);
              break;
            case 2: //Circle
              digitalWrite(board_map[i], (bool)blinkstate);
              break;
          }
        }
        //Render cursor
        if (state == 0)
         digitalWrite(board_map[selected], (bool)fastblinkstate);
      }else if (state == 1){
        for (int i=0;i<9;i++){
          switch(suc_board[i]){
            case 0: //Empty
              digitalWrite(board_map[i], true);
              break;
            case 1: //Cross
              digitalWrite(board_map[i], false);
              break;
            case 2: //Circle
              digitalWrite(board_map[i], (bool)blinkstate);
              break;
          }
        }
      }else{ //Aka state=3
        if (cur_player == 1){
          digitalWrite(board_map[0], true);
          digitalWrite(board_map[1], false);
          digitalWrite(board_map[2], true);
          digitalWrite(board_map[3], false);
          digitalWrite(board_map[4], true);
          digitalWrite(board_map[5], false);
          digitalWrite(board_map[6], true);
          digitalWrite(board_map[7], false);
          digitalWrite(board_map[8], true);
        }else{
          digitalWrite(board_map[0], false);
          digitalWrite(board_map[1], true);
          digitalWrite(board_map[2], false);
          digitalWrite(board_map[3], true);
          digitalWrite(board_map[4], false);
          digitalWrite(board_map[5], true);
          digitalWrite(board_map[6], false);
          digitalWrite(board_map[7], true);
          digitalWrite(board_map[8], false);
        }
      }
    }
    
    void updateInput(){
      //Check input change for both buttons
      for (int i=0; i<2; i++){
        if (analogRead(in_map[i]) > 0 && in_state[i] == 0){
          //New press
          in_state[i]=1;
        }else if (analogRead(in_map[i]) > 0 && in_state[i] == 1){
          //Holding (Held)
          in_state[i]=2;
        }else if (analogRead(in_map[i]) < 1 && (in_state[i] == 1 || in_state[i] == 2)){
          //Released
          in_state[i]=3;
        }else if (analogRead(in_map[i]) < 1){
          in_state[i]=0;
        }
      }
    }
    
    void moveCursor(){
      bool done=false;
      int i=selected,j=0;
      while (!done){
        i+=1;
        j+=1;
        if (i>=9){
          i=0;
        }
        //If empty, or if inf loop would be caused otherwise
        if (board[i] == 0 || j>8){
          selected=i;
          done=true;
        }
      }
    }
    
    void updateBoard()
    {
      int free_pos=0; //Counts free positions on the board
      for (int i=0; i<9; i++){
        if (board[i] == 0){
          free_pos+=1;
        }
      }
      //Check for 3 in a row
      for (int j=1; j<3; j++){
        for (int i=0; i<3; i++){
          //Vertical
          if (board[i] == j && board[3+i] == j && board[6+i] == j){
            suc_board[i]=j;
            suc_board[3+i]=j;
            suc_board[6+i]=j;
            state=1;
          }
          //Horizontal
          if (board[i*3] == j && board[i*3+1] == j && board[i*3+2] == j){
            suc_board[i*3]=j;
            suc_board[i*3+1]=j;
            suc_board[i*3+2]=j;
            state=1;
          }
        }
        //Diagnal top-left, bottom-right
        if (board[0] == j && board[4] == j && board[8] == j){
          suc_board[0]=j;
          suc_board[4]=j;
          suc_board[8]=j;
          state=1;
        }
        //Diagnal bottom-left, top-right
        if (board[2] == j && board[4] == j && board[6] == j){
          suc_board[2]=j;
          suc_board[4]=j;
          suc_board[6]=j;
          state=1;
        }
      }
      //If game not won, and no free spaces set game to tie.
      if (free_pos == 0 && state != 1){
        state=2;
      }
    }
    
    void place(int pos){
      //Set board pos to current player
      board[pos]=cur_player+1;
      cur_player=abs(cur_player-1); //Switch Players
      updateBoard();
    }
    
    void reset(){
      //Reset game
      for (int i=0; i<9; i++){
        board[i]=0;
        suc_board[i]=0;
      }
      selected=4;
      cur_player=0;
    }
    
    void loopTicTacToe() { 
      //Keep looping
      bool running=true;
      while (running){
        //Need buttons for this
        updateInput();
        //Blink logic
        blinktimer+=1;
        if (blinktimer>45){
          blinkstate=abs(blinkstate-1);
          blinktimer=0;
        }
        fastblinktimer+=1;
        if (fastblinktimer>15){
          fastblinkstate=abs(fastblinkstate-1);
          fastblinktimer=0;
        }
        if (state == 0){
          if (in_state[0] == 3){
            moveCursor();
            whoami_timer=0;
          }
          if (in_state[1] == 3){
            place(selected);
            moveCursor();
          }
          if (in_state[0] == 2){
            whoami_timer+=1;
            if (whoami_timer>30){
              state=3;
            }
          }

        }else if (state == 3){
          if (in_state[0] == 3 || in_state[0] == 0){
            whoami_timer=0;
            state=0;
          }
        }else if (state == 1 || state == 2){
          if (in_state[0] == 3){
            reset();
            state=0;
          }
        }
        render();


        //Back 2 menu
        if (in_state[0] == 2 && in_state[1] == 2){
          exit_timer+=1;
          if (exit_timer >= exit_timer_max){
            running=false;
          }
        }else{
          exit_timer=0;
        }

        delay(16); //60 refreshes per second
      }
    }
};
