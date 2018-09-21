

//Options
void optSave(char* opt, char* value) {
  int i;
  int j;
  bool valid;
  valid=false;
  for(i = 0; i < optsLenght; i++){
    if('\0'==opts[i][0]){break;}
    if(strcmp(opts[i],opt)==0){
      strcpy(optsValue[i],value);
      //delete leading 0s for the first 3 char
      for(j=0;j<3;j++){
        if(optsValue[i][j]==' ' || optsValue[i][j]=='0'){
          optsValue[i][j]=' ';
        }else{
          break;
        }
      }
      #ifdef debug
        valid=true;
        Serial.print(opt);
        Serial.print("<-");
        Serial.println(value);
        //Serial.println(optsValue[i]);
      #endif
      break;
    }
  }
  #ifdef debug
    if(!valid){
      Serial.println("Not Found(Save)");
    }
  #endif
}
void optRead(char* opt, char* buff) {
  int i;
  //char* value;
  bool valid;
  valid=false;
  strcpy(buff, "NaN");
  for (i = 0; i < 8; i++) {
    if('\0'==opts[i][0]){break;}
    if(strcmp(opts[i],opt)==0){
      strcpy(buff, optsValue[i]);
      #ifdef debug
        valid=true;
        Serial.print(opt);
        Serial.print(": ");
        Serial.println(buff);
      #endif
      break;
    }
  }
  #ifdef debug
    if(!valid){
      Serial.println("Not Found(Read)");
    }
  #endif
}
void optReset() {
  int i;
  for (i = 0; i < optsLenght; i++) {
    if('\0'==opts[i][0]){break;}
    /*for(j = 0; j < 5; j++){
      optsValue[i][j]=defValue[j];
    }*/
    strcpy(optsValue[i],defValue);
  }
}
