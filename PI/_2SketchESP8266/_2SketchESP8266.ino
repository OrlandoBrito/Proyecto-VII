#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <SPI.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

unsigned int localPort = 2390; 
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
WiFiUDP udp;

String API = "PKWMEOBKNDEZTFBR";

// Message to post
String tweet1 = "Estoy muy expuesto al sol, mi temperatura es elevada y nosciva.  @orlbrca @Breyking555XD @JoshuaRosales_  @linda_huacon ";
String tweet2 = "La temperatura a la que estoy expuesto no es suficiente para mi desarrollo.  @orlbrca @Breyking555XD @JoshuaRosales_  @linda_huacon ";
String tweet3 = "El sensor de humedad del suelo ha sido desconectado.  @orlbrca @Breyking555XD @JoshuaRosales_  @linda_huacon ";
String tweet4 = "Mi suelo se encontraba muy seco, en estos momentos el sistema me está regando. @orlbrca @Breyking555XD @JoshuaRosales_  @linda_huacon ";
String tweet5 = "Mi suelo se encuentra demasiado húmedo, tome precauciones.  @orlbrca @Breyking555XD @JoshuaRosales_  @linda_huacon ";
String tweet6 = "No estoy recibiendo la cantidad de luz solar ideal, por favor expongame al sol. @orlbrca @Breyking555XD @JoshuaRosales_  @linda_huacon ";
const char* GIRASOLV = "GIRASOLV";
const char* TELEMATICAV = "TELEMATICAV";
int y = 0;
String pral = "<html>" //Utilice el tipo de datos String para contener varios caracteres sin la sobrecarga de administración de matriz de Char()
"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>" //Codigo de la pagina web en donde se configurará el SSID y l contraseña de la red
"<title>GIRASOL</title> <head> <style type='text/css'> body,td,th { color: #000000; } body { background-color: #9BFA88; } </style> </head>"
"<body>"
"<CENTER><h1><br><span class='apple-style-span'  style='background-color: #FFFFFF; padding:15px;'>&nbsp &nbsp;<font size='7';  color='#2200FD'>GIRASOL</font>&nbsp &nbsp;</span></h1><br>"
    "<form action= 'config' method='get'  target='pantalla'  >"
    "<fieldset align='center' style='border-style:solid; border-color:#D010E5; width:230px; height:230px; padding:30px; margin: 10px;'>"
     "<legend><font size=5 color='blue'<strong>Configurar WI-FI</strong></font></legend>"
         "SSID: &nbsp; &nbsp;  <input name='ssid' type='text'  size='18' /> <br> <br>"
      "<br>  CLAVE: <input name='pass' type='password' size='18'/> <br><br><br>"
      "<input style='width:120px; height:75px';  type='submit'  value='GUARDAR' />"
       " </fieldset>"
  "</form>"
" <br> <br> <br> <br> <br> <br> <br> <h2>Creadores: </h2>"
  "<h4> *Brito Casanova Orlando <br> *Huacon Salazar Linda <br> *Ibarra Carre&#241;o Brayan <br> *Perez Steven Roger <br> *Rosales Zambrano Joshua</h4>"
"<h5> UNIVERSIDAD TECNICA ESTATAL DE QUEVEDO - FACULTAD DE CIENCIAS DE LA INGENIERIA - INGENIERIA EN TELEMATICA - V SEMESTRE - 2016</h5>"
"<CENTER>  <a class='twitter-timeline'  href='https://twitter.com/GirasolTelematV' data-widget-id='690559355440254976'>Tweets por el @GirasolTelematV.</a>"
               "<script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0],p=/^http:/.test(d.location)?'http':'https';if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src=p+'://platform.twitter.com/widgets.js';fjs.parentNode.insertBefore(js,fjs);}}(document,'script','twitter-wjs');</script></CENTER>"
"<iframe id='pantalla' name='pantalla' src='' width=1px height=1px frameborder='0' scrolling='no'></iframe>" 
"</body>"
"</html>";

