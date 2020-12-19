//////DECLARAÇÃO DE BIBLIOTECAS//////
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h> 
#include <TinyGPS++.h>
#include "SSD1306Wire.h" 

//////CONSTANTES DE CONFIGURAÇÃO//////
#define placaTransmissora 1 //1 = PLACA TRANSMISSORA; 0= PLACA RECEPTORA
#define gpsOn 1 //1 = Sim; 0 = Não; Se essa constante for 1, as mensagens transmitidas serão os dados do GPS (essa constante não é usada se a placa for receptora)

//////DEFINIÇÕES DOS PINOS ENTRE OUTROS//////
#define frequenciaLoRa   915E6 //868E6 ou 915E6, no caso das nossas placas

#define telaTipo         SSD1306Wire
#define telaEndereco     0x3C
#define telaSDA          21
#define telaSCL          22

#define CONFIG_MOSI      27
#define CONFIG_MISO      19
#define CONFIG_CLK       5
#define CONFIG_NSS       18
#define CONFIG_RST       23
#define CONFIG_DIO0      26

HardwareSerial GPSSerial1(1);

telaTipo display(telaEndereco, telaSDA, telaSCL); //Configuração dos pinos do display OLED das plaquinhas

TinyGPSPlus gps;
