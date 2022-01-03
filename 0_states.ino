#define NBSTATE 5
#define NBEFFECT 3

// SLEEP : wait for weakup time
// ANIM : Color animation
// WEATHER : Tell the weather with animation
// REACTIVE : Listen to sound and react to it
enum STATE {OFF, SLEEP, ANIM, WEATHER, REACTIVE};

// RANDBLINK : fade to a random color and back
// SIMPLE : set to One color
enum EFFECT {RANDBLINK, SIMPLE, SNAKE};

STATE state = ANIM;
EFFECT effect = RANDBLINK;

void commands_loop(){
  switch(state){
    case OFF: 
      leds_off();  break;
    case ANIM:
      switch(effect){
        case RANDBLINK: loop_randblink(); break;
        case SIMPLE:    loop_simple();    break;
        case SNAKE:     loop_snake();     break;
      }
      break;
  }
  
}
