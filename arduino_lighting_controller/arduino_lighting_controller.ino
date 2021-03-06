String inputString = "";              // Where we collect commands arriving over the serial connection

int rate = 151;                       // Proportional to the wave frequency - deliberately one more than a multiple of below
int rateincrement = 25;               // How much the faster/slower commands change, see above
const int baseTimePeriod = 5;         // How long to wait each time through the loop - defines maximum speed

const int cyclelength = 10000;        // Total number of steps in pattern. may be skipped or interpolated
int cyclepos = 0;                     // where are we in the wave?

const int channels = 3;               // Number of different outputs (equal phase separation

int brightnesses[channels];           // Where we put the output values before putting them out
float brightness = 0.0;               // The brightness of the channel currently being calculated for
const int brightnesslevels = 16;      // Resolution of the pattern definitions (one axis)

const int patternlength = 16;         // Resolution of the pattern definitions (other axis)

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

int currentwave[patternlength] =        {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
float phaseposition = 0;              // Where are we in the pattern, from 0 to 1

bool manual = false;                  // Disables automatic brightness etting from patterns
bool interpolate = true;              // Use linear interpolation for smoother fades?



void setup() {
  Serial.begin(9600);                 // Is default on some systems, makes the host side code simpler
  inputString.reserve(200);           // Lets hope we don't get any really long commands (FIXME!)
}

void loop() {
  handleSerial();
  if (!manual){
    calculateBrightnesses();
    cyclepos+= rate;
    if (cyclepos >= cyclelength) { cyclepos = 0; }    // Surely this could be done with a modulo? (tried but no luck)
  }
  setBrightnesses(brightnesses);
  delay(baseTimePeriod);
}



void setBrightnesses(int brightnessesToSet[]){
  analogWrite(9, brightnessesToSet[0]);
  analogWrite(10, brightnessesToSet[1]);
  analogWrite(11, brightnessesToSet[2]);
}

void calculateBrightnesses(){
  for(int channel = 0; channel < channels; ++channel){
    phaseposition = fmodf((((float)cyclepos)/cyclelength)+(((float)channel)/channels), 1.0);
    float p = patternlength * phaseposition;
    int lower = currentwave[(int)p];
    if (interpolate){
      int upper = currentwave[((int)(p+1)) % patternlength];
      float upperweighting = fmodf(p, 1.0);
      float lowerweighting = 1.0 - upperweighting;
      brightness = (lower * lowerweighting) + (upper * upperweighting);
    }
    else{
      brightness = lower;
    }
    brightnesses[channel] = brightness * (255.0/(brightnesslevels-1));      // Whats going on with types here?
    }
}

void handleSerial() {
  getCommand();
  if (inputString[inputString.length()-1]=='\n') {                     // complete command available!
    if (inputString == "faster\n"){         // For some (preferably all) commands, human-readable feedback is sent over serial. this takes the place of comments
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
    else if (inputString == "slow\n") {
      rate = 1;
      Serial.println("Reduced speed to minimum");
    }
    else if (inputString == "fast\n") {
      rate = (cyclelength/patternlength);
      Serial.println("Increased speed to maximum");
    }
    else if (inputString == "Off\n") {
      memcpy(currentwave, off, (patternlength*sizeof(currentwave[0])));
      for(int channel = 0; channel < channels; ++channel){
        brightnesses[channel] = 0;    
         }
      Serial.println("Turned all lights off");
    }
    else if (inputString == "On\n") {
      memcpy(currentwave, on, (patternlength*sizeof(currentwave[0])));
      for(int channel = 0; channel < channels; ++channel){
        brightnesses[channel] = 255;    
      }
      Serial.println("Turned all lights on");
    }
    else if (inputString == "Sine\n") {
      memcpy(currentwave, sinewave, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using sine wave pattern");
    }
    else if (inputString == "Square\n") {
      memcpy(currentwave, squarewave, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using square wave pattern");
    }
    else if (inputString == "Triangle\n") {
      memcpy(currentwave, trianglewave, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using triangle wave pattern");
    }
    else if (inputString == "Saw\n") {
      memcpy(currentwave, sawtoothwave, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using sawtooth wave pattern");
    }
    else if (inputString == "waS\n") {
      memcpy(currentwave, htootwaswave, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using backward sawtooth wave pattern");
    }
    else if (inputString == "Short Blip\n") {
      memcpy(currentwave, shortblip, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using short blip wave pattern");
    }
    else if (inputString == "Long Blip\n") {
      memcpy(currentwave, longblip, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using long blip wave pattern");
    }
    else if (inputString == "Short Unblip\n") {
      memcpy(currentwave, unshortblip, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using short negative blip wave pattern");
    }
    else if (inputString == "Long Unblip\n") {
      memcpy(currentwave, unlongblip, (patternlength*sizeof(currentwave[0])));
      Serial.println("Now using long negative blip wave pattern");
    }
    else if (inputString == "interpolation on\n"){
      interpolate = true;
      Serial.println("Turned interpolation on");
      }
    else if (inputString == "interpolation off\n"){
      interpolate = false;
      Serial.println("Turned interpolation off");
      }
    else if (inputString == "Manual\n"){
      manual = true;
      Serial.println("Manual control mode; automatic sequencing disabled");
    }
    else if (inputString == "Auto\n"){
      manual = false;
      Serial.println("Automatic sequencing enabled");
    }
    else if (inputString == "Red On\n"){
      manual = true;
      brightnesses[0] = 255;
      Serial.println("Turned red lights on");
    }
    else if (inputString == "Green On\n"){
      manual = true;
      brightnesses[1] = 255;
      Serial.println("Turned green lights on");
    }
    else if (inputString == "Yellow On\n"){
      manual = true;
      brightnesses[2] = 255;
      Serial.println("Turned yellow lights on");
    }
    else if (inputString == "Red Off\n"){
      manual = true;
      brightnesses[0] = 0;
      Serial.println("Turned red lights off");
    }
    else if (inputString == "Green Off\n"){
      manual = true;
      brightnesses[1] = 0;
      Serial.println("Turned green lights off");
    }
    else if (inputString == "Yellow Off\n"){
      manual = true;
      brightnesses[2] = 0;
      Serial.println("Turned yellow lights off");
    }
    else if (inputString == "ping\n"){
      Serial.println("Ping sucessful");
    }
    else if (inputString == "status\n"){
      sendStatus();
    }
    else {
      Serial.print("Unknown command recieved: [");
      Serial.print(inputString);
      Serial.println("]");
    }
    inputString = "";
  } 
}

void getCommand(){
  while (Serial.available()) {
    Serial.write(6);                        // ASCII Acknowledge NPC - tell the web interface the arduino is working, but simplifies pre-display processing
    char inChar = (char)Serial.read();
    inputString += inChar;
  }
}

void sendStatus(){              // Generating snippets of HTML here is yukky
  Serial.print("<br>");
  Serial.print("Channel 1 brightness = ");
  Serial.print(brightnesses[0]);
  Serial.print("<br>");
  Serial.print("Channel 2 brightness = ");
  Serial.print(brightnesses[1]);
  Serial.print("<br>");
  Serial.print("Channel 3 brightness = ");
  Serial.print(brightnesses[2]);
  Serial.print("<br>");
  if (manual)  { Serial.print("Automatic sequencing disabled<br>");}
  //if (!manual) { Serial.print("Automatic sequencing enabled<br>");} //somehow, this line break the arduino code, badly! (???)
}


