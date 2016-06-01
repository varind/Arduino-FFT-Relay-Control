/*
RelayFFT by varind 2013
this code is public domain, enjoy!
 */
#include <LiquidCrystal.h>
#include <fix_fft.h>
#define LCHAN 5
#define RCHAN 4
const int channels = 1;
const int xres = 4;
const int yres = 16;
const int gain = 1;
int decaytest = 1;
char im[64], data[64];
char Rim[64], Rdata[64];
char data_avgs[32];
float peaks[32];
int i = 0,val,Rval;
int x = 0, y=0, z=0;
LiquidCrystal lcd(7,8,9,10,11,12); 

// VU METER CHARACTERS
byte v1[8] = {
  B00000,B00000,B00000,B00000,B00000,B00000,B00000,B11111};
byte v2[8] = {
  B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111};
byte v3[8] = {
  B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111};
byte v4[8] = {
  B00000,B00000,B00000,B00000,B11111,B11111,B11111,B11111};
byte v5[8] = {
  B00000,B00000,B00000,B11111,B11111,B11111,B11111,B11111};
byte v6[8] = {
  B00000,B00000,B11111,B11111,B11111,B11111,B11111,B11111};
byte v7[8] = {
  B00000,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte v8[8] = {
  B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};


void setup() {
  //    Serial.begin(9600);


  pinMode(2, OUTPUT);     
  pinMode(3, OUTPUT);   
  pinMode(4, OUTPUT);     
  pinMode(5, OUTPUT);     

  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);


  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(1, v1);
  lcd.createChar(2, v2);
  lcd.createChar(3, v3);
  lcd.createChar(4, v4);
  lcd.createChar(5, v5);
  lcd.createChar(6, v6);
  lcd.createChar(7, v7);
  lcd.createChar(8, v8);

}

void loop() {
  
// For serial communication to Processing 
//  int val = analogRead(LCHAN);
//  Serial.write( 0xff );                                                         
//  Serial.write( (val >> 8) & 0xff );                                            
//  Serial.write( val & 0xff );

  //  delay(10);
  for (i=0; i < 64; i++){    
    val = ((analogRead(LCHAN) / 4 ) - 128);
    data[i] = val;                                      
    im[i] = 0;   
    if (channels ==2){
      Rval = ((analogRead(RCHAN) / 4 ) - 128); 
      Rdata[i] = Rval;                                      
      Rim[i] = 0;   
    }
  };

  fix_fft(data,im,6,0); // Send the data through fft
  if (channels == 2){
    fix_fft(Rdata,Rim,6,0); // Send the data through fft
  }

  // get the absolute value of the values in the array, so we're only dealing with positive numbers
  for (i=0; i< 32 ;i++){  
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); 
  }
  if (channels ==2){
    for (i=16; i< 32 ;i++){  
      data[i] = sqrt(Rdata[i-16] * Rdata[i-16] + Rim[i-16] * Rim[i-16]); 
    }
  }


  data_avgs[0] = (data[1]+data[2])/3;
  data_avgs[1] = (data[3]+data[4])/2;
  data_avgs[2] = (data[10]+data[11]+data[12]);
  data_avgs[3] = (data[15]+data[16]+data[17]);

  for (i=0; i<4; i++) {
    //  data_avgs[i] = ((data[i]));// + data[i*2+1])/2;// + data[i*3 + 2] + data[i*4 + 3])/4;  
    data_avgs[i] = constrain(data_avgs[i],1,12);  
    data_avgs[i] = map(data_avgs[i], 1, 12, 1, yres+4);        // remap averaged values
  }


  decay(1);
  monorelay();

} // end loop

void monorelay(){ 
  for (x=0; x < xres; x++) {  // repeat for each column of the display horizontal resolution
    y = data_avgs[x];  // get current column value 
    z= peaks[x];
    if (y > z){
      peaks[x]=y;
    }
    y= peaks[x]; 

    // turn on the relays
    if (y < 8){
      digitalWrite(x+2, HIGH);

    }
    else{
      digitalWrite(x+2, LOW);
    }


    // LCD Display
    if (y <= 8){    
      lcd.setCursor(x*4,0); // clear first row
      lcd.print(" ");
      lcd.setCursor(x*4+1,0);
      lcd.print(" ");
      lcd.setCursor(x*4+2,0);
      lcd.print(" ");
      lcd.setCursor(x*4+3,0);
      lcd.print(" ");

      if (y == 0){      
        lcd.setCursor(x*4,1); // draw second row
        lcd.print(" "); // save a glyph
        lcd.setCursor(x*4+1,1);
        lcd.print(" ");
        lcd.setCursor(x*4+2,1);
        lcd.print(" ");
        lcd.setCursor(x*4+3,1); 
        lcd.print(" "); 
      }
      else {
        lcd.setCursor(x*4,1); // draw second row
        lcd.write(y);
        lcd.setCursor(x*4+1,1);
        lcd.write(y);
        lcd.setCursor(x*4+2,1);
        lcd.write(y);
        lcd.setCursor(x*4+3,1);
        lcd.write(y);
      }

    }
    else{

      if (y == 9){
        lcd.setCursor(x*4,0);  // draw first row
        lcd.write(" ");  
        lcd.setCursor(x*4+1,0);
        lcd.write(" ");  
        lcd.setCursor(x*4+2,0);
        lcd.write(" ");  
        lcd.setCursor(x*4+3,0);
        lcd.write(" ");  
      }
      else {
        lcd.setCursor(x*4,0);  // draw first row
        lcd.write(y-8);  // same chars 1-8 as 9-16
        lcd.setCursor(x*4+1,0);
        lcd.write(y-8);
        lcd.setCursor(x*4+2,0);
        lcd.write(y-8);
        lcd.setCursor(x*4+3,0);
        lcd.write(y-8);
      }

      lcd.setCursor(x*4,1);
      lcd.write(8);  
      lcd.setCursor(x*4+1,1);
      lcd.write(8);  
      lcd.setCursor(x*4+2,1);
      lcd.write(8);  
      lcd.setCursor(x*4+3,1);
      lcd.write(8);  
    } // end display
  }  // end xres
}

void decay(int decayrate){
  //// reduce the values of the last peaks by 1 
  if (decaytest == decayrate){
    for (x=0; x < 32; x++) {
      peaks[x] = peaks[x] - 1;  // subtract 1 from each column peaks
      decaytest = 0;
    }
  }
  decaytest++;

}
