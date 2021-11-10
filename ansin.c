/*
 *	Análisis Sintáctico Descendente	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Práctica de Programación Nro. 2
 *	
 *	Descripcion:
 *	Implementar un analizador sintáctico descendente recursivo o LL(1) 
 *  para el lenguaje Json simplificado.
 */

#include "anlex.c"
#include "ansin.h"
#define CANT 2000
int unaVez = 1;
int chck2 = 1;

void match(int expectedToken){
    if(t.compLex == expectedToken){
        getToken();
    }
}


void error_sint(int sincronizacion []){
    aceptar = 0;
    if(t.compLex == EOF && unaVez == 1){
        unaVez = 0;
        printf("Error sintactico en linea: %d. No se esperaba EOF \n",numLinea);
    }else{
        printf("Error sintactico en linea: %d. No se esperaba %s.\n",numLinea,t.pe->componenteLexico); 
   
	int i = 0;
        while(t.compLex != sincronizacion[i] && t.compLex != EOF){   
            if (sincronizacion[i] == '\0'){
                getToken();
                i = 0;        
            }
            i++;
        } 
        getToken();
    }
}

void check_input(int primero[], int siguiente[]){
    int syncset[CANT];
    int i=0;  

    if(t.compLex == EOF && chck2 == 1){
        return;
    }
    while(primero[i] != '\0'){
        if(t.compLex == primero[i]){
            return;
        }
        i++;
    }
    i = 0;    
    while(primero[i] != '\0'){ 
        syncset[i] = primero[i];
        i++;
    }

    int j=0;
    while(siguiente[j] != '\0'){
        syncset[i] = siguiente[j];
        i++;
        j++;
    }
    
    
    error_sint(syncset);
    if(t.compLex == EOF){
        return;
    }
    
}

void json(){
    int primero[] = {L_LLAVE, L_CORCHETE,'\0'};
    int siguiente[] = {EOF, '\0'};
    element(siguiente);
}

void element(int syncset[]){
    int primero[] = {L_CORCHETE, L_LLAVE, '\0'};
    int siguiente[] = {COMA, R_CORCHETE, R_LLAVE, EOF, '\0'};
    chck2 = 1;
    check_input(primero,syncset);

    if(t.compLex == L_LLAVE){
        object(siguiente);
    }
    else if(t.compLex == L_CORCHETE){
        array(siguiente);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void array(int syncset[]){
    int primero [] = {L_CORCHETE, '\0'};
    int siguiente [] = {COMA, R_CORCHETE, R_LLAVE,EOF,'\0'};
    chck2 = 1;
    check_input(primero,syncset);
   
    if(t.compLex == L_CORCHETE){
        match(L_CORCHETE);
        arrayA(siguiente);
    }
    chck2 = 0;
    check_input(siguiente, primero);
}

void arrayA(int syncset[]){
    int primero[]={L_LLAVE, L_CORCHETE, R_CORCHETE, '\0'};
    int siguiente[] = {L_CORCHETE, COMA, R_CORCHETE, R_LLAVE,EOF,'\0'};
    chck2 = 1;
    check_input(primero,syncset);

    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element_list(siguiente);
        match(R_CORCHETE);
    }
    else if(t.compLex == R_CORCHETE){
        match(R_CORCHETE);
    }
    chck2 = 0; 
    check_input(siguiente,primero);
}

void element_list(int syncset[]){
    int primero[]={L_CORCHETE, L_LLAVE, '\0'};
    int siguiente[] = {R_CORCHETE, '\0'};
    chck2 = 1;
    check_input(primero, syncset);
    if(t.compLex == L_LLAVE  || t.compLex == L_CORCHETE){
        element(siguiente);
        element_listA(siguiente);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void element_listA(int syncset[]){
    if(t.compLex == R_CORCHETE){ 
        return;
    }

    int primero[]={COMA, '\0'};
    int siguiente[] = {R_CORCHETE,'\0'};
    chck2 = 1;
    check_input(primero,syncset);

    if(t.compLex == COMA){
        match(COMA);
        element(siguiente);
        element_listA(siguiente);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void object(int syncset[]){
    int primero[]={L_LLAVE,'\0'};
    int siguiente[] = {COMA, R_CORCHETE, R_LLAVE, EOF, '\0'};
    chck2 = 1;
    check_input(primero,syncset);
    if(t.compLex == L_LLAVE){
        match(L_LLAVE);
        objectA(siguiente);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void objectA(int syncset[]){
    int primero[]={STRING, R_LLAVE, '\0'};
    int siguiente[] = {L_LLAVE, COMA, R_CORCHETE, R_LLAVE, EOF, '\0'};
    chck2 = 1;
    check_input(primero, syncset);
 
    if(t.compLex == STRING){
        attributes_list(siguiente);
        match(R_LLAVE);
    }
    else if(t.compLex == R_LLAVE){
        match(R_LLAVE);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void attributes_list(int syncset[]){
    int primero[]={STRING,'\0'};
    int siguiente[] = {R_LLAVE, '\0'};
    chck2 = 1;
    check_input(primero,syncset);

    if(t.compLex == STRING){
        attribute(siguiente);
        attributes_listA(siguiente);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void attributes_listA(int syncset[]){
    if (t.compLex == R_LLAVE) return;

    int primero[]={COMA,'\0'};
    int siguiente[] = {R_LLAVE,'\0'};
    chck2 = 1;
    check_input(primero,syncset);

    if(t.compLex == COMA){
        match(COMA);
        attribute(siguiente);
        attributes_listA(siguiente);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void attribute(int syncset[]){
    int primero[]={STRING,'\0'};
    int siguiente[] = {COMA, R_LLAVE,'\0'};
    chck2 = 1;
    check_input(primero,siguiente);

    if(t.compLex == STRING){
        attribute_name(siguiente);
        match(DOS_PUNTOS);
        attribute_value(siguiente);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void attribute_name(int syncset[]){
    int primero[]={STRING,'\0'};
    int siguiente[] = {DOS_PUNTOS, '\0'};
    chck2 = 1;
    check_input(primero,syncset);

    if(t.compLex == STRING){
        match(STRING);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

void attribute_value(int syncset[]){
    int primero[]={L_CORCHETE, L_LLAVE, STRING, NUMBER, PR_TRUE, PR_FALSE, PR_NULL,'\0'};
    int siguiente[] = {COMA, R_LLAVE, '\0'};
    chck2 = 1;
    check_input(primero,syncset);

    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element(siguiente);
    }else if(t.compLex == STRING){
        match(STRING);
    }else if(t.compLex == NUMBER){
        match(NUMBER);
    }else if(t.compLex == PR_TRUE){
        match(PR_TRUE);
    }else if(t.compLex == PR_FALSE){
        match(PR_FALSE);
    }else if(t.compLex == PR_NULL){
        match(PR_NULL);
    }
    chck2 = 0;
    check_input(siguiente,primero);
}

/*
int main (int argc,char* args[]){
    initTabla();
	initTablaSimbolos();
    if(argc > 1){
        if (!(entrada_json = fopen(args[1],"rt"))){
            printf("Archivo no encontrado.\n");
            exit(1);
        }
        getToken();
        json();

        if(aceptar) 
            printf("Correctamente sintactico \n");

        fclose(entrada_json);

    }else{
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;
}
*/