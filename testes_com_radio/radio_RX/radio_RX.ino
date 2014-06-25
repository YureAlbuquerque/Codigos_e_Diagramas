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
  uint8_t fnc;
  uint8_t data[32];
  Radio(uint8_t cePIN,uint8_t csnPIN,uint8_t CH,char *address);
  boolean available();
  uint8_t writePacket(uint8_t *remoteAddress,uint8_t fnc,uint8_t *array,uint8_t len);
  uint8_t writePacket(uint8_t *remoteAddress,uint8_t fnc);
  uint8_t readPacket();

};
/**********************************************************************************************************************************************/
Radio::Radio(uint8_t cePIN,uint8_t csnPIN,uint8_t CH,char Myaddress[]){
  //Serial.print(Myaddress);
  for(uint8_t i=0;i<5;i++){
    myAddress[i] = Myaddress[i];   
  }
  //Serial.println((char*)myAddress);
  Mirf.cePin = cePIN;
  Mirf.csnPin = csnPIN;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();   
  Mirf.setRADDR((uint8_t *)Myaddress);   
  Mirf.payload = 32;
  Mirf.channel = CH;   
  Mirf.config();
  Mirf.configRegister(SETUP_RETR,0x2f);//habilita a possibilidade te 15 tentativas de envio em caso de falha
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
  uint8_t msg[32];
  while(1){
    /***************************/
    //limpa mensagem
    for(uint8_t i=0;i<32;i++){
      msg[i] = 0;
    }   
    /***************************/
    //byte de inicio
    msg[0] = 2;
    //endereço
    for(uint8_t i=0;i<5;i++){
      msg[i+1] = myAddress[i];
    }
    //função
    msg[6] = fnc;

    //acressente o array a mensagem 
    for(uint8_t i=0; i<len; i++){
      msg[i+7] = *(array+i);
    }

    Mirf.send((uint8_t*)msg);//Envia a mensagem.
    while(Mirf.isSending());//loop enquanto não terminar a transmissão.

    /********************************************************************************************************/
    //testes
    uint8_t Register = 0;
    Mirf.readRegister(OBSERVE_TX, &Register, sizeof(Register));//pegar o numero de tentativas
    Serial.print("Tentativas de reenvio: ");
    Serial.println(Register);
    /********************************************************************************************************/

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
      return 1;
    }
    //NACK
    else{
      Serial.println("NACK recebido ou nada:");
      err++;
    }
    if(err >= TENTATIVAS)return 0;
  }

}
/**********************************************************************************************************************************************/
uint8_t Radio::writePacket(uint8_t *remoteAddress,uint8_t fnc){//mensagem com no maximo 25 caracteres
  Mirf.setTADDR((byte *)remoteAddress);
  uint8_t err = 0;
  uint8_t msg[32];
  while(1){
    /***************************/
    //limpa mensagem
    for(uint8_t i=0;i<32;i++){
      msg[i] = 0;
    }   
    /***************************/
    //byte de inicio
    msg[0] = 2;
    //endereço
    for(uint8_t i=0;i<5;i++){
      msg[i+1] = myAddress[i];
    }
    //função
    msg[6] = fnc;

    Mirf.send((uint8_t*)msg);//Envia a mensagem.
    while(Mirf.isSending());//loop enquanto não terminar a transmissão.

    /********************************************************************************************************/
    //testes
    uint8_t Register = 0;
    Mirf.readRegister(OBSERVE_TX, &Register, sizeof(Register));//pegar o numero de tentativas
    Serial.print("Tentativas de reenvio: ");
    Serial.println(Register);
    /********************************************************************************************************/

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
uint8_t Radio::readPacket(){
  uint8_t msg[32];
  uint8_t *remoteAddress = NULL;

  Mirf.getData(msg);
  if(msg[0]==2){
    //Serial.println("Inicio do pacote ok");
    fnc = msg[6];
    //pegue o endereço do "remetente"
    remoteAddress = &msg[1];
    //pegue a mensagem
    for(uint8_t i=0;i<25;i++){
      *(data+i) = *(msg+i+7);
    }  
    uint8_t aux[32];
    aux[0]=ACK; 
    //resposta
    Mirf.setTADDR((byte *)remoteAddress);
    Mirf.send((uint8_t*)aux);
    while(Mirf.isSending());
    
    uint8_t Register = 0;
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


