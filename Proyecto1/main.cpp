#include <QCoreApplication>
#include "iostream"
#include <QString>
#include <parser.h>
#include <scanner.h>
#include <QTextStream>
#include "Lista.h"
#include "structs.h"
#include "Particiones.h"
#include "Clases.h"
#include "main.h"

#include <math.h>

using namespace std;
extern int yyparse();
extern int linea;
extern int columna;
extern int yylineno;
extern NodeL *lista;
Particion particion;
ListSMount *listaM= new ListSMount();

enum Choice{
    MKDISK = 1,
    SIZE = 2,
    F = 3,
    U = 4,
    PATH= 5,
    RMDISK = 8,
    FDISK = 9,
    TYPE = 10,
    DELETE = 11,
    NAME = 12,
    ADD = 13,
    MOUNT = 14,
    UMOUNT = 15,
    ID = 16,
    FS = 17,
    MKFS = 18,
    EXEC = 19
};




void crearDisco(QString direccion){
    string aux1 = direccion.toStdString();
    string delimitador = "/";
    size_t pos = 0;
    string res = "";
    while((pos = aux1.find(delimitador))!=string::npos){
        res += aux1.substr(0,pos)+"/";
        aux1.erase(0,pos + delimitador.length());
    }
    QString aux2 = QString::fromStdString(res);
    string comando = "sudo mkdir -p \'"+aux2.toStdString()+"\'";
    system(comando.c_str());
    string comando2 = "sudo chmod -R 777 \'"+aux2.toStdString()+"\'";
    system(comando2.c_str());
    string arch = direccion.toStdString();
    FILE *filep = fopen(arch.c_str(),"wb");
    if((filep = fopen(arch.c_str(),"wb")))
        fclose(filep);
    else
        cout << "Error al crear el archivo Binario" << endl;
}



void verificarMkdisk(NodeL *nodo)
{
    bool flagSize = false;
    bool flagPath = false;
    bool flagF = false;
    bool flagU = false;
    bool flag = false;
    int valSize = 0;
    char valFit = 0;
    char valUnit = 0;
    QString valPath = "";
    for(int i = 0; i < nodo->nodos.count(); i++)
    {
        NodeL n = nodo->nodos.at(i);
        switch (n.tipo_) {
        case SIZE:
        {
            if(flagSize){
                cout << "ERROR: Ya fue definido el parametro SIZE" << endl;
                flag = true;
                break;
            }
            flagSize = true;
            valSize = n.valor.toInt();
            if(!(valSize > 0)){
                flag = true;
                cout << "ERROR: El parametro SIZE debe ser mayor a cero" << endl;
                break;
            }
        }
            break;
        case F:
        {
            if(flagF){
                cout << "ERROR: Ya fue definido el parametro F" << endl;
                flag = true;
                break;
            }
            flagF = true;
            valFit = n.nodos.at(0).valor.toStdString()[0];
            if(valFit == 'b'){
                valFit = 'B';
            }else if(valFit == 'f'){
                valFit = 'F';
            }else if(valFit == 'w'){
                valFit = 'W';
            }
        }
            break;
        case U:
        {
            if(flagU){
                cout << "ERROR: Ya fue definido el parametro U" << endl;
                flag = true;
                break;
            }
            flagU = true;
            string temp = n.valor.toStdString();
            valUnit = temp[0];
            if(valUnit == 'k'|| valUnit == 'K'){
                valUnit = 'k';
            }else if(valUnit == 'm' || valUnit == 'M'){
                valUnit = 'm';
            }else{
                cout << "ERROR: Valor del parametro U desconocido "<< endl;
                flag = true;
                break;
            }
        }
            break;
        case PATH:
        {
            if(flagPath){
                cout << "ERROR: Ya fue definido el parametro PATH" << endl;
                flag = true;
                break;
            }
            flagPath = true;
            valPath = n.valor;
            valPath = valPath.replace("\"","");
        }
            break;
        }

    }
    if(!flag){
        if(flagPath){
            if(flagSize){
                MBR mbr;
                int total_size = 1024;
                crearDisco(valPath);
                mbr.mbr_date_created = time(nullptr);
                mbr.mbr_disk_signature = static_cast<int>(time(nullptr));
                if(flagU){
                    if(valUnit == 'm'){
                        mbr.mbr_size = valSize*1048576;
                        total_size = valSize * 1024;
                    }else{
                        mbr.mbr_size = valSize * 1024;
                        total_size = valSize;
                    }
                }else{
                    mbr.mbr_size = valSize*1048576;
                    total_size = valSize * 1024;
                }
                if(flagF)
                    mbr.mbr_disk_fit = valFit;
                else
                    mbr.mbr_disk_fit = 'F';
                for(int p = 0; p < 4; p++){
                    mbr.mbr_partitions[p].part_status = '0';
                    mbr.mbr_partitions[p].part_type = '0';
                    mbr.mbr_partitions[p].part_fit = '0';
                    mbr.mbr_partitions[p].part_size = 0;
                    mbr.mbr_partitions[p].part_start = -1;
                    strcpy(mbr.mbr_partitions[p].part_name,"");
                }
                string comand = "dd if=/dev/zero of=\""+valPath.toStdString()+"\" bs=1024 count="+to_string(total_size);
                system(comand.c_str());
                FILE *filep = fopen(valPath.toStdString().c_str(),"rb+");
                fseek(filep,0,SEEK_SET);
                fwrite(&mbr,sizeof(MBR),1,filep);
                fclose(filep);
                cout << "Disco creado con exito" << endl;
            }else{
                cout << "ERROR No ha sido definido el parametro SIZE " << endl;
            }
        }else{
            cout << "<< ERROR No ha sido definido el parametro" << endl;
        }
    }


}

