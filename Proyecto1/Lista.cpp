#include "Lista.h"

NodeL::NodeL(QString t, QString v){
    tipo =t;
    valor = v;
    tipo_ = getTipo();
    nodos = QList<NodeL>();
}

int NodeL::getTipo()
{
    if(this->tipo == "MKDISK")     return 1;
    if(this->tipo == "size")       return 2;
    if(this->tipo == "fit")        return 3;
    if(this->tipo == "unit")       return 4;
    if(this->tipo == "path")       return 5;
    if(this->tipo == "PARAMETRO")  return 6;
    if(this->tipo == "AJUSTE")     return 7;
    if(this->tipo == "RMDISK")     return 8;

    return 0;
}

void NodeL::add(NodeL n){
    nodos.append(n);
}
