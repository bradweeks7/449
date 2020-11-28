int count = 0;                         /* used for time to distance calc */
uint32_t std_dev_counter = 1;               /* will perform modulo every 10 when array is filled */
uint32_t std_buffer[10];                    /* for std deviation calcs */
float distance = 0;                         /* variable for count to distance calc */
double standard_dev = 0;
float average = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);                         /* TO HEX INVERTER -> ONE SHOT */
  pinMode(12, INPUT);                          /* FROM PLL -> MCU */                   
  memset(std_buffer, 0, sizeof(std_buffer)); /* Zero out array buffer */
  Serial.begin(9600);
  
}

void loop() {
  /*
   * HERE'S SOME LOOP LOGIC
   * 1. SEND PULSE, START COUNTING
   * 2. COUNT UNTIL THE SIGNAL IS RECIEVED
   * 3. CALCULATE DISTANCE BASED ON TIME
   * 4. CHECK IF VALUE BUFFER IS FULL, IF IT IS, CALCULATE STANDARD DEVIATION AND ZERO OUT ARRAY
   * 5. IF IT IS NOT FULL, APPEND VALUE TO ARRAY
   */
  
  // put your main code here, to run repeatedly:
  send_pulse();
  while(digitalRead(12) == HIGH) 
  {
     count++;
  }
  distance = time_to_distance(count);
  
  if( !(std_dev_counter % 11)){
    standard_dev = standardDeviation(std_buffer, sizeof(std_buffer)/sizeof(std_buffer[0]));
    average = get_avg(std_buffer, sizeof(std_buffer)/sizeof(std_buffer[0]));
    memset(std_buffer, 0, sizeof(std_buffer));
    std_dev_counter = 1;
    std_buffer[std_dev_counter-1] = distance;
  } else {
    std_buffer[std_dev_counter-1] = distance;
  }
  Serial.print("Distance: ");
  Serial.print(average, 4);
  Serial.print(" Meters");
  Serial.print(" | Standard Deviation: ");
  Serial.print(standard_dev, 3);
  Serial.print(" Meters");
  Serial.println();
  Serial.print("Debug Value | Count: ");
  Serial.print(count,10);
  Serial.println();
  count = 0;
  delay(250);
}

void send_pulse(void) {
   digitalWrite(13, HIGH);
   delay(50);
   digitalWrite(13, LOW);
}

float time_to_distance(uint32_t time){
  /* 330: meters per second at 25 degrees Celsius  */
  /* distance = time/2 (forth and back) * 330/second */
  return float(time*0.000165 * (1.38/1.52)); /* 1.38/1.52 = correction factor, correction factor = (wanted/measured) */ 
}

int variance(uint32_t std_buffer[], uint32_t n){
  /* compute mean */
  int sum = 0;
  for(int i = 0; i < n; i++){
     sum += std_buffer[i];
  }
  double mean = (double)sum / (double)n;

  /* compute sum squared, differences with mean */
  double sqDiff = 0;
  for(int i = 0; i < n; i++){
     sqDiff += (std_buffer[i] - mean) * (std_buffer[i] - mean);
  }
  return sqDiff/n;
}

double standardDeviation(uint32_t std_buffer[], uint32_t n) {
  return sqrt(variance(std_buffer,n));
}

float get_avg(uint32_t std_buffer[], uint32_t n){
  float sum = 0;
  float avg = 0;
  for(int i = 0; i < n; i++){
    sum += std_buffer[i];
  }
  avg = sum/(float)n;
  return avg;
}