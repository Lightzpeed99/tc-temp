// ARCHIVO PRINCIPAL
// Modulo de conexiones de tarjeta SD
#define MMCSD_SPI_HW            // Usar hardware del modulo SPI
#define MMCSD_PIN_SELECT PIN_C0 // El Chip Selector en el pin C0

// Archivos hexadecimales
#include <18F4550.h>
#include <stdint.h>
#device PASS_STRINGS = IN_RAM

#fuses NOMCLR HSPLL PLL4 CPUDIV1 NOBROWNOUT           // Para generar 48MHZ a partir de 16 MHz
#use delay(clock = 48MHz)                             // Frecuencia final de reloj
#use rs232(baud = 19200, xmit = PIN_C2, rcv = PIN_C1) // Inicializa comunicacion UART, para Debugguear

// Variables GearEngine
long int doublemsCMP; // Engranajes de tiempo CMP
int1 keyChange;       // Permite saber si ocurrio un cambio en los comandos recibidos
//Arreglos de la sennales
unsigned long int CMP_codec[400]; // Para procesar los datos extraidos de la SD
int1 stateCMP[400];               // Permite saber a quien le corresponde el pulso (bi-estable): se necesitan al menos 4 estados
int1 stateCKP[400];
int1 modeIndex; // Permite seleccionar el modo de indexacion {lineal, no lineal}
//Inicializacion de las sennales
int1 sckp;
int1 scmp1;
int1 scmp2;
//Ajuste de RPM
long int rpmMax; //V{0-7000} - rpmMin
const long int rpmMin = 1000;
int nfrag;                   //V{0...16} - plus
int plus;                    //V{0...16} - nfrag
int longIDX;                 //V{0...9} - idx
unsigned int noSennales;     //V{0-255}  // Para indexar nuevos archivos
unsigned char Direccion[11]; // Para concatenar CMD_DIR con el nombre del archivo solicitado

// Archivos del driver
#include <mmcsd_m.c> // Driver de la MMC-SD
#include <fat_m.c>   // Driver de la SD (fat depende de mmcsd)
// Archivos modulares sistema
#include <configs.c>    // Archivo estatico de configuraciones del pic
#include <carga.c>      // Archivo para manejar los indices de la aplicacion
#include <comandos.c>   // Archivo para comandar la sd
#include <interfazSD.c> // Interface que agrupa metodos de fat y valida su ejecucion ordenada

// LIMPIA LAS VARIABLES DE LA CLASE <<main.c>>
void clearVarsMain()
{
   doublemsCMP = 0;
   keyChange = 0;
   modeIndex = 0;
   sckp = 0, scmp1 = 0, scmp2 = 0;
   rpmMax = 0;
   nfrag = 0;
   plus = 0;
   longIDX = 0;
   noSennales = 0;
   strcpy(Direccion, "");
}

#int_TIMER2
void timer2_isr(void)
{                 // Gear's Heart {Depende de la cantidad de muestras en <<longCMP>> para dar la vuelta al arreglo}
   doublemsCMP++; // 10 us cada desbord
   //BLOQUE QUE PERMITE MOVER LOS ENGRANES DE TIEMPO
   if (modeIndex)
   { // Sennal in-serie (modeIndex=1)
      if (doublemsCMP == CMP_codec[index])
      {  // Primero se cubre el tiempo preciso
         // Dependiendo de la salida correspondiente a CMP[1:2] o CKP, alternara su estado en ese pin
         if (stateCMP[index])
         {
            sCKP = ~sCKP;
         }
         else if (stateCKP[index])
         {
            sCMP2 = ~sCMP2;
         }
         else
         {
            sCMP = ~sCMP;
         }
         // Reinicia dms y mueve el indice para evaluar el siguiente pulso
         doublemsCMP = 0;
         //Reindex: Indexacion sobre estructura lineal de la sennal
         index++; // Incrementa para acceder a la siguiente bloque de informacion que contiene la duracion del estado (ON/OFF)
         if (index == longCMP)
         { // Despues de n+1 capturas, La posicion 0, indica la longitud de sennal muestreada
            index = 0;
         }
      }
   }
   else
   { //Sennal w-jump (modeIndex=0)
      if (doublemsCMP == CMP_codec[index])
      { // Primero se cubre el tiempo preciso
         if (stateCMP[index])
         {
            sCKP = ~sCKP;
         }
         else if (stateCKP[index])
         {
            sCMP2 = ~sCMP2;
         }
         else
         {
            sCMP = ~sCMP;
         }
         // Reinicia dms y mueve el indice para evaluar el siguiente pulso
         doublemsCMP = 0;
         //Reindex Full: Indexacion sobre estructura no lineal de la sennal
         index++; // Incrementa para acceder a la siguiente bloque de informacion que contiene la duracion del estado (ON/OFF)
         if (index == longCMP)
         { // Despues de n-1 topes, La posicion del tope, indica la longitud de sennal muestreada en ese punto en el tiempo
            if (idx == longIDX)
            { // Si idx llega al limite se resetea toda la estructura de la sennal
               idx = 0;
               longCMP = topes[idx];
               index = 0;
            }
            else
            {
               index = topes[++idx];
               longCMP = topes[++idx];
            }
         }
      }
   }
   TMR2IF = 0; // Alista para el siguiente uso TMR2IF
   TMR2IE = 1; // Habilita para el siguiente uso
} //End interrupt()