void verificarRMdisk(NodeL *lista)
{
    QString valPath = lista->nodos.at(0).valor;
    valPath = valPath.replace("\"","");
    FILE *filep;
    if((filep=fopen(valPath.toStdString().c_str(),"r"))){
        string opcion = "";
        cout << "-- ¿Esta seguro que desea eliminar el disco? Y/N : ";
        getline(cin,opcion);
        if(opcion.compare("Y") == 0 || opcion.compare("y") == 0){
            string comando = "rm \""+valPath.toStdString()+"\"";
            system(comando.c_str());
            cout <<"El disco  fue eliminado con exito!" << endl;
        }else if(opcion.compare("N") || opcion.compare("n") == 0){
            cout << "Se ha cancelado la accion" << endl;;
        }else{
            cout << "Opcion incorrecta" << endl;
        }
        fclose(filep);
    }else{
        cout << "No existe el disco que desea eliminar" << endl;
    }
}

void verificarFDISK(NodeL *lista)
{
    bool flagSize = false;
    bool flagUnit = false;
    bool flagPath = false;
    bool flagType = false;
    bool flagFit = false;
    bool flagDelete = false;
    bool flagName = false;
    bool flagAdd = false;
    bool flag = false;
    int Size = 0;
    int valAdd = 0;
    char valU = 0;
    char valType = 0;
    char valF = 0;
    QString path = "";
    QString name = "";
    QString del = "";

    for(int i = 0; i < lista->nodos.count(); i++)
    {
        NodeL n = lista->nodos.at(i);
        switch (n.tipo_)
        {
        case SIZE:
        {
            if(flagSize){
                cout << "<< ERROR: Parametro SIZE ya fue definido" << endl;
                flag = true;
                break;
            }
            flagSize = true;
            Size = n.valor.toInt();
            if(!(Size > 0)){
                cout << "<< ERROR: Parametro SIZE debe ser mayor a cero" << endl;
                flag = true;
                break;
            }
        }
            break;
        case PATH:
        {
            if(flagPath){
                cout << "<< ERROR: El parametro PATH ya fue definido" << endl;
                flag = true;
                break;
            }
            flagPath = true;
            path = n.valor;
            path = path.replace("\"","");
        }
            break;
        case F:
        {
            if(flagFit)
            {
                cout << "<< ERROR:El parametro FIT ya fue definido" << endl;
                flag = true;
            }
            flagFit = true;
            valF = n.nodos.at(0).valor.toStdString()[0];
            if(valF == 'b'){
                valF = 'B';
            }else if(valF == 'f'){
                valF = 'F';
            }else if(valF == 'w'){
                valF = 'W';
            }
        }
            break;
        case U:
        {
            if(flagUnit){
                cout << "<< ERROR: Parametro U ya fue definido" << endl;
                flag = true;
                break;
            }
            flagUnit = true;
            string temp = n.valor.toStdString();
            valU = temp[0];
            if(valU == 'B' || valU == 'b'){
                valU = 'b';
            }else if(valU == 'K' || valU == 'k'){
                valU = 'k';
            }else if(valU == 'M' || valU == 'm'){
                valU = 'm';
            }else{
                flag = true;
                cout << "<< ERROR: El valor del parametro U no reconocido" << endl;
                break;
            }

        }
            break;

        case TYPE:
        {
            if(flagType){
                cout << "<< ERROR:El Parametro TYPE ya fue definido" << endl;
                flag = true;
                break;
            }
            flagType = true;
            string temp = n.valor.toStdString();
            valType = temp[0];
            if(valType == 'P' || valType == 'p'){
                valType = 'P';
            }else if(valType == 'E' || valType == 'e'){
                valType = 'E';
            }else if(valType == 'L' || valType == 'l'){
                valType = 'L';
            }else{
                cout << "<< ERROR: El valor del parametro TYPE no fue reconocido" << endl;
                flag = true;
                break;
            }
        }
            break;

        case DELETE:
        {
            if(flagDelete){
                cout << "<< ERROR: El Parametro DELETE ya fue definido" << endl;
                flag = true;
                break;
            }
            flagDelete = true;
            del = n.valor;
        }
            break;
        case NAME:
        {
            if(flagName){
                cout << "<< ERROR: El Parametro NAME ya fue definido" << endl;
                flag = true;
                break;
            }
            flagName = true;
            name = n.valor;
            name = name.replace("\"", "");
        }
            break;
        case ADD:
        {
            if(flagAdd){
                cout << "<< ERROR: El Parametro ADD ya fue definido" << endl;
                flag = true;
                break;
            }
            flagAdd = true;
            valAdd = n.valor.toInt();
        }
            break;
        }
    }

    if(!flag){//Flag para saber si entraron parametros repetidos
        if(flagPath){
            if(flagName){
                if(flagSize){
                    if(flagDelete || flagAdd){
                        cout << "<< ERROR: Parametros DELETE o ADD repetidos" << endl;
                    }else{
                        if(flagType){
                            if(valType == 'P'){
                                particion.crearPartPri(path, name, Size, valF, valU);
                            }else if(valType == 'E'){
                                particion.crearPartExt(path, name, Size, valF, valU);
                            }else if(valType == 'L'){
                                particion.crearPartL(path, name, Size, valF, valU);
                            }
                        }else{
                            particion.crearPartPri(path, name, Size, valF, valU);
                        }
                    }
                }else if(flagAdd){
                    if(flagSize || flagDelete){
                        cout << "<< ERROR: Parametros SIZE o DELETE repetidos" << endl;
                    }else{
                        //Verificar que la particion no este montada (falta comando montar)
                        bool montado = listaM->buscNodo(path,name);;
                        if(flagUnit){
                            if(!montado){
                                particion.comandADD(path, name, valAdd, valU);
                            }else
                                cout << "ERROR: Debe desmontar la particion para poder agregarle o quitarle" << endl;
                        }else{
                            cout << "<< ERROR parametro U no definido "<< endl;
                        }
                    }
                }else if(flagDelete){
                    if(flagSize || flagAdd || flagFit || flagType){
                        cout << "<< ERROR: Parametros repetidos" << endl;
                    }else{
                        bool mount = false;
                        if(!mount){
                            particion.eliminarPart(path, name, del);
                        }else
                            cout << "ERROR: Debe desmontar la particion para poder eliminarla" << endl;
                    }
                }
            }else {
                cout << "<< ERROR: El parametro NAME no fue definido" << endl;
            }
        }else{
            cout << "<< ERROR: El parametro PATH no fue definido" << endl;
        }
    }


}

