#include <esp_now.h>
#include <WiFi.h>

struct DataStruct {  // Define a estrutura DataStruct para troca de informações
  int value;         // Variável inteira value
};

uint8_t macAddress[][6] = {
  { 0x34, 0x85, 0x18, 0x05, 0x4C, 0x30 }, // Client 1
  { 0x34, 0x85, 0x18, 0x06, 0x63, 0x54 }, // Client 2
  { 0x34, 0x85, 0x18, 0x07, 0x03, 0x78 }, // Client 3
  { 0x34, 0x85, 0x18, 0x05, 0xAE, 0x00 },


};

const int quantESP = sizeof(macAddress) / 6; 

DataStruct arrayDataRecv[quantESP]; // Ajuste o tamanho do array conforme necessário

DataStruct dataRecv;

// Declaração de variáveis
//Calcular a quantidade de esp com base no array acima, ou seja, pega a quantidade de bytes do todo o vetor e divide pela quantidade de bytess que cada vetor tem
//Ex.: se tem 3 mac, então o total de bytes é 18, que dividido pela quantidade de bytes que cada vetor (6 bytes), o resultado será igual a 3 dispositivos
int count = 1;
int i = 0;

esp_now_peer_info_t peerInfo;  // Cria uma estrutura esp_now_peer_info_t, que é utilizada para registrar informações sobre um peer (dispositivo) que será adicionado à rede ESPNOW

void verificarConexao() {
  if (esp_now_init() != ESP_OK) {  // Inicializa o ESP-NOW e verifica se há erros
    Serial.println("Inicialização ESP-NOW com Erro. O dispositivo será reiniciado");
    Serial.println("Reiniciando ESP32");
    ESP.restart();
  }
}

// Função que é chamada quando dados são recebidos via ESP-NOW
void OnDataRecv(const esp_now_recv_info* info, const uint8_t *incomingData, int len){
  if (count == quantESP) {
    Serial.println("");
    count = 1;
  }
  else {
    count++;
  }

  for (i = 0; i < quantESP; i++) {
    if (memcmp(info->src_addr, macAddress[i], 6) == 0) {
      memcpy(&dataRecv, incomingData, sizeof(dataRecv));  // Copia os dados recebidos para a estrutura de dados dataRecv
      
      // Mostra na Serial o dado recebido
      Serial.print("«╣ Client");
      Serial.print(i + 1);
      Serial.print(": ");
      
      arrayDataRecv[i] = dataRecv;
      Serial.println(arrayDataRecv[i].value); // Imprime o valor do membro 'value'
 
    }
    
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.disconnect();    // Desconecta de qualquer rede WiFi previamente conectada
  WiFi.mode(WIFI_STA);  // Define o modo WiFi como Station (cliente)

  verificarConexao();

  esp_now_register_recv_cb(OnDataRecv);  // Registra a função de callback que é chamada quando os dados são recebidos

  peerInfo.channel = 0;                             // Define o canal de comunicação como 0 na estrutura peerInfo
  peerInfo.encrypt = false;                         // Define a encriptação como desativada na estrutura peerInfo --------------------------------------------------------------

  for (i = 0; i < quantESP; i++) {
    memcpy(peerInfo.peer_addr, macAddress[i], 6);  // Copia o endereço MAC do ESP32 1 para a estrutura 
    // Tenta adicionar o ESP32  1 à lista de pares de comunicação ESP-NOW e verifica se foi bem sucedido
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {  // Caso não seja possível adicionar o ESP32 2, exibe mensagem de falha no display e reinicia o dispositivo
      Serial.print("Falha ao adicionar peer");
      Serial.print(i + 1);
      Serial.println("O dispositivo reiniciará");

      Serial.println("Reiniciando ESP32");
      ESP.restart();
    }
  }
}

void loop() {
  // Seu código de loop aqui
}
