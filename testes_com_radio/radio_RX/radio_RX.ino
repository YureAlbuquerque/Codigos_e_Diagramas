#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#define FRENTE 70
#define RE 66
#define DIREITA 82
#define ESQUERDA 76
#define TENTATIVAS 2
#define ACK 6
#define NACK 21
/**********************************************************************************************************************************************/
//Mirf.configRegister(RF_SETUP,0x0D);
class Radio{
public:
  char myAddress[5]; 
  Radio(uint8_t cePIN,uint8_t csnPIN,uint8_t CH,char address[]);
  boolean available();
  uint8_t writePacket(uint8_t *remoteAddress,uint8_t fnc,uint8_t *array,uint8_t len);
  uint8_t readPacket(uint8_t *msg);

};
/**********************************************************************************************************************************************/
Radio::Radio(uint8_t cePIN,uint8_t csnPIN,uint8_t CH,char Myaddress[]){
  ////Serial.print(Myaddress);
  for(uint8_t i=0;i<5;i++){
    myAddress[i] = Myaddress[i];   
  }
  ////Serial.println((char*)myAddress);
  Mirf.cePin = cePIN;
  Mirf.csnPin = csnPIN;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();   
  Mirf.setRADDR((uint8_t *)Myaddress);   
  Mirf.payload = 32;
  Mirf.channel = CH;   
  Mirf.config();
  //Mirf.configRegister(SETUP_RETR,0x2f);//habilita a possibilidade te 15 tentativas de envio em caso de falha
  //Mirf.configRegister(EN_AA,0x01);
}
/**********************************************************************************************************************************************/
boolean Radio::available(){
  return Mirf.dataReady();
}
/**********************************************************************************************************************************************/
uint8_t Radio::writePacket(uint8_t *remoteAddress,uint8_t fnc,uint8_t *array,uint8_t len){//mensagem com no maximo 25 caracteres
  Mirf.setTADDR((byte *)remoteAddress);
  uint8_t err = 0;
  uint8_t data[32];
  /******************************************************/
  //primeira parte da mensagem
  /******************************************************/
  while(1){
    /**********************/
    //transmissão
    for(uint8_t i=0;i<32;i++){
      data[i] = 0;
    }
    //inicio
    data[0] = 1;
    //endereço
    for(uint8_t i=0;i<5;i++){
      data[i+1] = myAddress[i];
    }
    //tamanho
    data[6] = len;
    //função
    data[7] = fnc;
    //acressente o array a mensagem 
    for(uint8_t i=0;i<len;i++){
      data[i+8] = *(array+i);
    }
    //calcule checksum
     Mirf.send((uint8_t*)data);//Envia os dados.
    while(Mirf.isSending());//loop enquanto não terminar a transmissão.
    //Serial.println("Transmitido");
    uint8_t Register;
    Mirf.readRegister(OBSERVE_TX, &Register, sizeof(Register));//pegar o numero de tentativas
      Serial.print("Tentativas de reenvio: ");
      Serial.println(Register);
    /***********************************************************************/
    //ACK
    for(uint8_t i=0; i<100; i++){//esperar 1 segundo pela resposta
      if(Mirf.dataReady()){
        Mirf.getData(data); 
        break; 
      }
      delay(10);
    }
    if(data[0] == ACK){//mensagem recebida com sucesso
      Serial.println("ACK recebido");
      return 0;
    }
    //NACK
    else{
      Serial.println("NACK recebido ou nada:");
      err++;
    }
    if(err >= TENTATIVAS)return 1;
  }
}
/**********************************************************************************************************************************************/
uint8_t Radio::readPacket(uint8_t *msg){
  uint8_t data[32];
  uint8_t length;
  uint8_t fnc;
  uint8_t *remoteAddress = NULL;

  Mirf.getData(data);
  if(data[0]==1){
    //Serial.println("Inicio do pacote ok");
    length = data[6];
    fnc = data[7];
    //pegue o endereço do "remetente"
    remoteAddress = &data[1];
    //pegue a mensagem
      for(uint8_t i=0;i<25;i++){
        *(msg+i) = *(data+i+7);
        }  
      uint8_t aux[32];
      aux[0]=ACK; 
      uint8_t Register = 0;
      //Mirf.readRegister(i, Register, sizeof(Register));
      //resposta
      Mirf.setTADDR((byte *)remoteAddress);
      Mirf.send((uint8_t*)aux);
      while(Mirf.isSending());
      
      Mirf.readRegister(OBSERVE_TX, &Register, sizeof(Register));//pegar o numero de tentativas
      Serial.print("Tentativas de reenvio: ");
      Serial.println(Register);
      return 1;
  }
  else return 0;
}
/**********************************************************************************************************************************************/
/**********************************************************************************************************************************************/
/**********************************************************************************************************************************************/
