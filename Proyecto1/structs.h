#ifndef STRUCTS_H
#define STRUCTS_H
#include <time.h>


typedef struct {
    char part_status; //Indica si la particion esta activa o no
    char part_type; //Indica el tipo de particion
    char part_fit; //Tipo de ajuste de la particion
    int part_start; //Indica en que byte del disco inicia la particion
    int part_size; //Contiene el tamano de la particion en bytes
    char part_name[16]; //Nombre de la particion
} Partition;

typedef struct{
    int mbr_size; //Tamano total del disco en bytes
    time_t mbr_date_created; //Fecha y hora de creacion del disco
    int mbr_disk_signature; //Numero random, que identifica de forma unica cada disco
    char mbr_disk_fit; //Tipo de ajuste
    Partition mbr_partition[4]; //4 particiones
}MBR;

#endif // STRUCTS_H
