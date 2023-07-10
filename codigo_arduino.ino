#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#include <Keypad.h>
#include <Ethernet.h>
#include <NTPClient.h>
#include <EthernetUdp.h>
#include <ArduinoJson.h>
#include "Componente.h"
#include <avr/wdt.h>
#include <Thermistor.h>



#define DHTPINSENSORINTERNO 2
#define DHTPINSENSOREXTERNO A15
#define DHTTYPE DHT11

DHT dhtInterno(DHTPINSENSORINTERNO, DHTTYPE);
DHT dhtExterno(DHTPINSENSOREXTERNO, DHTTYPE);

//PINAGEM DOS RELES

#define CANAL1 43
#define CANAL2 45
#define CANAL3 47
#define CANAL4 49

#define RELE1  12
#define RELE2  11
#define RELE3  6
#define RELE4  9
#define RELE5  8

#define PIN1SENSORAGUA1 A9
#define PIN2SENSORAGUA1 A10
#define PIN3SENSORAGUA1 A11
#define PIN4SENSORAGUA1 A12
#define PIN5SENSORAGUA1 A13




#define PINSENSORTEMPBOILER A3
#define PINSENSORTEMPRESFRIADOR A2


#define PIN1SENSORAGUA2 A4
#define PIN2SENSORAGUA2 A5
#define PIN3SENSORAGUA2 A6
#define PIN4SENSORAGUA2 A7




Thermistor sensorTempBoiler(PINSENSORTEMPBOILER);
Thermistor sensorTempResfriador(PINSENSORTEMPRESFRIADOR);



// --- Hardware do LCD ---
// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const byte numRows = 4; // linhas
const byte numCols = 4; // colunas

char teclado [numRows][numCols] = // Definir as teclas
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {22, 24, 26, 28}; // Pinos digitais relacionados as linhas.
byte colPins[numCols] = {30, 32, 34, 36}; // Pinos digitais relacionados as colunas.



unsigned long intervalo_fiscalizacao = 5000;
unsigned long tempo_anterior_fiscalizacao = millis();



Keypad meuteclado = Keypad(makeKeymap(teclado), rowPins, colPins, numRows, numCols);
char menuAtual = 'A';



byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;
EthernetUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000);


String id_central = "1";
char BASE_URL[] = "192.168.100.8";
int HTTP_PORT   = 8080;


String id_reservatorio = "1";
String id_ventiladores = "2";
String id_boiler_caixa = "3";
String id_boiler_temp = "4";
String id_ventiladores_externos = "5";
String id_resfriador = "6";

/*
  DynamicJsonDocument doc_static(3000);

  Componente buscar_dados_componentes(String id_componente) {


  // connect to web server on port 80:
  Serial.println(freeRam());
  if (client.connect(BASE_URL, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println( "GET /v1/componentes/" + id_componente + " HTTP/1.1");
    client.println("Host: " + String(BASE_URL));
    client.println("Connection: close");
    client.println(); // end HTTP header

    String json = "";
    bool json_started = false;
    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();

        if (c == '{') {
          json_started = true;
        }
        if (json_started) {
          json.concat(c);
        }
        if (c == '}') {
          // assume that the JSON ends before the last '0'
          break;
        }

      }
    }

    // the server's disconnected, stop the client:
    client.flush();
    client.stop();
    //json.replace("|", "\n");

    Serial.println("");
    Serial.print("JSON: ");
    Serial.println(json);

    DeserializationError error = deserializeJson(doc_static, json);

    if (error) {
      Serial.println("Erro na desserialização do JSON");
      delay(1000);
      return;
    }

    int id = doc_static["id_componente"];
    String nome = doc_static["nome"];
    String descricao = doc_static["descricao"].as<String>();
    int modo_operacao = doc_static["modo_operacao"];
    int nivel = doc_static["nivel"];
    int status_rele = doc_static["status_rele"];
    int nivel_min = doc_static["nivel_min"];
    int ativar = doc_static["ativar"];




    Componente componente(id, nome, descricao, modo_operacao, nivel, nivelAtual(), status_rele, nivel_min);
    componente.setAtivar(ativar);

    Serial.print("Nome: ");
    Serial.println(componente.getNome());
    Serial.print("Modo de Operação: ");
    Serial.println(componente.getModoOperacao());
    Serial.println("Ativar?: ");
    Serial.println(componente.getAtivar());

    Serial.println();
    Serial.println("disconnected");

    return componente;
  } else {// if not connected:
    Serial.println("Falha ao buscar dados");
    delay(1000);
    return;
  }

  }
*/