// INICIALIZA VARIABLES
void initVars()
{
   // Vars from sd.c
   clearVarsMain();
   // Vars from index.c
   clearVarsLoad();
   // Vars from cmd.c
   clearVarsCmd();
   // Comandos from configMOD
   clearCmdConfigs();
}

void main(void)
{
   initPorts();  // Inicializa PUERTOS A,B,C,D
   AbleGLOBAL(); // Habilita interrupciones
   delay_ms(5);
   while (TRUE)
   {
      while (CMD9 == 0)
         ; // Hasta CMD9:STOP Activo para sincronizar ambos sistemas
      ConfigTMR2();
      initVars();
      while (fat_init() != 0)
      { // Espera a que la SD sea detectada
         //printf("iIniciando...\r\n");
         read_txt(Direccion, 0); // Extrae el 'noSennales', no importa que contenga 'Direccion'
         delay_ms(1);
      }
      SD = 1;
      //printf("Esperando CMD9 - STOP to SYNC: start%u stop%u pause%u SD%u ON%u\r\n",CMD8,CMD9,CMD10,SD,ON);
      while (CMD9 == 1)
         ; // Mantener activo CMD9:STOP hasta que nextion mande una nueva direccion
      procesar_CMDDIR();
      read_txt(Direccion, 1); // Extrae toda la data y directamente la codifica en CMP_codec y en stateC(M/K)P
      //verificarContenido();          // Para debuggear
      nfrag = (rpmMax - rpmMin) / 500; // Calcula la cantidad de fragmentos que tiene la sennal
      plus = nfrag;
      rpmMOD(plus); // Carga el RPM correspondiente a los registros del TMR0
      SD = 0;
      ON = 1; //sennal capturada
      while (CMD8 == 0 && CMD9 == 0)
         ; // Hasta que CMD8:START ACTIVO y que CMD9:STOP NO ACTIVO
      // Inicializaciones de las sennales, extraidas de la SD
      if (CMD8 == 1)
      {                 // Si CMD8:START ACTIVO, habilita la reproduccion de la sennal
         sCMP = scmp1;  // Se pasa la variable logica a la fisica, porque comenzara la salida de sennales
         sCMP2 = scmp2; // de esta manera se evita que las salidas fisicas estan prendidas durante la
         sCKP = sckp;   // la adquision de los datos.
         //[PREPARE UPDATE 22/12/2020] - Direcciones especiales (a 12 v)
         // Este if debe de ir dentro de un for, que compare con las n direcciones a 12 volts que hay.
         if (CMD_DIR == 122 || CMD_DIR == 123 || CMD_DIR == 124)
         {
            doceVolts = 1; //Manda pulso a un relay que hara la conmutacion
         }
         //printf("has roto el ciclo con start%u stop%u pause%u SD%u ON%u\r\n",CMD8,CMD9,CMD10,SD,ON);
         runTMR2();
      }
      while (CMD9 == 0)
      { // Hasta CMD9:STOP ACTIVO
         //BLOQUE QUE PERMITE MODIFICAR LAS REVOLUCIONES POR MINUTO (RPM).
         while (CMD10 == 1)
         {                     // Mientras CMD10:PAUSE (un pulso ALTO)
            stopTMR2();        // Detiene el TMR2 para e vitar que se pierda en el tiempo
            keyChange = 1;     // Se activa la key change para evitar que siempre se este reseteando el TMR2 con runTMR2()
            procesar_CMDRPM(); // Monitorea en caso de haber cambiado el  RPM
         }                     //End while
         if (keyChange == 1)
         { // Gracias a la KeyChange permite continuar el funcionamiento normal del TMR2
            // Ajusta RPM's en base al boton presionado y al numero de botones (fragmentos) que tenga la vista de la sennal.
            plus = nfrag - CMD_RPM;
            rpmMOD(plus);
            // Reinicia parametros puesto que se requiere un tiempo distinto al anterior
            keyChange = 0;
            doublemsCMP = 0;
            runTMR2(); // Arranca el TMR2 para continuar mandando sennales
         }             //End if
      }                //End While STOP
   }                   //End While TRUE
} // End Main
