int disp_res = 2;
int disp_pin = 3;
int deci_pin = 4;
int bits_pins[4] = {6, 7, 8, 5};

int buzz_pin = 9;

int P1_button = 13;
int P2_button = 10;
int next_button = 11;
int pause_button = 12;

int inc_i = 0;
int inc = 0;
int inc_time;

int temp_time;

unsigned long last_time = 0;
unsigned long int P1_time;
unsigned long int P2_time;

bool is_no_disp;
bool button_debounce;

unsigned long int bd_time;
int bd_dif = 100;

unsigned long int conv[4] = {6000, 600, 100, 10};

enum stateMachine {HOLD_TIME, SET_TIME, SET_READY, P1_TURN, P2_TURN};
enum stateMachine gameState;

int bits_bins[11][4] = {{0,0,0,0},
{0,0,0,1},{0,0,1,0},{0,0,1,1},
{0,1,0,0},{0,1,0,1},{0,1,1,0},
{0,1,1,1},{1,0,0,0},{1,0,0,1},{1,1,1,1}};

void setup(){
  Serial.begin(9600);
  
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
  
  gameState = SET_READY;
  
  P1_time = 600;
  P2_time = 600;
  inc_time = 10;

  is_no_disp = false;

  last_time = millis();

  Serial.println("Setup complete");
}

void loop()
{
  //HOLD_TIME//
  if(gameState == HOLD_TIME)
  {
    Serial.println("HOLD_TIME");
    while(gameState == HOLD_TIME)
    {
      display(P1_time);
      display(P2_time);
      res_disp();

      if(digitalRead(P1_button) == HIGH)
      {
        Serial.println("P1 BUTTON PRESSED");
        buzzer();
      }
      if(digitalRead(P2_button) == HIGH)
      {
        Serial.println("P2 BUTTON PRESSED");
        buzzer();
      }
      if(digitalRead(pause_button) == HIGH || digitalRead(next_button) == HIGH)
      {
        gameState = SET_READY;
      }
    }
  }
  
  //SET_READY//
  if(gameState == SET_READY)
  {
    Serial.println("SET_READY");
    while(gameState == SET_READY)
    {
      display(P1_time);
      display(P2_time);
      res_disp();
  
      if(digitalRead(P1_button) == HIGH)
      {
        last_time = millis();
        gameState = P2_TURN;
      }
      else if(digitalRead(P2_button) == HIGH)
      {
        last_time = millis();
        gameState = P1_TURN;
      }
      if(digitalRead(next_button) == HIGH)
      {
        P1_time = 1800;
        P2_time = 1800;
        inc_time = 20;
      }
      if(digitalRead(pause_button) == HIGH)
      {
        P1_time = 3000;
        P2_time = 3000;
        inc_time = 10;
      }
    }
  }
  
  //P1_TURN//
  if(gameState == P1_TURN)
  {
    Serial.println("P1_TURN");
    while(digitalRead(P1_button) == LOW)
    {
      temp_time = P1_time - ((millis() - last_time)/100);
      display(temp_time);
      display(P2_time);
      res_disp();

      if(check_timeout(temp_time))
      {
        Serial.println("Timeout P1");
        P1_time = 0;
        gameState = HOLD_TIME;
        break;
      }
      if(digitalRead(pause_button) == HIGH)
      {
        gameState = SET_READY;
        break;
      }
    }
    if(gameState != HOLD_TIME)
      P1_time -=  (millis() - last_time)/100;
    if(gameState == P1_TURN)
    {
      P1_time += inc_time;
      gameState = P2_TURN;
    }
    last_time = millis();
  }
  
  //P2_TURN//
  if(gameState == P2_TURN)
  {
    Serial.println("P2_TURN");
    while(digitalRead(P2_button) == LOW)
    {
      temp_time = P2_time - ((millis() - last_time)/100);
      display(P1_time);
      display(temp_time);
      res_disp();
      
      if(check_timeout(temp_time))
      {
        Serial.println("Timeout P2");
        P2_time = 0;
        gameState = HOLD_TIME;
        break;
      }
      if(digitalRead(pause_button) == HIGH)
      {
        gameState = SET_READY;
        break;
      }
    }
    if(gameState != HOLD_TIME)
      P2_time -=  (millis() - last_time)/100;
    if(gameState == P2_TURN)
    {
      P2_time += inc_time;
      gameState = P1_TURN;
    }
    last_time = millis();
  }
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
void display(unsigned long int time)
{
  int time_to_disp[4] = {0, 0, 0, 0};
  disp_seg(10);
  
  time /= 10; //into seconds
  unsigned long int temp_time = 0;
   
  if((temp_time = time / 600) >= 1)
  {
    //disp_seg(temp_time % 10);
    time_to_disp[0] = temp_time % 10;
    time -= temp_time * 600;
  }
  
  if((temp_time = time / 60) >= 1)
  {
    //disp_seg(temp_time % 10);
    time_to_disp[1] = temp_time % 10;
    time -= temp_time * 60;
  }
  if((temp_time = time / 10) >= 1)
  {
    //disp_seg((time / 10) % 10); 
    time_to_disp[2] = ((time / 10) % 10);
    time -= temp_time * 10;
  }

  //disp_seg((time % 10));
  time_to_disp[3] = time % 10;
  
  disp_seg(time_to_disp[3]);
  delay(2);
  inc_disp();
  disp_seg(time_to_disp[2]);
  delay(2);
  inc_disp();
  digitalWrite(deci_pin, HIGH);
  delay(2);
  digitalWrite(deci_pin, LOW);
  disp_seg(time_to_disp[1]);
  delay(2);
  inc_disp();
  disp_seg(time_to_disp[0]);
  delay(2);
  inc_disp();
  disp_seg(10);
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
  disp_seg(10);
  inc_disp();
}
void no_disp()
{
  digitalWrite(disp_res, HIGH);
  digitalWrite(disp_res, LOW);
  disp_seg(10);
}
void set_bd(int dif) //set button debounce
{
  bd_dif = dif;
  bd_time = millis();
  button_debounce = true;
}
void reset_bd()
{
  if(button_debounce && (millis() - bd_time) > bd_dif)
  {
    button_debounce = false;
  }
}