%{
    #include <iostream>
    #include "parser.h"
    char coment[400];
    int yylex();
    void yyerror(const char *s);
    int linea = 0;
    int columna = 1;
    int anterior = 0;
    NodeL *lista;
%}

%option case-insensitive
%option noyywrap

letra [a-zA-Z]
entero ("-")?[0-9]+
caracter ({letra}|{entero})
id {letra}({letra}|{entero}|"_")*
idmount {entero}{letra}

password ({letra}|{entero}|[!$@+*])+
cadena "\""[^"\""]*"\""
extension "."{id}
diagonal "/"
ruta ({diagonal}{id})*({diagonal}{id}{extension})
directorio ({diagonal}{id})+

%x COM

%%
INITIAL
{
    "#"         { BEGIN COM; }

    "mkdisk"    { return mkdisk; }
    "rmdisk"    { return rmdisk; }
    "fdisk"     { return fdisk; }
    "mount"     { return mount; }
    "umount"   { return umount; }
    "rep"       { return rep; }
    "exec"      { return exec; }
    "-size"     { return size; }
    "-u"     { return unit; }
    "-path"     { return path; }
    "-f"      { return fit; }
    "-name"     { return name; }
    "-type"     { return type; }
    "-delete"   { return del; }
    "-add"      { return add; }
    "-id"       { return id; }
    "bf"        { return bf; }
    "ff"        { return ff; }
    "wf"        { return wf; }
    "Fast"      { return fast; }
    "Full"      { return full; }
    "mbr"       { return mbr; }
    "disk"      { return disk; }


    "mkfs"       { return mkfs; }
    "login"      { return login; }
    "logout"     { return logout; }
    "mkgrp"      { return mkgrp; }
    "rmgrp"      { return rmgrp; }
    "mkusr"      { return mkusr; }
    "rmusr"      { return rmusr; }
    "chmod"      { return Rchmod; }
    "mkfile"     { return mkfile; }
    "cat"        { return cat; }
    "rem"        { return rem; }
    "edit"       { return edit; }
    "ren"        { return ren; }
    "mkdir"      { return Rmkdir; }
    "cp"         { return cp; }
    "mv"         { return mv; }
    "find"       { return find; }
    "chown"      { return Rchown; }
    "chgrp"      { return chgrp; }
    "pause"      { return pausa; }
    "recovery"   { return recovery; }
    "loss"       { return loss; }
    "-fs"        { return fs; }
    "2fs"        { return fs2; }
    "3fs"        { return fs3; }
    "-usr"       { return usr; }
    "-pwd"       { return pwd;  }
    "-grp"       { return grp; }
    "-ugo"       { return ugo; }
    "-r"         { return r; }
    "-p"         { return p; }
    "-cont"      { return cont; }
    "-file"      { return file; }
    "-dest"      { return dest; }
    "-ruta"      { return rutaRep; }
    "inode"      { return inode; }
    "journaling" { return journaling; }
    "block"      { return block; }
    "bm_inode"   { return bm_inode; }
    "bm_block"   { return bm_block; }
    "tree"       { return tree; }
    "sb"         { return sb; }
    "file"       { return fileRep; }
    "ls"         { return ls; }


    "="         { return igual; }

    {entero}    {strcpy(yylval.text, yytext); return num;}
    {caracter}  {strcpy(yylval.text, yytext); return caracter;}
    {cadena}    {strcpy(yylval.text, yytext); return cadena;}
    {id}        {strcpy(yylval.text, yytext); return identificador;}
    {idmount}        {strcpy(yylval.text, yytext); return idmount;}
    {ruta}      {strcpy(yylval.text, yytext); return ruta;}
    {extension} {strcpy(yylval.text, yytext); return extension;}
    {password}  {strcpy(yylval.text, yytext); return password; }
    {directorio} {strcpy(yylval.text, yytext); return directorio;}

    [[:blank:]] {}

    .            {std::cout <<yytext <<" Error Lexico" << std::endl;}
}

<COM>"\n"        { BEGIN INITIAL; /*strcpy(yylval.text, coment); memset(coment,0,400); return comentario;*/ };
<COM>.           { /*strcat(coment, yytext);*/ };

%%
