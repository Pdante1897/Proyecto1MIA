#include <QCoreApplication>
#include "iostream"
#include <QString>
#include <parser.h>  // Nuestro parser
#include <scanner.h>  // Nuestro scanner
#include <QTextStream>
#include "Lista.h"
#include "structs.h"

using namespace std;
extern int yyparse(); //
//static QList<discosmontados> discosenmemoria;
extern int linea; // Linea del token
extern int columna; // Columna de los tokens
extern int yylineno;
extern NodeL *lista;


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




QString getDirectorio(QString direccion){
    string aux = direccion.toStdString();
    string delimiter = "/";
    size_t pos = 0;
    string res = "";
    while((pos = aux.find(delimiter))!=string::npos){
        res += aux.substr(0,pos)+"/";
        aux.erase(0,pos + delimiter.length());
    }
    return QString::fromStdString(res);
}

void crearDisco(QString direccion){
    QString aux = getDirectorio(direccion);
    string comando = "sudo mkdir -p \'"+aux.toStdString()+"\'";
    system(comando.c_str());
    string comando2 = "sudo chmod -R 777 \'"+aux.toStdString()+"\'";
    system(comando2.c_str());
    string arch = direccion.toStdString();
    FILE *fp = fopen(arch.c_str(),"wb");
    if((fp = fopen(arch.c_str(),"wb")))
        fclose(fp);
    else
        cout << "Error al crear el archivo" << endl;
}

QString getFileName(QString direccion){
    string aux = direccion.toStdString();
    string delimiter = "/";
    size_t pos = 0;
    string res = "";
    while((pos = aux.find(delimiter))!=string::npos){
        aux.erase(0,pos + delimiter.length());
    }
    delimiter = ".";
    pos = aux.find(delimiter);
    res = aux.substr(0,pos);
    return QString::fromStdString(res);
}

