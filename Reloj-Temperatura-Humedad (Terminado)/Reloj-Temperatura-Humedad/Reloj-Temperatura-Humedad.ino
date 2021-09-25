#include <Wire.h>
#include "ds3231.h" //Libreria del DS3231
#include <LiquidCrystal.h> //Libreria del LCD

#include "DHT.h" //cargamos la librería DHT
#define DHTPIN 23 //Seleccionamos el pin en el que se //conectará el sensor
#define DHTTYPE DHT11 //Se selecciona el DHT11 (hay //otros DHT)
DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que será usada por Arduino para comunicarse con el sensor

LiquidCrystal lcd(8,9,4,5,6,7); //Definimos los pines del LCD

#define BUFF_MAX 128 

uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 1000;

void setup()
{
    Serial.begin(9600); //Se incicia la comunicacion con el puerto serial
    Wire.begin();

    dht.begin(); //Se inicia el sensor de temperatura
    
    DS3231_init(DS3231_INTCN);
    memset(recv, 0, BUFF_MAX);
    Serial.println("GET time");
    lcd.begin(16, 2); //Definimos las Dimenciones del LCD
    lcd.clear(); //Borramos pantalla 
    
    //Serial.println("Setting time");  //Para establecer la hroa final
    //parse_cmd("T000306630042016",16);
}

