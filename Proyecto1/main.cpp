#include <QCoreApplication>
#include "iostream"
#include <QString>
#include <parser.h>
#include <scanner.h>
#include <QTextStream>
#include "Lista.h"
#include "structs.h"
#include "Particiones.h"

using namespace std;
extern int yyparse();
extern int linea;
extern int columna;
extern int yylineno;
extern NodeL *lista;
Particion particion;

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
    ADD = 13
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
        cout << ">> ¿Esta seguro que desea eliminar el disco? Y/N : ";
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
    int valSize = 0;
    int valAdd = 0;
    char valU = 0;
    char valType = 0;
    char valF = 0;
    QString valPath = "";
    QString valName = "";
    QString valDelete = "";

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
            valSize = n.valor.toInt();
            if(!(valSize > 0)){
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
            valPath = n.valor;
            valPath = valPath.replace("\"","");
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
            valDelete = n.valor;
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
            valName = n.valor;
            valName = valName.replace("\"", "");
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
                                particion.crearPartPri(valPath, valName, valSize, valF, valU);
                            }else if(valType == 'E'){
                                particion.crearPartExt(valPath, valName, valSize, valF, valU);
                            }else if(valType == 'L'){
                                particion.crearPartL(valPath, valName, valSize, valF, valU);
                            }
                        }else{
                            particion.crearPartPri(valPath, valName, valSize, valF, valU);
                        }
                    }
                }else if(flagAdd){
                    if(flagSize || flagDelete){
                        cout << "<< ERROR: Parametros SIZE o DELETE repetidos" << endl;
                    }else{
                        //Verificar que la particion no este montada (falta comando montar)
                        bool montado = false;
                        if(flagUnit){
                            if(!montado){
                                particion.comandADD(valPath, valName, valAdd, valU);
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
                            particion.eliminarPart(valPath, valName, valDelete);
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
    default: printf("ERROR no se reconoce el comando");
    }

}



int main()
{
    string p="------------------------------SISTEMA DE ARCHIVOS-----------------------------\n"
             "By: Bryan Gerardo Paez Morales_______________________________________201700945\n";

    string l="Ingrese un comando:\n";
    cout << p;
    QTextStream qtin(stdin);
    QString line;

    while(line!="exit"){
        cout << l;
        line = qtin.readLine();
        if(line!="exit"){
            if(line.isEmpty()==false){
                YY_BUFFER_STATE buffer;
                buffer = yy_scan_string(line.toUtf8().constData());;
                if(yyparse() == 0){
                    if(lista!=nullptr){
                        reconocerComando(lista);
                    }
                }else
                    cout << "Comando no reconocido" << endl;

            }
        }

    }


}

