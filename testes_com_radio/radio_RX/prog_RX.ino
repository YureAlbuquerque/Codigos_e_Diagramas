Radio radio(8,7,10,"radRX");
void setup(){
  Serial.begin(9600);
}
void loop(){
  //verifica o radio
  if(radio.available()){
    //radio.readAddress();
    Serial.println("Ha algo a ser lido");
    //Serial.println(radio.readPacket());
    radio.readPacket();
    Serial.print("funcao: ");
    Serial.println(radio.fnc);
    Serial.print("dados: ");
    Serial.println((char*)radio.data);
  }
  //Serial.println("-");
  //radio.readAddress();
  //delay(10000);
  //radio.writePacket((uint8_t*)"radTX",11,cmds,sizeof(cmds));
}