Componente buscar_dados_componentes(String id_componente) {


  // connect to web server on port 80:
  Serial.println(freeRam());
  if (client.connect(BASE_URL, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println( "GET /v1/arduino/list/" + id_componente + " HTTP/1.1");
    client.println("Host: " + String(BASE_URL));
    client.println("Connection: close");
    client.println(); // end HTTP header

    String json = "";
    bool json_started = false;
    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();

        if (c == '{') {
          json_started = true;
        }
        if (json_started) {
          json.concat(c);
        }
        if (c == '}') {
          // assume that the JSON ends before the last '0'
          break;
        }

      }
    }


    // the server's disconnected, stop the client:
    client.flush();
    client.stop();
    //json.replace("|", "\n");

    Serial.println("");
    Serial.print("JSON: ");
    Serial.println(json);

    int w = json.indexOf('W');
    int arroba = json.indexOf('@');
    int hash = json.indexOf('#');
    int porcentagem = json.indexOf('%');
    int ecom = json.indexOf('&');
    int sifrao = json.indexOf('$');
    int asterico = json.indexOf('*');
    int z = json.indexOf('Z');

    String Sid = json.substring(w + 1, arroba);
    String nome = json.substring(arroba + 1, hash);
    String SmodoOp = json.substring(hash + 1, porcentagem);
    String Snivel = json.substring(porcentagem + 1, ecom);
    String Snivel_min = json.substring(ecom + 1, sifrao);
    String Sstatus = json.substring(sifrao + 1, asterico);
    String sativar = json.substring(asterico + 1, z);



    int id = Sid.toInt();
    int modo_operacao =  SmodoOp.toInt();
    int nivel =  Snivel.toInt();
    int status_rele =  Sstatus.toInt();
    int nivel_min = Snivel_min.toInt();
    int ativar =  sativar.toInt();




    Componente componente(id, nome, "", modo_operacao, nivel, nivelAtual(), status_rele, nivel_min);
    componente.setAtivar(ativar);

    Serial.print("Nome: ");
    Serial.println(componente.getNome());
    Serial.print("Modo de Operação: ");
    Serial.println(componente.getModoOperacao());
    Serial.println("Ativar?: ");
    Serial.println(componente.getAtivar());

    Serial.println();
    Serial.println("disconnected");

    return componente;
  } else {// if not connected:
    Serial.println("Falha ao buscar dados");
    delay(500);
    return;
  }

}


void buscarTodosComponentes() {

  // connect to web server on port 80:
  Serial.println(freeRam());
  if (client.connect(BASE_URL, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println( "GET /v1/arduino/listAll HTTP/1.1");
    client.println("Host: " + String(BASE_URL));
    client.println("Connection: close");
    client.println(); // end HTTP header

    String json = "";
    bool json_started = false;
    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();

        if (c == '{') {
          json_started = true;
        }
        if (json_started) {
          json.concat(c);
        }
        if (c == '}') {
          // assume that the JSON ends before the last '0'
          break;
        }

      }
    }

    client.flush();
    client.stop();


    Serial.print("JSON:");
    Serial.println(json);


    int Q = json.indexOf('Q');
    int Y = json.indexOf('Y');
    int K = json.indexOf('K');
    int X = json.indexOf('X');
    int barra = json.indexOf('/');
    int mais = json.indexOf('[');
    int menos = json.indexOf(']');

    String componentes[6];

    componentes[0] = json.substring(Q + 1, Y);
    componentes[1] = json.substring(Y + 1, K);
    componentes[2] = json.substring(K + 1, X);
    componentes[3] = json.substring(X + 1, barra);
    componentes[4] = json.substring(barra + 1, mais);
    componentes[5] = json.substring(mais + 1, menos);


    for (int i = 0; i <= 5; i++) {

      Serial.print("Json ");
      Serial.print(i);
      Serial.print(" separada: ");
      Serial.println(componentes[i]);


      int w = componentes[i].indexOf('W');
      int arroba = componentes[i].indexOf('@');
      int hash = componentes[i].indexOf('#');
      int porcentagem = componentes[i].indexOf('%');
      int ecom = componentes[i].indexOf('&');
      int sifrao = componentes[i].indexOf('$');
      int asterico = componentes[i].indexOf('*');
      int z = componentes[i].indexOf('Z');

      String Sid = componentes[i].substring(w + 1, arroba);
      String nome = componentes[i].substring(arroba + 1, hash);
      String SmodoOp = componentes[i].substring(hash + 1, porcentagem);
      String Snivel = componentes[i].substring(porcentagem + 1, ecom);
      String Snivel_min = componentes[i].substring(ecom + 1, sifrao);
      String Sstatus = componentes[i].substring(sifrao + 1, asterico);
      String sativar = componentes[i].substring(asterico + 1, z);

      Serial.print("Sid: ");
      Serial.println(Sid);

      Serial.print("nome: ");
      Serial.println(nome);

      Serial.print("SmodoOp: ");
      Serial.println(SmodoOp);

      int id = Sid.toInt();
      int modo_operacao =  SmodoOp.toInt();
      int nivel =  Snivel.toInt();
      int status_rele =  Sstatus.toInt();
      int nivel_min = Snivel_min.toInt();
      int ativar =  sativar.toInt();




      Componente componente(id, nome, "", modo_operacao, nivel, nivelAtual(), status_rele, nivel_min);
      componente.setAtivar(ativar);

      Serial.print("Nome: ");
      Serial.println(componente.getNome());
      Serial.print("Modo de Operação: ");
      Serial.println(componente.getModoOperacao());
      Serial.println("Ativar?: ");
      Serial.println(componente.getAtivar());


      if (id == 1) {
        funcaoReservatorio(componente);
      } else if (id == 2) {
        funcaoVentiladores(componente);
      } else if (id == 3) {
        funcaoBoilerCaixa(componente);
      } else if (id == 4) {
        funcaoBoilerTemp(componente);
      } else if (id == 5) {
        funcaoVentiladoresExternos(componente);
      } else if (id == 6) {
        funcaoResfriador(componente);
      }
    }

  } else {// if not connected:
    Serial.println("Falha ao buscar dados");
    delay(500);
    return;
  }

}

