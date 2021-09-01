#include "Particiones.h"

Particion::Particion()
{

}
bool Particion::existePart(QString dir, QString name){
    int ext = 10;
    FILE *filep;
    if((filep = fopen(dir.toStdString().c_str(),"rb+"))){
        MBR mbr;
        fseek(filep,0,SEEK_SET);
        fread(&mbr, sizeof(MBR),1,filep);
        for(int i = 0; i < 4; i++){
            if(strcmp(mbr.mbr_partitions[i].part_name,name.toStdString().c_str()) == 0){
                fclose(filep);
                return true;
            }else if(mbr.mbr_partitions[i].part_type == 'E'){
                ext = i;
            }
        }
        if(ext != 10){
            fseek(filep, mbr.mbr_partitions[ext].part_start,SEEK_SET);
            EBR ebr;
            while((fread(&ebr,sizeof(EBR),1,filep))!=0 && (ftell(filep) < (mbr.mbr_partitions[ext].part_size + mbr.mbr_partitions[ext].part_start))){
                if(ebr.part_next == -1){
                    fclose(filep);
                    return false;
                }
                if(strcmp(ebr.part_name,name.toStdString().c_str()) == 0){
                    fclose(filep);
                    return true;
                }
            }
        }
    }
    fclose(filep);
    return false;
}
void Particion::crearPartPri(QString dir, QString name, int size, char f, char u){
    char auxU = 0;
    char auxF = 0;
    string auxPath = dir.toStdString();
    int sizeBytes;
    char buff = '1';
    if(f != 0)
        auxF = f;
    else
        auxF = 'W';

    if(u != 0){
        auxU = u;
        if(auxU == 'b'){
            sizeBytes = size;
        }else if(auxU == 'k'){
            sizeBytes = size * 1024;
        }else{
            sizeBytes = size*1024*1024;
        }
    }else{
        sizeBytes = size * 1024;
    }
    FILE *filep;
    MBR mbr;
    if((filep = fopen(auxPath.c_str(), "rb+"))){
        int numPart = 0;
        bool flagPart = false;
        fseek(filep,0,SEEK_SET);
        fread(&mbr,sizeof(MBR),1,filep);
        for(int i = 0; i < 4; i++){
            if(mbr.mbr_partitions[i].part_start == -1 || (mbr.mbr_partitions[i].part_status == '1' && mbr.mbr_partitions[i].part_size>=sizeBytes)){
                flagPart = true;
                numPart = i;
                break;
            }
        }
        if(flagPart){
            int espUtilizado = 0;
            for(int i = 0; i < 4; i++){
                if(mbr.mbr_partitions[i].part_status!='1'){
                    espUtilizado += mbr.mbr_partitions[i].part_size;
                }
            }
            cout << "Espacio necesario:  " << sizeBytes << " Bytes" << endl;
            cout << "Espacio disponible: " << (mbr.mbr_size - espUtilizado) << " Bytes" << endl;
            if((mbr.mbr_size - espUtilizado) >= sizeBytes){
                if(!existePart(dir,name)){
                    if(mbr.mbr_disk_fit == 'B'){
                        int bestF = numPart;
                        for(int i = 0; i < 4; i++){
                            if(mbr.mbr_partitions[i].part_start == -1 || (mbr.mbr_partitions[i].part_status == '1' && mbr.mbr_partitions[i].part_size>=sizeBytes)){
                                if(i != numPart){
                                    if(mbr.mbr_partitions[bestF].part_size > mbr.mbr_partitions[i].part_size){
                                        bestF = i;
                                        break;
                                        }
                                    }
                                }
                        }
                        mbr.mbr_partitions[bestF].part_type = 'P';
                        mbr.mbr_partitions[bestF].part_fit = auxF;
                        if(bestF == 0){
                            mbr.mbr_partitions[bestF].part_start = sizeof(mbr);
                        }else{
                            mbr.mbr_partitions[bestF].part_start = mbr.mbr_partitions[bestF-1].part_start + mbr.mbr_partitions[bestF-1].part_size;
                        }
                        mbr.mbr_partitions[bestF].part_size = sizeBytes;
                        mbr.mbr_partitions[bestF].part_status = '0';
                        strcpy(mbr.mbr_partitions[bestF].part_name,name.toStdString().c_str());
                    }else if(mbr.mbr_disk_fit == 'F'){
                        mbr.mbr_partitions[numPart].part_type = 'P';
                        mbr.mbr_partitions[numPart].part_fit = auxF;
                        if(numPart == 0){
                            mbr.mbr_partitions[numPart].part_start = sizeof(mbr);
                        }else{
                            mbr.mbr_partitions[numPart].part_start = mbr.mbr_partitions[numPart-1].part_start + mbr.mbr_partitions[numPart-1].part_size;
                        }
                        mbr.mbr_partitions[numPart].part_size = sizeBytes;
                        mbr.mbr_partitions[numPart].part_status = '0';
                        strcpy(mbr.mbr_partitions[numPart].part_name,name.toStdString().c_str());
                    }else if(mbr.mbr_disk_fit == 'W'){
                        int  worstIndex= numPart;
                        for(int i = 0; i < 4; i++){
                            if(mbr.mbr_partitions[i].part_start == -1 || (mbr.mbr_partitions[i].part_status == '1' && mbr.mbr_partitions[i].part_size>=sizeBytes)){
                                if(i != numPart){
                                    if(mbr.mbr_partitions[worstIndex].part_size < mbr.mbr_partitions[i].part_size){
                                        worstIndex = i;
                                        break;
                                    }
                                }
                            }
                        }
                        mbr.mbr_partitions[worstIndex].part_type = 'P';
                        mbr.mbr_partitions[worstIndex].part_fit = auxF;
                        if(worstIndex == 0){
                            mbr.mbr_partitions[worstIndex].part_start = sizeof(mbr);
                        }else{
                            mbr.mbr_partitions[worstIndex].part_start = mbr.mbr_partitions[worstIndex-1].part_start + mbr.mbr_partitions[worstIndex-1].part_size;
                        }
                        mbr.mbr_partitions[worstIndex].part_size = sizeBytes;
                        mbr.mbr_partitions[worstIndex].part_status = '0';
                        strcpy(mbr.mbr_partitions[worstIndex].part_name,name.toStdString().c_str());
                    }
                    fseek(filep,0,SEEK_SET);
                    fwrite(&mbr,sizeof(MBR),1,filep);
                    fseek(filep,mbr.mbr_partitions[numPart].part_start,SEEK_SET);
                    for(int i = 0; i < sizeBytes; i++){
                        fwrite(&buff,1,1,filep);
                    }
                    cout << "...\n" << "La particion primaria fue creada con exito" <<  endl;
                }else{
                    cout << "ERROR: Ya existe una particion con el mismo nombre" << endl;
                }

            }else{
                cout << "ERROR: La particion que desea crear excede el espacio libre" << endl;
            }
        }else{
            cout << "ERROR: Ya se llego al limite de particiones (4 particiones por Disco)" << endl;
            cout << "Para crear una nueva debera eliminar al menos una." << endl;
        }
    fclose(filep);
    }else{
        cout << "ERROR: no existe el Disco especificado" << endl;
    }
}
void Particion::crearPartExt(QString dir, QString name, int size, char f, char u){
    char auxU = 0;
    char auxF = 0;
    string auxPath = dir.toStdString();
    int size_bytes = 1024;
    char buff = '1';
    if(f != 0)
        auxF = f;
    else
        auxF = 'W';

    if(u != 0){
        auxU = u;
        if(auxU == 'b'){
            size_bytes = size;
        }else if(auxU == 'k'){
            size_bytes = size * 1024;
        }else{
            size_bytes = size*1024*1024;
        }
    }else{
        size_bytes = size * 1024;
    }
    MBR mbr;
    EBR ebr;
    FILE *filep;
    if((filep = fopen(auxPath.c_str(), "rb+"))){
        bool flagPart = false;
        bool flagExt = false;
        int numPart = 0;
        fseek(filep,0,SEEK_SET);
        fread(&mbr,sizeof(MBR),1,filep);
        for(int i = 0; i < 4; i++){
            if (mbr.mbr_partitions[i].part_type == 'E'){
                flagExt = true;
                break;
            }
        }
        if(!flagExt){
            for(int i = 0; i < 4; i++){
                if(mbr.mbr_partitions[i].part_start == -1 || (mbr.mbr_partitions[i].part_status == '1' && mbr.mbr_partitions[i].part_size>=size_bytes)){
                    flagPart = true;
                    numPart = i;
                    break;
                }
            }
            if(flagPart){
                int espUtilizado = 0;
                for(int i = 0; i < 4; i++){
                    if(mbr.mbr_partitions[i].part_status!='1'){
                       espUtilizado += mbr.mbr_partitions[i].part_size;
                    }
                }
                cout << "Espacio disponible: " << (mbr.mbr_size - espUtilizado) <<" Bytes"<< endl;
                cout << "Espacio necesario:  " << size_bytes << " Bytes" << endl;
                if((mbr.mbr_size - espUtilizado) >= size_bytes){ //espacio suficiente
                    if(!(existePart(dir,name))){
                        if(mbr.mbr_disk_fit == 'F'){
                            mbr.mbr_partitions[numPart].part_type = 'E';
                            mbr.mbr_partitions[numPart].part_fit = auxF;
                            if(numPart == 0){
                                mbr.mbr_partitions[numPart].part_start = sizeof(mbr);
                            }else{
                                mbr.mbr_partitions[numPart].part_start =  mbr.mbr_partitions[numPart-1].part_start + mbr.mbr_partitions[numPart-1].part_size;
                            }
                            mbr.mbr_partitions[numPart].part_size = size_bytes;
                            mbr.mbr_partitions[numPart].part_status = '0';
                            strcpy(mbr.mbr_partitions[numPart].part_name,name.toStdString().c_str());
                            fseek(filep,0,SEEK_SET);
                            fwrite(&mbr,sizeof(MBR),1,filep);
                            fseek(filep, mbr.mbr_partitions[numPart].part_start,SEEK_SET);
                            ebr.part_fit = auxF;
                            ebr.part_status = '0';
                            ebr.part_start = mbr.mbr_partitions[numPart].part_start;
                            ebr.part_size = 0;
                            ebr.part_next = -1;
                            strcpy(ebr.part_name, "");
                        }else if(mbr.mbr_disk_fit == 'B'){
                            int bestIndex = numPart;
                            for(int i = 0; i < 4; i++){
                                if(mbr.mbr_partitions[i].part_start == -1 || (mbr.mbr_partitions[i].part_status == '1' && mbr.mbr_partitions[i].part_size>=size_bytes)){
                                    if(i != numPart){
                                        if(mbr.mbr_partitions[bestIndex].part_size > mbr.mbr_partitions[i].part_size){
                                            bestIndex = i;
                                            break;
                                        }
                                    }
                                }
                            }
                            mbr.mbr_partitions[bestIndex].part_type = 'E';
                            mbr.mbr_partitions[bestIndex].part_fit = auxF;
                            if(bestIndex == 0){
                                mbr.mbr_partitions[bestIndex].part_start = sizeof(mbr);
                            }else{
                                mbr.mbr_partitions[bestIndex].part_start =  mbr.mbr_partitions[bestIndex-1].part_start + mbr.mbr_partitions[bestIndex-1].part_size;
                            }
                            mbr.mbr_partitions[bestIndex].part_size = size_bytes;
                            mbr.mbr_partitions[bestIndex].part_status = '0';
                            strcpy(mbr.mbr_partitions[bestIndex].part_name,name.toStdString().c_str());
                            fseek(filep,0,SEEK_SET);
                            fwrite(&mbr,sizeof(MBR),1,filep);
                            fseek(filep, mbr.mbr_partitions[bestIndex].part_start,SEEK_SET);
                            ebr.part_fit = auxF;
                            ebr.part_status = '0';
                            ebr.part_start = mbr.mbr_partitions[bestIndex].part_start;
                            ebr.part_size = 0;
                            ebr.part_next = -1;
                            strcpy(ebr.part_name, "");
                        }else if(mbr.mbr_disk_fit == 'W'){
                            int  worstIndex= numPart;
                            for(int i = 0; i < 4; i++){
                                if(mbr.mbr_partitions[i].part_start == -1 || (mbr.mbr_partitions[i].part_status == '1' && mbr.mbr_partitions[i].part_size>=size_bytes)){
                                    if(i != numPart){
                                        if(mbr.mbr_partitions[worstIndex].part_size < mbr.mbr_partitions[i].part_size){
                                            worstIndex = i;
                                            break;
                                        }
                                    }
                                }
                            }
                            mbr.mbr_partitions[worstIndex].part_type = 'E';
                            mbr.mbr_partitions[worstIndex].part_fit = auxF;
                            if(worstIndex == 0){
                                mbr.mbr_partitions[worstIndex].part_start = sizeof(mbr);
                            }else{
                                mbr.mbr_partitions[worstIndex].part_start =  mbr.mbr_partitions[worstIndex-1].part_start + mbr.mbr_partitions[worstIndex-1].part_size;
                            }
                            mbr.mbr_partitions[worstIndex].part_size = size_bytes;
                            mbr.mbr_partitions[worstIndex].part_status = '0';
                            strcpy(mbr.mbr_partitions[worstIndex].part_name,name.toStdString().c_str());
                            fseek(filep,0,SEEK_SET);
                            fwrite(&mbr,sizeof(MBR),1,filep);
                            fseek(filep, mbr.mbr_partitions[worstIndex].part_start,SEEK_SET);
                            ebr.part_fit = auxF;
                            ebr.part_status = '0';
                            ebr.part_start = mbr.mbr_partitions[worstIndex].part_start;
                            ebr.part_size = 0;
                            ebr.part_next = -1;
                            strcpy(ebr.part_name, "");
                        }
                        fwrite(&ebr,sizeof (EBR),1,filep);
                        for(int i = 0; i < (size_bytes - (int)sizeof(EBR)); i++){
                            fwrite(&buff,1,1,filep);
                        }
                        cout << "...\n" << "Particion extendida creada con exito"<< endl;
                    }else{
                        cout << "ERROR: Ya existe una particion con el mismo nombre" << endl;
                    }
                }else{
                    cout << "ERROR: La particion que desea crear excede el espacio libre" << endl;
                }
            }else{
                cout << "ERROR: Ya existen 4 particiones, no se puede crear otra" << endl;
                cout << "Elimine alguna para poder crear una" << endl;
            }
        }else{
            cout << "ERROR: Ya se llego al limite de particiones (4 particiones por Disco)" << endl;
            cout << "Para crear una nueva debera eliminar al menos una." << endl;
        }
    fclose(filep);
    }else{
        cout << "ERROR no existe el Disco especificado" << endl;
    }
}