void verificarMkdisk(NodeL *nodo)
{
    //las flags son para verificar si se repite el parametro ingresado
    bool flagSize = false;
    bool flagPath = false;
    bool flagFit = false;
    bool flagUnit = false;
    bool flag = false;//Si se repite un valor se activa esta bandera
    /*Variables para obtener los valores de cada nodo*/
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
                break; //ERROR
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
            if(flagFit){
                cout << "ERROR: Ya fue definido el parametro F" << endl;
                flag = true;
                break; //ERROR
            }
            flagFit = true;
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
            if(flagUnit){
                cout << "ERROR: Ya fue definido el parametro U" << endl;
                flag = true;
                break;// ERROR;
            }
            flagUnit = true;
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
            valPath = n.valor;//Quitarle comillas si tiene
            valPath = valPath.replace("\"","");
        }
            break;
        }

    }

    if(!flag){//Flag para ver si hay parametros repetidos
        if(flagPath){//Parametro path obligatorio
            if(flagSize){//Parametro size obligatorio
                MBR mbr;
                int total_size = 1024;
                //Archivo principal
                crearDisco(valPath);
                //Archivo raid
                mbr.mbr_date_created = time(nullptr);
                mbr.mbr_disk_signature = static_cast<int>(time(nullptr));

                if(flagUnit){//Si hay parametro unit
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

                if(flagFit)//Si hay parametro fit
                    mbr.mbr_disk_fit = valFit;
                else
                    mbr.mbr_disk_fit = 'F';

                //Se inicializan las particiones en el MBR
                for(int p = 0; p < 4; p++){
                    mbr.mbr_partition[p].part_status = '0';
                    mbr.mbr_partition[p].part_type = '0';
                    mbr.mbr_partition[p].part_fit = '0';
                    mbr.mbr_partition[p].part_size = 0;
                    mbr.mbr_partition[p].part_start = -1;
                    strcpy(mbr.mbr_partition[p].part_name,"");
                }
                //Comando para genera un archivo de cierto tamano
                string comando = "dd if=/dev/zero of=\""+valPath.toStdString()+"\" bs=1024 count="+to_string(total_size);
                system(comando.c_str());
                FILE *fp = fopen(valPath.toStdString().c_str(),"rb+");
                fseek(fp,0,SEEK_SET);
                fwrite(&mbr,sizeof(MBR),1,fp);
                fclose(fp);
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
    FILE *fp;
    if((fp=fopen(valPath.toStdString().c_str(),"r"))){
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
        fclose(fp);
    }else{
        cout << "No existe el disco que desea eliminar" << endl;
    }
}

void verificarFDISK(NodeL *lista)
{
    /*Banderas para verificar cuando venga un parametro y si se repite*/
    bool flagSize = false;
    bool flagUnit = false;
    bool flagPath = false;
    bool flagType = false;
    bool flagFit = false;
    bool flagDelete = false;
    bool flagName = false;
    bool flagAdd = false;
    bool flag = false;
    /*Variables para obtener los valores de cada nodo*/
    int valSize = 0;
    int valAdd = 0;
    char valUnit = 0;
    char valType = 0;
    char valFit = 0;
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
                cout << "<< ERROR: Parametro -size ya definido" << endl;
                flag = true;
                break;
            }
            flagSize = true;
            valSize = n.valor.toInt();
            if(!(valSize > 0)){
                cout << "<< ERROR: parametro -size menor a cero" << endl;
                flag = true;
                break;
            }
        }
            break;
        case U:
        {
            if(flagUnit){
                cout << "<< ERROR: Parametro -unit ya definido" << endl;
                flag = true;
                break;
            }
            flagUnit = true;
            string temp = n.valor.toStdString();
            valUnit = temp[0];
            if(valUnit == 'B' || valUnit == 'b'){
                valUnit = 'b';
            }else if(valUnit == 'K' || valUnit == 'k'){
                valUnit = 'k';
            }else if(valUnit == 'M' || valUnit == 'm'){
                valUnit = 'm';
            }else{
                cout << "<< ERROR: Valor del parametro unit no reconocido" << endl;
                flag = true;
                break;
            }

        }
            break;
        case PATH:
        {
            if(flagPath){
                cout << "<< ERROR: Parametro -path ya definido" << endl;
                flag = true;
                break;
            }
            flagPath = true;
            valPath = n.valor;
            valPath = valPath.replace("\"","");
        }
            break;
        case TYPE:
        {
            if(flagType){
                cout << "<< ERROR: Parametro -type ya definido" << endl;
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
                cout << "<< ERROR: Valor del parametro -type no reconocido" << endl;
                flag = true;
                break;
            }
        }
            break;
        case F:
        {
            if(flagFit)
            {
                cout << "<< ERROR: Parametro -fit ya definido" << endl;
                flag = true;
                //ERROR
            }
            flagFit = true;
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
        case DELETE:
        {
            if(flagDelete){
                cout << "<< ERROR: Parametro -delete ya definido" << endl;
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
                cout << "<< ERROR: Parametro -name ya definido" << endl;
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
                cout << "<< ERROR: Parametro -add ya definido" << endl;
                flag = true;
                break;
            }
            flagAdd = true;
            valAdd = n.valor.toInt();
        }
            break;
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
        verificarMkdisk(&nodo);//mandamos a verificar la lista de parametros de mkdisk, si son correctos creara el disco.
    }
      break;
    case RMDISK:
    {
        NodeL nodo= lista->nodos.at(0);
        verificarRMdisk(lista);//mandamos a verificar la lista de parametros de RMdisk, si son correctos borrara el disco
        break;
    }
    case FDISK:
    {
        NodeL nodo= lista->nodos.at(0);
        verificarFdisk(&nodo);//mandamos a verificar la lista de parametros de fdisk, si son correctos formateara el disco
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

    while(line!="exit"){    //esto me sirve para seguir leyendo siempre los comandos sin salirme
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

