#include <dht11.h>
dht11 DHT;
#define DHT11_PIN 30
int Hum_suelo=A0;
int Luminosidad=A1;
int luz = 0;       
int valor_sensor = 0;
int boton_1=22;
int boton_2=24;
int boton_3=26;
int boton_4=28;
int pin_riego=33;
int total_luz_por_min=0;

int dia;



void setup() {
Serial.begin(115200);
Serial3.begin(115200);
pinMode(boton_1,INPUT);
pinMode(boton_2,INPUT);
pinMode(boton_3,INPUT);
pinMode(boton_4,INPUT);
pinMode(23,OUTPUT);
pinMode(pin_riego,OUTPUT);

}



void loop() {
  if(Serial3.available()>0){
   dia =Serial3.read(); // leer dia o noche

   Serial.println(dia);
   Serial3.flush();
  }
//medir temperatura y humedad
int chk;
chk=DHT.read(DHT11_PIN);

if(dia=='1')
{
  if(DHT.temperature>=22 || DHT.temperature<=28)
  {
    //normal
  }
  else
  {
    if(DHT.temperature>28)
    {
      Serial3.write("1");
    }
    else
    {
      Serial3.write("2");
    }
  }
}
else
{
  if(DHT.temperature<18)
  {
      Serial3.write("2");
  }
}




//medir estado de suelo
int estado_suelo;
estado_suelo=analogRead(Hum_suelo);
if(estado_suelo>1100)
{
      Serial3.write("3");
}
else
{
  if(estado_suelo>=650)
  {
    //seco
    
    Serial3.write("4");
    digitalWrite(pin_riego,HIGH);//se enciende el motor
    delay(35000);//se lo deja funcionando por 35 seg
    digitalWrite(pin_riego,LOW);//se apaga el motor
  }
  else
  {
    if(estado_suelo>=380)
    {
      //normal
    }
    else
    {
      Serial3.write("5");
    }
  }
}


//medir luminosidad


//verificar los datos del if--- para saber cuando esta iluminado y cuando no(les puse un aprox de 350).
if(dia=='1')
{valor_sensor = analogRead(Luminosidad); 
luz = (5.0 * valor_sensor * 100.0)/1024.0; 
  if (luz <= 250)   //Si la resistendia que produce el ldr es menor o igual que 490
  {
     Serial3.write("6");
  }
}

digitalWrite(23,HIGH);
digitalWrite(pin_riego,LOW);
digitalWrite(boton_1,LOW);
digitalWrite(boton_2,LOW);
digitalWrite(boton_3,LOW);
digitalWrite(boton_4,LOW);
for(int i=0;i<1000;i++)
{
  if(digitalRead(boton_1)==HIGH)
  {Serial.println("boton 1");
    Serial3.write("1");
    delay(400);
  }
  if(digitalRead(boton_2)==HIGH)
  {
    Serial3.write("2");
    Serial.println("boton 2");
    delay(400);
  }
  if(digitalRead(boton_3)==HIGH)
  {
    Serial3.write("6");
    Serial.println("boton 3");
    delay(400);
  }
  if(digitalRead(boton_4)==HIGH)
  {Serial.println("boton 4");
    Serial3.write("4");
    digitalWrite(23,LOW);
    digitalWrite(pin_riego,HIGH);//se enciende el motor
    delay(35000);//se lo deja funcionando por 35 seg
    digitalWrite(pin_riego,LOW);//se apaga el motor
    i+=70;
    delay(400);
    digitalWrite(23,HIGH);
  }
  delay(120);
  
}
digitalWrite(23,LOW);

}
