Radio radio(8,7,10,"radTX");
char radio_dado[4];//dado recebido pelo radio.
void setup(){
  //delay(5000);
  Serial.begin(9600);
}
uint8_t cmds[10]={
  'F','R','R','B','L','F','R','R','B','F'}; 
void loop(){
  //verifica o radio
  //  if(radio.available()){
  //    radio.read(radio_dado);
  //  }
  if(Serial.available()){
    Serial.read();
    Serial.print("Tamanho: ");
    Serial.println(sizeof(cmds));
    radio.writePacket((uint8_t*)"radRX",11,cmds,sizeof(cmds));
    //radio.write((uint8_t*)"radRX",cmds,9);
    Serial.println("\t\t\tretorno!");
  }
}