void Particion::crearPartL(QString dir, QString name, int size, char f, char u){
    char auxF = 0;
    char auxU = 0;
    string auxPath = dir.toStdString();
    int size_bytes = 1024;
    char buff = '1';
    if(f != 0)
        auxF = f;
    else
        auxF = 'W';
    if(u != 0){
        auxU = u;
        if(auxU == 'b'){
            size_bytes = size;
        }else if(auxU == 'k'){
            size_bytes = size * 1024;
        }else{
            size_bytes = size*1024*1024;
        }
    }else{
        size_bytes = size * 1024;
    }

    FILE *filep;
    MBR mbr;
    if((filep = fopen(auxPath.c_str(), "rb+"))){
        int numExt = -1;
        fseek(filep,0,SEEK_SET);
        fread(&mbr,sizeof(MBR),1,filep);
        for(int i = 0; i < 4; i++){
            if(mbr.mbr_partitions[i].part_type == 'E'){
                numExt = i;
                break;
            }
        }
        if(!existePart(dir,name)){
            if(numExt != -1){
                EBR ebr;
                int partStart = mbr.mbr_partitions[numExt].part_start;
                fseek(filep,partStart,SEEK_SET);
                fread(&ebr, sizeof(EBR),1,filep);
                if(ebr.part_size == 0){//Si es la primera particion
                    if(mbr.mbr_partitions[numExt].part_size < size_bytes){
                        cout << "ERROR la particion logica a crear excede el espacio disponible de la particion extendida " << endl;
                    }else{
                        ebr.part_status = '0';
                        ebr.part_fit = auxF;
                        ebr.part_start = ftell(filep) - sizeof(EBR); //Para regresar al comienzo de la extendida
                        ebr.part_size = size_bytes;
                        ebr.part_next = -1;
                        strcpy(ebr.part_name, name.toStdString().c_str());
                        fseek(filep, mbr.mbr_partitions[numExt].part_start ,SEEK_SET);
                        fwrite(&ebr,sizeof(EBR),1,filep);
                        cout << "...\nLa particion logica fue creada con exito "<< endl;
                    }
                }else{
                    while((ebr.part_next != -1) && (ftell(filep) < (mbr.mbr_partitions[numExt].part_size + mbr.mbr_partitions[numExt].part_start))){
                        fseek(filep,ebr.part_next,SEEK_SET);
                        fread(&ebr,sizeof(EBR),1,filep);
                    }
                    int espNece = ebr.part_start + ebr.part_size + size_bytes;
                    if(espNece <= (mbr.mbr_partitions[numExt].part_size + mbr.mbr_partitions[numExt].part_start)){
                        ebr.part_next = ebr.part_start + ebr.part_size;
                        fseek(filep,ftell(filep) - sizeof (EBR),SEEK_SET);
                        fwrite(&ebr, sizeof(EBR),1 ,filep);
                        fseek(filep,ebr.part_start + ebr.part_size, SEEK_SET);
                        ebr.part_status = 0;
                        ebr.part_fit = auxF;
                        ebr.part_start = ftell(filep);
                        ebr.part_size = size_bytes;
                        ebr.part_next = -1;
                        strcpy(ebr.part_name,name.toStdString().c_str());
                        fwrite(&ebr,sizeof(EBR),1,filep);
                        cout << "La particion logica fue creada con exito "<< endl;
                    }else{
                        cout << "ERROR: la particion logica que desea crear excede el" << endl;
                        cout << "espacio disponible de la particion extendida utilizada" << endl;
                    }
                }
            }else{
                cout << "ERROR: Es necesaria una particion extendida para guardar la particion logica " << endl;
            }
        }else{
            cout << "ERROR: Ya existe una particion con el mismo nombre" << endl;
        }

    fclose(filep);
    }else{
        cout << "ERROR: no existe el Disco especificado" << endl;
    }

}
void Particion::eliminarPart(QString direccion, QString nombre, QString typeDel){
    string auxPath = direccion.toStdString();
    string auxName = nombre.toStdString();
    FILE *filep;
    if((filep = fopen(auxPath.c_str(), "rb+"))){
        MBR mbr;
        fseek(filep,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,filep);
        int ind = -1;
        int indExt = -1;
        bool flagExt = false;
        string opc = "";
        char buffer = '\0';
        for(int i = 0; i < 4; i++){
            if((strcmp(mbr.mbr_partitions[i].part_name, auxName.c_str())) == 0){
                ind = i;
                if(mbr.mbr_partitions[i].part_type == 'E')
                    flagExt = true;
                break;
            }else if(mbr.mbr_partitions[i].part_type == 'E'){
                indExt = i;
            }
        }
        cout << "¿Esta seguro que desea eliminar la particion? Y/N : " ;
        getline(cin, opc);
        opc = "Y";
        if(opc.compare("Y") == 0 || opc.compare("y") == 0){
            if(ind != -1){//Si hay principales
                if(!flagExt){//pri
                    if(typeDel == "fast"){
                        mbr.mbr_partitions[ind].part_status = '1';
                        strcpy(mbr.mbr_partitions[ind].part_name,"");
                        fseek(filep,0,SEEK_SET);
                        fwrite(&mbr,sizeof(MBR),1,filep);

                    }else{//full
                        mbr.mbr_partitions[ind].part_status = '1';
                        strcpy(mbr.mbr_partitions[ind].part_name,"");
                        fseek(filep,0,SEEK_SET);
                        fwrite(&mbr,sizeof(MBR),1,filep);
                        fseek(filep,mbr.mbr_partitions[ind].part_start,SEEK_SET);
                        fwrite(&buffer,1,mbr.mbr_partitions[ind].part_size,filep);
                    }
                    cout << "La particion primaria fue eliminada con exito" << endl;

                }else{//ext
                    if(typeDel == "fast"){
                        mbr.mbr_partitions[ind].part_status = '1';
                        strcpy(mbr.mbr_partitions[ind].part_name,"");
                        fseek(filep,0,SEEK_SET);
                        fwrite(&mbr,sizeof(MBR),1,filep);
                    }else{//full
                        mbr.mbr_partitions[ind].part_status = '1';
                        strcpy(mbr.mbr_partitions[ind].part_name,"");
                        fseek(filep,0,SEEK_SET);
                        fwrite(&mbr,sizeof(MBR),1,filep);
                        fseek(filep,mbr.mbr_partitions[ind].part_start,SEEK_SET);
                        fwrite(&buffer,1,mbr.mbr_partitions[ind].part_size,filep);
                    }
                    cout << "La particion extendida fue eliminada con exito" << endl;

                }
            }else{//
                if(indExt != -1){
                    bool flag = false;//flag indica si existe
                    EBR erb;
                    fseek(filep,mbr.mbr_partitions[indExt].part_start, SEEK_SET);
                    fread(&erb,sizeof(EBR),1,filep);
                    if(erb.part_size!=0){
                        fseek(filep, mbr.mbr_partitions[indExt].part_start,SEEK_SET);
                        while((fread(&erb,sizeof(EBR),1,filep))!=0 && (ftell(filep) < (mbr.mbr_partitions[indExt].part_start + mbr.mbr_partitions[indExt].part_size))) {
                            if(strcmp(erb.part_name,nombre.toStdString().c_str()) == 0 && erb.part_status != '1'){
                                flag = true;
                                break;
                            }else if(erb.part_next == -1){//No se encontro
                                break;
                            }
                        }
                    }
                    if(flag){
                        if(typeDel == "fast"){
                            erb.part_status = '1';
                            strcpy(erb.part_name, "");
                            fseek(filep, ftell(filep)-sizeof(EBR),SEEK_SET);
                            fwrite(&erb,sizeof(EBR),1,filep);
                        }else{//full
                            erb.part_status = '1';
                            strcpy(erb.part_name, "");
                            fseek(filep, ftell(filep)-sizeof(EBR),SEEK_SET);
                            fwrite(&erb,sizeof(EBR),1,filep);
                            fwrite(&buffer,1,erb.part_size,filep);
                        }
                        cout << "La particion logica fue eliminada con exito" << endl;

                    }else{
                        cout << "ERROR: no se encuentra la particion que desea eliminar" << endl;
                    }
                }else{
                    cout << "ERROR: no se encuentra la particion que desea eliminar" << endl;
                }
            }
        }else if(opc.compare("N") || opc.compare("n") == 0){
            cout << "Operacion Cancelada" << endl;;
        }else{
            cout << "Opcion ingresada incorrecta" << endl;
        }

        fclose(filep);
    }else{
        cout << "ERROR: el disco que contiene la particion especificada no existe" << endl;
    }
}


