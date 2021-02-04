// ARCHIVO PARA COMANDOS DE 8 BITS
//PUERTO D DIRECCIONAMIENTO  PC18F  PC16F
#BIT CMD0 = 0xF83.0 // 1     D0 IN  OUT D0
#BIT CMD1 = 0xF83.1 // 2     D1 IN  OUT D1
#BIT CMD2 = 0xF83.2 // 4     D2 IN  OUT D2
#BIT CMD3 = 0xF83.3 // 8     D3 IN  OUT D3
#BIT CMD4 = 0xF83.4 // 16    D4 IN  OUT D4
#BIT CMD5 = 0xF83.5 // 32    D5 IN  OUT D5
#BIT CMD6 = 0xF83.6 // 64    D6 IN  OUT D6
#BIT CMD7 = 0xF83.7 // 128   D7 IN  OUT D7    Hasta 255 direcciones diferentes
//PUERTO C DIRECCIONAMIENTO EXTENDIDO
#BIT CMD15 = 0xF82.4 // 256  C4 IN  OUT B4
#BIT CMD16 = 0xF82.5 // 512  C5 IN  OUT B5
//                            -------
//                              1023   Direcciones diferentes
#BYTE PORTD = 0xF83 //
//PUERTO E STOP PAUSE START (PAUSE & STOP SIRVEN COMO AUXILIARES PARA EL DIRECCIONAMIENTO)
//         PIN18F            FUNCION    PIN16F
#BIT CMD8 = 0xF84.0  //E0    // START    (E0)
#BIT CMD10 = 0xF84.2 //E2    // PAUSE    (E1)
#BIT CMD9 = 0xF84.1  //E1    // STOP     (E2)
//PUERTO B RPM
//bit 0 SERIAL DATA INPUT
//bit 1 CLK OUT
//         PIN18F            FUNCION    PIN16F
#BIT CMD11 = 0xF81.3 //B3  // 1  IN      (B3)
#BIT CMD12 = 0xF81.4 //B4  // 2  IN      (B2)
#BIT CMD13 = 0xF81.5 //B5  // 4  IN      (B1)
#BIT CMD14 = 0xF81.6 //B6  // 8  IN      (B0)

int CMD_RPM = 0;          // Para configurar el RPM de la sennal
unsigned int CMD_DIR = 0; // Para configurar direccion del archivo de la sennal

// LIMPIA LAS VARIABLES DE LA CLASE <<comandos.c>>
void clearVarsCmd()
{
   CMD_RPM = 0;
   CMD_DIR = 0;
}

//PERMITE GENERAR LOS VALORES ENTEROS DE RPM
int procesar_CMDRPM()
{
   // un solo '|' porque la operacion es a nivel bit, RPM
   CMD_RPM = CMD14;
   CMD_RPM = (CMD_RPM << 1) | (CMD13);
   CMD_RPM = (CMD_RPM << 1) | (CMD12);
   CMD_RPM = (CMD_RPM << 1) | (CMD11);
   return CMD_RPM;
}

//PERMITE GENERAR LOS VALORES ENTEROS DE DIRECCIONAMIENTO (de 0 a 1023)
void procesar_CMDDIR()
{
   CMD_DIR = CMD15;                    // si CMD15 es 0, CMD_DIR tiene 0
   CMD_DIR = (CMD_DIR << 1) | (CMD14); // si CMD_DIR con 0, se desplaza 1 a la izq, da 0 ...
   CMD_DIR = (CMD_DIR << 1) | (CMD7);
   CMD_DIR = (CMD_DIR << 1) | (CMD6);
   CMD_DIR = (CMD_DIR << 1) | (CMD5);
   CMD_DIR = (CMD_DIR << 1) | (CMD4);
   CMD_DIR = (CMD_DIR << 1) | (CMD3);
   CMD_DIR = (CMD_DIR << 1) | (CMD2);
   CMD_DIR = (CMD_DIR << 1) | (CMD1);
   CMD_DIR = (CMD_DIR << 1) | (CMD0);
   //Imprime la direccion procesada
   sprintf(Direccion, "/%d.txt", CMD_DIR);
   pasado = CMD_DIR;
   CMD_DIR = 0;
   strcpy(Dx, "");
   //Imprime la direccion procesada desde PORTD
   //printf("Puerto Direccion: %u \r\n",PORTD);
}