ESP8266WebServer server(80);
WiFiClient client; 
char ssid[20];
char pass[20];
String ssid_leido;
String pass_leido;
int ssid_tamano=0;
int pass_tamano=0;

  String arregla_simbolos(String a){ //cuando pasamos los datos del formulario mediante modo GET los caracteres especiales son reemplazados por codigo UTF-8, esta función reemplaza todos los codigos utf de caracteres especiales por el caracter especial correspondiente, o sea si encuentra un %23 en el string lo reemplazará por un #. 
     a.replace("%C3%A1","á");  a.replace("%C3%A9","é");
     a.replace("%C3%A","í");   a.replace("%C3%B3","ó");
     a.replace("%C3%BA","ú");  a.replace("%21","!");
     a.replace("%23","#");     a.replace("%24","$");
     a.replace("%25","%");     a.replace("%26","&");
     a.replace("%2F","/");     a.replace("%28","(");
     a.replace("%29",")");     a.replace("%3D","=");
     a.replace("%3F","?");     a.replace("%27","'");
     a.replace("%C2%BF","¿");  a.replace("%C2%A1","¡");
     a.replace("%C3%B1","ñ");  a.replace("%C3%91","Ñ");
     a.replace("+"," ");       a.replace("%2B","+");
     a.replace("%22","\"");     return a;
      }
      
  void graba (int addr, String a){ // graba string en eeprom
    int tamano = (a.length()+1); //determino el tamaño del string.
    Serial.print(tamano); //solo para debug.
    char inchar[tamano]; //al tamaño lo transformo en un caracter, o sea en un byte, lo que me limita a un tamáño máximo de string de 256 caracteres
    a.toCharArray(inchar,tamano); //transformo el string que llega a la función, o sea el str a grabar en un array de caracteres. (uno de los motivos x los que necesitaba calcular el tamaño)
    EEPROM.write(addr,tamano); //aca Viene el truco... en el primer byte grabo el tamaño. para que cuando lea. primero leo el tamaño y así sabré cuántos caracteres tendré que leer a continuación.
   Serial.println(a);
    for (int i=0; i<tamano; i++) { //ciclo for para ir grabando caracter x caracter el char array en sucesivos y consecutivos lugares de memoria.
        addr++;
        EEPROM.write(addr,inchar[i]);
        }
    EEPROM.commit();  // para que la grabación de datos cobre efecto.
    Serial.println("se grabo algo");
}
  void wifi_conf(){  //esta función se dispara al presionar el botón "setear conexión de la página de configuración

    int cuenta=0;
    
    String getssid = server.arg("ssid"); //recibo los valores que envía por get el formulario web
    String getpass = server.arg("pass");
    
    getssid=arregla_simbolos(getssid);  // reemplazo los símbolos que aparecen con UTF8 con el símbolo correcto
    getpass=arregla_simbolos(getpass);
    
    ssid_tamano=getssid.length()+1;  //calculamos la cantidad de caracteres que tiene el ssid y la clave
    pass_tamano=getpass.length()+1;
    
    getssid.toCharArray(ssid,ssid_tamano); // tansformamos el string en un char array ya que es lo que nos pide WiFI.begin();
    getpass.toCharArray(pass,pass_tamano);
    
    Serial.println(ssid);  //para depuración
    Serial.println(pass);  
   
   
    WiFi.begin(ssid, pass);  // intentamos conectar
    

   
    
      
  
        while (WiFi.status() != WL_CONNECTED) {
        delay(500);
       
        cuenta++;
          if (cuenta>20) {
         
        
            graba(70,"noconfigurado"); // si no se puede conectar... es porque el ssid o la contraseña están mal. entonces, no grabamos estos datos, pero en la dirección 70 grabamos "noconfigurado", para que cuando la placa arranque si lee en la dirección 70 "noconfigurado" omite intento de conexion a la red.
       /*   
            delay(5000);
             server.send(200, "text/html", String("<h2>Ups, La conexión no pudo realizarse<br> por lo que El SSid y la clave no se Guardaron</h2>"));
           // server.send(200, "text/plain",pral + String("Ups, La conexión no pudo realizarse<br> por lo que El SSid y la clave no se Guardaron")); // hago aparecer en el iframe "ups la conexión falló.
             delay(5000);*/
        digitalWrite(2, LOW);
        digitalWrite(0, HIGH);
            return; //salgo de la función.
           
          }
        }
    // esto sucede si nos conectamos correctamente...
    graba(70,"configurado"); // grabamos en la dire 70 "configurado" para que cuando la placa arranque, sepa que tiene datos correctos, entonces proceda a intentar una conexión.
    graba(1,getssid); // en la dirección 1 grabamos el ssid.
   
   graba(30,getpass);//en la dirección 30 grabamos el pass. (si sospechamos de que usaran ssid o pass muy largos ojo con los solapamientos, preferiblemente correr el pass de la 30 a la 60 por ejemplo, y el "configurado" pasarlo a la 120 por ejemplo.
 /*
   delay(5000);
   server.send(200, "text/html", String("<h2>Bien!!!! Conexión Exitosa a: " + getssid + "<br> El pass ingresado es: " + getpass + "<br> SSid y Clave correctamente Guardados <br> Estado: Conectado.</h2>"));
    //server.send(200, "text/plain", String("Bien!!!! Conexión Exitosa a: " + getssid + "<br> El pass ingresado es: " + getpass + "<br> SSid y Clave correctamente Guardados <br> Estado: Conectado.")); // muestro en el iframe el éxito de la conexión.
          
          delay(5000);
         */
       digitalWrite(2, HIGH);
       digitalWrite(0, LOW);
 }
  


  String lee(int addr){ //lee un string de la eeprom
    Serial.println("leo ");
    String nuevostring;
    int valor;
    int tamano=EEPROM.read(addr); // leo el primer byte porque ahí tengo guardada la cantidad de lugares, o direcciones que tengo que leer.
    for (int i=0; i<tamano; i++) { //for para ir leyendo byte a byte 
        addr++;
        valor=EEPROM.read(addr);
        nuevostring +=(char)(valor); // voy concatenando letra a letra hsta formar el string.
     }
return nuevostring; // devuelvo el string.
}

  void intento_conexion(){ //esta función se llama desde el setup en caso de que se lea en la dirección 70 "configurado" porque esto quiere decir que hay ssid y pass ciertos.

if (lee(70).equals("configurado")){
  
    ssid_leido=lee(1); //leemos ssid y clave
    pass_leido=lee(30);
  Serial.println("conectado guardado");
    Serial.println(ssid_leido); //para depuración
    Serial.println(pass_leido);
    
    ssid_tamano=ssid_leido.length()+1;  //calculamos la cantidad de caracteres que tiene el ssid y la clave
    pass_tamano=pass_leido.length()+1;
    
    ssid_leido.toCharArray(ssid,ssid_tamano); // transformamos el string en un char array ya que es lo que nos pide WiFI.begin();
    pass_leido.toCharArray(pass,pass_tamano);
 
    int cuenta=0;
    WiFi.begin(ssid, pass);  // intentamos conectar
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      cuenta++;
      if (cuenta>20) {
        Serial.println("Fallo en la Conexion");
        digitalWrite(0, HIGH);
        digitalWrite(2, LOW);
        return;  
        }
    }
}
        
    if (WiFi.status() == WL_CONNECTED){
        Serial.print("Exito! Conestados a: ");
        Serial.println(ssid);
        Serial.println(WiFi.localIP());
       digitalWrite(2, HIGH);
       digitalWrite(0, LOW);
        }
  }

