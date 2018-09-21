int Buttons() {
  if (digitalRead(bSelect) == LOW) {
    return 2;
  } else if (digitalRead(bUp) == LOW) {
    return 1;
  } else if (digitalRead(bDown) == LOW) {
    return -1;
  }
  return 0;
}
//Menu
void menuReset() {
  menuStatus[0] = 0;
  menuStatus[1] = 0;
  menuStatus[2] = 0;
}
void createMenuArray(char tmpMenu[][15], char* menu, int lenght){
  int i;
  int j;
  int k;
  int to;
  for(i=0;i<8;i++){
    //k=i * lenght;
    //strncpy(tmpMenu[0],*(menu+k),lenght); 
    to=max(lenght,15);
    for(j=0;j<to;j++){
      k=j + i * lenght;
      tmpMenu[i][j]=menu[k];
      //Serial.print(menu[k]);
    }
  }
}
void drawMenu(char menuArray[][15], int x=0){
  u8x8.setCursor(x,0);
  if(4>x){
    u8x8.print("Back");
  }
  int i;
  for(i=0;i<8;i++){
    if('\0'==menuArray[i][0]){
      break;
    }
    u8x8.setCursor(x,i+1);
    u8x8.print(menuArray[i]);
  }
  if(0>menuStatus[menuStatus[0]]){menuStatus[menuStatus[0]]=0;}
  if(i<menuStatus[menuStatus[0]]){menuStatus[menuStatus[0]]=i;}
  u8x8.setCursor(0,menuStatus[menuStatus[0]]);
  u8x8.print(curs);
  #ifdef debug
    int j;
    u8x8.setCursor(12,0);
    for(j=0;j<4;j++){
      u8x8.print(menuStatus[j]);
    }
  #endif
  return i;
}
void dispUpdate() {
  int button;
  float w;
  int w_int;
  char tmpMenu[8][15];
  button = Buttons();
  if (0 != button) {
    u8x8.clear();
    if(button == 2){//Select
      if(0 == menuStatus[menuStatus[0]] && 0 != menuStatus[0]){
        menuStatus[0]--;
      }else{
        menuStatus[0]++;
      }
    }else{//Menu and Value Up/Down
      menuStatus[menuStatus[0]] += -button;
    }
    if(0>menuStatus[0]){menuStatus[0]=0;}
  }
  switch(menuStatus[0]){
    case 0://HOME
      u8x8.setCursor(0, 0);
      strcpy(tmpOpt, "TYPE");
      optRead(tmpOpt,tmpVal);
      u8x8.draw2x2String(0,0,tmpVal);
      u8x8.draw2x2String(2*strlen(tmpVal),0,":");
      //Temp
      u8x8.setCursor(0, 3);
      u8x8.print("T: ");
      strcpy(tmpOpt, "HEND");
      optRead(tmpOpt,tmpVal);
      u8x8.print(tmpVal);
      u8x8.print("/");
      strcpy(tmpOpt, "HBED");
      optRead(tmpOpt,tmpVal);
      u8x8.print(tmpVal);
      //Weight
      u8x8.setCursor(0, 5);
      u8x8.print("W: ");
      w = LoadCell.getData();
      w_int=(int)w;
        strcpy(tmpOpt, "WGHT");
        optRead(tmpOpt,tmpVal);
        if(0!=strcmp(defValue,tmpVal)){
          w_int=(int)w_int-atoi(tmpVal);
        }
      u8x8.print(w_int,DEC);
      u8x8.print("g  ");
      break;
    case 1://Menu
      drawMenu(menu,2);
      break;
    case 2:
      switch(menuStatus[1]){
        case 1:
          createMenuArray(tmpMenu,&opts[0][0],5);
          drawMenu(tmpMenu,2);
          
          createMenuArray(tmpMenu,&optsValue[0][0],5);
          drawMenu(tmpMenu,7);
          //tmpVal=optsValue[menuStatus[2]-1];
          if(0<=menuStatus[2]-1){
            strncpy(tmpVal, optsValue[menuStatus[2]-1], 4);
            if(strcmp(tmpVal,defValue)==0){
              strcpy(tmpVal,"   0");
            }
          }
          break;
        case 2:
          readNFC();
          break;
        case 3:
          updateNFC();
          break;
        case 4:
          createMenuArray(tmpMenu,&filaments[0][0],5);
          drawMenu(tmpMenu,2);
          break;
        case 5:
          u8x8.setCursor(0,0);
          u8x8.print("Tare");
          LoadCell.tareNoDelay();
          delay(500);
          menuReset();
          break;
        case 6:
          menuStatus[0]++;
          break;
        default:
        u8x8.setCursor(0,0);
        u8x8.print("No function");
        delay(500);
        menuReset();
      }
      break;
    case 3:
      switch (menuStatus[1]){
        case 1:
            switch(menuStatus[2]){
              case 2:
                createMenuArray(tmpMenu,&filaments[0][0],5);
                drawMenu(tmpMenu,2);
                break;
              default:
              drawMenu(settings,2);
              u8x8.setCursor(6,7);
              u8x8.print(" to local");
              u8x8.setCursor(9,2);
              u8x8.print(opts[menuStatus[2]-1]);
              u8x8.setCursor(9,4);
              //u8x8.print(optsValue[menuStatus[2]-1]);
              u8x8.print(tmpVal);
            }
          break;
        case 4:
          //Load data to NFC
          initNFC();
          break;
        case 6:
          //Scale calibrate
          u8x8.setCursor(0,0);
          u8x8.print("No function");
          delay(500);
          menuReset();
          break;
      }
      break;
    case 4:
        switch(menuStatus[1]){
          case 1:
            switch(menuStatus[2]){
              case 2:
                strcpy(tmpVal,filaments[menuStatus[3]-1]);
                optSave(opts[menuStatus[2]-1],tmpVal);
                menuStatus[0]--;
                break;
              default:
                if(4>menuStatus[3]){
                  //+1,10,100
                  if(32==tmpVal[menuStatus[3]]){//32->Ascii " "
                    tmpVal[menuStatus[3]]=48;//48->Ascii 0
                  }
                  tmpVal[menuStatus[3]]++;
                  if(57<tmpVal[menuStatus[3]]){//57->Ascii 9
                    //overflow
                    tmpVal[menuStatus[3]]=48;//48->Ascii 0
                    menuStatus[0]=5;
                    menuStatus[3]--;
                  }
                }else if(7>menuStatus[3]){
                  //-1,10,100
                  tmpVal[7-menuStatus[3]]--;
                  if(48>tmpVal[7-menuStatus[3]]){//48->Ascii 0
                    //Overflow
                    tmpVal[7-menuStatus[3]]=57;//57->Ascii 9
                    menuStatus[0]=5;
                    menuStatus[3]++;
                  }
                }else{
                  if(0==button){//-1000
                    tmpVal[7-menuStatus[3]]--;
                    if(48>tmpVal[7-menuStatus[3]]){//48->Ascii 0
                      tmpVal[7-menuStatus[3]]=48;
                    }
                  }else{
                    u8x8.print("Saved");
                    optSave(opts[menuStatus[2]-1],tmpVal);
                    menuStatus[0]=3;
                    menuStatus[3]=0;
                    delay(500);
                  }
                }
            }
            menuStatus[0]--;
            break;
        }
        
      break;
    default:
      menuReset();
  }
}
