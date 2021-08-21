#include <QCoreApplication>
#include "iostream"
#include <QString>
#include <parser.h>  // Nuestro parser
#include <scanner.h>  // Nuestro scanner
#include <QTextStream>
using namespace std;
extern int yyparse(); //
//static QList<discosmontados> discosenmemoria;
extern int linea; // Linea del token
extern int columna; // Columna de los tokens
extern int yylineno;
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
                YY_BUFFER_STATE buffer = yy_scan_string(line.toUtf8().constData());
                linea = 0;
                columna = 0;
                yylineno = 0;
                if(yyparse()==0) // Si nos da un número negativo, signifca error.
                {
                    cout<<"\n\Comando ejecutado correctamente\n\n"<<endl;

                }else {
                    cout<<"\n\nhay errores\n\n"<<endl;
                }

            }
        }

    }


}
