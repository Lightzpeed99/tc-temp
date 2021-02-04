// ARCHIVO PARA EXTRACCION DE DATOS DE LA SD
FILE myfile;

// LEE ARCHIVO DE TEXTO: REQUIERE EL FORMATO EN 'param[]' -> "/%lu.txt"
void read_txt(char param[])
{
    delay_ms(5);
    // [PREPARE UPDATE 22 DICIEMBRE 2020] - Annadir condicion vinculada a timer para deteccion de SD
    while (fat_init() != 0)
    {
        //printf("iIniciando...\r\n");
        delay_ms(1);
    }
    delay_ms(5);
    while (fatopen(param, "r", &myfile) != 0)
    {
        //printf("ileyendo archivo %s...\r\n",archivo);
        delay_ms(1);
    }
        longCMP = -127;
        while (longCMP == (-127))
        { //Si cambia de -127, la extraccion fue exitosa
            //printf("iExtrayendo datos...");
            delay_ms(1);
            longCMP = fatbuffer(&myfile);
        }
    fatclose(&myfile);
    strcpy(Direccion, ""); //Sirve para reutilizar la direccion en caso de que se haya cambiado el comando en el menu
}
