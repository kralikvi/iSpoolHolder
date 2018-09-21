void readNFC(){
  int i;

  u8x8.clear();
  
  u8x8.print(F("Search"));
  for(i=0;i<8;i++){
    u8x8.print('.');
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);
    if(success){break;}
  }
  u8x8.println();
  if(!success){
    u8x8.print(F("No card found."));
    delay(1000);
    menuReset();
    return false;
  }
  optReset();

  //Need to test its a NDEF card
  /*if (!((data[0] == 0xE1) && (data[1] == 0x10))){
    u8x8.println(F("This not a NDEF formatted tag."));
    return false;
  }*/
  //dataLength = data[2]>>3;
  //Need to read the lenght of payload

  
  for(i=7;i<20;i++){
    success = nfc.ntag2xx_ReadPage(i, data);
    i++;//need a second increment
    if(!success){u8x8.println(F("Read Error")); continue;}
    if(0xFE==data[0]){break;};
    strncpy(tmpOpt,(char*) &data,4);
    u8x8.print(tmpOpt);
    u8x8.print(": ");
    success = nfc.ntag2xx_ReadPage(i, data);
    if(!success){u8x8.println(F("Read Error")); continue;}
    strncpy(tmpVal,(char*) &data,4);
    u8x8.print(tmpVal);
    optSave(tmpOpt,tmpVal);
    u8x8.println();
  }
  delay(1000);
  menuReset();
  u8x8.clear();
}
void initNFC(){
  //Write the preconfig texts
  int i;

  u8x8.clear();
  
  u8x8.print(F("Search"));
  for(i=0;i<8;i++){
    u8x8.print('.');
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);
    if(success){break;}
  }
  u8x8.println();
  if(!success){
    u8x8.print(F("No card found."));
    delay(1000);
    menuReset();
    return false;
  }
  memset(data, 0, 4);
  success = nfc.ntag2xx_ReadPage(3, data);
  if (!success){
    u8x8.println(F("Read Error"));
    return;
  }
  /*if (!((data[0] == 0xE1) && (data[1] == 0x10))){
    u8x8.println(F("This not a NDEF formatted tag."));
  }*/
  dataLength = data[2]*8;
  
  success = nfc.ntag2xx_WriteNDEFURI(NDEF_URIPREFIX_NONE, filamentsTXT[menuStatus[2]-1], dataLength);
  if (success)
  {
    u8x8.println(F("Formated!"));
  }
  else
  {
    u8x8.println(F("ERROR!"));
  }
  i =(int) LoadCell.getData();
  sprintf (tmpVal, "%04i", i);
  success=nfc.ntag2xx_WritePage(8,tmpVal);
  delay(1000);
  menuReset();
  readNFC();
}
void updateNFC(){
  //Save the current settings to nfc
  int i;
  u8x8.print(F("Search"));
  for(i=0;i<8;i++){
    u8x8.print('.');
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);
    if(success){break;}
  }
  u8x8.println();
  if(!success){
    u8x8.print(F("No card found."));
    delay(1000);
    menuReset();
    return false;
  }
  memset(data, 0, 4);
  success = nfc.ntag2xx_ReadPage(3, data);
  if (!success){
    u8x8.println(F("Read Error"));
    return false;
  }
  if (!((data[0] == 0xE1) && (data[1] == 0x10))){
    u8x8.println(F("This not a NDEF formatted tag."));
    return false;
  }


  //Lenght

  int j;
  for (i = 0; i < optsLenght; i++) {
    if('\0'==opts[i][0]){break;}
    
    strcpy(tmpOpt,opts[i]);
    optRead(tmpOpt,tmpVal);
    if(0==strcmp(tmpVal,defValue)){
      break;
    }
    
    for(j=7;j<20;j++){
      success = nfc.ntag2xx_ReadPage(j, data);
      j++;//need a second increment
      if(!success){u8x8.println(F("Read Error")); continue;}
      if(0xFE==data[0]){break;};
      if(0==strcmp(data,tmpOpt)){break;}
    }
    if(0xFE==data[0]){
      //New data set
      success = nfc.ntag2xx_WritePage(j-1, tmpOpt);
      success = nfc.ntag2xx_WritePage(j+1, data);
      success = nfc.ntag2xx_ReadPage(5, data);
      data[2]+=8;
      success = nfc.ntag2xx_WritePage(5, data);
      success = nfc.ntag2xx_ReadPage(6, data);
      data[1]+=8;
      success = nfc.ntag2xx_WritePage(6, data);
    }
    success = nfc.ntag2xx_WritePage(j, tmpVal);
  }
  delay(1000);
  menuReset();
  u8x8.clear();
}