void acao_mudar_modo_operacao(String id_componente, int modo_operacao, int status_rele, int nivel, int registrar) {
  Serial.println(freeRam());

  String queryString = "?modo_operacao=";
  String sModoOperaca = String(modo_operacao);
  String sStatusRele = String(status_rele);
  String sNivelAtual = String(nivel);
  String Sregistrar = String(registrar);


  queryString.concat(sModoOperaca);
  queryString.concat("&status_rele=");
  queryString.concat(sStatusRele);
  queryString.concat("&nivel_atual=");
  queryString.concat(sNivelAtual);
  queryString.concat("&registrar=");
  queryString.concat(Sregistrar);

  Serial.println(queryString);

  if (client.connect(BASE_URL, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println( "GET /v1/componentes/arduino/" + id_componente + queryString + " HTTP/1.1");
    client.println("Host: " + String(BASE_URL));
    client.println("Connection: close");
    client.println(); // end HTTP header

    // send HTTP body
    //client.println(queryString);

    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.flush();
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("Falha ao enviar dados");
    delay(500);
  }
}







void setup() {
  Serial.begin(115200);                                   // Inicializa a Serial
  Serial.println("Iniciando...");


  lcd.begin(20, 4);                                     // Inicializa LCD 20 x 4
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");


  //CONFIGURA TODOS OS RELES
  pinMode(CANAL1, OUTPUT);
  pinMode(CANAL2, OUTPUT);
  pinMode(CANAL3, OUTPUT);
  pinMode(CANAL4, OUTPUT);

  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);
  pinMode(RELE4, OUTPUT);
  pinMode(RELE5, OUTPUT);

  //DESABILITA TODOS OS RELES
  digitalWrite(CANAL1, HIGH);
  digitalWrite(CANAL2, HIGH);
  digitalWrite(CANAL3, HIGH);
  digitalWrite(CANAL4, HIGH);

  digitalWrite(RELE1, LOW);
  digitalWrite(RELE2, LOW);
  digitalWrite(RELE3, LOW);
  digitalWrite(RELE4, LOW);
  digitalWrite(RELE5, LOW);



  //Sensor de Agua 1
  pinMode(PIN1SENSORAGUA1, INPUT);
  pinMode(PIN2SENSORAGUA1, INPUT);
  pinMode(PIN3SENSORAGUA1, INPUT);
  pinMode(PIN4SENSORAGUA1, INPUT);
  pinMode(PIN5SENSORAGUA1, INPUT);

  //Sensor de Agua 2
  pinMode(PIN1SENSORAGUA2, INPUT);
  pinMode(PIN2SENSORAGUA2, INPUT);
  pinMode(PIN3SENSORAGUA2, INPUT);
  pinMode(PIN4SENSORAGUA2, INPUT);

  //Inicia os sensores de temperatura e umidade
  dhtInterno.begin();
  dhtExterno.begin();


  //INTERNET
  Serial.println("Iniciando DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());



  timeClient.begin();


  // Configura o Watchdog para um período de tempo de 10 segundos
  wdt_enable(WDTO_8S);



  data_hora();



}


String getDataAtual() {
  timeClient.update();
  String data = timeClient.getFormattedDate();
  return data;
}






String  HoraCompleta() {
  timeClient.update();
  int horas = timeClient.getHours();
  int minutos = timeClient.getMinutes();
  int segundos = timeClient.getSeconds();

  String hora_completa = "";
  hora_completa.concat(horas);
  hora_completa.concat(":");
  if (minutos < 10) {
    hora_completa.concat("0");
  }
  hora_completa.concat(minutos);
  hora_completa.concat(":");


  if (segundos < 10) {
    hora_completa.concat("0");
  }
  hora_completa.concat(segundos);

  return hora_completa;
}

void data_hora()                                        // Data e Hora (menu1)
{

  lcd.setCursor(0, 0);
  lcd.print("***** TITANIWM *****");

  lcd.setCursor(0, 1);
  lcd.print(getDataAtual());

  lcd.setCursor(12, 1);
  lcd.print(HoraCompleta());


  lcd.setCursor(0, 4);
  lcd.print("*** GREEN HOUSE ***");

} // end data_hora()

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void changeMenu()                                         // Modifica o menu atual
{
  char keypressed = meuteclado.getKey();
  if (keypressed !=  NO_KEY)
  {
    lcd.clear();
    Serial.println(keypressed); //Escreve na serial

    switch (keypressed)
    {
      case 'A': {
          menuAtual = keypressed;
          data_hora();
        } break;
      case 'B': {
          menuAtual = keypressed;

          Componente ventiladores = buscar_dados_componentes(id_ventiladores);
          temperaturaInterna(ventiladores);

        } break;
      case 'C': {
          menuAtual = keypressed;
          temperaturaExterna();


        } break;
      case 'D': {
          menuAtual = keypressed;
          Componente reservatorio = buscar_dados_componentes(id_reservatorio);
          nivelAgua1(reservatorio);
        } break;
      case '*': {
          menuAtual = keypressed;
          menuReles();
        } break;

      case '0': {
          menuAtual = keypressed;
          Componente boilerCaixa = buscar_dados_componentes(id_boiler_caixa);
          nivelAgua2(boilerCaixa);
        } break;

      case '#': {

          menuAtual = keypressed;

          Componente boilerTemp = buscar_dados_componentes(id_boiler_temp);
          tempBoiler(boilerTemp);


        } break;



      case '1': {
          if (menuAtual == '*') {

            digitalWrite(CANAL1, !digitalRead(CANAL1));
            menuReles();
          }
          if (menuAtual == 'D') {
            Componente reservatorio = buscar_dados_componentes(id_reservatorio);
            acao_mudar_modo_operacao(id_reservatorio, reservatorio.getModoOperacao() == 0 ? 1 : 0 , digitalRead(CANAL1) ? 0 : 1 , nivelAtual(), 0);
            reservatorio = buscar_dados_componentes(id_reservatorio);
            nivelAgua1(reservatorio);
          }

          if (menuAtual == 'B') {
            Componente ventiladores = buscar_dados_componentes(id_ventiladores);
            acao_mudar_modo_operacao(id_ventiladores, ventiladores.getModoOperacao() == 0 ? 1 : 0 , digitalRead(CANAL2) ? 0 : 1 , tempInternaAtual(), 0 );
            ventiladores = buscar_dados_componentes(id_ventiladores);
            temperaturaInterna(ventiladores);
          }

          if (menuAtual == '0') {
            Componente caixaBoiler = buscar_dados_componentes(id_boiler_caixa);
            acao_mudar_modo_operacao(id_boiler_caixa, caixaBoiler.getModoOperacao() == 0 ? 1 : 0 , digitalRead(RELE1), nivelAtualBoiler() , 0);
            caixaBoiler = buscar_dados_componentes(id_boiler_caixa);
            nivelAgua2(caixaBoiler);
          }

          if (menuAtual == '#') {
            Componente boilerTemp = buscar_dados_componentes(id_boiler_temp);
            acao_mudar_modo_operacao(id_boiler_temp, boilerTemp.getModoOperacao() == 0 ? 1 : 0 , digitalRead(RELE2), nivelAtualBoiler(), 0 );
            boilerTemp = buscar_dados_componentes(id_boiler_temp);
            tempBoiler(boilerTemp);
          }

        } break;
      case '2': {
          if (menuAtual == '*') {

            digitalWrite(CANAL2, !digitalRead(CANAL2));
            menuReles();
          }
        } break;
      case '3': {
          if (menuAtual == '*') {

            digitalWrite(CANAL3, !digitalRead(CANAL3));
            menuReles();
          }

          if (menuAtual == 'C') {

            digitalWrite(CANAL4, !digitalRead(CANAL4));
            Componente ventiladores = buscar_dados_componentes(id_ventiladores_externos);
            acao_mudar_modo_operacao(id_ventiladores_externos, ventiladores.getModoOperacao(), digitalRead(CANAL4) ? 0 : 1, tempExternaAtual(), 1 );
            temperaturaExterna();

          }


          if (menuAtual == 'D') {
            Componente reservatorio = buscar_dados_componentes(id_reservatorio);
            if (reservatorio.getModoOperacao() == 0) {

              int registrar_ativar = 0;
              if (digitalRead(CANAL1) == 1) {
                registrar_ativar = 1;
              }

              digitalWrite(CANAL1, !digitalRead(CANAL1));

              acao_mudar_modo_operacao(id_reservatorio, reservatorio.getModoOperacao(), digitalRead(CANAL1) ? 0 : 1, nivelAtual() , registrar_ativar);
              reservatorio = buscar_dados_componentes(id_reservatorio);
              nivelAgua1(reservatorio);
            }
          }

          if (menuAtual == 'B') {
            Componente ventiladores = buscar_dados_componentes(id_ventiladores);
            if (ventiladores.getModoOperacao() == 0) {

              int registrar_ativar = 0;
              if (digitalRead(CANAL2) == 1) {
                registrar_ativar = 1;
              }

              digitalWrite(CANAL2, !digitalRead(CANAL2));
              digitalWrite(CANAL3, !digitalRead(CANAL3));
              acao_mudar_modo_operacao(id_ventiladores, ventiladores.getModoOperacao(), digitalRead(CANAL2) ? 0 : 1, tempInternaAtual(), registrar_ativar );
              ventiladores = buscar_dados_componentes(id_ventiladores);
              temperaturaInterna(ventiladores);
            }
          }


          if (menuAtual == '0') {
            Componente boilerCaixa = buscar_dados_componentes(id_boiler_caixa);
            if (boilerCaixa.getModoOperacao() == 0) {

              int registrar_ativar = 0;
              if (digitalRead(RELE1) == 0) {
                registrar_ativar = 1;
              }

              digitalWrite(RELE1, !digitalRead(RELE1));
              acao_mudar_modo_operacao(id_boiler_caixa, boilerCaixa.getModoOperacao(), digitalRead(RELE1), nivelAtualBoiler(), registrar_ativar );
              boilerCaixa = buscar_dados_componentes(id_boiler_caixa);
              nivelAgua2(boilerCaixa);
            }
          }

          if (menuAtual == '#') {
            Componente boilerTemp = buscar_dados_componentes(id_boiler_temp);
            if (boilerTemp.getModoOperacao() == 0) {
              int registrar_ativar = 0;
              if (digitalRead(RELE2) == 0) {
                registrar_ativar = 1;
              }


              digitalWrite(RELE2, !digitalRead(RELE2));
              acao_mudar_modo_operacao(id_boiler_temp, boilerTemp.getModoOperacao(), digitalRead(RELE2), nivelAtualBoiler() , registrar_ativar);
              boilerTemp = buscar_dados_componentes(id_boiler_temp);
              tempBoiler(boilerTemp);
            }
          }

          /*if (menuAtual == '#') {
            Componente boilerTemp = buscar_dados_componentes(id_boiler_temp);
            if (boilerTemp.getModoOperacao() == 0) {
              digitalWrite(RELE2, !digitalRead(RELE2));
              acao_mudar_modo_operacao(id_boiler_temp, boilerTemp.getModoOperacao(), digitalRead(RELE2), nivelAtualBoiler() );
              boilerTemp = buscar_dados_componentes(id_boiler_temp);
              tempBoiler(boilerTemp);
            }
            }
          */



        } break;
      case '4': {
          if (menuAtual == '*') {

            digitalWrite(CANAL4, !digitalRead(CANAL4));
            menuReles();
          }
        } break;
      case '5': {
          if (menuAtual == '*') {

            digitalWrite(RELE1, !digitalRead(RELE1));
            menuReles();
          }
        } break;
      case '6': {
          if (menuAtual == '*') {

            digitalWrite(RELE2, !digitalRead(RELE2));
            menuReles();
          }
        } break;
      case '7': {
          if (menuAtual == '*') {

            digitalWrite(RELE3, !digitalRead(RELE3));
            menuReles();
          }
        } break;
      case '8': {
          if (menuAtual == '*') {

            digitalWrite(RELE4, !digitalRead(RELE4));
            menuReles();
          }
        } break;
      case '9': {
          if (menuAtual == '*') {


            digitalWrite(RELE5, !digitalRead(RELE5));
            menuReles();
          }
          if (menuAtual == 'A') {
            menuAtual = keypressed;
            Componente resfriador = buscar_dados_componentes(id_resfriador);
            menuResfriador(resfriador);
          }

        } break;
    }








  }

}
//=================================================================================================================


void loop() {


  wdt_reset();

  changeMenu();

  // fiscalizacao();

  novaFiscalizacao();
}


void temperaturaInterna(Componente ventiladores)                                                         // Temperatura (menu2)
{
  // Guardamos o tempo anterior como o ultimo intervalo de tempo lido
  float umi = dhtInterno.readHumidity();                                      // Le o valor da umidade
  float temp = dhtInterno.readTemperature();                                  // Le o valor da temperatura
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(temp) || isnan(umi))
  {

    lcd.setCursor(0, 0);
    lcd.print("Erro na Leitura Temp");
    Serial.println("Erro na Leitura Temp");
  }
  else
  {

    lcd.setCursor(0, 0);
    lcd.print(ventiladores.getNome());

    lcd.setCursor(0, 1);
    lcd.print("1-MODO: ");


    lcd.setCursor(9, 1);
    lcd.print(ventiladores.getModoOperacao() == 0 ? "MANUAL" : "AUTO");


    lcd.setCursor(0, 2);
    lcd.print("2-*");
    lcd.setCursor(3, 2);
    String nivel_desejado = String(ventiladores.getNivel());
    nivel_desejado.concat("C-");
    lcd.print(nivel_desejado);


    lcd.setCursor(7, 2);
    String nivel_min = String(ventiladores.getNivelMin());
    nivel_min.concat("C");
    lcd.print(nivel_min);


    lcd.setCursor(13, 2);
    String nivel_atual =  String(temp);
    nivel_atual.concat("C");
    lcd.print(nivel_atual);

    lcd.setCursor(0, 3);
    lcd.print("3-STATUS: ");
    lcd.setCursor(10, 3);
    lcd.print(digitalRead(CANAL2) ? "OFF" : "ON");






  }

} //end temperatura()

