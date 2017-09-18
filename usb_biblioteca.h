/*******************************************************************************
            usb_biblioteca.h
Esta � uma biblioteca para transmiss�o de dados atrav�s do protocolo de USB de
comunica��o. Cont�m fun��es b�sicas de transmiss�o e recep��o de dados. � imple-
mentada para simunlar uma PORTA COM VIRTUAL.

Obs.:
     
     -> A inclus�o de um descritor CDC diferente.
A inclus�o de descritores � feita no arquivo usb_cdc.h que est� localizado na 
pasta "\...\PICC\Drivers", deve - se  trocar o arquivo usb_desc_cdc.h pelo nome 
novo arquivo do descritor que se deseja trocar.

     -> Cria��o de um novo arquivo descritor usb
Caso deseje efetuar alguma modifica��o do descritor, o mesmo se encontra na pas-
ta "\..\PICC\Drivers" com o nome usb_desc_cdc.h. Fa�a as modifica��es e salve na
mesma pasta onde se encontra o projeto, N�O esquecendo de adicion�-lo nos arqui-
vos de configura��o.

*******************************************************************************/
 
 #include  <usb_cdc.h>
 
 /*usb_putString
Envia uma string
char *p � o ponteiro que representa essa string
obs.: A string deve estar declarada para se enviada, por exemplo:
      
      char temp[] = {"valor da string"};
      usb_cdc_putString(temp);
      
Existe ainda um aforma alternativa para o envio de um a string e com ela n�o �
necess�rio a pr�via declara��o da string.

      printf(usc_cdc_putc, "valor da string"); */
void usb_cdc_putString(char *p)
{
     char i = 0;
     while(*(p+i)!='\0')
     {
         usb_cdc_putc(*(p+i));
         i++;
     }
}

/* usb_cdc_putEEPROM
Envia um bloco da mem�ria de dados
int posicao -> Posi��o inicial do bloco de mem�ria
int tamanho -> Comprimento do bloco de mem�ria
obs.: Pode enviar os valores  em hex ou em caracteres ASCII(c�digo comentado) */
void usb_cdc_putEEPROM(int posicao, int tamanho)
{
     int i;
     for(i=0;i<tamanho;i++)
     {
         printf(usb_cdc_putc, "%2x", read_eeprom(i + posicao));
         //usb_cdc_putc(read_eeprom(i+posicao));
     }
}

/* usb_cdc_putFLASH
Envia um bloco da mem�ria do programa
int posicao -> Posi��o inicial do bloco de mem�ria
int tamanho -> Comprimento do bloco de mem�ria
obs.: Pode enviar os valores  em hex ou em caracteres ASCII(c�digo comentado) */

void usb_cdc_putFLASH(char posicao, char tamanho)
{
     char i;
     int16 dado;
     for(i=0;i<=tamanho;i++)
     {
         dado=read_program_eeprom(i + posicao);
         usb_cdc_putc(dado);
     }
}

/* unsb_cdc_conectar
Inicializa, configura e enumera o dispositivo USB
obs.: Deve ser usado quando for conectado o dispositivo USB no computador
para efetuar a transmiss�o */

void usb_cdc_conectar()
{
     //usb_cdc_init();
     usb_init_cs();//n�o rava na configura��o e � necessario chamar usb_task
     usb_init();
     usb_task();
     while(!usb_cdc_connected()){}
}

/*   Fun��es nativas

usb_enumerated()
Verifica se o dispositivo est� pronto para comunica��o
retorna True ou False

usb_detach()
Desconecta o dispositivo, usado antes de sua remo��o f�sica do computador

usb_attach()
re-conecta o dispositivo, usado para reconecta-lo quando o dispositivo foi des-
conectado mas ainda n�o removido

usb_cdc_putc(char c)
Envia um caracter para a transmiss�o

usb_cdc_getc()
Recebe um caracter do buffer de recep��o, deve-se usar o usb_cdc_kbhit() antes
para verificar se existe dados a serem recebidos

usb_cdc_kbhit()
Verifica se existe algum dado no buffer de recp��o
Retorna True ou False

get_float_usb()
recebe um numero ponto flutuante 

get_long_usb()
Recebe um numero longo

get_int_usb()
Recebe um inteiro

get_string_usb(char *s, int max)
Recebe uma string

gethex_usb()
Recebe um hexadecimal

Obs.: Para mairores informa��es sobre estas fun��es e outra mais, est�o no 
arquivo usb_cdc.h em \...\PICC\Driers */
