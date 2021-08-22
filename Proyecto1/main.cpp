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
    PATH= 5
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

    while(line!="salir"){    //esto me sirve para seguir leyendo siempre los comandos sin salirme
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