void verificarMount(NodeL *Lista){
    QString path = "";
    QString name = "";
    bool flagPath = false;
    bool flagName = false;
    bool flag = false;
    for(int i = 0; i < Lista->nodos.count(); i++)
    {
        NodeL nodito = Lista->nodos.at(i);
        switch(nodito.tipo_)
        {
        case PATH:
        {
            if(flagPath){
                flag = true;
                cout << "ERROR: Parametro PATH ya definido" << endl;
                break;
            }
            flagPath = true;
            path = nodito.valor;
            path = path.replace("\"","");
        }
            break;
        case NAME:
        {
            if(flagName){
                flag = true;
                cout << "ERROR: Parametro NAME ya definido" << endl;
                break;
            }
            flagName = true;
            name = nodito.valor;
            name = name.replace("\"","");
        }
            break;
        }
    }
    if(!flag){
        if(flagPath){//Parametro obligatorio
            if(flagName){//Parametro obligtaorio
                int indexP = particion.buscarPartP_E(path,name);
                if(indexP != -1){
                    FILE *filep;
                    if((filep = fopen(path.toStdString().c_str(),"rb+"))){
                        MBR mbr;
                        fseek(filep, 0, SEEK_SET);
                        fread(&mbr, sizeof(MBR),1,filep);
                        mbr.mbr_partitions[indexP].part_status = '2';
                        fseek(filep,0,SEEK_SET);
                        fwrite(&mbr,sizeof(MBR),1,filep);
                        fclose(filep);
                        int letra = listaM->bLetra(path,name);
                        int num = listaM->bNum(path,name);
                        if(num == -1){
                            cout << "ERROR la particion ya esta montada" << endl;
                        }else{
                            char auxL = static_cast<char>(letra);
                            string id = "45";
                            id += auxL + to_string(num);
                            NodoM *noditoM= new NodoM(path,name,auxL,num);
                            listaM->insert(noditoM);
                            cout << "Particion montada con exito" << endl;
                            listaM->mostList();
                        }
                    }else{
                        cout << "ERROR: No se encuentra el disco" << endl;
                    }
                }else{//Posiblemente logica
                    int indexP = particion.buscarPart_L(path,name);
                    if(indexP != -1){
                        FILE *filep;
                        if((filep = fopen(path.toStdString().c_str(), "rb+"))){
                            EBR extendedBoot;
                            fseek(filep, indexP, SEEK_SET);
                            fread(&extendedBoot, sizeof(EBR),1,filep);
                            extendedBoot.part_status = '2';
                            fseek(filep,indexP,SEEK_SET);
                            fwrite(&extendedBoot,sizeof(EBR),1, filep);
                            fclose(filep);

                            int letra = listaM->bLetra(path,name);
                            if(letra == -1){
                                cout << "ERROR: la particion ya esta montada" << endl;
                            }else{
                                int num = listaM->bNum(path,name);
                                char auxLetra = static_cast<char>(letra);
                                string id = "45";
                                id += auxLetra + to_string(num);
                                NodoM *noditoM = new NodoM(path, name, auxLetra, num);
                                listaM->insert(noditoM);
                                cout << "La Particion fue montada con exito" << endl;
                                listaM->mostList();
                            }
                        }else{
                            cout << "ERROR: No se encuentra el disco" << endl;
                        }
                    }else{
                        cout << "ERROR: No se encuentra la particion a montar" << endl;
                    }
                }
            }else{
                cout << "ERROR: Parametro Name no definido" << endl;
            }
        }else{
            cout << "ERROR: Parametro PATH no definido" << endl;
        }
    }
}

