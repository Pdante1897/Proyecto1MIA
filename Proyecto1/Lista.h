#ifndef LISTA_H
#define LISTA_H

#include <qstring.h>
#include <QList>

class NodeL
{
public:
    NodeL(QString tipo, QString valor);
    QString tipo;
    QString valor;
    int linea;
    int columna;
    int tipo_;
    QString cadenaDot;
    QList<NodeL> nodos;
    int getTipo();
    void add(NodeL n);
};


#endif // LISTA_H
