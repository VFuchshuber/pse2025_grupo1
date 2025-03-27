// LCD module connections
sbit LCD_RS at RB1_bit;
sbit LCD_EN at RB0_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;
sbit LCD_RS_Direction at TRISB1_bit;
sbit LCD_EN_Direction at TRISB0_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;

//Buttons connections
sbit BUTTON_CONFIRM at RB2_bit;
sbit BUTTON_ROLL at RB3_bit;

//LEDs Connections
sbit BEV1 at RA0_bit;
sbit BEV2 at RA1_bit;
sbit BEV3 at RA2_bit;
sbit BEV4 at RA3_bit;

//variaveis globais
char confirmVal = 0;
char rollVal = 0;
char confirm_old = 0;
char roll_old = 0;

char bevMenu [4][8] = {"Caipi", "Wine", "Beer", "Whisky"};
char sizeOptions[3][8] = {"Small", "Medium", "Large"};

char buttonPressed(char val, char old_val){
     if(!val && old_val){
             delay_ms(50);
             return 1;
      }
     else return 0;
}

void greetUser(){
     Lcd_Cmd(_LCD_CLEAR);
     Lcd_Out(1,1,"Welcome!");//Write text 'Welcome!' in first row
     Lcd_Out(2,1,"Press any button");//Write text in second row
     delay_ms(500);

     while(1){
              confirm_old = confirmVal;
              roll_old = rollVal;

              confirmVal = BUTTON_CONFIRM;
              rollVal = BUTTON_ROLL;

              //check if button is pressed to continue
              if(buttonPressed(confirmVal, confirm_old) | buttonPressed(rollVal, roll_old)) break;
     }
     delay_ms(100);
}

char selectBeverage(){
    char bevCode = 0;  //number to identify beverage
    char prev_bevCode = 3; //memory of the previous value of bevCode, initialized with value != from bevCode
    Lcd_Cmd(_LCD_CLEAR);

    delay_ms(250);
    while(1){
             confirm_old = confirmVal;
             roll_old = rollVal;

             confirmVal = BUTTON_CONFIRM;
             rollVal = BUTTON_ROLL;

             //Change LCD beverage name
             if(bevCode != prev_bevCode){
                Lcd_Cmd(_LCD_CLEAR);
                Lcd_Out(1,1,"Select beverage");

                Lcd_Out(2,1,bevMenu[bevCode]);
                prev_bevCode = bevCode;
             }

             if(buttonPressed(confirmVal, confirm_old)) {
             delay_ms(100);
             return bevCode;
             }

             if(buttonPressed(rollVal, roll_old)){
                bevCode ++;           //incrementa para selecionar a pr xima bebida
                bevCode = bevCode%4;  //utiliza valor mod4
                delay_ms(100);
             }
    }
}

char selectSize(){
     char size = 0;
     char sizeOld = 2;
     Lcd_Cmd(_LCD_CLEAR);
     delay_ms(500);
     while(1){
              confirm_old = confirmVal;
              roll_old = rollVal;

              confirmVal = BUTTON_CONFIRM;
              rollVal = BUTTON_ROLL;

              //Change LCD beverage name
              if(size != sizeOld){
                  delay_ms(250);
                  Lcd_Cmd(_LCD_CLEAR);
                  Lcd_Out(1,1,"Select size");

                  Lcd_Out(2,1,sizeOptions[size]);

                  sizeOld = size;
               }

             if(buttonPressed(confirmVal, confirm_old)){
              delay_ms(100);
              return size;

             }
             if(buttonPressed(rollVal, roll_old)){
                size ++;    //incrementa para selecionar a pr xima bebida
                size = size%3;  //utiliza valor mod3
                delay_ms(100);
             }
    }
}

void pourBeverage(char bev, char size){
     int time = 2*size+3;
     int i;

     Lcd_Cmd(_LCD_CLEAR);
     delay_ms(100);
     Lcd_Out(1,1,"Please Wait");
     Lcd_Out(2,1,sizeOptions[size]);
     Lcd_Out(2,8,bevMEnu[bev]);


     switch(bev){
        case 0:
             BEV1 = 1;
             for(i = time; i>0; i--) delay_ms(1000);
             BEV1 = 0;
             break;
        case 1:
             BEV2 = 1;
             for(i = time; i>0; i--) delay_ms(1000);
             BEV2 = 0;
             break;
        case 2:
             BEV3 = 1;
             for(i = time; i>0; i--) delay_ms(1000);
             BEV3 = 0;
             break;
        case 3:
             BEV4 = 1;
             for(i = time; i>0; i--) delay_ms(1000);
             BEV4 = 0;
             break;
     }


     Lcd_Cmd(_LCD_CLEAR);
     Lcd_Out(1,1,"Enjoy your");
     Lcd_Out(2,1,"beverage!");
     delay_ms(2000);
}

void main() {
     char size;
     char bev;

     //Define pin input directions
     TRISB4_bit = 1;
     TRISB5_bit = 1;

     //Define pin output directions
     TRISA = 0;

     Lcd_Init();          //Initialize LCD
     Lcd_Cmd(_LCD_CLEAR); // Clear display
     Lcd_Cmd(_LCD_CURSOR_OFF); // Cursor off

     while(1){
       greetUser();
       bev = selectBeverage();
       size = selectSize();
       pourBeverage(bev, size);
     }
}