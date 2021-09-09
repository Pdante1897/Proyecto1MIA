#include "Graficar.h"

Reporte::Reporte()
{

}

void Reporte::graficarDisco(QString direccion, QString destino, QString extension){
    FILE *filep;
    FILE *fileDot;
    if((filep = fopen(direccion.toStdString().c_str(),"r"))){
        fileDot = fopen("grafica.dot","w");
        fprintf(fileDot,"digraph G{\n\n");
        fprintf(fileDot, "  tbl [\n    shape=box\n    label=<\n");
        fprintf(fileDot, "     <table border=\'0\' cellborder=\'2\' width=\'600\' height=\"200\" color=\'LIGHTSTEELBLUE\'>\n");
        fprintf(fileDot, "     <tr>\n");
        fprintf(fileDot, "     <td height=\'200\' width=\'100\'> MBR </td>\n");
        MBR mbr;
        fseek(filep,0,SEEK_SET);
        fread(&mbr,sizeof(MBR),1,filep);
        int total = mbr.mbr_size;
        double espUsado = 0;
        for(int i = 0; i < 4; i++){
            int parcial = mbr.mbr_partitions[i].part_size;
            if(mbr.mbr_partitions[i].part_start != -1){//Particion vacia
                double porcen_real = (parcial*100)/total;
                double porcentaje_aux = (porcen_real*500)/100;
                espUsado += porcen_real;
                if(mbr.mbr_partitions[i].part_status != '1'){
                    if(mbr.mbr_partitions[i].part_type == 'P'){
                        fprintf(fileDot, "     <td height=\'200\' width=\'%.1f\'>PRIMARIA <br/> Ocupado: %.1f%c</td>\n",porcentaje_aux,porcen_real,'%');
                        //Verificar que no haya espacio fragmentado
                        if(i!=3){
                            int part1 = mbr.mbr_partitions[i].part_start + mbr.mbr_partitions[i].part_size;
                            int part2 = mbr.mbr_partitions[i+1].part_start;
                            if(mbr.mbr_partitions[i+1].part_start != -1){
                                if((part2-part1)!=0){//Hay fragmentacion
                                    int fragmentacion = part2-part1;
                                    double porcentaje_real = (fragmentacion*100)/total;
                                    double porcentaje_aux = (porcentaje_real*500)/100;
                                    fprintf(fileDot,"     <td height=\'200\' width=\'%.1f\'>LIBRE<br/> Ocupado: %.1f%c</td>\n",porcentaje_aux,porcentaje_real,'%');
                                }
                            }

                        }else{
                            int p1 = mbr.mbr_partitions[i].part_start + mbr.mbr_partitions[i].part_size;
                            int mbr_size = total + (int)sizeof(MBR);
                            if((mbr_size-p1)!=0){//Libre
                                double libre = (mbr_size - p1) + sizeof(MBR);
                                double porcentaje_real = (libre*100)/total;
                                double porcentaje_aux = (porcentaje_real*500)/100;
                                fprintf(fileDot, "     <td height=\'200\' width=\'%.1f\'>LIBRE<br/> Ocupado: %.1f%c</td>\n",porcentaje_aux, porcentaje_real, '%');
                            }
                        }
                    }else{//Extendida
                        EBR ebr;
                        fprintf(fileDot,"     <td  height=\'200\' width=\'%.1f\'>\n     <table border=\'0\'  height=\'200\' WIDTH=\'%.1f\' cellborder=\'1\'>\n",porcen_real,porcen_real);
                        fprintf(fileDot,"     <tr>  <td height=\'60\' colspan=\'15\'>EXTENDIDA</td>  </tr>\n     <tr>\n");
                        fseek(filep, mbr.mbr_partitions[i].part_start,SEEK_SET);
                        fread(&ebr,sizeof(EBR),1,filep);
                        if(ebr.part_size != 0){//Si hay mas de alguna logica
                            fseek(filep, mbr.mbr_partitions[i].part_start,SEEK_SET);
                            while(fread(&ebr,sizeof (EBR),1,filep)!=0 && (ftell(filep) < (mbr.mbr_partitions[i].part_start + mbr.mbr_partitions[i].part_size))){
                                parcial = ebr.part_size;
                                porcen_real = (parcial*100)/total;
                                if(porcen_real!=0){
                                    if(ebr.part_status != '1'){
                                        fprintf(fileDot, "     <td height=\'140\'>EBR</td>\n");
                                        fprintf(fileDot, "     <td height=\'140\'>LOGICA<br/>Ocupado: %.1f%c</td>\n",porcen_real,'%');
                                    }else{//Espacio no asignado
                                        fprintf(fileDot, "      <td height=\'150\'>LIBRE 1 <br/> Ocupado: %.1f%c</td>\n",porcen_real,'%');
                                    }
                                    if(ebr.part_next==-1){
                                        parcial = (mbr.mbr_partitions[i].part_start + mbr.mbr_partitions[i].part_size) - (ebr.part_start + ebr.part_size);
                                        porcen_real = (parcial*100)/total;
                                        if(porcen_real!=0){
                                            fprintf(fileDot, "     <td height=\'150\'>LIBRE 2<br/> Ocupado: %.1f%c </td>\n",porcen_real,'%');
                                        }
                                        break;
                                    }else
                                        fseek(filep,ebr.part_next,SEEK_SET);
                                }
                            }
                        }else{
                            fprintf(fileDot,"     <td height=\'140\'> Ocupado %.1f%c</td>",porcen_real,'%');
                        }
                        fprintf(fileDot,"     </tr>\n     </table>\n     </td>\n");
                        //Verificar que no haya espacio fragmentado
                        if(i!=3){
                            int p1 = mbr.mbr_partitions[i].part_start + mbr.mbr_partitions[i].part_size;
                            int p2 = mbr.mbr_partitions[i+1].part_start;
                            if(mbr.mbr_partitions[i+1].part_start != -1){
                                if((p2-p1)!=0){//Hay fragmentacion
                                    int fragmentacion = p2-p1;
                                    double porcentaje_real = (fragmentacion*100)/total;
                                    double porcentaje_aux = (porcentaje_real*500)/100;
                                    fprintf(fileDot,"     <td height=\'200\' width=\'%.1f\'>LIBRE<br/> Ocupado: %.1f%c</td>\n",porcentaje_aux,porcentaje_real,'%');
                                }
                            }
                        }else{
                            int p1 = mbr.mbr_partitions[i].part_start + mbr.mbr_partitions[i].part_size;
                            int mbr_size = total + (int)sizeof(MBR);
                            if((mbr_size-p1)!=0){//Libre
                                double libre = (mbr_size - p1) + sizeof(MBR);
                                double porcentaje_real = (libre*100)/total;
                                double porcentaje_aux = (porcentaje_real*500)/100;
                                fprintf(fileDot, "     <td height=\'200\' width=\'%.1f\'>LIBRE<br/> Ocupado: %.1f%c</td>\n",porcentaje_aux, porcentaje_real, '%');
                            }
                        }
                    }
                }else{//Espacio no asignado
                     fprintf(fileDot,"     <td height=\'200\' width=\'%.1f\'>LIBRE <br/> Ocupado: %.1f%c</td>\n",porcentaje_aux,porcen_real,'%');
                }
            }
        }

        //fprintf(graphDot,"     <td height='200'> ESPACIO LIBRE <br/> Ocupado: %.1f%c\n     </td>",(100-espacioUsado),'%');


        fprintf(fileDot,"     </tr> \n     </table>        \n>];\n\n}");
        fclose(fileDot);
        fclose(filep);
        string comando = "dot -T"+extension.toStdString()+" grafica.dot -o "+destino.toStdString();
        system(comando.c_str());
        cout << "Reporte generado con exito " << endl;
    }else{
        cout << "ERROR al crear reporte, disco no encontrado" << endl;
    }
}


