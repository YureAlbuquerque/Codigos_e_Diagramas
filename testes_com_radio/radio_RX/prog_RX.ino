Radio radio(8,7,10,"radRX");
uint8_t radio_dado[32];//32 dado recebido pelo radio.
void setup(){
  Serial.begin(9600);
}
/*uint8_t cmds[10]={
 'F','R','R','B','L','F','R','R','B','L'}; */
void loop(){
  //verifica o radio
  if(radio.available()){
    //radio.readAddress();
    Serial.println("Ha algo a ser lido");
    radio.readPacket(radio_dado);
    //radio.read(radio_dado);
    Serial.print("(Fora)Dados lidos: ");
    Serial.println((char*)radio_dado);
  }
  //Serial.println("-");
  //radio.readAddress();
  //delay(10000);
  //radio.writePacket((uint8_t*)"radTX",11,cmds,sizeof(cmds));
}

