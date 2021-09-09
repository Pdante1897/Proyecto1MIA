#ifndef GRAFICAR_H
#define GRAFICAR_H
#include <QString>
#include <string>
#include <iostream>
#include "structs.h"

using namespace std;

class Reporte
{
public:
    Reporte();
    void graficarMBR(QString, QString, QString);
    void graficarDisco(QString, QString, QString);
};

#endif // GRAFICAR_H
