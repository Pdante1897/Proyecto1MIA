#include "Node.h"

Node::Node(QString t, QString v){
    tipo =t;
    valor = v;
    tipo_ = getTipo();
    hijos = QList<Node>();
}

int Node::getTipo()
{
    if(this->tipo == "MKDISK")     return 1;

    return 0;
}

void Node::add(Node n){
    hijos.append(n);
}
