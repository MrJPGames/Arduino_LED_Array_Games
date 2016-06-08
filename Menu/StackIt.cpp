#include "Arduino.h"

/*
	Remake of gambling arcade game stack it for Arduino (LED)
*/

class StackIt{
	public:
		void startSI(){
			setupSI();
			loopSI();
		}
	private:
		int screen[4][3]={
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0},
		};
    	int screen_map[4][3]={
	    	{13,12,11},
	    	{10,9,8},
	    	{7,6,5},
	    	{4,3,2}
	    };

	    int cur_x=0;
	    int cur_y=3;
	    int cur_dir=1;

	    int pile_x=0;

	    int cur_speed=15;

	    int timer=0;

	    //Define controls
	    int in_state[2]={4,4}; //Read as 4 to avoid confusion due to menu button press before game launch
	    int in_map[2]={A0,A1};

	    int exit_timer=0;
	    int exit_timer_max=180; //~3 sec

	    bool GameOver=false;
	    bool Win=false;


		void setupSI(){
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
		        	digitalWrite(screen_map[i][j], !(bool)screen[i][j]); //Render screen data
		    	}
		    }
    	}

    	void clearScreen(){
    		for (int i=0; i<4; i++){
    			for (int j=0; j<3; j++){
    				screen[i][j]=0;
    			}
    		}
    	}

    	void clearScreenLine(int line){
    		for (int i=0; i<3; i++){
    			screen[line][i]=0;
    		}
    	}

    	void reset(){
    		cur_speed=15;
    		cur_x=0;
    		cur_y=3;
    		timer=0;
    		GameOver=false;
    		Win=false;
    		clearScreen();
    	}

    	void place(){
    		if (cur_y == 3){
    			pile_x=cur_x;
    		}
    		if (cur_x != pile_x){
    			GameOver=1;
    		}
    		if (cur_y == 0){
    			Win=1;
    		}
    		if (!GameOver)
    			cur_y-=1;
    	}

    	void moveDot(){
    		cur_x+=cur_dir;
    		if (cur_x > 2){
    			cur_x=1;
    			cur_dir=-1;
    		}else if (cur_x < 0){
    			cur_x=1;
    			cur_dir=1;
    		}
    	}

		void loopSI(){
			bool running=true;
			while (running){
				updateInput();

				if (!GameOver){
					timer+=1;
					if (in_state[1] == 1){
						place();
						cur_speed-=3; //Is faster
					}else if (timer >= cur_speed){
						timer=0;
						moveDot();
					}
					

					clearScreenLine(cur_y);
					screen[cur_y][cur_x]=1;

					render();
				}else if (GameOver){
					if (in_state[0] == 2 && in_state[1] == 2){ //Both buttons pressed (technically held)
		   				reset(); //Reset all vars that need reseting to start game from beginning
					}
					render();
				}else if (Win){
					if (in_state[0] == 2 && in_state[1] == 2){ //Both buttons pressed (technically held)
		   				reset(); //Reset all vars that need reseting to start game from beginning
					}
					//renderWin();
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

		        delay(16);
			}
		}
};
