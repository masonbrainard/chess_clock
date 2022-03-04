int disp_res = 2;
int disp_pin = 3;
int deci_pin = 4;
int bits_pins[4] = {6, 7, 8, 5};

int buzz_pin = 9;

int player1_button = 10;
int player2_button = 13;
int next_button = 11;
int pause_button = 12;

int gamemodes[4] = {600, 1800, 3000, 6000};
int increment[4] = {0, 20, 0, 0};
int inc_i = 0;
int inc = 0;

unsigned long last_time = 0;
int player1_time;
int player2_time;

bool settings;
bool player1_turn;
bool player2_turn;
bool low_power;

bool interrupted;

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
 
  pinMode(player1_button, INPUT);
  pinMode(player2_button, INPUT);
  pinMode(next_button, INPUT);
  pinMode(pause_button, INPUT);
  
  settings = true;
  player1_turn = false;
  player2_turn = false;
  
  player1_time = gamemodes[0];
  player2_time = gamemodes[0];
  inc = 0;

  last_time = millis();

  //Serial.print("Setup complete\n");
}

void loop()
{
  if(settings)
  {
   last_time = millis();
   while(true)
   {
    //display time
    display(player1_time);
    display(player2_time);
    res_disp();
    
    //next game
    if(digitalRead(next_button) == HIGH)
    {
      inc_i = (inc_i <=4 ? inc_i + 1 : 0);
      player1_time = gamemodes[inc_i];
      player2_time = gamemodes[inc_i]; 
      inc = increment[inc_i];

      last_time = millis(); //reset low power timer
    }
    
    //player button pressed
    if(digitalRead(player1_button) == HIGH)
    {
       player2_turn = true;
       break;
    }
    if(digitalRead(player2_button) == HIGH)
    {
      player1_turn = true;
      break;
    }
   }
   settings = false;
   last_time = millis();
  }

  if(player1_turn)
  {
    while(digitalRead(player1_button) == LOW)
    {
      display(player1_time - ((millis() - last_time)/100));
      display(player2_time);
      res_disp();
      if(check_pause())
      {
        player1_time -= (millis() - last_time)/100;
        break;
      }
      if(check_timeout(player1_time - ((millis() - last_time)/100)))
      {
        player1_time = 0;
        break;
      }
    }
    if(!settings)
    {
      player1_time -= (millis() - last_time)/100;
      player1_time += inc;
      last_time = millis();
      player2_turn = true;
    }
  }
  
  if(player2_turn)
  {
    while(digitalRead(player2_button) == LOW)
    {
      display(player1_time);
      display(player2_time - ((millis() - last_time)/100));
      res_disp();
      if(check_pause())
      {
        player2_time -= (millis() - last_time)/100;
        break;
      }
      if(check_timeout(player2_time - ((millis() - last_time)/100)))
      {
        player2_time = 0;
        break;
      }
    }
    if(!settings)
    {
      player2_time -= (millis() - last_time)/100;
      player2_time += inc;
      last_time = millis();
      player1_turn = true;
    }
  }
}
bool check_pause()
{
 if(digitalRead(pause_button) == HIGH)
 {
   player1_turn = false;
   player2_turn = false;
   settings = true;
   return true;
 }
 return false;
}
bool check_timeout(int time)
{
 if(time <= 0)
 {
   player1_turn = false;
   player2_turn = false;
   settings = true;
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
