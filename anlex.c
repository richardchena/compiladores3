/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Práctica de Programación Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador léxico que reconoce números, identificadores, 
 * 	palabras reservadas, operadores y signos de puntuación para el lenguaje
 * 	Json simplificado.
 */

/*********** Inclusión de cabecera **************/
#include "anlex.h"

/************* Variables globales **************/
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico
FILE *entrada_json;          // Fuente JSON
//FILE *salida;			// Salida esparada
char lexema[TAMLEX];	// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
int aceptar = 1;

/**************** Funciones **********************/
// Rutinas del analizador lexico
void error(const char* mensaje){
	//fprintf(salida, "Lin %d: Error Lexico. %s.\n",numLinea,mensaje);
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);
	aceptar = 0;
}

void getToken(){
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(entrada_json))!=EOF){	
		if (c==' ' || c=='\t'){
			//fprintf(salida,"%c", c);
			continue;

		}else if(c=='\n'){
			//fprintf(salida, "%c", c);
			numLinea++;
		}

		else if (tolower(c)=='t' || tolower(c)=='f' || tolower(c)=='n'){
			//es un boolean o null
			i=0;
			do{
				lexema[i]=c;
				i++;
				c=fgetc(entrada_json);
			}while(isalpha(c));
			lexema[i]='\0';
			if (c!=EOF)
				ungetc(c,entrada_json);
			else
				c=0;
			
			if (strcmp(lexema, "true")==0 || strcmp(lexema, "TRUE")==0){
				t.pe=buscar("true");
				t.compLex=PR_TRUE;
			}else if (strcmp(lexema, "false")==0 || strcmp(lexema, "FALSE")==0){
				t.pe=buscar("false");
				t.compLex=PR_FALSE;
			}else if (strcmp(lexema, "null")==0 || strcmp(lexema, "NULL")==0){
				t.pe=buscar("null");
				t.compLex=PR_NULL;
			}else{
				t.pe = NULL;
				error("No se reconoce");
			}
			break;
		}
		else if (isdigit(c)){
			//es un numero
			i=0;
			estado=0;
			acepto=0;
			lexema[i]=c;
			
			while(!acepto){
				switch(estado){
				case 0: //una secuencia netamente de digitos, puede ocurrir . o e
					c=fgetc(entrada_json);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=0;
					}else if(c=='.'){
						lexema[++i]=c;
						estado=1;
					}else if(tolower(c)=='e'){
						lexema[++i]=c;
						estado=3;
					}else{
						estado=6;
					}
					break;

				case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(entrada_json);						
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							//fseek(entrada_json,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
				case 2://la fraccion decimal, pueden seguir los digitos o e
					c=fgetc(entrada_json);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=2;
					}else if(tolower(c)=='e'){
						lexema[++i]=c;
						estado=3;
					}else
						estado=6;
					
					break;
				case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(entrada_json);
						if (c=='+' || c=='-')
						{
							lexema[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
				case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(entrada_json);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
				case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(entrada_json);
						if (isdigit(c)){
							lexema[++i]=c;
							estado=5;
						}else{
							estado=6;
						}break;
				case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,entrada_json);
						else
							c=0;
						
						lexema[++i]='\0';
						acepto=1;
						t.pe=buscar(lexema);
						if (t.pe->compLex==-1){
							strcpy(e.lexema,lexema);
							e.compLex=NUMBER;
							strcpy(e.componenteLexico,"NUMBER");
							insertar(e);
							t.pe=buscar(lexema);
						}
						t.compLex=NUMBER;
						break;
				case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else{
                                                	if (c=='\n')
 								sprintf(msg,"No se esperaba '\\n'");	       
							error(msg);
                                                }
                                                t.pe = NULL;
						acepto=-1;
				}
			}
			break;
		}
		else if (c==':'){
			t.compLex=DOS_PUNTOS;
			t.pe=buscar(":");
			break;
		}
		else if (c==','){
			t.compLex=COMA;
			t.pe=buscar(",");
			break;
		}
		else if (c=='['){
			t.compLex=L_CORCHETE;
			t.pe=buscar("[");
			break;
		}
		else if (c==']'){
			t.compLex=R_CORCHETE;
			t.pe=buscar("]");
			break;
		}
		else if (c=='\"'){ 
			i=0;
			lexema[i]=c;
			i++;
			int cantidadSalto = 0;
			do{
				c=fgetc(entrada_json);
				if (c=='\"'){
					c=fgetc(entrada_json);
					if (c=='\"'){
						lexema[i]=c;
						i++;
						lexema[i]=c;
						i++;
					}else{
						lexema[i]='\"';
						i++;
						break;
					}
				}else if(c==EOF){
					error("Se llego al fin de archivo sin finalizar un literal");
					break;				
				}else{
					if (c=='\n')
						cantidadSalto++;
					
					lexema[i]=c;
					i++;
				}
			}while(c!=EOF);
			numLinea = numLinea + cantidadSalto;
			lexema[i]='\0';
			if (c!=EOF){
				ungetc(c,entrada_json);
			}else{
				c=0;
			}
                        if (c==0){
				t.compLex=EOF;
				t.pe=NULL;
			}else{
				t.pe=buscar(lexema);
				t.compLex=STRING;
				if (t.pe->compLex==-1){
					strcpy(e.lexema,lexema);
					e.compLex=STRING;
					strcpy(e.componenteLexico,"STRING");
					insertar(e);
					t.pe=buscar(lexema);
					t.compLex=STRING;
				}
			}
			break;
		}
		else if (c=='{'){
			t.compLex=L_LLAVE;
			t.pe=buscar("{");
			break;
		}else if (c=='}'){
			t.compLex=R_LLAVE;
			t.pe=buscar("}");
			break;
		}else if (c!=EOF){
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}if (c==EOF){
		t.compLex=EOF;
		t.pe=NULL;
	}
}

//Principal
/*int main(int argc,char* args[])
{
	initTabla();
	initTablaSimbolos();

    salida = fopen("salida.txt", "w");
    if(!salida){
        printf("Ocurrio un error en la generacion del archivo salida.\n");
        exit(1);
    }

	if(argc > 1){
		if (!(entrada_json=fopen(args[1],"rt"))){
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (t.compLex!=EOF){
			getToken();
			if (t.compLex!=EOF) 
				if (t.pe != NULL){
                                    fprintf(salida,"%s ", t.pe->componenteLexico);
				}		
		}
		fclose(entrada_json);
        fclose(salida);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}
*/