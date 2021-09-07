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
    if(this->tipo == "FDISK")     return 9;
    if(this->tipo == "TYPE")     return 10;
    if(this->tipo == "DELETE")     return 11;
    if(this->tipo == "NAME")     return 12;
    if(this->tipo == "ADD")     return 13;
    if(this->tipo == "MOUNT")     return 14;
    if(this->tipo == "UMOUNT")     return 15;
    if(this->tipo == "id")         return 16;
    if(this->tipo == "MKFS")        return 17;


    return 0;
}

void NodeL::add(NodeL n){
    nodos.append(n);
}
