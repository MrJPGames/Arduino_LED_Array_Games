#include "Arduino.h" //Included Arduino to allow for Arduino specific functions like read/write to/from pins etc.

/*
  Modified CarGame to inverse process,
  essentially where cars would have been
  there is now an free space where the dot
  can go through.
*/

class AvoidGame{
  public:
    void startAG(){
      setupAG();
      loopAG();
    }
  private:
    //Car game
    int screen_map[4][3]={
      {13,12,11},
      {10,9,8},
      {7,6,5},
      {4,3,2}
    };
    
    //Init car pos
    int car_pos=1; // 0=left, 1=middle, 2=right
    
    //Init other cars
    int enemies[10][3]={ 
      {0,0,0},
      {0,0,0},
      {0,0,0},
      {0,0,0},
      {0,0,0}
    };

    int spawn_next=true;
    
    int update_timer=0;
    int update_timer_min=20; //Slower than CarGame, as more movement must be done (eg. left to right instead of left-midle being good enough with 1 car per row)
    int update_timer_max=40;
    
    int speed_up_timer=0;
    int speed_up_timer_max=5;
    
    //Define controls
    int in_state[2]={4,4}; //Read as 4 to avoid confusion due to menu button press before game launch
    int in_map[2]={A0,A1};
    
    bool GameOver=false;
    
    //Flash for game over
    int flash_timer=0;
    int flash_timer_max=20;
    bool flash_state=true; //off

    int exit_timer=0;
    int exit_timer_max=180; //~3 sec
    
    void setupAG() {
      //Set all screen pins to output and clear screen (not needed but idk)
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
      pinMode(A5, INPUT); //Set as input for random seed
      randomSeed(analogRead(A5)); //This is almost random, best random seed easily availible for arduino (recommended by Arduino makers)
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
      //Clear screen
      for (int i=0; i<4; i++){
        for (int j=0; j<3; j++){
          digitalWrite(screen_map[i][j], true);
        }
      }
      //Render other cars
      for (int i=0; i<5; i++){
        if (enemies[i][0] == 1){
          int x=enemies[i][1];
          int y=enemies[i][2];
          digitalWrite(screen_map[x][y], false);
        }
      }
      //Render car
      digitalWrite(screen_map[3][car_pos], false);
    }
    
    void renderGameOver(){
      //Render current flashing state
      flash_timer+=1;
      if (flash_timer >= flash_timer_max){
        flash_timer=0;
        flash_state=!flash_state;
      }
      for (int i=0; i<4; i++){
        for (int j=0; j<3; j++){
          digitalWrite(screen_map[i][j], flash_state);
        }
      }
    }
    
    void reset(){ //Reset all vars needed for a new game
      car_pos=1;
      GameOver=false;
      update_timer_max=45;
      speed_up_timer_max=5;
      for (int i=0; i<5; i++){
        for (int j=0; j<3; j++){
          enemies[i][j]=0;
        }
      }
    }
    
    void spawn(){
      randomSeed(analogRead(A5)); //Seed random every time to get around crappy default random calculation
      int pos=round(random(0,3)); //Get random enemy pos
      for (int i=0; i<5; i++){ //Do for all 5 possible cars
        if (enemies[i*2][0] == 0){
          enemies[i*2][0] = 1; //Birth
          enemies[i*2+1][0] = 1; //Birth
          enemies[i*2][1] = 0; //Top Row
          enemies[i*2+1][1] = 0; //Top Row
          switch (pos){
            case 0:
              enemies[i*2][2]=1;
              enemies[i*2+1][2]=2;
              break;
            case 1:
              enemies[i*2][2]=0;
              enemies[i*2+1][2]=2;
              break;
            case 2:
              enemies[i*2][2]=0;
              enemies[i*2+1][2]=1;
              break;
          }
          break;
        }
      }
    }
    
    void updateEnemies(){
      for (int i=0; i<10; i++){ //Do for all 5 possible cars
        if (enemies[i][0] == 1){
          enemies[i][1]+=1; //Move enemy down 1 pixel
          if (enemies[i][1] >= 4){
            enemies[i][0]=0; //Kill
          }
        }
      }
      //Spawn after moving to make them appear at row 1 instead of 2
      if (spawn_next == true){ //Spawn every other row
        spawn_next=false;
        spawn();
      }else{
        spawn_next=true;
      }
    }
    
    void loopAG() {
      bool running=true; //Allows for back2menu functionality
      while (running){
        updateInput();
        if (!GameOver){
          if (in_state[0] == 1 && car_pos != 0){ //Left button pressed and car is not all the way to the left
            car_pos-=1; //Move car left
          }
          if (in_state[1] == 1 && car_pos != 2){ //Right button pressed and car is not all the way to the right
            car_pos+=1;   //Move car right
          }
          update_timer+=1;
          if (update_timer >= update_timer_max){ //Should all cars move, and possibly spawn a new car?
            update_timer=0;
            speed_up_timer+=1;
            if (update_timer_max > update_timer_min && speed_up_timer >= speed_up_timer_max){ //Make time until next speed up larger
              speed_up_timer=0;
              speed_up_timer_max+=3; 
              update_timer_max-=5; //Make the time until next update smaller
            }
            updateEnemies(); //update car positions and possibly spawn a new car
          }
          for (int i=0; i<5; i++){ //Do for all 5 possible cars (Never more than 3 on screen)
            if (enemies[i][0] == 1 && enemies[i][1] == 3 && enemies[i][2] == car_pos){ //Collision?
              GameOver=true; //Hit player
            }
          }
          render(); //Renders current game state to screen
        }else{
          if (in_state[0] == 2 && in_state[1] == 2){ //Both buttons pressed (technically held)
            reset(); //Reset all vars that need reseting to start game from beginning
          }
          renderGameOver(); //Renders the game over blinking screen.
        }

        //Back 2 menu
        if (in_state[0] == 2 && in_state[1] == 2){
          exit_timer+=1;
          if (exit_timer >= exit_timer_max){
            running=false;
          }
        }else{
          exit_timer=0;
        }

        delay(16); //For 60Hz updates
      }
    }
};