void Particion::comandADD(QString dir, QString name, int add, char unit){
    string auxPath = dir.toStdString();
    string auxName = name.toStdString();
    int size_Bytes = 0;
    QString type = "";

    if(add > 0)
        type = "add";

    if(type != "add")
        add = add*(-1);

    if(unit == 'm')
        size_Bytes = add * 1024 * 1024;
    else if(unit == 'k')
        size_Bytes = add * 1024;
    else
        size_Bytes = add;

    FILE *filep;
    if((filep = fopen(auxPath.c_str(), "rb+"))){
        bool flagExt = false;
        MBR mbr;
        fseek(filep,0,SEEK_SET);
        fread(&mbr,sizeof(MBR),1,filep);
        int ind = -1;
        int indExt = -1;
        for(int i = 0; i < 4; i++){
            if((strcmp(mbr.mbr_partitions[i].part_name, auxName.c_str())) == 0){
                ind = i;
                if(mbr.mbr_partitions[i].part_type == 'E')
                    flagExt = true;
                break;
            }else if(mbr.mbr_partitions[i].part_type == 'E'){
                indExt = i;
            }
        }
        if(ind != -1){
            if(!flagExt){//Primaria
                if(type == "add"){
                    if(ind!=3){
                        int part1 = mbr.mbr_partitions[ind].part_start + mbr.mbr_partitions[ind].part_size;
                        int part2 = mbr.mbr_partitions[ind+1].part_start;
                        if((part2 - part1) != 0){
                            int frag = part2-part1;
                            if(frag >= size_Bytes){
                                mbr.mbr_partitions[ind].part_size = mbr.mbr_partitions[ind].part_size + size_Bytes;
                                fseek(filep,0,SEEK_SET);
                                fwrite(&mbr,sizeof(MBR),1,filep);
                                cout << "Se agrego espacio a la particion exitosamente!" << endl;
                            }else{
                                cout << "ERROR: no es posible agregar espacio a la particion ya que no hay suficiente espacio a su  derecha" << endl;
                            }
                        }else{
                            if(mbr.mbr_partitions[ind + 1].part_status == '1'){
                                if(mbr.mbr_partitions[ind + 1].part_size >= size_Bytes){
                                    mbr.mbr_partitions[ind].part_size = mbr.mbr_partitions[ind].part_size + size_Bytes;
                                    mbr.mbr_partitions[ind + 1].part_size = (mbr.mbr_partitions[ind + 1].part_size - size_Bytes);
                                    mbr.mbr_partitions[ind + 1].part_start = mbr.mbr_partitions[ind + 1].part_start + size_Bytes;
                                    fseek(filep,0,SEEK_SET);
                                    fwrite(&mbr,sizeof(MBR),1,filep);
                                    cout << "Se agrego espacio a la particion exitosamente!" << endl;
                                }else{
                                    cout << "ERROR: no es posible agregar espacio a la particion ya que no hay suficiente espacio a su  derecha" << endl;
                                }
                            }
                        }
                    }else{
                        int p = mbr.mbr_partitions[ind].part_start + mbr.mbr_partitions[ind].part_size;
                        int total = mbr.mbr_size + (int)sizeof(MBR);
                        if((total-p) != 0){
                            int fragmentacion = total - p;
                            if(fragmentacion >= size_Bytes){
                                mbr.mbr_partitions[ind].part_size = mbr.mbr_partitions[ind].part_size + size_Bytes;
                                fseek(filep,0,SEEK_SET);
                                fwrite(&mbr,sizeof(MBR),1,filep);
                                cout << "Se agrego espacio a la particion exitosamente!" << endl;
                            }else{
                                cout << "ERROR: no es posible agregar espacio a la particion porque no hay suficiente espacio disponible a su derecha" << endl;
                            }
                        }else{
                            cout << "ERROR: no es posible agregar espacio a la particion ya que no hay suficiente espacio a su  derecha" << endl;
                        }
                    }
                }else{//Quitar espacio pri
                    if(size_Bytes >= mbr.mbr_partitions[ind].part_size){
                        cout << "ERROR: No es posible quitar esta cantidad de espacio a la particion porque seria borrada" << endl;
                    }else{
                        mbr.mbr_partitions[ind].part_size = mbr.mbr_partitions[ind].part_size - size_Bytes;
                        fseek(filep,0,SEEK_SET);
                        fwrite(&mbr,sizeof(MBR),1,filep);
                        cout << "Se quito espacio a la particion exitosamente!" << endl;
                    }
                }
            }else{//Extendida
                if(type == "add"){//Agregar
                    if(ind!=3){
                        int part1 = mbr.mbr_partitions[ind].part_start + mbr.mbr_partitions[ind].part_size;
                        int part2 = mbr.mbr_partitions[ind+1].part_start;
                        if((part2 - part1) != 0){//Hay fragmentacion
                            int fragmentacion = part2-part1;
                            if(fragmentacion >= size_Bytes){
                                mbr.mbr_partitions[ind].part_size = mbr.mbr_partitions[ind].part_size + size_Bytes;
                                fseek(filep,0,SEEK_SET);
                                fwrite(&mbr,sizeof(MBR),1,filep);
                                cout << "Se agrego espacio a la particion exitosamente!" << endl;
                            }else{
                                cout << "ERROR: no es posible agregar espacio a la particion ya que no hay suficiente espacio a su  derecha" << endl;
                            }
                        }else{
                            if(mbr.mbr_partitions[ind + 1].part_status == '1'){
                                if(mbr.mbr_partitions[ind + 1].part_size >= size_Bytes){
                                    mbr.mbr_partitions[ind].part_size = mbr.mbr_partitions[ind].part_size + size_Bytes;
                                    mbr.mbr_partitions[ind + 1].part_size = (mbr.mbr_partitions[ind + 1].part_size - size_Bytes);
                                    mbr.mbr_partitions[ind + 1].part_start = mbr.mbr_partitions[ind + 1].part_start + size_Bytes;
                                    fseek(filep,0,SEEK_SET);
                                    fwrite(&mbr,sizeof(MBR),1,filep);
                                    cout << "Se agrego espacio a la particion exitosamente!" << endl;
                                }else{
                                    cout << "ERROR: no es posible agregar espacio a la particion ya que no hay suficiente espacio a su  derecha" << endl;
                                }
                            }
                        }
                    }else{
                        int p = mbr.mbr_partitions[ind].part_start + mbr.mbr_partitions[ind].part_size;
                        int total = mbr.mbr_size + (int)sizeof(MBR);
                        if((total-p) != 0){
                            int fragmentacion = total - p;
                            if(fragmentacion >= size_Bytes){
                                mbr.mbr_partitions[ind].part_size = mbr.mbr_partitions[ind].part_size + size_Bytes;
                                fseek(filep,0,SEEK_SET);
                                fwrite(&mbr,sizeof(MBR),1,filep);
                                cout << "Se agrego espacio a la particion exitosamente!" << endl;
                            }else{
                                cout << "ERROR: no es posible agregar espacio a la particion ya que no hay suficiente espacio a su  derecha" << endl;
                            }
                        }else{
                            cout << "ERROR: no es posible agregar espacio a la particion ya que no hay suficiente espacio a su  derecha" << endl;
                        }
                    }
                }else{//Quitar espacio ext
                    if(size_Bytes >= mbr.mbr_partitions[indExt].part_size){
                        cout << "ERROR: No es posible quitar esta cantidad de espacio a la particion porque seria borrada" << endl;
                    }else{
                        EBR ebr;
                        fseek(filep, mbr.mbr_partitions[indExt].part_start,SEEK_SET);
                        fread(&ebr,sizeof(EBR),1,filep);
                        while((ebr.part_next != -1) && (ftell(filep) < (mbr.mbr_partitions[indExt].part_size + mbr.mbr_partitions[indExt].part_start))){
                            fseek(filep,ebr.part_next,SEEK_SET);
                            fread(&ebr,sizeof(EBR),1,filep);
                        }
                        int ultimaL = ebr.part_start+ebr.part_size;
                        int aux = (mbr.mbr_partitions[indExt].part_start + mbr.mbr_partitions[indExt].part_size) - size_Bytes;
                        if(aux > ultimaL){
                            mbr.mbr_partitions[indExt].part_size = mbr.mbr_partitions[indExt].part_size - size_Bytes;
                            fseek(filep,0,SEEK_SET);
                            fwrite(&mbr,sizeof(MBR),1,filep);
                            cout << "Se quito espacio a la particion exitosamente!" << endl;
                        }else{
                            cout << "ERROR: si quita este espacio daniaria una particion logica" << endl;
                        }
                    }
                }
            }
        }else{//Posiblemente logica
            if(indExt != -1){
                int log = buscarPart_L(dir, name);
                if(log != -1){
                    if(type == "add"){
                        //Verificar que exista espacio libre a su derecha
                        EBR ebr;
                        fseek(filep,log,SEEK_SET);
                        fread(&ebr,sizeof(EBR),1,filep);

                    }else{//Quitar
                        //Verificar que no la elimine
                        EBR ebr;
                        fseek(filep,log,SEEK_SET);
                        fread(&ebr,sizeof(EBR),1,filep);
                        if(size_Bytes >= ebr.part_size){
                            cout << "ERROR si quita ese espacio eliminaria la logica" << endl;
                        }else{
                            ebr.part_size = ebr.part_size - size_Bytes;
                            fseek(filep,log,SEEK_SET);
                            fwrite(&ebr,sizeof(EBR),1,filep);
                            cout << "Se quito espacio a la particion de manera exitosa" << endl;
                        }
                    }
                }else{
                    cout << "ERROR no se encuentra la particion" << endl;
                }
            }else{
                cout << "ERROR no se encuentra la particion a redimensionar" << endl;
            }
        }
        fclose(filep);
    }else{
        cout << "ERROR el disco donde se desea agregar/quitar unidades no existe" << endl;
    }
}
int Particion::buscarPartP_E(QString direccion, QString nombre){
    string auxPath = direccion.toStdString();
    string auxName = nombre.toStdString();
    FILE *fp;
    if((fp = fopen(auxPath.c_str(),"rb+"))){
        MBR masterboot;
        fseek(fp,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        for(int i = 0; i < 4; i++){
            if(masterboot.mbr_partitions[i].part_status != '1'){
                if(strcmp(masterboot.mbr_partitions[i].part_name,auxName.c_str()) == 0){
                    return i;
                }
            }
        }

    }
    return -1;
}
int Particion::buscarPart_L(QString dir, QString name){
    string auxPath = dir.toStdString();
    string auxName = name.toStdString();
    FILE *filep;
    MBR mbr;
    EBR extendedBoot;
    if((filep = fopen(auxPath.c_str(),"rb+"))){
        int ext = -1;
        fseek(filep,0,SEEK_SET);
        fread(&mbr,sizeof(MBR),1,filep);
        for(int i = 0; i < 4; i++){
            if(mbr.mbr_partitions[i].part_type == 'E'){
                ext = i;
                break;
            }
        }
        if(ext != -1){
            fseek(filep, mbr.mbr_partitions[ext].part_start,SEEK_SET);
            while(fread(&extendedBoot,sizeof(EBR),1,filep)!=0 && (ftell(filep) < mbr.mbr_partitions[ext].part_start + mbr.mbr_partitions[ext].part_size)){
                if(strcmp(extendedBoot.part_name, auxName.c_str()) == 0){
                    return (ftell(filep) - sizeof(EBR));
                }
            }
        }
        fclose(filep);
    }
    return -1;
}
