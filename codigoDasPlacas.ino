#include "configuracoes.h"

static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (GPSSerial1.available())
      gps.encode(GPSSerial1.read());
  } while (millis() - start < ms);
}

void setup(){//void setup é uma função feita apenas uma vez, antes de começar a ser feito o void loop
    Serial.begin(115200);
    while (!Serial);
    
    GPSSerial1.begin(9600, SERIAL_8N1, 12, 15); //17-TX 18-RX (se o GPS não estiver lendo dados, tente trocar o 12 e o 15 de lugar)

    ///Configurações do Display e o textinho que aparece no começo
    display.init();
    display.flipScreenVertically();
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(display.getWidth()/2,0, "Placa de LoRa");
    display.drawString(display.getWidth()/2,10, "Código por Bebagrê");
    display.display();
    delay(2500);

    ///Configuração dos pinos de Lora e da frequência
    SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
    LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);
    if (!LoRa.begin(frequenciaLoRa)) {
        Serial.println("Erro da frequência utilizada...");
        while (1);
    }
  
    if (!placaTransmissora) {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0,0, "Esperando Dados");
        display.display();
    }
}

///Essa variável conta quantas mensagens foram enviadas/recebidas
int mensagens = 0;

void loop(){//Essa função é feita depois do void setup e vai ser repetida para sempre

////Códigos para transmissão de mensagens////
#if placaTransmissora
//Envio de "ovelhas", caso o GPS não esteja ativado
      if(gpsOn==0){ 
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 0, "Pacotes enviados: " + String(mensagens));
        display.drawString(0, display.getHeight() - 10, "Enviando Dados...");
        display.display();
        
        LoRa.beginPacket();
        if(mensagens == 1){
          LoRa.print(mensagens);
          LoRa.println(" Ovelha");
        } else {
          LoRa.print(mensagens);
          LoRa.println(" Ovelhas");
        }
        LoRa.print("T: "); //Hora Local (Horário de Brasília)
        LoRa.print(gps.time.hour()-3);
        LoRa.print(":");
        LoRa.print(gps.time.minute());
        LoRa.print(":");
        LoRa.println(gps.time.second());
        LoRa.endPacket();
        mensagens++;
        smartDelay(1000);

//Código para a T-Beam transmitir os dados do GPS
      } else {
      //Mensagem a ser transmitida via LoRa
      LoRa.beginPacket();
        if (millis() > 5000 && gps.charsProcessed() < 10){
          Serial.println(F("No GPS data received: check wiring"));
      } else {
        Serial.print("La: "); //Latitude
        Serial.print(gps.location.lat(), 5);
        Serial.println("°");
        Serial.print("Lo: "); //Longitude
        Serial.print(gps.location.lng(), 5);
        Serial.println("°");
        Serial.print("Sat: "); //Número do Satélite
        Serial.print(gps.satellites.value());
        Serial.print(" H: "); //Altitude
        Serial.print(gps.altitude.feet() / 3.2808);
        Serial.println("m");
        Serial.print("T: "); //Hora Local (Horário de Brasília)
        Serial.print(gps.time.hour()-3);
        Serial.print(":");
        Serial.print(gps.time.minute());
        Serial.print(":");
        Serial.println(gps.time.second());
        smartDelay(1000);  //Se não tiver essa função, os dados do GPS não serão lidos!
      
      LoRa.endPacket(); 
      }
    }

////Código para Recepção das mensagens via LoRa
#else
    ///Recepção em si das mensagens
    if (LoRa.parsePacket()) { //"Se um pacote LoRa foi recebido"?
        String dadosRecebidos = ""; //Isso garante que os dados recebidos por LoRa serão convertidos para uma string
        while (LoRa.available()) { //Enquanto houver uma conexão LoRa
            dadosRecebidos += (char)LoRa.read(); //Atualize o valor dos dados recebidos
        }
        mensagens++; //Variável que mostra quantas mensagens foram recebidas

    ///Mostrar as mensagens recebidas no display
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 0, dadosRecebidos);
        display.drawString(0, display.getHeight()-10, "[" + String(mensagens) + "] RSSI: " + String(LoRa.packetRssi()));
        display.display();
    }
#endif
}