void temperaturaExterna()                                                         // Temperatura (menu2)
{
  // Guardamos o tempo anterior como o ultimo intervalo de tempo lido
  float umi = dhtExterno.readHumidity();                                      // Le o valor da umidade
  float temp = dhtExterno.readTemperature();                                  // Le o valor da temperatura
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(temp) || isnan(umi))
  {

    lcd.setCursor(0, 0);
    lcd.print("Erro na Leitura Temp");
    Serial.println("Erro na Leitura Temp");
  }
  else
  {
    //--- Exibe as informações no LCD ---
    lcd.setCursor(0, 0);
    lcd.print("SENSOR TEMP. EXTERNA");

    lcd.setCursor(0, 1);
    lcd.print("Temp.: ");
    lcd.setCursor(7, 1);
    lcd.print(temp, 1);
    lcd.setCursor(12, 1);
    lcd.print("C");

    lcd.setCursor(0, 2);
    lcd.print("Umi..: ");
    lcd.setCursor(7, 2);
    lcd.print(umi, 1);
    lcd.setCursor(12, 2);
    lcd.print("%");


    lcd.setCursor(0, 3);
    lcd.print("3-STATUS: ");
    lcd.setCursor(10, 3);
    lcd.print(digitalRead(CANAL4) ? "OFF" : "ON");

  }

} //end temperatura()


