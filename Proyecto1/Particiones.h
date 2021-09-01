#ifndef PARTICIONES_H
#define PARTICIONES_H

#include <QString>
#include <string>
#include <iostream>
#include "structs.h"

using namespace std;

class Particion
{
public:
    Particion();
    void crearPartPri(QString, QString, int, char, char);
    void crearPartExt(QString, QString, int, char, char);
    void crearPartL(QString, QString, int, char, char) ;
    void eliminarPart(QString, QString, QString);
    void comandADD(QString, QString ,int ,char);
    bool existePart(QString, QString);
    int buscarPartP_E(QString, QString);
    int buscarPart_L(QString, QString);
};

#endif // PARTICIONES_H
