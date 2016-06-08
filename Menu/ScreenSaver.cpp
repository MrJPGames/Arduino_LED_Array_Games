#include "Arduino.h" //Included Arduino to allow for Arduino specific functions like read/write to/from pins etc.

/*
 This is mostly demo software so very little documentation.
 Essentially each frame is stored in a multi-dimensional array and render to the screen with the render() function.
 The render function uses for loops combined with the cur_frame variable to render eacht "pixel" or LED correctly and
 allow for 'animation' on the screen.
*/

class ScreenSaver{
	public:
		void startSS(){
			SSsetup();
			SSloop();
		}
	private:
		int screen_map[4][3] = {
		  {13,12,11},
		  {10,9,8},
		  {7,6,5},
		  {4,3,2}
		};

		int frame=-1;
		int total_frames=26;

    	int delay_time=150;

		int anim_dir=1;

		int frames[26][4][3] = {
		  {
		    {0,0,0},
		    {0,0,0},
		    {0,0,0}, 
		    {0,0,0},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {0,0,0}, 
		    {0,1,0},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {0,1,0}, 
		    {1,1,1},  
		  },
		  {
		    {0,0,0},
		    {0,1,0},
		    {1,1,1}, 
		    {1,1,1},  
		  },
		  {
		    {0,1,0},
		    {1,1,1},
		    {1,1,1}, 
		    {1,0,1},  
		  },
		  {
		    {1,1,1},
		    {1,1,1},
		    {1,0,1}, 
		    {0,0,0},  
		  },
		  {
		    {1,1,1},
		    {1,0,1},
		    {0,0,0}, 
		    {0,0,0},  
		  },
		  {
		    {1,0,1},
		    {0,0,0},
		    {0,0,0}, 
		    {0,0,0},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {0,0,0}, 
		    {0,0,0},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {0,0,0}, 
		    {1,0,0},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {1,0,0}, 
		    {1,1,0},  
		  },
		  {
		    {0,0,0},
		    {1,0,0},
		    {1,1,0}, 
		    {0,1,1},  
		  },
		  {
		    {1,0,0},
		    {1,1,0},
		    {0,1,1}, 
		    {0,0,1},  
		  },
		  {
		    {1,1,0},
		    {0,1,1},
		    {0,0,1}, 
		    {0,0,0},  
		  },
		  {
		    {0,1,1},
		    {0,0,1},
		    {0,0,0}, 
		    {0,0,0},  
		  },
		  {
		    {0,0,1},
		    {0,0,0},
		    {0,0,0}, 
		    {0,0,0},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {0,0,0}, 
		    {0,0,0},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {0,0,0}, 
		    {1,1,1},  
		  },
		  {
		    {0,0,0},
		    {0,0,0},
		    {1,1,1}, 
		    {1,1,1},  
		  },
		  {
		    {0,0,0},
		    {1,1,1},
		    {1,1,1}, 
		    {1,1,1},  
		  },
		  {
		    {1,1,1},
		    {1,1,1},
		    {1,1,1}, 
		    {1,1,1},  
		  },
	      {
	        {1,1,1},
	        {1,1,1},
	        {1,1,1}, 
	        {1,1,1},  
	      },
	      {
	        {1,1,1},
	        {1,1,1},
	        {1,1,1}, 
	        {1,1,1},  
	      },
		  {
		    {0,1,1},
		    {0,1,1},
		    {0,1,1}, 
		    {0,1,1},  
		  },
		  {
		    {0,0,1},
		    {0,0,1},
		    {0,0,1}, 
		    {0,0,1},  
		  },
	      {
	        {0,0,0},
	        {0,0,0},
	        {0,0,0}, 
	        {0,0,0},  
	      },
		};

		int in_state[2]={4,4};
		int in_map[2]={A0, A1};

	    int exit_timer=0;
	    int exit_timer_max=12; //~3 sec

		void SSsetup() {
		  for (int i=0; i<4; i++){
		    for (int j=0; j<3; j++){
		      pinMode(screen_map[i][j], OUTPUT);
		    }
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
		      digitalWrite(screen_map[i][j], !(bool)frames[frame][i][j]);
		    }
		  }
		}

		void SSloop() {
			bool running=true;
			while (running){
			  frame+=anim_dir;
			  if (frame == total_frames-1 || frame < 0){
			    switch(anim_dir){
			      case -1:
			        anim_dir=1;
			        frame=0;
			        break;
			      case 1: 
			        anim_dir=-1;
			        break;
			    }
			  }

			  //Back 2 menu
			  updateInput();
		      if (in_state[0] == 2 && in_state[1] == 2){
		        exit_timer+=1;
		        if (exit_timer >= exit_timer_max){
		          running=false;
		        }
		      }else{
		        exit_timer=0;
		      }
         
        if (in_state[0] == 1){
          delay_time-=10;
          if (delay_time < 10){
            delay_time = 10;
          }
        }
        if (in_state[1] == 1){
          delay_time+=10;
          if (delay_time > 500){
            delay_time=500;
          }
        }
			  render();
			  delay(delay_time);
			}
		}
};
