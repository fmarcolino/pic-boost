#include <18f2550.h>
#DEVICE ADC=10

#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOUSBDIV,PLL5,CPUDIV1,MCLR
#use delay(clock=48000000)
#use fast_io(A)
#use fast_io(B)
#use fast_io(C)
//Não alterar o tris ao longo do programa

#bit UTRDIS=0xF6F.3
#bit USBEN=0xF6D.3
#define FCLOCK 48 // MHZ

//Constantes do controle PI
#define K_I 2.0
#define K_P 0.1

//Biblioteca do LCD
//#define use_portb_lcd TRUE
//#include <LCD.C>

//Fórmula para achar valor inicial do timer 1
//#define Ts 5000// Em microsegundos
//#define contagem_T0 (65535.0-FCLOCK*Ts*0.25)
#define contagem_T1 5535 //contagem_T0 

//Variáveis globais - podem ser utilizadas em todas as funções
unsigned long num=0;
long long tensao_boost = 0;
unsigned long temp_amb = 0;
float erro = 0.0;
float u_i=0.0;
float u_p = 0.0;
float u_u = 0.0;
int16 duty = 0;
long long REF = 16000;
int imprimir = 0;
int16 botao1=0;
int16 botao2=0;

#int_timer1
void timer()
{
//INTERRUPÇÃO A CADA 5 milisegundos
	set_timer1(contagem_T1);

//Canal 0 - TENSÃO DE SAÍDA DO CONVERSOR BOOST
	tensao_boost = read_adc() * 53.76;


//Atraso de transporte
	if(tensao_boost>5000)
	{
//O controle só atua com a presença da tensão de entrada.
		erro = (float)(((float)REF - (float)tensao_boost)*0.001);
		u_p = (K_P * erro);
		u_i = K_I * 0.005 * erro + u_i;
//Sinal do controlador
		u_u = u_p + u_i;

//Saturação em 0% e 60%, aproximadamente.
		duty = (u_u>150)? (150): ((u_u<0)? 0: (int16)u_u);

	}else
	{
//Zerar duty quando é retirada a tensão de saída
		duty = 0;
//Zerar a informação do controle integral
		u_i = 0;
	}

//Setar o novo duty
	set_pwm1_duty((int8)duty);

	if(num > 500)
	{	
//acontece a cada 1 segundo a amostragem da temperatura
		num = 0;

//Canal 3 - LM 35
		set_adc_channel(3);
		delay_us(20);
		temp_amb = (long)(read_adc() * 500./1023.);

		set_adc_channel(1);
		delay_us(20);
		botao1 = read_adc();
		set_adc_channel(2);
		delay_us(20);
		botao2 = read_adc();

		set_adc_channel(0);
		delay_us(10);

//flag para imprimir no lcd algumas informações importantes
		imprimir = 1;
	}

	num++;

}

void setup()
{

//UTRDIS = 1;
//PORT A 
	set_tris_a(0b001111);
//PORT B
	set_tris_b(0b00000000);
//PORT C
	set_tris_c(0b00110000);

	setup_adc(ADC_CLOCK_INTERNAL);
	setup_adc_ports(AN0_TO_AN3);
	set_adc_channel(0);


	SETUP_TIMER_2(T2_DIV_BY_4,255,1);

	SETUP_CCP1(CCP_PWM);

//dUTY DE 0% INICIAL
	SET_PWM1_DUTY(0);

	SETUP_SPI(SPI_DISABLED);
	SETUP_TIMER_3(T3_DISABLED);
	SETUP_TIMER_0(T0_OFF);

// Para clock de 48 MHz, tempo de estouro de 5,5ms
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);

//acender led verde - funcionando
	output_high(pin_a5);

	enable_interrupts(GLOBAL);
//ou seja, o controle já começa a controlar o duty.

}	
void main()
{	
//Ativar LCD
//lcd_init();

//Função setup: configurações gerais
	setup();

//Acender backlight lcd (pino C1)
	output_high(pin_c1);

//limpar LCD
//	lcd_putc('\f');
//	printf(lcd_putc,"Ligando...");

	delay_ms(1000);
	duty = 0;
//	lcd_putc('\f');
//	printf(lcd_putc,"Ligado!");

	delay_ms(1000);
	set_pwm1_duty(duty);

	enable_interrupts(INT_TIMER1); //habilito a interrupção do timer 1

	while(1)
	{

		if(imprimir){

			imprimir = 0;
//Tempo de 1 segundo atualizar o LCD
//			
//			output_toggle(pin_a4);
//			if(botao1>800)
//			{
//				
//				//botão 1 pressionado
//				REF = REF + 1000;
//				printf(lcd_putc,"\fAUMENTAR TENSAO");
//				lcd_gotoxy(1,2);
//				printf(lcd_putc,"REF %10lumv",REF);
//				
//			}else if(botao2>800)
//			{
//				//botão 2 pressionado
//				REF = REF - 1000;
//				printf(lcd_putc,"\fDIMINUIR TENSAO");
//				lcd_gotoxy(1,2);
//				printf(lcd_putc,"REF %10lumv",REF);
//			
//			}else	
//			{
//				//enquanto os botões estão soltos, imprimir normalmente as informações básicas do boost
//				printf(lcd_putc,"\f%5ldmV D:%5ld%%",tensao_boost, (long)(duty*0.392));
//				lcd_gotoxy(1,2);
//				printf(lcd_putc,"%5ldmV T: %2ld %cC", (long)(erro*1000),temp_amb,0xDF);
//				
//			}
		}

	}
}	
