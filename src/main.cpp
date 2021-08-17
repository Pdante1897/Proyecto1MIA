#include <QCoreApplication>
#include "iostream"
#include <QString>
#include <QTextStream>
using namespace std;
extern int yyparse(); //
//static QList<discosmontados> discosenmemoria;
int p;
extern int linea; // Linea del token
extern int columna; // Columna de los tokens
extern int yylineno;
int main(int argc, char *argv[])
{

    string p="------------------------------Ingrese un comando------------------------------\n";



    QTextStream qtin(stdin);
    QString line;

    while(line!="salir"){    //esto me sirve para seguir leyendo siempre los comandos sin salirme
        cout << p;
        line = qtin.readLine();
       
        }

    }


}