void menuReles() {


  lcd.setCursor(0, 0);
  lcd.print("RELES");

  lcd.setCursor(0, 1);
  lcd.print("1-");
  lcd.setCursor(2, 1);
  lcd.print(digitalRead(CANAL1) ? "OFF" : "ON");

  lcd.setCursor(0, 2);
  lcd.print("2-");
  lcd.setCursor(2, 2);
  lcd.print(digitalRead(CANAL2) ? "OFF" : "ON");

  lcd.setCursor(0, 3);
  lcd.print("3-");
  lcd.setCursor(2, 3);
  lcd.print(digitalRead(CANAL3) ? "OFF" : "ON");

  lcd.setCursor(7, 1);
  lcd.print("4-");
  lcd.setCursor(9, 1);
  lcd.print(digitalRead(CANAL4) ? "OFF" : "ON");


  lcd.setCursor(7, 2);
  lcd.print("5-");
  lcd.setCursor(9, 2);
  lcd.print(digitalRead(RELE1) ? "ON" : "OFF");

  lcd.setCursor(7, 3);
  lcd.print("6-");
  lcd.setCursor(9, 3);
  lcd.print(digitalRead(RELE2) ? "ON" : "OFF");

  lcd.setCursor(14, 1);
  lcd.print("7-");
  lcd.setCursor(16, 1);
  lcd.print(digitalRead(RELE3) ? "ON" : "OFF");


  lcd.setCursor(14, 2);
  lcd.print("8-");
  lcd.setCursor(16, 2);
  lcd.print(digitalRead(RELE4) ? "ON" : "OFF");

  lcd.setCursor(14, 3);
  lcd.print("9-");
  lcd.setCursor(16, 3);
  lcd.print(digitalRead(RELE5) ? "ON" : "OFF");





}


