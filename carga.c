// ARCHIVO PARA INDEXAR Y CODIFICAR SOBRE LA ESTRUCTURA DEL CMP_codec
// ESPECIFICA EL FORMATO Y LIMITES DE LA ESTRUCTURA A RECORRER CON EL CORE
unsigned long int lengthCMP = 0; // Permite hacer saber a reindex cuantas posiciones deberia recorrer el arreglo
//unsigned long int index = 0;              //V{0-400}                 // Apuntadores de sennal CMP;
unsigned int lengthIDX;          // Cuantos saltos se debe dar en el modo dual
int idx = 0;                     // Es el indice del arreglo de saltos en modo dual
unsigned long int limitJump[10]; // Arreglo que contiene los limites hasta donde llegaran los saltos en modo dual

// RESETEA LA CADENA
void reset_limitJump(){
   for(int k=0;k<10;k++){        // La cadena tiene una longitud de 5 caracteres
      limitJump[k] = '\0';       // Todo como nulo
   }
}

// LIMPIA VARIABLES lengthCMP, lengthIDX e idx
void clearVarsLoad(){
   lengthCMP = 0;
   lengthIDX = 0;
   idx = 0;
   reset_limitJump();
}

// PASA LAS SENNALES GUARDADAS EN FORMATO DE MILISEGUNDOS A FORMATO PP (pseudopreciso)
unsigned long dataToCodec(float t){
   unsigned long int charge = 0;
   unsigned long int ims = 0, idoublems = 0;   // Variables de pseudoprecision (pp) para el interprete
   //VARIABLES DE USO GENERAL PARA PASAR DE MILISEGUNDOS A PSEUDOPRECISION MILIS (PP).
   float milis = 1.000, rupt = 0.01;           // Constantes que se presentan en el sistema
   float verif = 0.0, doub = 0.0;              // Para realizar la codificacion de la sennal guardada a formato pp.
   ims = t/milis;             // El tiempo que toma del arreglo principal, lo divide entre los pseudomilis, para determinar el tiempo en pseudomilis
   verif = ims*milis;         // Obtiene el tiempo aproximado real que se tendria de la captura de milisegundos
   doub = t - verif;          // Verifica cuanto le falta para ser pseudopreciso
   idoublems = doub/rupt;     // Rounded(Anterior)
   charge = (idoublems + (ims*100));
   if(charge == 0){           // El generador no puede mostrar 0 segundos, solo apartir de 0.01 seg.
      charge = 2;             // Se incrementa a 2, porque en la siguiente linea de codigo se le resta 1
   }
   //printf("\n[carga.c] - charge: %lu",charge);
   return (--charge);
}//End codecCMP()