void loop()
{
    float humedadTH = dht.readHumidity(); //Se lee la humedad
    float temperaturaTH = dht.readTemperature(); //Se lee la temperatura

    
    char in;
    char tempF[6]; 
    float temperature;
    char buff[BUFF_MAX];
    unsigned long now = millis();
    struct ts t;

    // show time once in a while
    if ((now - prev > interval) && (Serial.available() <= 0)) {
        DS3231_get(&t); //Get time
        parse_cmd("C",1);
        temperature = DS3231_get_treg(); // Optenemos las tempretuta del DS3231
        dtostrf(temperature, 5, 1, tempF);

        lcd.clear();
        lcd.setCursor(1,0);
        
        lcd.print(t.mday);
        
        printMonth(t.mon);
        
        lcd.print(t.year);
        
        lcd.setCursor(0,1);
        lcd.print("Hora: "); 
        lcd.print(t.hour);
        lcd.print(":");
        if(t.min<10)
        {
          lcd.print("0");
        }
        lcd.print(t.min);
        lcd.print(":");
        if(t.sec<10)
        {
          lcd.print("0");
        }
        lcd.print(t.sec);
        
        prev = now;
    }

    
    if (Serial.available() > 0) {
        in = Serial.read();

        if ((in == 10 || in == 13) && (recv_size > 0)) {
            parse_cmd(recv, recv_size);
            recv_size = 0;
            recv[0] = 0;
        } else if (in < 48 || in > 122) {;       // ignore ~[0-9A-Za-z]
        } else if (recv_size > BUFF_MAX - 2) {   // drop lines that are too long
            // drop
            recv_size = 0;
            recv[0] = 0;
        } else if (recv_size < BUFF_MAX - 2) {
            recv[recv_size] = in;
            recv[recv_size + 1] = 0;
            recv_size += 1;
        }

    }
        delay(2000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Temp DS:");
        lcd.print(temperature);
        lcd.print((char)223);
        lcd.print("C");

        lcd.setCursor(0,1);
        lcd.print("Temp TH: ");
        lcd.print(int(temperaturaTH));
        lcd.print((char)223);
        lcd.print("C");
         
        delay(2000);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("T.Prom: ");
        lcd.print(((temperaturaTH)+(temperature))/2);
        lcd.print((char)223);
        lcd.print("C");

        lcd.setCursor(0,1);
        lcd.print("Humedad: ");
        lcd.print(int(humedadTH));

        delay(2000);
      
        
        

   
}

void parse_cmd(char *cmd, int cmdsize)
{
    uint8_t i;
    uint8_t reg_val;
    char buff[BUFF_MAX];
    struct ts t;

    //snprintf(buff, BUFF_MAX, "cmd was '%s' %d\n", cmd, cmdsize);
    //Serial.print(buff);

    // TssmmhhWDDMMYYYY aka set time
    if (cmd[0] == 84 && cmdsize == 16) {
        //T355720619112011
        t.sec = inp2toi(cmd, 1);
        t.min = inp2toi(cmd, 3);
        t.hour = inp2toi(cmd, 5);
        t.wday = inp2toi(cmd, 7);
        t.mday = inp2toi(cmd, 8);
        t.mon = inp2toi(cmd, 10);
        t.year = inp2toi(cmd, 12) * 100 + inp2toi(cmd, 14);
        DS3231_set(t);
        Serial.println("OK");
    } else if (cmd[0] == 49 && cmdsize == 1) {  // "1" get alarm 1
        DS3231_get_a1(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 50 && cmdsize == 1) {  // "2" get alarm 1
        DS3231_get_a2(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 51 && cmdsize == 1) {  // "3" get aging register
        Serial.print("aging reg is ");
        Serial.println(DS3231_get_aging(), DEC);
    } else if (cmd[0] == 65 && cmdsize == 9) {  // "A" set alarm 1
        DS3231_set_creg(DS3231_INTCN | DS3231_A1IE);
        //ASSMMHHDD
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // ss, mm, hh, dd
        }
        byte flags[5] = { 0, 0, 0, 0, 0 };
        DS3231_set_a1(time[0], time[1], time[2], time[3], flags);
        DS3231_get_a1(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 66 && cmdsize == 7) {  // "B" Set Alarm 2
        DS3231_set_creg(DS3231_INTCN | DS3231_A2IE);
        //BMMHHDD
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // mm, hh, dd
        }
        byte flags[5] = { 0, 0, 0, 0 };
        DS3231_set_a2(time[0], time[1], time[2], flags);
        DS3231_get_a2(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 67 && cmdsize == 1) {  // "C" - get temperature register
        Serial.print("temperature reg is ");
        Serial.println(DS3231_get_treg(), DEC);
    } else if (cmd[0] == 68 && cmdsize == 1) {  // "D" - reset status register alarm flags
        reg_val = DS3231_get_sreg();
        reg_val &= B11111100;
        DS3231_set_sreg(reg_val);
    } else if (cmd[0] == 70 && cmdsize == 1) {  // "F" - custom fct
        reg_val = DS3231_get_addr(0x5);
        Serial.print("orig ");
        Serial.print(reg_val,DEC);
        Serial.print("month is ");
        Serial.println(bcdtodec(reg_val & 0x1F),DEC);
    } else if (cmd[0] == 71 && cmdsize == 1) {  // "G" - set aging status register
        DS3231_set_aging(0);
    } else if (cmd[0] == 83 && cmdsize == 1) {  // "S" - get status register
        Serial.print("status reg is ");
        Serial.println(DS3231_get_sreg(), DEC);
    } else {
        Serial.print("unknown command prefix ");
        Serial.println(cmd[0]);
        Serial.println(cmd[0], DEC);
    }
}

void printMonth(int month)
{
  switch(month)
  {
    case 1: lcd.print(" Enero ");break;
    case 2: lcd.print(" Febrero ");break;
    case 3: lcd.print(" Marzo ");break;
    case 4: lcd.print(" Abril ");break;
    case 5: lcd.print(" Mayo ");break;
    case 6: lcd.print(" Junio ");break;
    case 7: lcd.print(" Julio ");break;
    case 8: lcd.print(" Agosto ");break;
    case 9: lcd.print(" Septiembre ");break;
    case 10: lcd.print(" Octubre ");break;
    case 11: lcd.print(" Noviembre ");break;
    case 12: lcd.print(" Diciembre ");break;
    default: lcd.print(" Error ");break;
  } 
}
