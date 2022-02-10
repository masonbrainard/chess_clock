// C++ code
//

int bits_pins[4] = {4, 5, 6, 7};
int disp_pin = 8;
bool segbits[4];

void setup()
{
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop()
{
  for(int i = 0; i < 11; i++)
  {
  	conv4bit(i);
    digitalWrite(disp_pin, HIGH);
    delay(50);
    digitalWrite(disp_pin, LOW);
    
    for(int j = 0; j < 4; j++)
    {
     	digitalWrite(bits_pins[j], segbits[j]); 
    }
    
    delay(1000);
  }
  delay(1000);
}
void conv4bit(int i)
{
  if(i > 8)
 {
   i -= 8;
   segbits[3] = 1;
 }
  else
  	segbits[3] = 0;
 if(i > 4)
 {
  i -= 4;
   segbits[2] = 1;
 }
  else
    segbits[2] = 0;
 if(i > 2)
 {
  i -= 2;
  segbits[1] = 1;
 }
  else
    segbits[1] = 0;
  if(i > 1)
  {
   i -= 1;
    segbits[0] = 1;
  }
  else
    segbits[0] = 0;
}