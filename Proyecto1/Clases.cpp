#include "Clases.h"

NodoM::NodoM(QString dir, QString name, char let, int num)
{
    this->dir = dir;
    this->name = name;
    this->let = let;
    this->num = num;
    this->sig = nullptr;
}


ListSMount::ListSMount(){
    inicio = nullptr;
}

void ListSMount::insert(NodoM *nuevo){
    NodoM *aux = inicio;
    if(inicio == nullptr){
        inicio = nuevo;
    }else{
        while(aux->sig!=nullptr){
            aux = aux->sig;
        }
        aux->sig = nuevo;
    }

}

int ListSMount::deleteNodo(QString ID){
    NodoM *aux = inicio;
    QString prefijo = "45";
    prefijo += QString::number(aux->num) + aux->let;
    if(ID == prefijo){
        inicio = aux->sig;
        free(aux);
        return 1;
    }else{
        NodoM *aux2 = nullptr;
        while(aux!=nullptr){
            prefijo = "45";
            prefijo += QString::number(aux->num) + aux->let;
            if(ID == prefijo){
                aux2->sig = aux->sig;
                return 1;
            }
            aux2 = aux;
            aux = aux->sig;
        }
    }
    return 0;
}

int ListSMount::bNum(QString dir, QString name){
    NodoM *aux = inicio;
    int ret = 1;
    bool flag= false;
    while(aux!=NULL){
        if((dir == aux->dir) && (name == aux->name)){
            return -1;
        }else{
            if(dir == aux->dir){
                return aux->num;
            }else if(ret <= aux->num){
                ret++;
            }
        }
        aux = aux->sig;
        flag = false;
    }
    return ret;
}

int ListSMount::bLetra(QString dir, QString name){
    int ret = 'a';
    NodoM *aux = inicio;
    while(aux!=nullptr){
        if((dir == aux->dir) && (ret == aux->let)){
            ret++;
        }
        aux = aux->sig;
    }
    return ret;
}

bool ListSMount::buscNodo(QString dir, QString name){
    NodoM *aux = inicio;
    while(aux!=nullptr){
        if((aux->dir == dir) && (aux->name == name)){
            return true;
        }
        aux = aux->sig;
    }
    return false;
}

NodoM* ListSMount::getNodo(QString id){
    NodoM *aux = inicio;
    while(aux!=nullptr){
        QString prefijo = "45";
        prefijo +=   QString::number(aux->num) + aux->let;
        if(id == prefijo){
            return aux;
        }
        aux = aux->sig;
    }
    return nullptr;
}

void ListSMount::mostList(){
    cout << "---------------------------------------------------" << endl;
    cout << "|                Particiones montadas              |" << endl;
    cout << "---------------------------------------------------"<< endl;
    cout << "| Path        | Nombre          | ID               |" << endl;
    cout << "---------------------------------------------------" << endl;
    NodoM *aux = inicio;
    while(aux!=nullptr){
        cout << "| "<< aux->dir.toStdString()<<"        " << aux->name.toStdString() << "          " << "45"<<aux->num<<aux->let << endl;
    cout << "---------------------------------------------------" << endl;
        aux = aux->sig;
    }
}

bool ListSMount::existeDir(QString dir){
    NodoM *aux = inicio;
    while(aux!=nullptr){
        if((aux->dir == dir)){
            return true;
        }
        aux = aux->sig;
    }
    return false;
}
