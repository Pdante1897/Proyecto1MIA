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
    int tipo_; // Este nos servirá para la ejecución, ya que aquí no hay swicht(QString); vamos a hacer un Swicht(int);
    QString cadenaDot;
    QList<NodeL> nodos;
    int getTipo();
    void add(NodeL n);
};


#endif // LISTA_H
