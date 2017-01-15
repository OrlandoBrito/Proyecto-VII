int incomingByte = 0;
char inString[48];

void setup() {
  
   Serial.begin(115200);
   Serial3.begin(115200);
   Serial.flush();
}

void loop() {
char j[9];
if (Serial3.available() > 0){
   
     for(int i = 0;i<9;i++)
     {
       j[i]=Serial3.read();
        
       
     }
     Serial.println(atoi(j));
    
   }
         /*       delay(500);
                unsigned long epoch = atol(inString);
                Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
       Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
     
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
   
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
                Serial3.flush();
         
delay(1000);*/



                             
               
}