void verificarUNMOUNT(NodeL *lista){
    QString id = lista->nodos.at(0).valor;
    int eliminado = listaM->deleteNodo(id);
    if(eliminado == 1)
        cout << "La unidad fue desmontada con exito" << endl;
    else
        cout << "ERROR: no se encuentra montada la unidad" << endl;
}


void Ext2(int inicio, int tamanio, QString dir){
        double n = (tamanio - static_cast<int>(sizeof(SuperBloque)))/(4 + static_cast<int>(sizeof(InodoTable)) +3*static_cast<int>(sizeof(BloqueDeArchivos)));
        int num_estructuras = static_cast<int>(floor(n));//Numero de inodos
        int num_bloques = 3*num_estructuras;
        SuperBloque superB;
        FILE *filep = fopen(dir.toStdString().c_str(),"rb+");

        superB.s_filesystem_type = 2;
        superB.s_inodes_count = num_estructuras;
        superB.s_blocks_count = num_bloques;
        superB.s_free_blocks_count = num_bloques -2;
        superB.s_free_inodes_count = num_estructuras -2;
        superB.s_mtime = time(nullptr);
        superB.s_umtime = 0;
        superB.s_mnt_count = 0;
        superB.s_magic = 0xEF53;
        superB.s_inode_size = sizeof(InodoTable);
        superB.s_block_size = sizeof(BloqueDeArchivos);
        superB.s_first_ino = 2;
        superB.s_first_blo = 2;
        superB.s_bm_inode_start = inicio + static_cast<int>(sizeof(SuperBloque));
        superB.s_bm_block_start = inicio + static_cast<int>(sizeof(SuperBloque)) + num_estructuras;
        superB.s_inode_start = inicio + static_cast<int>(sizeof (SuperBloque)) + num_estructuras + num_bloques;
        superB.s_block_start = inicio + static_cast<int>(sizeof(SuperBloque)) + num_estructuras + num_bloques + (static_cast<int>(sizeof(InodoTable))*num_estructuras);

        InodoTable inodoT;
        BloqueCarpeta bloqueC;
        char buffer = '0';
        char buffer2 = '1';
        char buffer3 = '2';

        fseek(filep,inicio,SEEK_SET);//superbloque
        fwrite(&superB,sizeof(SuperBloque),1,filep);

        for(int i = 0; i < num_estructuras; i++){//bitmap de inodos
            fseek(filep,superB.s_bm_inode_start + i,SEEK_SET);
            fwrite(&buffer,sizeof(char),1,filep);
        }
        fseek(filep,superB.s_bm_inode_start,SEEK_SET);//bit para / y users.txt en BM
        fwrite(&buffer2,sizeof(char),1,filep);
        fwrite(&buffer2,sizeof(char),1,filep);
        for(int i = 0; i < num_bloques; i++){//bitmap de bloques
            fseek(filep,superB.s_bm_block_start + i,SEEK_SET);
            fwrite(&buffer,sizeof(char),1,filep);
        }
        fseek(filep,superB.s_bm_block_start,SEEK_SET);//bit para / y users.txt en BM
        fwrite(&buffer2,sizeof(char),1,filep);
        fwrite(&buffer3,sizeof(char),1,filep);
        inodoT.i_uid = 1;//inodo para carpeta root
        inodoT.i_gid = 1;
        inodoT.i_size = 0;
        inodoT.i_atime = time(nullptr);
        inodoT.i_ctime = time(nullptr);
        inodoT.i_mtime = time(nullptr);
        inodoT.i_block[0] = 0;
        for(int i = 1; i < 15;i++){
            inodoT.i_block[i] = -1;
        }
        inodoT.i_type = '0';
        inodoT.i_perm = 664;
        fseek(filep,superB.s_inode_start,SEEK_SET);
        fwrite(&inodoT,sizeof(InodoTable),1,filep);

        //-bloque para carpeta root
        strcpy(bloqueC.b_content[0].b_name,".");//Actual (el mismo)
        bloqueC.b_content[0].b_inodo=0;

        strcpy(bloqueC.b_content[1].b_name,"..");//Padre
        bloqueC.b_content[1].b_inodo=0;

        strcpy(bloqueC.b_content[2].b_name,"users.txt");
        bloqueC.b_content[2].b_inodo=1;

        strcpy(bloqueC.b_content[3].b_name,".");
        bloqueC.b_content[3].b_inodo=-1;
        fseek(filep,superB.s_block_start,SEEK_SET);
        fwrite(&bloqueC,sizeof(BloqueCarpeta),1,filep);

        inodoT.i_uid = 1;//inodo para users.txt
        inodoT.i_gid = 1;
        inodoT.i_size = 27;
        inodoT.i_atime = time(nullptr);
        inodoT.i_ctime = time(nullptr);
        inodoT.i_mtime = time(nullptr);
        inodoT.i_block[0] = 1;
        for(int i = 1; i < 15;i++){
            inodoT.i_block[i] = -1;
        }
        inodoT.i_type = '1';
        inodoT.i_perm = 755;
        fseek(filep,superB.s_inode_start + static_cast<int>(sizeof(InodoTable)),SEEK_SET);
        fwrite(&inodoT,sizeof(InodoTable),1,filep);

        BloqueDeArchivos archivo;//Bloque para users.txt
        memset(archivo.b_content,0,sizeof(archivo.b_content));
        strcpy(archivo.b_content,"1,G,root\n1,U,root,root,123\n");
        fseek(filep,superB.s_block_start + static_cast<int>(sizeof(BloqueCarpeta)),SEEK_SET);
        fwrite(&archivo,sizeof(BloqueDeArchivos),1,filep);
        cout << "Formato Ext2" << endl;
        cout << "..." << endl;
        cout << "Disco formateado con exito!" << endl;
        fclose(filep);
}

