#include <18f2550.h>
#DEVICE ADC=10

#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOUSBDIV,PLL5,CPUDIV1,MCLR
#use delay(clock=48000000)
#use fast_io(A)
#use fast_io(B)
#use fast_io(C)
//Fast io não altera o tris ao longo do programa

#bit UTRDIS=0xF6F.3
#bit USBEN=0xF6D.3
#define FCLOCK 48 // MHZ

#include <usb_biblioteca.h>

int16 tensao_boost = 0;
int8 buf[4];

void duty_boost()
{	
	//Setar o novo duty
	set_pwm1_duty((int8)buf[1]);
}
void parar_boost()
{
	set_pwm1_duty(0);
}	
void leitura_tensao()
{
	//Canal 0 - TENSÃO DE SAÍDA DO CONVERSOR BOOST
	tensao_boost = read_adc() * 53.76;	
	usb_cdc_putc((int8)(tensao_boost>>8)); //byte mais significativo
	usb_cdc_putc((int8)(tensao_boost)); // byte menos significativo
}	
void desligar_fonte()
{
	//desabilitar Relé (pino C1)
	output_low(pin_c1);	
	//acender led verde - funcionando
	output_low(pin_a5);
}	
void ligar_fonte()
{
	//Habilitar Relé (pino C1)
	output_high(pin_c1);	
	//acender led verde - funcionando
	output_high(pin_a5);
}	
void alterar(){ 
	switch(buf[0])
	{
		case '0':	duty_boost();	leitura_tensao();	break;
		case 'D':	duty_boost();										break;
		case 'Z':	parar_boost();									break;
		case 'T':	leitura_tensao(); 							break;
		case 'P':	desligar_fonte(); 							break;
		case 'L':	ligar_fonte(); 									break;
	}
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
	
	setup_comparator(NC_NC_NC_NC);
	SETUP_SPI(SPI_DISABLED);
	SETUP_TIMER_3(T3_DISABLED);
	SETUP_TIMER_0(T0_OFF);
	desligar_fonte();
	// Para clock de 48 MHz, tempo de estouro de 5,5ms
	//	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
	
	//	enable_interrupts(GLOBAL);
	//ou seja, o controle já começa a controlar o duty.

	usb_cdc_conectar();
	while(usb_enumerated()==0) ;
}	
void main()
{	
	int i = 0;
	char letra;
	//Função setup: configurações gerais
	setup();
	set_pwm1_duty(0);
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
				alterar();
			}
			else
			{
				buf[i] = letra;
				i++;
			} 	
		}	
	}
}	