void Reporte::graficarMBR(QString direccion, QString destino, QString extension){
    string auxDir = direccion.toStdString();
    FILE *fp;
    FILE *graphDot;
    if((fp = fopen(auxDir.c_str(),"r"))){
        graphDot = fopen("grafica.dot", "w");
        fprintf(graphDot,"digraph G{ \n");
        fprintf(graphDot,"subgraph cluster{\n label=\"MBR\"");
        fprintf(graphDot,"\ntbl[shape=box,label=<\n");
        fprintf(graphDot,"<table border=\'0\' cellborder=\'1\' cellspacing=\'0\' width=\'300\'  height=\'200\' >\n");
        fprintf(graphDot, "<tr>  <td width=\'150\'> <b>Nombre</b> </td> <td width=\'150\'> <b>Valor</b> </td>  </tr>\n");
        MBR mbr;
        fseek(fp,0,SEEK_SET);
        fread(&mbr,sizeof(MBR),1,fp);
        int tamano = mbr.mbr_size;
        fprintf(graphDot,"<tr>  <td><b>mbr_tamaño</b></td><td>%d</td>  </tr>\n",tamano);
        struct tm *tm;
        char fecha[100];
        tm = localtime(&mbr.mbr_date_created);
        strftime(fecha,100,"%d/%m/%y %H:%S",tm);
        fprintf(graphDot,"<tr>  <td><b>mbr_fecha_creacion</b></td> <td>%s</td>  </tr>\n",fecha);
        fprintf(graphDot,"<tr>  <td><b>mbr_disk_signature</b></td> <td>%d</td>  </tr>\n",mbr.mbr_disk_signature);
        fprintf(graphDot,"<tr>  <td><b>Disk_fit</b></td> <td>%c</td>  </tr>\n",mbr.mbr_disk_fit);

        int index_Extendida = -1;
        for (int i = 0; i < 4; i++){
            if(mbr.mbr_partitions[i].part_start!=-1 && mbr.mbr_partitions[i].part_status!='1'){
                if(mbr.mbr_partitions[i].part_type == 'E'){
                    index_Extendida = i;
                }
                char status[3];
                if(mbr.mbr_partitions[i].part_status == '0')
                    strcpy(status,"0");
                else if(mbr.mbr_partitions[i].part_status == '2')
                    strcpy(status,"2");
                fprintf(graphDot,"<tr>  <td><b>part_status_%d</b></td> <td>%s</td>  </tr>\n",(i+1),status);
                fprintf(graphDot,"<tr>  <td><b>part_type_%d</b></td> <td>%c</td>  </tr>\n",(i+1),mbr.mbr_partitions[i].part_type);
                fprintf(graphDot,"<tr>  <td><b>part_fit_%d</b></td> <td>%c</td>  </tr>\n",(i+1),mbr.mbr_partitions[i].part_fit);
                fprintf(graphDot,"<tr>  <td><b>part_start_%d</b></td> <td>%d</td>  </tr>\n",(i+1),mbr.mbr_partitions[i].part_start);
                fprintf(graphDot,"<tr>  <td><b>part_size_%d</b></td> <td>%d</td>  </tr>\n",(i+1),mbr.mbr_partitions[i].part_size);
                fprintf(graphDot,"<tr>  <td><b>part_name_%d</b></td> <td>%s</td>  </tr>\n",(i+1),mbr.mbr_partitions[i].part_name);
            }
        }

        fprintf(graphDot,"</table>\n");
        fprintf(graphDot, ">];\n}\n");

        if(index_Extendida != -1){
            int index_ebr = 1;
            EBR extendedBoot;
            fseek(fp,mbr.mbr_partitions[index_Extendida].part_start,SEEK_SET);
            while(fread(&extendedBoot,sizeof(EBR),1,fp)!=0 && (ftell(fp) < mbr.mbr_partitions[index_Extendida].part_start + mbr.mbr_partitions[index_Extendida].part_size)) {
                if(extendedBoot.part_status != '1'){
                    fprintf(graphDot,"subgraph cluster_%d{\n label=\"EBR_%d\"\n",index_ebr,index_ebr);
                    fprintf(graphDot,"\ntbl_%d[shape=box, label=<\n ",index_ebr);
                    fprintf(graphDot, "<table border=\'0\' cellborder=\'1\' cellspacing=\'0\'  width=\'300\' height=\'160\' >\n ");
                    fprintf(graphDot, "<tr>  <td width=\'150\'><b>Nombre</b></td> <td width=\'150\'><b>Valor</b></td>  </tr>\n");
                    char status[3];
                    if(extendedBoot.part_status == '0')
                        strcpy(status,"0");
                    else if(extendedBoot.part_status == '2')
                        strcpy(status,"2");
                    fprintf(graphDot, "<tr>  <td><b>part_status_1</b></td> <td>%s</td>  </tr>\n",status);
                    fprintf(graphDot, "<tr>  <td><b>part_fit_1</b></td> <td>%c</td>  </tr>\n",extendedBoot.part_fit);
                    fprintf(graphDot, "<tr>  <td><b>part_start_1</b></td> <td>%d</td>  </tr>\n",extendedBoot.part_start);
                    fprintf(graphDot, "<tr>  <td><b>part_size_1</b></td> <td>%d</td>  </tr>\n",extendedBoot.part_size);
                    fprintf(graphDot, "<tr>  <td><b>part_next_1</b></td> <td>%d</td>  </tr>\n",extendedBoot.part_next);
                    fprintf(graphDot, "<tr>  <td><b>part_name_1</b></td> <td>%s</td>  </tr>\n",extendedBoot.part_name);
                    fprintf(graphDot, "</table>\n");
                    fprintf(graphDot, ">];\n}\n");
                    index_ebr++;
                }

                if(extendedBoot.part_next == -1)
                    break;
                else
                    fseek(fp,extendedBoot.part_next,SEEK_SET);
            }
        }
        fprintf(graphDot,"}\n");
        fclose(graphDot);
        fclose(fp);
        string comando = "dot -T"+extension.toStdString()+" grafica.dot -o "+destino.toStdString();
        system(comando.c_str());
        cout << "Reporte generado con exito " << endl;
    }
}

