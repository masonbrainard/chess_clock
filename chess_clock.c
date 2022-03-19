int disp_res = 2;
int disp_pin = 3;
int deci_pin = 4;
int bits_pins[4] = {6, 7, 8, 5};

int buzz_pin = 9;

int P1_button = 10;
int P2_button = 13;
int next_button = 11;
int pause_button = 12;

int inc_i = 0;
int inc = 0;

unsigned long last_time = 0;
int P1_time;
int P2_time;

bool is_no_disp;
bool button_debounce;

int bd_time;

int conv[4] = {6000, 600, 10, 1};

enum stateMachine {HOLD_TIME, SET_TIME, SET_READY, P1_TURN, P2_TURN};
enum stateMachine gameState;

int bits_bins[10][4] = {{0,0,0,0},
{0,0,0,1},{0,0,1,0},{0,0,1,1},
{0,1,0,0},{0,1,0,1},{0,1,1,0},
{0,1,1,1},{1,0,0,0},{1,0,0,1}};

void setup(){
  //Serial.begin(9600);
  
  pinMode(bits_pins[0], OUTPUT);
  pinMode(bits_pins[1], OUTPUT);
  pinMode(bits_pins[2], OUTPUT);
  pinMode(bits_pins[3], OUTPUT);
  pinMode(deci_pin, OUTPUT);
  
  pinMode(disp_pin, OUTPUT);
  pinMode(disp_res, OUTPUT);

  pinMode(buzz_pin, OUTPUT);
 
  pinMode(P1_button, INPUT);
  pinMode(P2_button, INPUT);
  pinMode(next_button, INPUT);
  pinMode(pause_button, INPUT);
  
  gameState = SET_TIME;
  
  P1_time = 0;
  P2_time = 0;
  inc = 0;

  is_no_disp = false;

  last_time = millis();

  //Serial.print("Setup complete\n");
}

