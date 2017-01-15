#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

String pral = "<html>" //este string contiene el código html de la página de configuración.
"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/> "
"<title>WIFI CONFIG</title> <style type='text/css'> body,td,th { color: #036; } body { background-color: #999; } </style> </head>" 
"<body> "
"<h1>WIFI CONF</h1><br>" 
    "<form action= 'config' method='get' target='pantalla'>" 
    "<fieldset align='left' style='border-style:solid; border-color:#336666; width:200px; height:180px; padding:10px; margin: 5px;'>"
        "<legend><strong>Confifurar WI-FI</strong></legend>" 
        "SSID: <br> <input name='ssid' type='text' size='15' /><br> <br>"
        "PASSWORD: <br> <input name='pass' type='password' size='15'/> <br><br>"
      "<input type='submit' value='Setear Conexión' />" 
        "</fieldset>"
  "</form>"
"<iframe id='pantalla' name='pantalla' src='' width=900px height=400px frameborder='0' scrolling='no'></iframe>"             
"</body>" 
"</html>";

ESP8266WebServer server(80);

char ssid[20];
char pass[20];
String ssid_leido;
String pass_leido;
int ssid_tamano=0;
int pass_tamano=0;

  String arregla_simbolos(String a){ //cuando pasamos los datos del formulario mediante modo GET los caracteres especiales son reemplazados por codigo UTF-8, esta función reemplaza todos los codigos utf de caracteres especiales por el caracter especial correspondiente, o sea si encuentra un %23 en el string lo reemplazará por un #. 
     a.replace("%C3%A1","á");
     a.replace("%C3%A9","é");
     a.replace("%C3%A","í");
     a.replace("%C3%B3","ó");
     a.replace("%C3%BA","ú");
     a.replace("%21","!");
     a.replace("%23","#");
     a.replace("%24","$");
     a.replace("%25","%");
     a.replace("%26","&");
     a.replace("%2F","/");
     a.replace("%28","(");
     a.replace("%29",")");
     a.replace("%3D","=");
     a.replace("%3F","?");
     a.replace("%27","'");
     a.replace("%C2%BF","¿");
     a.replace("%C2%A1","¡");
     a.replace("%C3%B1","ñ");
     a.replace("%C3%91","Ñ");
     a.replace("+"," ");
     a.replace("%2B","+");
     a.replace("%22","\"");
     return a;
      }
  void graba (int addr, String a){ // graba string en eeprom
    int tamano= (a.length()+1); //determino el tamaño del string.
    Serial.println("GRABADO"); //solo para debug.
    char inchar[tamano]; //al tamaño lo transformo en un caracter, o sea en un byte, lo que me limita a un tamáño máximo de string de 256 caracteres
    a.toCharArray(inchar,tamano); //transformo el string que llega a la función, o sea el str a grabar en un array de caracteres. (uno de los motivos x los que necesitaba calcular el tamaño)
    EEPROM.write(addr,tamano); //aca biene el truco... en el primer byte grabo el tamaño. para que cuando lea. primero leo el tamaño y así sabré cuántos caracteres tendré que leer a continuación.
    for (int i=0; i<tamano; i++) { //ciclo for para ir grabando caracter x caracter el char array en sucesivos y consecutivos lugares de memoria.
        addr++;
        EEPROM.write(addr,inchar[i]);
        }
    EEPROM.commit();  // para que la grabación de datos cobre efecto.
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
        Serial.print(".");
        cuenta++;
          if (cuenta>20) {
            graba(70,"noconfigurado"); // si no se puede conectar... es porque el ssid o la contraseña están mal. entonces, no grabamos estos datos, pero en la dirección 70 grabamos "noconfigurado", para que cuando la placa arranque si lee en la dirección 70 "noconfigurado" omite intento de conexion a la red.
            server.send(200, "text/html", String("<h2>Ups, La conexión no pudo realizarse<br> por lo que El SSid y la clave no se Guardaron</h2>")); // hago aparecer en el iframe "ups la conexión falló.
            return; //salgo de la función.
          }
        }
    Serial.print(WiFi.localIP());  // esto sucede si nos conectamos correctamente...
    graba(70,"configurado"); // grabamos en la dire 70 "configurado" para que cuando la placa arranque, sepa que tiene datos correctos, entonces proceda a intentar una conexión.
    graba(1,getssid); // en la dirección 1 grabamos el ssid.
    graba(30,getpass);//en la dirección 30 grabamos el pass. (si sospechamos de que usaran ssid o pass muy largos ojo con los solapamientos, preferiblemente correr el pass de la 30 a la 60 por ejemplo, y el "configurado" pasarlo a la 120 por ejemplo.
    server.send(200, "text/html", String("<h2>Bien!!!! Conexión Exitosa a: " + getssid + "<br> El pass ingresado es: " + getpass + "<br> SSid y Clave correctamente Guardados <br> Estado: Conectado.</h2>")); // muestro en el iframe el éxito de la conexión.-
}
  


  String lee(int addr){ //lee un string de la eeprom
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
        return;  
        }
    }
}
        
    if (WiFi.status() == WL_CONNECTED){
        Serial.print("Exito! Conestados a: ");
        Serial.println(ssid);
        Serial.println(WiFi.localIP());
        }
  }


void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    EEPROM.begin(4096);
    WiFi.softAP("ESP-8266");
 
    server.on("/", []() {
      server.send(200, "text/html", pral);}); 
      
      server.on("/config", wifi_conf);
  
    server.begin();
     Serial.println("Webserver iniciado...");

 Serial.println(lee(70));
 Serial.println(lee(1));
 Serial.println(lee(30));
 intento_conexion();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  delay(2000);
  }


