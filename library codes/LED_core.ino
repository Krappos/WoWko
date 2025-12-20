#define LED_OK <int>
#define LED_WARNING <int>
#define LED_ERROR <int>


void setup() {

Serial.begin(115200);
pinMode(LED_OK,OUTPUT);
pinMode(LED_WARNING, OUTPUT);
pinMode(LED_ERROR,OUTPUT);

}

void loop() {
timer(<defined>);
 
}

void timer(int status) {

  digitalWrite(status, HIGH); 
  delay(1000);                     
  digitalWrite(status, LOW);    
  delay(1000);   

}