void Ext3(int inicio, int tamanio, QString direccion){
    double n = (tamanio - static_cast<int>(sizeof(SuperBloque)))/(4 + static_cast<int>(sizeof(InodoTable)) +3*static_cast<int>(sizeof(BloqueDeArchivos)));
    int num_estructuras = static_cast<int>(floor(n));//Bitmap de indos
    int num_bloques = 3*num_estructuras;//Bitmap de bloques
    int super_size = static_cast<int>(sizeof(SuperBloque));
    int journal_size = static_cast<int>(sizeof(Journal))*num_estructuras;
    SuperBloque superB;
    InodoTable inodoT;
    BloqueCarpeta bloqueC;
    superB.s_filesystem_type = 3;
    superB.s_inodes_count = num_estructuras;
    superB.s_blocks_count = num_bloques;
    superB.s_free_blocks_count = num_bloques - 2;
    superB.s_free_inodes_count = num_estructuras - 2;
    superB.s_mtime = time(nullptr);
    superB.s_umtime = 0;
    superB.s_mnt_count = 0;
    superB.s_magic = 0xEF53;
    superB.s_inode_size = sizeof(InodoTable);
    superB.s_block_size = sizeof(BloqueDeArchivos);
    superB.s_first_ino = 2;
    superB.s_first_blo = 2;
    superB.s_bm_inode_start = inicio + super_size + journal_size;
    superB.s_bm_block_start = inicio + super_size + journal_size + num_estructuras;
    superB.s_inode_start = inicio + super_size + journal_size + num_estructuras + num_bloques;
    superB.s_block_start = inicio + super_size + journal_size + num_estructuras + num_bloques + static_cast<int>(sizeof(InodoTable))*num_estructuras;
    char buffer1 = '0';
    char buffer2 = '1';
    char buffer3 = '2';
    FILE *filep = fopen(direccion.toStdString().c_str(),"rb+");

    fseek(filep,inicio,SEEK_SET);//superbloque
    fwrite(&superB,sizeof(SuperBloque),1,filep);
    for(int i = 0; i < num_estructuras; i++){//BITMAP DE INODOS
        fseek(filep,superB.s_bm_inode_start + i,SEEK_SET);
        fwrite(&buffer1,sizeof(char),1,filep);
    }

    fseek(filep,superB.s_bm_inode_start,SEEK_SET);//bit para / y users.txt en BM
    fwrite(&buffer2,sizeof(char),1,filep);
    fwrite(&buffer2,sizeof(char),1,filep);

    for(int i = 0; i < num_bloques; i++){//BITMAP DE BLOQUES
        fseek(filep,superB.s_bm_block_start + i,SEEK_SET);
        fwrite(&buffer1,sizeof(char),1,filep);
    }

    fseek(filep,superB.s_bm_block_start,SEEK_SET);//bit para / y users.txt en BM
    fwrite(&buffer2,sizeof(char),1,filep);
    fwrite(&buffer3,sizeof(char),1,filep);

    inodoT.i_uid = 1;//inodo para carpeta root
    inodoT.i_gid = 1;
    inodoT.i_size = 0;
    inodoT.i_atime = time(nullptr);
    inodoT.i_ctime = time(nullptr);
    inodoT.i_mtime = time(nullptr);
    inodoT.i_block[0] = 0;
    for(int i = 1; i < 15;i++){
        inodoT.i_block[i] = -1;
    }
    inodoT.i_type = '0';
    inodoT.i_perm = 664;
    fseek(filep,superB.s_inode_start,SEEK_SET);
    fwrite(&inodoT,sizeof(InodoTable),1,filep);
    //-Bloque para carpeta root
    strcpy(bloqueC.b_content[0].b_name,".");//Actual
    bloqueC.b_content[0].b_inodo=0;

    strcpy(bloqueC.b_content[1].b_name,"..");//Padre
    bloqueC.b_content[1].b_inodo=0;

    strcpy(bloqueC.b_content[2].b_name,"users.txt");
    bloqueC.b_content[2].b_inodo=1;

    strcpy(bloqueC.b_content[3].b_name,".");
    bloqueC.b_content[3].b_inodo=-1;
    fseek(filep,superB.s_block_start,SEEK_SET);
    fwrite(&bloqueC,sizeof(BloqueCarpeta),1,filep);


    inodoT.i_uid = 1;//inodo para users.txt
    inodoT.i_gid = 1;
    inodoT.i_size = 27;
    inodoT.i_atime = time(nullptr);
    inodoT.i_ctime = time(nullptr);
    inodoT.i_mtime = time(nullptr);
    inodoT.i_block[0] = 1;
    for(int i = 1; i < 15;i++){
        inodoT.i_block[i] = -1;
    }
    inodoT.i_type = '1';
    inodoT.i_perm = 755;
    fseek(filep,superB.s_inode_start + static_cast<int>(sizeof(InodoTable)),SEEK_SET);
    fwrite(&inodoT,sizeof(InodoTable),1,filep);

    BloqueDeArchivos archivo;    //Bloque para users.txt
    memset(archivo.b_content,0,sizeof(archivo.b_content));
    strcpy(archivo.b_content,"1,G,root\n1,U,root,root,123\n");
    fseek(filep,superB.s_block_start + static_cast<int>(sizeof(BloqueCarpeta)),SEEK_SET);
    fwrite(&archivo,sizeof(BloqueDeArchivos),1,filep);

    cout << "Formato Ext3" << endl;
    cout << "..." << endl;
    cout << "Disco formateado con exito!" << endl;
    fclose(filep);
}