void nivelAgua1(Componente reservatorio) {

  int leituraPin1SensorAgua1 = digitalRead(PIN1SENSORAGUA1);
  int leituraPin2SensorAgua1 = digitalRead(PIN2SENSORAGUA1);
  int leituraPin3SensorAgua1 = digitalRead(PIN3SENSORAGUA1);
  int leituraPin4SensorAgua1 = digitalRead(PIN4SENSORAGUA1);
  int leituraPin5SensorAgua1 = digitalRead(PIN5SENSORAGUA1);

  lcd.setCursor(0, 0);
  lcd.print(reservatorio.getNome());

  lcd.setCursor(0, 1);
  lcd.print("1-MODO: ");

  lcd.setCursor(9, 1);
  lcd.print(reservatorio.getModoOperacao() == 0 ? "MANUAL" : "AUTO");


  lcd.setCursor(0, 2);
  lcd.print("2-NIVEL: *");
  lcd.setCursor(10, 2);
  String nivel_desejado = String(reservatorio.getNivel());
  nivel_desejado.concat("%");
  lcd.print(nivel_desejado);
  lcd.setCursor(15, 2);
  String nivel_atual =  String(nivelAtual());
  nivel_atual.concat("%");
  lcd.print(nivel_atual);

  lcd.setCursor(0, 3);
  lcd.print("3-STATUS: ");
  lcd.setCursor(10, 3);
  lcd.print(digitalRead(CANAL1) ? "OFF" : "ON");

}


void nivelAgua2(Componente reservatorioBoiler) {

  int leituraPin1SensorAgua2 = digitalRead(PIN1SENSORAGUA2);
  int leituraPin2SensorAgua2 = digitalRead(PIN2SENSORAGUA2);
  int leituraPin3SensorAgua2 = digitalRead(PIN3SENSORAGUA2);
  int leituraPin4SensorAgua2 = digitalRead(PIN4SENSORAGUA2);

  lcd.setCursor(0, 0);
  lcd.print(reservatorioBoiler.getNome());

  lcd.setCursor(0, 1);
  lcd.print("1-MODO: ");

  lcd.setCursor(9, 1);
  lcd.print(reservatorioBoiler.getModoOperacao() == 0 ? "MANUAL" : "AUTO");


  lcd.setCursor(0, 2);
  lcd.print("2-NIVEL: *");
  lcd.setCursor(10, 2);
  String nivel_desejado = String(reservatorioBoiler.getNivel());
  nivel_desejado.concat("%");
  lcd.print(nivel_desejado);
  lcd.setCursor(15, 2);
  String nivel_atual =  String(nivelAtualBoiler());
  nivel_atual.concat("%");
  lcd.print(nivel_atual);

  lcd.setCursor(0, 3);
  lcd.print("3-STATUS: ");
  lcd.setCursor(10, 3);
  lcd.print(digitalRead(RELE1) ? "ON" : "OFF");



}


float tempInternaAtual() {
  float temp = dhtInterno.readTemperature();                                  // Le o valor da temperatura
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(temp))
  {
    return 0.0;
  }
  else {
    return temp;
  }
}

float tempExternaAtual() {
  float temp = dhtExterno.readTemperature();                                  // Le o valor da temperatura
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(temp))
  {
    return 0.0;
  }
  else {
    return temp;
  }
}

