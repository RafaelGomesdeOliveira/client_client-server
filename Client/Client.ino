#include <esp_now.h>  // Inclui a biblioteca esp_now para o uso do protocolo de comunicação ESP-NOW
#include <WiFi.h>     // Inclui a biblioteca WiFi para configuração da rede sem fio

#define PinSensorUmidade D1

uint8_t macAddress_Servidor[] = { 0x34, 0x85, 0x18, 0x05, 0x4C, 0x30 };  // Define o endereço MAC do ESP32 1. Coloque o endereço da sua placa aqui

struct DataStruct {  // Define a estrutura DataStruct para troca de informações
  int value;         // Variável inteira value
};

const String name_Client = "Cliente 03"; //Dê o nome do client Ex.: Cliente 01 ou Cliente 02

int umidade = 0;
int i = 0;
DataStruct dataSend;  // Declara a estrutura dataSend para armazenar dados enviados para o servidor

esp_now_peer_info_t peerInfo;  // Cria uma estrutura esp_now_peer_info_t, que é utilizada para registrar informações sobre um peer (dispositivo) que será adicionado à rede ESPNOW


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {  // Função chamada quando os dados são enviados
  Serial.print("╠» Enviado de ");
  Serial.print(name_Client);
  Serial.print(" para Servidor ╠» ");
  if (status == ESP_NOW_SEND_SUCCESS) {               // Se o envio foi bem sucedido, ...
    dataSend.value = calcularUmidade();
    Serial.println(dataSend.value);  // Converte o valor de dataSend.value para uma string formatada e envia para a porta serial
  } 
  else {
    Serial.println("ERRO!!!!");  
  }

}

int calcularUmidade(){
  for(i = 0; i < 5; i++){
    umidade = umidade + analogRead(PinSensorUmidade);
    Serial.print('Count');
    Serial.print(i);
    Serial.print(': ');
    Serial.println(umidade);
  }
  
  Serial.print('Umidade: ');
  Serial.println(umidade); 

  umidade = analogRead(PinSensorUmidade);
  if(analogRead(PinSensorUmidade) >= 1500){
    Serial.print("Baixa umidade: ");
  }
  else if (analogRead(PinSensorUmidade) > 1000 && analogRead(PinSensorUmidade) < 1500) {
    Serial.print("Umidade moderada: ");
  }
  else {
    Serial.print("Umidade calibrada: ");
  }
  return umidade;
}


void setup() {

  Serial.begin(115200); // Inicia a comunicação serial com uma taxa de 115200 bits por segundo
 

  WiFi.disconnect();    // Desconecta de qualquer rede WiFi previamente conectada
  WiFi.mode(WIFI_STA);  // Define o modo WiFi como Station (cliente)

  if (esp_now_init() != ESP_OK) {                                                    // Inicializa o ESP-NOW e verifica se há erros
    Serial.println("Inicialização ESP-NOW com Erro. O dispositivo reiniciará");  // Se houver um erro, imprime uma mensagem de reinicialização na porta serial e após alguns instantes, reinicia o dispositivo
    Serial.println("Reiniciando ESP32");
    ESP.restart();  // Reinicia o dispositivo
  }

  esp_now_register_send_cb(OnDataSent);  // Registra a função de callback que é chamada quando os dados são enviados


  memcpy(peerInfo.peer_addr, macAddress_Servidor, 6);  // Copia o endereço MAC do ESP32 Servidor para a estrutura peerInfo
  peerInfo.channel = 0;                                // Define o canal de comunicação como 0 na estrutura peerInfo
  peerInfo.encrypt = false;                            // Define a encriptação como desativada na estrutura peerInfo

  // Tenta adicionar o ESP32 1 à lista de pares de comunicação ESP-NOW e verifica se foi bem sucedido
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {  // Caso não seja possível adicionar o ESP32 1, exibe mensagem de falha na Serial e reinicia o dispositivo
    Serial.println("Falha ao adicionar peer 1. O dispositivo reiniciará");

    Serial.println("Reiniciando ESP32");
    ESP.restart();
  }

  // Inicializa as variáveis com os valores 0

  dataSend.value = 0;
}

void loop() {
  esp_now_send(macAddress_Servidor, (uint8_t *)&dataSend, sizeof(DataStruct));  // Envie os dados para o Servidor

}