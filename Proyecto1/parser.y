%{
    #include "scanner.h"
    #include "Node.h"
    #include <iostream>

    extern int yylineno; //linea actual donde se encuentra el parser (analisis lexico) lo maneja BISON
    extern int columna; //columna actual donde se encuentra el parser (analisis lexico) lo maneja BISON
    extern char *yytext; //lexema actual donde esta el parser (analisis lexico) lo maneja BISON
    extern Node *raiz; // Raiz del arbol

    int yyerror(const char* mens){
        std::cout<<mens<<std::endl;
        return 0;
    }


%}

%defines "parser.h"
%output "parser.cpp"
%define parse.error verbose
%union
    {
        char text[400];
        class Node *nodito;
    }

/*--------------Terminals-------------*/
%token <text> mkdisk
%token <text> rmdisk
%token <text> fdisk
%token <text> mount
%token <text> unmount
%token <text> rep
%token <text> exec
%token <text> size
%token <text> unit
%token <text> path
%token <text> fit
%token <text> name
%token <text> type
%token <text> del
%token <text> add
%token <text> id
%token <text> bf
%token <text> ff
%token <text> wf
%token <text> fast
%token <text> full
%token <text> mbr
%token <text> disk
%token <text> igual
%token <text> extension
%token <text> num
%token <text> caracter
%token <text> cadena
%token <text> identificador
%token <text> ruta

%token <text> mkfs
%token <text> login
%token <text> logout
%token <text> mkgrp
%token <text> rmgrp
%token <text> mkusr
%token <text> rmusr
%token <text> Rchmod
%token <text> mkfile
%token <text> cat
%token <text> rem
%token <text> edit
%token <text> ren
%token <text> Rmkdir
%token <text> cp
%token <text> mv
%token <text> find
%token <text> Rchown
%token <text> chgrp
%token <text> pausa
%token <text> recovery
%token <text> loss
%token <text> fs
%token <text> fs2
%token <text> fs3
%token <text> usr
%token <text> pwd
%token <text> grp
%token <text> ugo
%token <text> r
%token <text> p
%token <text> cont
%token <text> file
%token <text> dest
%token <text> rutaRep
%token <text> inode
%token <text> journaling
%token <text> block
%token <text> bm_inode
%token <text> bm_block
%token <text> tree
%token <text> sb
%token <text> fileRep
%token <text> ls
%token <text> password
%token <text> directorio

/*----------Not terminals------------*/
%type <nodito> INIT
%type <nodito> COMANDO
%type <nodito> MKDISK
%type <nodito> PARAMETRO_MK

%type <nodito> AJUSTE

%start INIT

%%

INIT:  COMANDO { };

COMANDO: mkdisk MKDISK {

                       }




MKDISK: MKDISK PARAMETRO_MK {
                            }
        |PARAMETRO_MK {
                     };

PARAMETRO_MK: size igual num { }
            |fit igual AJUSTE {

                               }
            |unit igual caracter { }
            |path igual cadena {
                                }
            |path igual ruta {
                             };
AJUSTE: bf {}
        | ff { }
        | wf {  };
