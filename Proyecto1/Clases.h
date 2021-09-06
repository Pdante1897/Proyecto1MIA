#ifndef CLASES_H
#define CLASES_H
#include <qstring.h>
#include <iostream>

using namespace std;


class NodoM
{
public:
    NodoM(QString, QString, char, int);
    QString dir;
    QString name;
    char let;
    int num;
    NodoM *sig;
};



class ListSMount//lista simplemente enlazada
{
public:
    NodoM *inicio;
    ListSMount();
    int bLetra(QString dir, QString name);
    int bNum(QString dir,   QString name);
    void mostList();
    void insert(NodoM*);
    int deleteNodo(QString);
    bool buscNodo(QString, QString);
    QString getDir(QString);
    NodoM* getNodo(QString);
};

#endif // CLASES_H