int nivelAtual() {
  //modo automatico
  int leituraPin1SensorAgua1 = digitalRead(PIN1SENSORAGUA1);
  int leituraPin2SensorAgua1 = digitalRead(PIN2SENSORAGUA1);
  int leituraPin3SensorAgua1 = digitalRead(PIN3SENSORAGUA1);
  int leituraPin4SensorAgua1 = digitalRead(PIN4SENSORAGUA1);
  int leituraPin5SensorAgua1 = digitalRead(PIN5SENSORAGUA1);

  int nivel_atual = 0;

  if (leituraPin5SensorAgua1 == 1 && leituraPin4SensorAgua1 == 1) {
    nivel_atual = 100;
  } else if (leituraPin4SensorAgua1 == 1 && leituraPin3SensorAgua1 == 1) {
    nivel_atual = 80;
  } else if (leituraPin3SensorAgua1 == 1 && leituraPin2SensorAgua1 == 1) {
    nivel_atual = 60;
  } else if (leituraPin2SensorAgua1 == 1 && leituraPin1SensorAgua1 == 1) {
    nivel_atual = 40;
  } else if (leituraPin1SensorAgua1 == 1) {
    nivel_atual = 20;
  } else {
    nivel_atual = 0;
  }

  return nivel_atual;
}


int nivelAtualBoiler() {
  //modo automatico
  int leituraPin1SensorAgua2 = digitalRead(PIN1SENSORAGUA2);
  int leituraPin2SensorAgua2 = digitalRead(PIN2SENSORAGUA2);
  int leituraPin3SensorAgua2 = digitalRead(PIN3SENSORAGUA2);
  int leituraPin4SensorAgua2 = digitalRead(PIN4SENSORAGUA2);

  int nivel_atual = 0;

  if (leituraPin4SensorAgua2 == 1 && leituraPin3SensorAgua2 == 1) {
    nivel_atual = 100;
  } else if (leituraPin3SensorAgua2 == 1 && leituraPin2SensorAgua2 == 1) {
    nivel_atual = 80;
  } else if (leituraPin2SensorAgua2 == 1 && leituraPin1SensorAgua2 == 1) {
    nivel_atual = 50;
  } else if (leituraPin1SensorAgua2 == 1) {
    nivel_atual = 25;
  } else {
    nivel_atual = 0;
  }

  return nivel_atual;
}



void tempBoiler(Componente boilerTemp)                                                         // Temperatura (menu2)
{

  lcd.setCursor(0, 0);
  lcd.print(boilerTemp.getNome());

  lcd.setCursor(0, 1);
  lcd.print("1-MODO: ");


  lcd.setCursor(9, 1);
  lcd.print(boilerTemp.getModoOperacao() == 0 ? "MANUAL" : "AUTO");


  lcd.setCursor(0, 2);
  lcd.print("2-*");
  lcd.setCursor(3, 2);
  String nivel_desejado = String(boilerTemp.getNivel());
  nivel_desejado.concat("C-");
  lcd.print(nivel_desejado);


  lcd.setCursor(7, 2);
  String nivel_min = String(boilerTemp.getNivelMin());
  nivel_min.concat("C");
  lcd.print(nivel_min);

  int temperature = sensorTempBoiler.getTemp();

  lcd.setCursor(13, 2);
  String nivel_atual =  String(temperature);
  nivel_atual.concat("C");
  lcd.print(nivel_atual);

  lcd.setCursor(0, 3);
  lcd.print("3-STATUS: ");
  lcd.setCursor(10, 3);
  lcd.print(digitalRead(RELE2) ? "ON" : "OFF");




} //end temperatura()


void menuResfriador(Componente resfriador)                                                         // Temperatura (menu2)
{

  lcd.setCursor(0, 0);
  lcd.print(resfriador.getNome());


  lcd.setCursor(0, 1);
  lcd.print("1-*");
  lcd.setCursor(3, 1);
  String nivel_desejado = String(resfriador.getNivel());
  nivel_desejado.concat("C-");
  lcd.print(nivel_desejado);


  int temperature = sensorTempResfriador.getTemp();

  lcd.setCursor(13, 1);
  String nivel_atual =  String(temperature);
  nivel_atual.concat("C");
  lcd.print(nivel_atual);



} //end temperatura()


int getTemperaturaBoiler() {
  return sensorTempBoiler.getTemp();

}


int getTemperaturaResfriador() {
  return sensorTempResfriador.getTemp();

}




void funcaoVentiladores(Componente ventiladores) {

  /*Ventiladores */
  if (ventiladores.getModoOperacao() == 1) {


    int temp_atual = tempInternaAtual();
    int temp_programado = ventiladores.getNivel();
    int temp_min_programado = ventiladores.getNivelMin();

    if (temp_atual > temp_programado) {
      digitalWrite(CANAL2, LOW);
      digitalWrite(CANAL3, LOW);
      acao_mudar_modo_operacao(id_ventiladores, ventiladores.getModoOperacao(), 1 , temp_atual, 1);

    } else {
      digitalWrite(CANAL2, HIGH);
      digitalWrite(CANAL3, HIGH);
      acao_mudar_modo_operacao(id_ventiladores, ventiladores.getModoOperacao(), 0 , temp_atual, 0);

    }

  } else {

    int registrar_ativar = 0;
    if (ventiladores.getStatusRele() == 1) {
      digitalWrite(CANAL2, LOW);
      digitalWrite(CANAL3, LOW);
      registrar_ativar = 1;
    } else {
      digitalWrite(CANAL2, HIGH);
      digitalWrite(CANAL3, HIGH);

    }

    acao_mudar_modo_operacao(id_ventiladores, ventiladores.getModoOperacao(), digitalRead(CANAL2) ? 0 : 1 , tempInternaAtual(), registrar_ativar);


  }
  /*Ventiladores */
}