void verificarMKFS(NodeL *lista){
    bool flagId = false;
    bool flagType = false;
    bool flagFs = false;
    bool flag = false;
    QString id = "";
    QString type = "";
    int fs = 2;//si no viene un valor por defecto usa el ext2
    for(int i = 0; i < lista->nodos.count(); i++)
    {
        NodeL nodito = lista->nodos.at(i);
        switch (nodito.tipo_) {
        case ID:
        {
            if(flagId){
                cout << "ERROR: El parametro ID ya fue definido" << endl;
                flag = true;
                break;
            }
            flagId = true;
            id = nodito.valor;
        }
            break;
        case TYPE:
        {
            if(flagType){
                cout << "ERROR: El parametro TYPE ya fue definido" << endl;
                flag = true;
                break;
            }
            flagType = true;
            type = nodito.valor;
        }
            break;
        case FS:
        {
            if(flagFs){
                cout << "ERROR: El parametro FS ya fue definido " << endl;
                flag = true;
                break;
            }
            flagFs = true;
            if(nodito.valor == "3fs")
                fs = 3;
            else
                fs = 2;
        }
            break;
        }
    }
    if(!flag){
        if(flagId){
            NodoM *aux = listaM->getNodo(id);
            if(aux!=nullptr){
                int index = particion.buscarPartP_E(aux->dir,aux->name);
                if(index != -1){
                    MBR mbr;
                    FILE *filep = fopen(aux->dir.toStdString().c_str(),"rb+");
                    fread(&mbr,sizeof(MBR),1,filep);
                    int inicio = mbr.mbr_partitions[index].part_start;
                    int tamanio = mbr.mbr_partitions[index].part_size;
                    QString dir;
                    if(fs == 3){
                        Ext3(inicio, tamanio, aux->dir);
                    }
                    else{
                            Ext2(inicio, tamanio, aux->dir);
                    }
                    fclose(filep);
                }else{
                    index = particion.buscarPart_L(aux->dir,aux->name);
                }
            }else
                cout << "ERROR: No se ha encontrado ninguna particion montada con el id ingresado" << endl;
        }else
            cout << "ERROR: El parametro ID no fue definido" << endl;
    }


}




