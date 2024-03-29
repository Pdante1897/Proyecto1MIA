%{
    #include "scanner.h"
    #include "Lista.h"
    #include <iostream>

    extern int yylineno; //linea actual donde se encuentra el parser (analisis lexico) lo maneja BISON
    extern int columna; //columna actual donde se encuentra el parser (analisis lexico) lo maneja BISON
    extern char *yytext; //lexema actual donde esta el parser (analisis lexico) lo maneja BISON
    extern NodeL *lista; // Raiz del arbol

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
        class NodeL *NodeL;
    }

/*--------------Terminals-------------*/
%token <text> mkdisk
%token <text> rmdisk
%token <text> fdisk
%token <text> mount
%token <text> umount
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
%token <text> idmount
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
%token <text> directorio

/*----------Not terminals------------*/
%type <NodeL> INICIO
%type <NodeL> COMANDO
%type <NodeL> MKDISK
%type <NodeL> PARAMETROMK
%type <NodeL> AJUSTE
%type <NodeL> RMDISK
%type <NodeL> FDISK
%type <NodeL> PARAMETROF
%type <NodeL> MOUNT
%type <NodeL> PARAMETRO_M
%type <NodeL> UMOUNT
%type <NodeL> MKFS
%type <NodeL> PARAM_MKFS
%type <NodeL> EXEC
%type <NodeL> REP
%type <NodeL> PARAMETRO_REP


%start INICIO

%%

INICIO:  COMANDO {lista = new NodeL("",""); lista=$$; };

COMANDO: mkdisk MKDISK {$$ = new NodeL("MKDISK",""); $$->add(*$2);}
        | RMDISK { $$ = $1; }
        | fdisk FDISK { $$ = new NodeL("FDISK","");
                        $$->add(*$2);
                        }
        | mount MOUNT {
                         $$ = new NodeL("MOUNT", "");
                         $$->add(*$2);
                       }
        | UMOUNT { $$ = $1; }
        | mkfs MKFS {
                        $$ = new NodeL("MKFS","");
                        $$->add(*$2);
                     }
        | EXEC { $$ = $1; }
        | rep REP { $$ = new NodeL("REP","");
                     $$->add(*$2);
                   };



MKDISK: MKDISK PARAMETROMK {$$ = $1; 
                            $$->add(*$2);}
        |PARAMETROMK {$$ = new NodeL("PARAMETRO",""); 
                        $$->add(*$1); };

PARAMETROMK: size igual num { $$= new NodeL("size",$3); }
            |fit igual AJUSTE {$$ = new NodeL ("fit", ""); 
                                $$->add(*$3);}
            |unit igual caracter { $$ = new NodeL("unit",$3);}
            |path igual cadena {$$ = new NodeL("path",$3);}
            |path igual ruta {$$ = new NodeL("path",$3);};
AJUSTE: bf { $$ = new NodeL("AJUSTE", "bf"); }
        | ff { $$ = new NodeL("AJUSTE", "ff"); }
        | wf { $$ = new NodeL("AJUSTE", "wf"); };
RMDISK: rmdisk path igual ruta {
                                $$ = new NodeL("RMDISK","");
                                NodeL *n = new NodeL("path",$4);
                                $$->add(*n);
                               }
         | rmdisk path igual cadena {
                                      $$ = new NodeL("RMDISK","");
                                      NodeL *ruta = new NodeL("path",$4);
                                      $$->add(*ruta);
                                    };
FDISK: FDISK PARAMETROF {
                            $$ = $1;
                            $$->add(*$2);
                          }
        | PARAMETROF {
                        $$ = new NodeL("PARAMETRO","");
                        $$->add(*$1);
                      };
PARAMETROF: PARAMETROMK { $$ = $1; }
              | type igual caracter { $$ = new NodeL("TYPE",$3); }
              | del igual fast { $$ = new NodeL("DELETE", "fast"); }
              | del igual full { $$ = new NodeL("DELETE", "full"); }
              | name igual identificador { $$ = new NodeL("NAME", $3); }
              | name igual cadena { $$ = new NodeL("NAME", $3); }
              | add igual num { $$ = new NodeL("ADD", $3); };

MOUNT: MOUNT PARAMETRO_M {
                           $$ = $1;
                           $$->add(*$2);
                         }
       | PARAMETRO_M {
                        $$ = new NodeL("PARAMETRO","");
                        $$->add(*$1);
                      };

PARAMETRO_M: path igual cadena { $$ = new NodeL("path",$3); }
             | path igual ruta { $$ = new NodeL("path", $3); }
             | name igual identificador { $$ = new NodeL("NAME", $3); }
             | name igual cadena { $$ = new NodeL("NAME",$3); };

UMOUNT: umount id igual idmount {
                                          $$ = new NodeL("UMOUNT", "");
                                          NodeL *n = new NodeL("id",$4);
                                          $$->add(*n);
                                        };


MKFS: MKFS PARAM_MKFS {
                        $$ = $1;
                        $$->add(*$2);
                      }
      | PARAM_MKFS {
                      $$ = new NodeL("PARAMETRO", "");
                      $$->add(*$1);
                   };

PARAM_MKFS: id igual idmount{ $$ = new NodeL("ident",$3); }
            | type igual fast { $$ = new NodeL("type", "fast"); }
            | type igual full { $$ = new NodeL("type", "full"); }
            | fs igual fs2 { $$ = new NodeL("FS", "2fs"); }
            | fs igual fs3 { $$ = new NodeL("FS", "3fs"); };

EXEC: exec path igual cadena {
              $$ = new NodeL("EXEC","");
              NodeL *path = new NodeL("path", $4);
              $$->add(*path);
              }
        | exec path igual ruta {
              $$ = new NodeL("EXEC","");
              NodeL *n = new NodeL("path", $4);
              $$->add(*n);
              };
              
REP: REP PARAMETRO_REP{
                     $$ = $1;
                     $$->add(*$2);
                    }
     | PARAMETRO_REP
    {
                    $$ = new NodeL("PARAMETRO", "");
                    $$->add(*$1);
                  };

PARAMETRO_REP: name igual mbr { $$ = new NodeL("NAME","mbr"); }
             | name igual disk { $$ = new NodeL("NAME","disk"); }
             | path igual cadena{ $$ = new NodeL("path", $3); }
             | path igual ruta { $$ = new NodeL("path",$3); }
             | id igual idmount { $$ = new NodeL("ident", $3); };