void loop()
{
  if(gameState == HOLD_TIME)
  {
     while(true)
     {
       display(P1_time);
       display(P2_time);
       res_disp();
       
       if(digitalRead(next_button) == HIGH || digitalRead(pause_button) == HIGH)
       {
          gameState = SET_TIME;
          break;
       }
       if(digitalRead(P1_button) == HIGH || digitalRead(P2_button) == HIGH)
       {
          no_disp();
          buzzer();
          res_disp();
       }
     }
  }
  if(gameState == SET_TIME)
  {
     int t = 0;
     int i = 0;

     last_time = millis();
     while(true)
     {
        if(millis() - last_time < 500)
        {
          display(P1_time + (t * conv[i]));
          display(P2_time + (t * conv[i]));
          res_disp();
        }
        else if(millis() - last_time < 1000)
        {
          if(!is_no_disp)
          {
            no_disp();
            is_no_disp = true;
          }
        }
        else
        {
          last_time = millis();
          res_disp();
          is_no_disp = false;
        }       
        if(digitalRead(P1_button) == HIGH && !button_debounce)
        {
           if(t > 0)
              t -= 1;
           else
              t = 9;
           set_bd();
        }
        if(digitalRead(P2_button) == HIGH && !button_debounce)
        {
          if(t < 9)
            t += 1;
          else
            t = 0;
          set_bd();
        }
        if(digitalRead(next_button) == HIGH && !button_debounce)
        {
          if(i < 4)
          {
            P1_time += t * conv[i];
            P2_time += t * conv[i];
            i += 1;
            t = 0;
          }
          else
          {
            gameState = SET_READY;
            break;
          }
          set_bd();
        }
        reset_bd();
     }
     //set p2 time (auto set to p1 time, but allow change
     //set increment (set to 0000:0000 only go up to 10 sec?)
  }
  if(gameState == SET_READY)
  {
     while(true)
     {
         display(P1_time);
         display(P2_time);
         res_disp();      
         
         //hold time waiting
         if(digitalRead(P1_button) == HIGH)
         {
            gameState = P2_TURN;
            break;
         }
         if(digitalRead(P2_button) == HIGH)
         {
            gameState = P1_TURN;
            break;
         }
         if(digitalRead(next_button) == HIGH && !button_debounce)
         {
            set_bd();
            gameState = SET_TIME;
            break;
         }
         reset_bd();
     }
     last_time = millis();
  }
  if(gameState == P1_TURN)
  {
    while(digitalRead(P1_button) != HIGH)
    {
      display(P1_time - ((millis() - last_time)/100));
      display(P2_time);
      res_disp();
      if(check_pause())
      {
        P1_time -= (millis() - last_time)/100;
        break;
      }
      if(check_timeout(P1_time - ((millis() - last_time)/100)))
      {
        P1_time = 0;
        break;
      }
    }
    if(gameState == P1_TURN)
    {
      P1_time -= (millis() - last_time)/100;
      P1_time += inc;
      last_time = millis();
      gameState = P2_TURN;
    }
  }
  if(gameState == P2_TURN)
  {
    while(digitalRead(P2_button) != HIGH)
    {
      display(P1_time);
      display(P2_time - ((millis() - last_time)/100));
      res_disp();
      if(check_pause())
      {
        P2_time -= (millis() - last_time)/100;
        break;
      }
      if(check_timeout(P2_time - ((millis() - last_time)/100)))
      {
        P2_time = 0;
        break;
      }
    }
    if(gameState == P2_TURN)
    {
      P2_time -= (millis() - last_time)/100;
      P2_time += inc;
      last_time = millis();
      gameState = P1_TURN;
    }
  }
}
bool check_pause()
{
 if(digitalRead(pause_button) == HIGH)
 {
   gameState = SET_READY;
   return true;
 }
 return false;
}
bool check_timeout(int time)
{
 if(time <= 0)
 {
   gameState = HOLD_TIME;
   buzzer();
   return true;
 }
 return false;
}
void display(int time)
{
  time /= 10; //into seconds
  int temp_time = 0;
  if((temp_time = time / 600) >= 1)
  {
    disp_seg(temp_time % 10);
    time -= temp_time * 600;
  }
  else
  {
    disp_seg(0);
  }
  digitalWrite(deci_pin, HIGH);
  delay(2);
  digitalWrite(deci_pin, LOW);
  if((temp_time = time / 60) >= 1)
  {
    disp_seg(temp_time % 10);
    time -= temp_time * 60;
  }
  else
  {
    disp_seg(0);
  }
  if((temp_time = time / 10) >= 1)
  {
    disp_seg((time / 10) % 10); 
    time -= temp_time * 10;
  }
  else
  {
    disp_seg(0);
  }
  disp_seg((time % 10));
}
void buzzer()
{
  no_disp();
  tone(buzz_pin, 2000, 100);
  delay(200);
  tone(buzz_pin, 2000, 200);
  delay(300);
  tone(buzz_pin, 2000, 300);
  res_disp();
}
void disp_seg(int i)
{
  digitalWrite(bits_pins[0], bits_bins[i][0]);
  digitalWrite(bits_pins[1], bits_bins[i][1]);
  digitalWrite(bits_pins[2], bits_bins[i][2]);
  digitalWrite(bits_pins[3], bits_bins[i][3]);
  delay(2);
  inc_disp();
}
void inc_disp()
{
  digitalWrite(disp_pin, LOW);
  digitalWrite(disp_pin, HIGH);
}
void res_disp()
{
  digitalWrite(disp_res, HIGH);
  digitalWrite(disp_res, LOW);
}
void no_disp()
{
  res_disp();
  inc_disp();
  inc_disp();
  inc_disp();
  inc_disp();
  inc_disp();
  inc_disp();
  inc_disp();
  inc_disp();
  inc_disp();
}
void set_bd() //set button debounce
{
  bd_time = millis();
  button_debounce = true;
}
void reset_bd()
{
  if(button_debounce && millis() - bd_time > 250)
  {
    button_debounce = false;
  }
}