void verificarEXEC(NodeL *lista)
{
    QString path = lista->nodos.at(0).valor;
    string auxPath = path.toStdString();
    FILE *filepEx;
    if((filepEx = fopen(auxPath.c_str(),"r"))){
        char line[400]="";
        memset(line,0,sizeof(line));
        while(fgets(line,sizeof line,filepEx)){
            if(line[0]!='\n'){
                cout << "~"<< line;
                leerComand(line);
            }
            memset(line,0,sizeof(line));
        }
        fclose(filepEx);
    }else
        cout << "ERROR: script no encontrado" << endl;
}

void reconocerComando(NodeL *lista)
{
    switch (lista->tipo_)
    {
    case MKDISK:
    {
        NodeL nodo = lista->nodos.at(0);
        verificarMkdisk(&nodo);
    }
      break;
    case RMDISK:
    {
        NodeL nodo= lista->nodos.at(0);
        verificarRMdisk(lista);
        break;
    }
    case FDISK:
    {
        NodeL nodo= lista->nodos.at(0);
        verificarFDISK(&nodo);
        break;
    }
    case MOUNT:
    {
        NodeL nodo= lista->nodos.at(0);
        verificarMount(&nodo);
        break;
    }
    case UMOUNT:
    {
        verificarUNMOUNT(lista);
        break;
    }
    case MKFS:
    {
        NodeL nodo= lista->nodos.at(0);
        verificarMKFS(&nodo);
        break;
    }
    case EXEC:
    {
        verificarEXEC(lista);
        break;
    }

        break;
    default: printf("ERROR no se reconoce el comando ");
    }

}

void leerComand(char comand[400]){
    if(comand[0] != '#'){
        YY_BUFFER_STATE buffer;
        buffer = yy_scan_string(comand);
        if(yyparse() == 0){
            if(lista!=nullptr){
                reconocerComando(lista);
            }
        }else
            cout << "Error: Comando no reconocido" << endl;
    }
}
bool exit(char exit[400]){
    if(exit[0]=='e' && exit[1]=='x' && exit[2]=='i' && exit[3]=='t' ){
        return true;
    }
    else return false;

}


int main()
{
    string p="------------------------------SISTEMA DE ARCHIVOS-----------------------------\n"
             "By: Bryan Gerardo Paez Morales_______________________________________201700945\n";
    cout << p;
    string l="Ingrese un comando:\n";
    QTextStream qtin(stdin);
    char line[400];
    QString strline= line;
    while(true){
        cout << l;
        cout << "~";
        fgets(line,sizeof (line),stdin);
        if(exit(line))break;
        leerComand(line);
        memset(line,0,400);

    }


}

