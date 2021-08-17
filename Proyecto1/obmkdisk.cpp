#include "obmkdisk.h"

obmkdisk::obmkdisk()
{

}

void obmkdisk::mostrardatos(obmkdisk *disco){
    try {
        printf("\n\n---------DATOS----------\n\n");
        printf("EL tamano es %d \n",disco->size);
    }  catch (exception e) {

    }



}
