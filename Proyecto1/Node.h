#ifndef NODE_H
#define NODE_H

#include <qstring.h>
#include <QList>

class Node
{
public:
    Node(QString tipo, QString valor);
    QString tipo;
    QString valor;
    int linea;
    int columna;
    int tipo_; // Este nos servirá para la ejecución, ya que aquí no hay swicht(QString); vamos a hacer un Swicht(int);
    QString cadenaDot;
    QList<Node> hijos;
    int getTipo();
    void add(Node n);
};


#endif // NODE_H