unsigned long sendNTPpacket(IPAddress& address)
{
  
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void Hora(){

  
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); 
  delay(1000);
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no hay paquete todavia");
  }
  else {
    udp.read(packetBuffer, NTP_PACKET_SIZE); 
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    //epoch es la variale que contiene la fecha y hora 
    int hou = (epoch  % 86400L) / 3600 -5;
    if(hou>6&&hou<18){Serial.write("1");}else{Serial.write("0");}
  }
/*
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
  }*/


}



void setup() {
   pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
       digitalWrite(0, LOW);
       WiFi.disconnect(); 
  WiFi.mode(WIFI_AP_STA);
    Serial.begin(115200);
    EEPROM.begin(4096);
    WiFi.disconnect(); 
    WiFi.softAP(GIRASOLV,TELEMATICAV);
  intento_conexion();
 
    server.on("/", []() { //el slash sirve para indicar la direccion por defecto 192.168.4.1
      server.send(200, "text/html", pral);}); // envia la pagina web
      
      server.on("/config", wifi_conf);
  
    server.begin();

   udp.begin(localPort);
  
   
}

void loop() {
 
  
  server.handleClient();

   Hora();
  if (Serial.available() > 0) {
   int entrada = Serial.read();

 if (client.connect("184.106.153.149", 80))
{ String   enviar;
  switch(entrada)
  {case '1':
    enviar = "api_key="+API+"&status=" +tweet1 +y; 
       break;
    case '2':
    enviar = "api_key="+API+"&status="+tweet2+y; 
       break;
     case '3':
     enviar = "api_key="+API+"&status="+tweet3+y; 
       break;
     case '4':
     enviar = "api_key="+API+"&status="+tweet4+y; 
       break;
     case '5':
     enviar = "api_key="+API+"&status="+tweet5+y; 
       break;
     case '6':
     enviar = "api_key="+API+"&status="+tweet6+y; 
       break;
     default:
        enviar = "api_key="+API+"&status= Mensaje incorrecto, algo pasó mal "+y; 
       break; 

  }

   
              
   client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n"); 
   client.print("Host: api.thingspeak.com\n"); 
   client.print("Connection: close\n"); 
    client.print("Content-Type: application/x-www-form-urlencoded\n"); 
    client.print("Content-Length: "); 
   client.print(enviar.length()); 
   client.print("\n\n"); 

 y++;
 
   client.print(enviar); 
     }
 
  }
  

}



