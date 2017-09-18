/*******************************************************************************
            usb_biblioteca.h
Esta é uma biblioteca para transmissão de dados através do protocolo de USB de
comunicação. Contém funções básicas de transmissão e recepção de dados. È imple-
mentada para simunlar uma PORTA COM VIRTUAL.

Obs.:
     
     -> A inclusão de um descritor CDC diferente.
A inclusão de descritores é feita no arquivo usb_cdc.h que está localizado na 
pasta "\...\PICC\Drivers", deve - se  trocar o arquivo usb_desc_cdc.h pelo nome 
novo arquivo do descritor que se deseja trocar.

     -> Criação de um novo arquivo descritor usb
Caso deseje efetuar alguma modificação do descritor, o mesmo se encontra na pas-
ta "\..\PICC\Drivers" com o nome usb_desc_cdc.h. Faça as modificações e salve na
mesma pasta onde se encontra o projeto, NÂO esquecendo de adicioná-lo nos arqui-
vos de configuração.

*******************************************************************************/
 
 #include  <usb_cdc.h>
 
 /*usb_putString
Envia uma string
char *p é o ponteiro que representa essa string
obs.: A string deve estar declarada para se enviada, por exemplo:
      
      char temp[] = {"valor da string"};
      usb_cdc_putString(temp);
      
Existe ainda um aforma alternativa para o envio de um a string e com ela não é
necessário a prévia declaração da string.

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
Envia um bloco da memória de dados
int posicao -> Posição inicial do bloco de memória
int tamanho -> Comprimento do bloco de memória
obs.: Pode enviar os valores  em hex ou em caracteres ASCII(código comentado) */
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
Envia um bloco da memória do programa
int posicao -> Posição inicial do bloco de memória
int tamanho -> Comprimento do bloco de memória
obs.: Pode enviar os valores  em hex ou em caracteres ASCII(código comentado) */

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
para efetuar a transmissão */

void usb_cdc_conectar()
{
     //usb_cdc_init();
     usb_init_cs();//não rava na configuração e é necessario chamar usb_task
     usb_init();
     usb_task();
     while(!usb_cdc_connected()){}
}

/*   Funções nativas

usb_enumerated()
Verifica se o dispositivo está pronto para comunicação
retorna True ou False

usb_detach()
Desconecta o dispositivo, usado antes de sua remoção física do computador

usb_attach()
re-conecta o dispositivo, usado para reconecta-lo quando o dispositivo foi des-
conectado mas ainda não removido

usb_cdc_putc(char c)
Envia um caracter para a transmissão

usb_cdc_getc()
Recebe um caracter do buffer de recepção, deve-se usar o usb_cdc_kbhit() antes
para verificar se existe dados a serem recebidos

usb_cdc_kbhit()
Verifica se existe algum dado no buffer de recpção
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

Obs.: Para mairores informações sobre estas funções e outra mais, estão no 
arquivo usb_cdc.h em \...\PICC\Driers */
