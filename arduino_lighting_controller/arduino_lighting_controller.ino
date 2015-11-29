
String inputString = "";
boolean stringComplete = false;

int rate = 151;
int rateincrement = 25;

const int cyclelength = 10000;
int cyclepos = 0;

const int channels = 3;

int brightnesses[channels];
float brightness = 0.0;
const int brightnesslevels = 16;

const int patternlength = 16;
const int off[patternlength] =          {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
const int on[patternlength] =           {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
const int squarewave[patternlength] =   {15, 15, 15, 15, 15, 15, 15, 15, 0,  0,  0,  0,  0,  0,  0,  0};
const int sinewave[patternlength] =     {8,  10, 13, 14, 15, 14, 13, 10, 8,  5,  2,  1,  0,  1,  2,  5};
const int trianglewave[patternlength] = {15, 13, 11, 9,  7,  5,  3,  1,  0,  1,  3,  5,  7,  9,  11, 13};
const int sawtoothwave[patternlength] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15};
const int htootwaswave[patternlength] = {15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0};
const int shortblip[patternlength] =    {15, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
const int longblip[patternlength] =     {15, 15, 15, 15, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
const int unshortblip[patternlength] =  {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 0,  0};
const int unlongblip[patternlength] =   {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 0,  0,  0,  0,  0};

int currentwave[patternlength] =        {8,  10, 13, 14, 15, 14, 13, 10, 8,  5,  2,  1,  0,  1,  2,  5};

bool manual = false;
bool interpolate = true;

float phaseposition = 0;

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);
}

void loop() {
  handleSerial();

  for(int channel = 0; channel < channels; ++channel){
    phaseposition = fmodf((((float)cyclepos)/cyclelength)+(((float)channel)/channels), 1.0);
    //brightness = currentwave[(int)(patternlength*phaseposition)] * ((256/patternlength)+1);
    //
    
    //brightness =   (fmodf(patternlength*phaseposition, 1.0) * currentwave[(int)(patternlength*phaseposition)]) +
    //             (1-fmodf(patternlength*phaseposition, 1.0) * currentwave[(int)((patternlength*phaseposition)+1)]);
                 
    float p = patternlength * phaseposition;
    int lower = currentwave[(int)p];
    int upper = currentwave[((int)(p+1)) % patternlength];
    float upperweighting = fmodf(p, 1.0);
    float lowerweighting = 1.0 - upperweighting;
    if (interpolate){
      brightness = (lower * lowerweighting) + (upper * upperweighting);
    }
    else{
      brightness = lower;
    }
      int outputbrightness = brightness * (255.0/(brightnesslevels-1));
    //
    brightnesses[(int) channel] = outputbrightness; 
  }
    
  if (manual == false){
    analogWrite(9, brightnesses[0]);
    analogWrite(10, brightnesses[1]);
    analogWrite(11, brightnesses[2]);
    cyclepos+= rate;
  }
  if (cyclepos >= cyclelength) { cyclepos = 0; }
  

  delay(5);
}

void handleSerial() {
  while (Serial.available()) {
    Serial.write(6);
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  if (stringComplete) {
    if (inputString == "faster\n"){ 
      if ((rate+rateincrement)<(cyclelength/patternlength)){
        rate += rateincrement;
        Serial.print("Increased rate to "); Serial.println(rate);
      }
      else {
        Serial.println("Attempted to increase rate, but rate already at maximum!");
      }
    }
    else if (inputString == "slower\n"){ 
      if ((rate-rateincrement)>0){
        rate -= rateincrement;
        Serial.print("Decreased rate to "); Serial.println(rate);
      }
      else {
        Serial.println("Attempted to decrease rate, but rate already at minimum!");
      }
    }
    else if (inputString == "slow\n") { rate = 1; Serial.println("Reduced speed to minimum");}
    else if (inputString == "fast\n") { rate = 251; Serial.println("Increased speed to maximum");}
    
    else if (inputString == "off\n") {
      memcpy(currentwave, off, (patternlength*sizeof(currentwave[0])));
      Serial.println("Turned all lights off");
    }
    else if (inputString == "on\n") {
      memcpy(currentwave, on, (patternlength*sizeof(currentwave[0])));
      Serial.println("Turned all lights on");
    }
    else if (inputString == "sine\n") {
      memcpy(currentwave, sinewave, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using sine wave pattern");
    }
    else if (inputString == "square\n") {
      memcpy(currentwave, squarewave, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using square wave pattern");
    }
    else if (inputString == "triangle\n") {
      memcpy(currentwave, trianglewave, (patternlength*sizeof(currentwave[0])));
    }
    else if (inputString == "saw\n") {
      memcpy(currentwave, sawtoothwave, (patternlength*sizeof(currentwave[0])));
    }
    else if (inputString == "was\n") {
      memcpy(currentwave, htootwaswave, (patternlength*sizeof(currentwave[0])));
    }
    else if (inputString == "shortblip\n") {
      memcpy(currentwave, shortblip, (patternlength*sizeof(currentwave[0])));
    }
    else if (inputString == "longblip\n") {
      memcpy(currentwave, longblip, (patternlength*sizeof(currentwave[0])));
    }
    else if (inputString == "unshortblip\n") {
      memcpy(currentwave, unshortblip, (patternlength*sizeof(currentwave[0])));
    }
    else if (inputString == "unlongblip\n") {
      memcpy(currentwave, unlongblip, (patternlength*sizeof(currentwave[0])));
    }
    else if (inputString == "interpolation on\n"){
      interpolate = true;
      Serial.println("Turned interpolation on");
      }
    else if (inputString == "interpolation off\n"){
      interpolate = false;
      Serial.println("Turned interpolation off");
      }
    else if (inputString == "manual\n"){ manual = true;}
    else if (inputString == "auto\n"){ manual = false;}
    else if (inputString == "red on\n"){
      manual = true;
      analogWrite(9, 255);
    }
    else if (inputString == "green on\n"){
      manual = true;
      analogWrite(10, 255);
    }
    else if (inputString == "yellow on\n"){
      manual = true;
      analogWrite(11, 255);
    }
    else if (inputString == "red off\n"){
      manual = true;
      analogWrite(9, 0);
    }
    else if (inputString == "green off\n"){
      manual = true;
      analogWrite(10, 0);
    }
    else if (inputString == "yellow off\n"){
      manual = true;
      analogWrite(11, 0);
    }
    else if (inputString == "ping\n"){
      Serial.println("Ping sucessful");
    }
    else {
      Serial.println("?");
    }
    inputString = "";
    stringComplete = false;
  } 
}

void sendStatus(){
  Serial.print("<br>");
  Serial.print("Channel 1 brightness = ");
  Serial.print(brightnesses[0]);
  Serial.print("<br>");
  Serial.print("Channel 2 brightness = ");
  Serial.print(brightnesses[1]);
  Serial.print("<br>");
  Serial.print("Channel 3 brightness = ");
  Serial.print(brightnesses[2]);
}

