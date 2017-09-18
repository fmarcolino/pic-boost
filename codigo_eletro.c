#include <18f2550.h>
#DEVICE ADC=10

#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN,MCLR,PUT
#use delay(clock=48000000)
//#use rs232(baud=115200, xmit=PIN_c6,rcv=PIN_c7)

#use fast_io(A)
//#use fast_io(E)

#define FCLOCK 48 // MHZ

#define K_I 1.9
#define K_P 0.08

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <usb_biblioteca.h>
#define use_portb_lcd TRUE
#include <LCD.C>
#define Ts 5000// Em microsegundos
#define contagem_T0 (65535.0-FCLOCK*Ts*0.25)
#define contagem_T1 5535 //tagem_T0 é burro -> não dá o valor correto
#define N 83 // Quantidade de dois ciclos de uma senóide de 60 Hz

unsigned long num=0;
long long tensao_boost = 0;
unsigned long temp_amb = 0;
float erro = 0.0;
float u_i=0.0;
float u_p = 0.0;
float u_u = 0.0;
int16 duty = 0;
long long REF = 24000;
int aquisicao = 0;
int imprimir = 0;
int botao1=0;
int botao2=0;
float dt=0;

//Interrupção timer 1
#int_timer1
void timer()
{
	//INTERRUPÇÃO A CADA 5 milisegundos
	set_timer1(contagem_T1);

	//Canal 0 - TENSÃO DE SAÍDA
	tensao_boost = read_adc() * 53.76;
	
	//printf(usb_cdc_putc,"%lu\n",tensao_boost);
	
	if(tensao_boost>5000)
	{
		erro = (float)(((float)REF - (float)tensao_boost)*0.001);
		u_p = (K_P * erro);
		u_i = K_I * 0.005 * erro + u_i;
		u_u = u_p + u_i;
		
		//Saturação em 
		duty = (u_u>150)? (150): ((u_u<0)? 0: (int16)u_u);
		
	}else
	duty = 0;

	set_pwm1_duty((int8)duty);

	if(aquisicao){
			//enviar informações para o matlab quando solicitado por ele

		printf(usb_cdc_putc,"%ld %d %ld\n",tensao_boost, (int)(duty * 0.3906), (long)(erro*1000));
		aquisicao = 0;

	}  
	if(num > 500)
	{	
		//acontece a cada 1 segundo a amostragem da temperatura
		num = 0;
		//Canal 3 - LM 35
		set_adc_channel(3);
		delay_us(20);
		temp_amb = (long)(read_adc() * 500./1023.);
		
		set_adc_channel(0);
		delay_us(10);
		
		SETUP_ADC(ADC_OFF);
		botao1 = input(pin_a1);
		botao2 = input(pin_a2);
		
		SETUP_ADC(ADC_CLOCK_INTERNAL);
		//flag para imprimir no lcd algumas informações importantes
		imprimir = 1;
	}
	num++;
}

void setup()
{
	//PORT A 
	set_tris_a(0b001111);
	//PORT B
	set_tris_b(0b00000000);
	//PORT C
	set_tris_c(0b00110000);
//	//PORT D
//	set_tris_d(0b00000000);
//	//PORT E
//	set_tris_e(0b111);
	
	setup_adc(ADC_CLOCK_INTERNAL);
	setup_adc_ports(AN0_TO_AN3);
	set_adc_channel(0);
	
	// Configuro o timer 0,  para uma determinada frequência.
	//setup_timer_0(RTCC_INTERNAL|T0_DIV_1);  
	SETUP_TIMER_2(T2_DIV_BY_4,255,1);

	SETUP_CCP1(CCP_PWM);

   //dUTY DE 0% INICIAL
	SET_PWM1_DUTY(0);

   // Configuro o timer 1,  para uma determinada frequência.
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);

   //output_bit(pin_a5,1);
   //delay_ms(1000);

//	 enable_interrupts(GLOBAL);  // configuro as interrupções

	usb_cdc_conectar();
	while(usb_enumerated()==0) ;

   //acender led verde - funcionando
	output_high(pin_a5);

  //led vermelho só piscando inicialmente
	output_high(pin_a4);
	delay_ms(3000);
	output_low(pin_a4);

	enable_interrupts(INT_TIMER1); //habilito a interrupção do timer 1
}	
void main()
{
	char buf[40],letra, i;
	long int menu;
	
  //Ativar LCD
	lcd_init();

  //Função setup: configurações gerais
	setup();
	
  //limpar LCD
	output_high(pin_c1);
	lcd_putc('\f');
	printf(lcd_putc,"BOOST\nLigando...");  
	delay_ms(1000);
	duty = 0;
	lcd_putc('\f');
	printf(lcd_putc,"BOOST\nLigado!");
	
	set_pwm1_duty(duty);

	i=0;
	while(1)
	{
		usb_task();
		if(usb_cdc_kbhit())
		{
			letra = usb_cdc_getc();
			if(letra=='\n')
			{
				buf[i] = '\0';
				i = 0;
				menu = atoi(buf);
				if(menu == 1)
				{
					num = 0;
					 //Duty de 50%
   					//SET_PWM1_DUTY(50);
					output_high(pin_c1);
					output_high(pin_a5);
					lcd_putc('\f');
					printf(lcd_putc,"PWM ligado\nBOOST MICRO");

					delay_ms(2000);

					set_timer1(contagem_T1);

				}	else if(menu == 2){

			   	//Desativar aquisição de dados
   				//SET_PWM1_DUTY(0);
					aquisicao = 0;
					output_high(pin_c1);
					output_low(pin_a5);
					lcd_putc('\f');
					printf(lcd_putc,"Envio de informacoes\nencerrado.");

				  //disable_interrupts(INT_TIMER1); //habilito a interrupção  de timer0
					delay_ms(1000);

					output_low(pin_c1);

				  //printf(lcd_putc,"PROJETO BOOST\nMICRO 2016");


				}else if(menu == 3)
				{
				   //Exibir na tela algumas informações
					output_high(pin_c1);

					printf(usb_cdc_putc,"PROJETO PIC ELETROBOOST\n");
					lcd_putc('\f');

					printf(lcd_putc,"D %d %% REF. %lumV\n Tensao: %lumV",(int)duty,REF,tensao_boost);

				   //enable_interrupts(INT_TIMER1); //habilito a interrupção  de timer0
					delay_ms(5000);

					output_low(pin_c1);

				}else if(menu == 4)
				{

					aquisicao = 1;

				}
			}
			else
			{
				buf[i] = letra;
				i++;
			} 	
		}
		if(imprimir){
			imprimir = 0;
			//Tempo de 1 segundo atualizar o LCD
			
			printf(lcd_putc,"\f%ldmV  D=%ld%%\ne=%ldmV - %ld ºC\n",tensao_boost, duty, (long)(erro*1000),temp_amb);  
		} 
		
		if(botao1)
		{
		  //botao 1
		  //incrementar valor da ref
			output_high(pin_a5);	
			while(botao1) ; 
		  //espera o botão ser solto

			printf(usb_cdc_putc,"Botao 1 pressionado\n");
		}else if(botao2)
		{
		  //botao 2
		  //decrementar valor da ref
			output_low(pin_a5);
			while(botao2) ; 
		  //espera o botão ser solto
			aquisicao = 1;
			printf(usb_cdc_putc,"Botao 2 pressionado\n");
		}
	}	
}	