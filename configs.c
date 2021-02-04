// ARCHIVO PARA CONFIGURAR LOS PUERTOS Y COMPORTAMIENTOS DEL SISTEMA
// #BYTE's de los registros con su direccion del banco de memoria
// PUERTOS
#BYTE TRISA = 0xF92
#BYTE LATA = 0xF89
#BYTE TRISB = 0xF93
#BYTE LATB = 0xF8A
#BYTE TRISC = 0xF94
#BYTE LATC = 0xF8B
#BYTE TRISD = 0xF95
#BYTE LATD = 0xF8C
#BYTE TRISE = 0xF96
#BYTE LATE = 0xF8D
// TMR2
#BYTE T2CON = 0xFCA
#BYTE PR2 = 0xFCB
#BYTE TMR2 = 0xFCC
// INTERRUPCIONES
#BYTE INTCON = 0xFF2
#BYTE PIE1 = 0xF9D
#BYTE PIR1 = 0xF9E
#BIT TMR2IE = 0xF9D.1
#BIT TMR2IF = 0xF9E.1
// SENNALES
#BIT sCMP = 0xF89.0
#BIT sCKP = 0xF8A.2
#BIT sCMP2 = 0xF89.1
//                  PIN18F        FUNCION    PIN16F
#BIT doceVolts = 0xF89.2 //A2 // SE CAMBIO A 0xF89.2 con nombre de S12V[Simulador 1.1 SCH.pdf]
#BIT ON = 0xF89.3        //A3       // sennal      (B6)
#BIT SD = 0xF89.4        //A4       // medios      (B7)

// LIMPIA LAS VARIABLES DE LA CLASE <<configs.c>>
void clearCmdConfigs()
{
     sCKP = 0;
     sCMP = 0;
     sCMP2 = 0;
     ON = 0;
     SD = 0;
     doceVolts = 0;
}

// INICIA PUERTOS {A,B,C,D,E}
void initPorts()
{
     //             DESC(PIN):[IN:1]/[OUT:0]
     LATA = 0x00;  // OFF
     TRISA = 0x00; // SENNAL CMP(0):0, CMP2(1):0, TTL(2):0, ON(3):0-16F(B6):1, SD(4):0-16F(B7):1
     LATB = 0x00;  // OFF
     ////COMANDOS 01111001
     TRISB = 0x79; // SPI: SDI(0):1,SCK(1):0 y SENAL CKP(2):0; B3(3):1; B2(4):1; B1(5):1; B0(6):1; -(7):0;
     LATC = 0x00;  // OFF
     TRISC = 0x32; // CS(0):0, rcv(1):1<=TX2, tx(2):0<=RX2, usb(3):0, CMD15(4):1, CMD16(5):1, C6:0, MOSI(7):0
     LATD = 0x00;  // OFF
     TRISD = 0xFF; // ENTRADA: COMANDOS DE DIRECCION
     LATE = 0x00;  // OFF
     TRISE = 0xFF; // ENTRADA: COMANDOS DE REPRODUCCION Y CARGA DE SENNAL
} //End initPorts()

// ARRANCA EL TMR2
void runTMR2()
{
     TMR2IF = 0; // Se limpia la bandera de interrupcion de desb. TMR2
     TMR2IE = 1; // Se habilitan interrupciones de TMR2
} //End runTMR2

// DETIENE EL TMR2
void stopTMR2()
{
     T2CON = 0x00; // Se apaga el TMR2
     TMR2IE = 0;   // Desabililita interrupciones del TMR2
     TMR2IF = 0;   // No se generan interrupciones de desbordamiento del TMR2
     PR2 = 0x77;   // Se reconfigura PR2 antes de modificar los RPM
     TMR2 = 0x00;  // Lo mismo con TMR2, ya que estos dos resetean el pre y el postcalador
} //End stopTMR2()

// TMR2 COMO TEMPORIZADOR
void ConfigTMR2()
{
     TMR2 = 0x00; // Valor inicial del registro del temporizador TMR2
     PR2 = 0x77;
     T2CON = 0x04; // Configurar el temporizador TMR2
     TMR2IF = 0;   // Pone la bandera del tmr2 como no desbordada
     TMR2IE = 0;   // Habilita interrupcion del TMR1
} //End ConfigTMR2()

// Habililita interrupciones
void AbleGLOBAL()
{
     INTCON = 0xC0; // Habililita interrupciones globales y las interrupciones perifericas
} //End AbleGLOBAL()

// CONFIGURA LOS REGISTROS PARA GENERAR LAS RPM CORRESPONDIENTES
void rpmMOD(int rpm)
{
     switch (rpm)
     {
     case 0:            // POST: 1 PRES: 1  Q: 119
          T2CON = 0x04; // Encender el TMR2 y configurar postcaler a 1.
          PR2 = 0x77;
          break;
     case 1: // POST: 1 PRES: 1  Q: 179
          T2CON = 0x04;
          PR2 = 0xB3;
          break;
     case 2: // POST: 2 PRES: 1  Q: 119
          T2CON = 0x0C;
          PR2 = 0x77;
          break;
     case 3: // POST: 3 PRES: 1  Q: 119
          T2CON = 0x14;
          PR2 = 0x77;
          break;
     case 4: // POST: 4 PRES: 1  Q: 119
          T2CON = 0x1C;
          PR2 = 0x77;
          break;
     case 5: // POST: 6 PRES: 1  Q: 119
          T2CON = 0x2C;
          PR2 = 0x77;
          break;
     case 6: // POST: 8 PRES: 1  Q: 119
          T2CON = 0x3C;
          PR2 = 0x77;
          break;
     case 7: // POST: 12 PRES: 1  Q: 119
          T2CON = 0x5C;
          PR2 = 0x77;
          break;
     case 8: // POST: 8  PRES: 1  Q: 239
          T2CON = 0x3C;
          PR2 = 0xEF;
          break;
     case 9: // POST: 2 PRES: 16  Q: 119
          T2CON = 0x07;
          PR2 = 0xEF;
          break;
     case 10: // POST: 12 PRES: 4  Q: 119
          T2CON = 0x5D;
          PR2 = 0x77;
          break;
     case 11: // POST: 4 PRES: 16  Q: 119
          T2CON = 0x1F;
          PR2 = 0x77;
          break;
     case 12: // POST: 4 PRES: 16  Q: 179
          T2CON = 0x1F;
          PR2 = 0xB3;
          break;
     case 13: // POST: 4 PRES: 16  Q: 239
          T2CON = 0x1F;
          PR2 = 0xEF;
          break;
     default:
          T2CON = 0x00;
          PR2 = 0x77;
          //Imprime el error y apaga TMR
          //printf("!Error! %d",rpm);
          break;
     }
     //Imprime carga RPM
     //printf("rpm %u\r\n",rpm);
     TMR2IF = 0; // Pone la bandera del tmr2 como no desbordada
     TMR2IE = 0; // Habilita interrupcion del TMR2
}
