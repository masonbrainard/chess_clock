// C++ code
//

int bits_pins[4] = {7, 6, 5, 4};
int bits_bins[10][4] = {{0,0,0,0},
{0,0,0,1},{0,0,1,0},{0,0,1,1},
{0,1,0,0},{0,1,0,1},{0,1,1,0},
{0,1,1,1},{1,0,0,0},{1,0,0,1}};
int player1_button = 10;
int player2_button = 9;
int disp_pin = 8;
int disp_res = 13;

unsigned long last_time = 0;
long player1_time = 83;
long player2_time = 83;

bool settings = false;

void setup()
{
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(disp_pin, OUTPUT);
  pinMode(disp_res, OUTPUT);
  
  pinMode(player1_button, INPUT);
  pinMode(player2_button, INPUT);
}

void loop()
{
  last_time = millis();
  while(digitalRead(player1_button) == LOW)
  {
    display(player1_time - ((millis() - last_time)/1000));
  	display(player2_time);
    res_disp();
  }
  player1_time -= (millis() - last_time)/1000;
  last_time = millis();
  while(digitalRead(player2_button) == LOW)
  {
    display(player1_time);
  	display(player2_time - ((millis() - last_time)/1000));
    res_disp();
  }
  player2_time -= (millis() - last_time)/1000;
  last_time = millis();
}

void display(int time)
{
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
void disp_seg(int i)
{
  digitalWrite(bits_pins[0], bits_bins[i][0]);
  digitalWrite(bits_pins[1], bits_bins[i][1]);
  digitalWrite(bits_pins[2], bits_bins[i][2]);
  digitalWrite(bits_pins[3], bits_bins[i][3]);
  delay(10);
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