void funcaoVentiladoresExternos(Componente ventiladoresExternos) {
  /*Ventiladores Externos*/
  int registrar_ativar = 0;
  if (ventiladoresExternos.getStatusRele() == 1) {
    digitalWrite(CANAL4, LOW);
    registrar_ativar = 1;
  } else {
    digitalWrite(CANAL4, HIGH);

  }

  acao_mudar_modo_operacao(id_ventiladores_externos, ventiladoresExternos.getModoOperacao(), digitalRead(CANAL4) ? 0 : 1 , tempExternaAtual(), registrar_ativar);
  /*Ventiladores Externos*/
}

void funcaoBoilerCaixa(Componente boilerCaixa) {
  /*Boiler Caixa */

  if (boilerCaixa.getModoOperacao() == 1) {

    int nivel_atual = nivelAtualBoiler();
    int nivel_programado = boilerCaixa.getNivel();

    if (nivel_atual < nivel_programado) {
      digitalWrite(RELE1, HIGH);
      acao_mudar_modo_operacao(id_boiler_caixa, boilerCaixa.getModoOperacao(), 1 , nivelAtualBoiler(), 1);

    } else {
      digitalWrite(RELE1, LOW);
      acao_mudar_modo_operacao(id_boiler_caixa, boilerCaixa.getModoOperacao(), 0 , nivelAtualBoiler(), 0);

    }

  } else {
    int registrar_ativar = 0;

    if (boilerCaixa.getStatusRele() == 1) {
      digitalWrite(RELE1, HIGH);
      registrar_ativar = 1;
    } else {
      digitalWrite(RELE1, LOW);
    }

    acao_mudar_modo_operacao(id_boiler_caixa, boilerCaixa.getModoOperacao(), digitalRead(RELE1) , nivelAtualBoiler(), registrar_ativar);


  }

  /*Boiler Caixa */
}

void funcaoBoilerTemp(Componente boilerTemp) {
  /*Boiler Temp */

  if (boilerTemp.getModoOperacao() == 1) {


    int temp_atual = getTemperaturaBoiler();
    int temp_max = boilerTemp.getNivel();
    int temp_min = boilerTemp.getNivelMin();

    int ativar = boilerTemp.getAtivar();


    if (ativar == 1) {

      if (temp_atual < temp_min) {
        digitalWrite(RELE2, HIGH);
        acao_mudar_modo_operacao(id_boiler_temp, boilerTemp.getModoOperacao(), 1 , temp_atual, 1);

      } else if (temp_atual > temp_max) {
        digitalWrite(RELE2, LOW);
        acao_mudar_modo_operacao(id_boiler_temp, boilerTemp.getModoOperacao(), 0 , temp_atual, 0);
      }

    } else {
      digitalWrite(RELE2, LOW);
      acao_mudar_modo_operacao(id_boiler_temp, boilerTemp.getModoOperacao(), 0 , temp_atual, 0);

    }



  } else {

    int registrar_ativar = 0;

    if (boilerTemp.getStatusRele() == 1) {
      digitalWrite(RELE2, HIGH);
      registrar_ativar = 1;
    } else {
      digitalWrite(RELE2, LOW);
    }

    acao_mudar_modo_operacao(id_boiler_temp, boilerTemp.getModoOperacao(), digitalRead(RELE2) , getTemperaturaBoiler(), registrar_ativar);


  }

  /*Boiler Temp */
}

void funcaoReservatorio(Componente reservatorio) {
  /*Reservatorio */
  if (reservatorio.getModoOperacao() == 1) {


    int nivel_atual = nivelAtual();
    int nivel_programado = reservatorio.getNivel();

    Serial.print("Nivel de agua atual: ");
    Serial.println(nivel_atual);

    Serial.print("Nivel de agua programado: ");
    Serial.println(nivel_programado);

    if (nivel_atual < nivel_programado) {

      Serial.println("Nivel atual e menor, ligando! ");


      digitalWrite(CANAL1, LOW);
      acao_mudar_modo_operacao(id_reservatorio, reservatorio.getModoOperacao(), 1 , nivelAtual(), 1);

    } else {

      Serial.println("Nivel atual e maior, desligando! ");

      digitalWrite(CANAL1, HIGH);
      acao_mudar_modo_operacao(id_reservatorio, reservatorio.getModoOperacao(), 0 , nivelAtual(), 0);

    }

  } else {

    int registrar_ativar = 0;
    if (reservatorio.getStatusRele() == 1) {
      digitalWrite(CANAL1, LOW);
      registrar_ativar = 1;
    } else {
      digitalWrite(CANAL1, HIGH);
    }

    acao_mudar_modo_operacao(id_reservatorio, reservatorio.getModoOperacao(), digitalRead(CANAL1) ? 0 : 1 , nivelAtual(), registrar_ativar);


  }



  /*Reservatorio */
}

void funcaoResfriador(Componente resfriador) {
  /*Resfriador*/
  acao_mudar_modo_operacao(id_resfriador, resfriador.getModoOperacao(), 0, getTemperaturaResfriador(), 0);
  /*Resfriador*/
}

void novaFiscalizacao() {

  if (millis() - tempo_anterior_fiscalizacao > intervalo_fiscalizacao) {
    buscarTodosComponentes();
    tempo_anterior_fiscalizacao = millis();

  }
}
