/*
 * RobotRecorreLaberinto.c
 *
 * Created: 25/06/2018 03:38:33 p. m.
 *  Author: JARP-Laptop
 */ 

#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
//EEPROM
#include <avr/eeprom.h>

volatile char orientacion, instruccion, movimiento, sigInst, movCPU;
volatile uint8_t intento, veces, paso;
volatile int locINST, locMOV, loclocINST, locLocMOV;
volatile bool activado, recivido;
volatile char x='F';

void config();
void resetPaso();

void limpiarEEPROM(){
	for (int i =0; i<501;i++)
	{
		eeprom_write_byte((uint8_t *)(i),0xFF);
	}
}

uint8_t apretado (uint8_t PuertoIn, uint8_t PinNo) {	uint8_t respuesta;
	if(!(PuertoIn&(1<<PinNo)))
	{
		respuesta=1;
	}
	else
	{
		respuesta=0;
	}
	return respuesta;
}

void configINT(){
	MCUCR = 0b00001100;//0x0F;
	MCUCSR = 0b01000000;
	GIFR = 0b11100000;
	GICR=0b1110000;
	sei();
}

void configSERIAL(){
	UCSRA = 0b00100010 ; //Sin banderas,  U2X = 1 en modo normal
	UCSRB = 0b10011000 ; //Para cambiar las x ver Pág. 191 (se configura si quiero enviar y/o recibir datos), 8 bits
	UCSRC = 0b10000110 ; //Usar el UCSRC, modo asíncrono, sin paridad a 1 bit (stop), para 8 bits
	UBRRL = 12 ; //baud rate a 9600
}

void configTIMER(){
	TCCR0 = 0b00001101 ; //Modo por comparación con prescaler a 1024
	TIMSK = 0b00000010 ; //Modo por comparación
	TIFR = 0b00000011 ; //Banderas de interrupción desactivadas
	TCNT0 = 0 ; //Inicia el contador en 0
	OCR0 = 243 ;
}

void config(){
	// Botones
	DDRC= 0x00;
	PORTC = 0XFF;
	// Motores
		//Motor 1 (Izquierda)
		DDRA = 0xFF;
		// Motor 2 (Derecha)
		DDRB = 0xFF;
	// BUMPER
	DDRD = 0b00000010;
	PORTD = 0xFF;
	
	//INTERRUPCIONES
	configINT();
	//TIMER
	configTIMER();
	//SERIAL
	configSERIAL();
}

void recorrer(){
	PORTA = 1;
	PORTB = 1;
	//resetPaso();
	activado = true;
}

void detener(){
	PORTA = 0;
	PORTB = 0;
}

void reversa(){
	PORTA = 2;
	PORTB = 2;
	_delay_ms(200);
	detener();
	_delay_ms(100);
}

void rutaCompletada() {
	detener();
	activado=false;
	eeprom_write_byte((uint8_t *)(locINST),'F');
	locINST++;
	eeprom_write_byte((uint8_t *)(locMOV),'F');
	locMOV--;
	locINST=0;
	locMOV =500;
}

void escribirInsMov(){
	eeprom_write_byte((uint8_t *)(locINST), instruccion);
	locINST++;
	eeprom_write_byte((uint8_t *)(locMOV), movimiento);
	locMOV--;
}

void escribirMOV(char mov){
	eeprom_write_byte((uint8_t *)(locMOV), mov);
	locMOV--;
}

void vueltaDerecha(char inst, char mov, char ori) {
	 instruccion = inst;
	 movimiento = mov;
	 orientacion = ori;
	 PORTA = 1;
	 PORTB = 0;
	 _delay_ms(1500);
	 resetPaso();
	 recorrer();
}

void vueltaIzquierda(char inst, char mov, char ori) {
	instruccion = inst;
	movimiento = mov;
	orientacion = ori;
	PORTA = 0;
	PORTB = 1;
	_delay_ms(3000);
	
	escribirInsMov();
	intento=0;
	resetPaso();
	
	recorrer();
}
// APPPPPPAPPAPDBPPIPPIF
// APPPPPPIPPBPIPAPPDPPIF

void vueltaDerechaSolo(){
	PORTA = 1;
	PORTB = 0;
	_delay_ms(1500);
}

void vueltaIzquierdaSolo(){
	PORTA = 0;
	PORTB = 1;
	_delay_ms(3000);
}

void resetPaso(){
	veces = 0;
	paso = 0;
}

void escribirPaso(){
	eeprom_write_byte((uint8_t *)(locINST), 'P');
	locINST++;
	eeprom_write_byte((uint8_t *)(locMOV),'P');
	locMOV--;
}

// SERIAL
ISR (USART_RXC_vect) 
{

	//uint8_t p = UDR;
	//UDR='E';
	recivido = true;
	UCSRA = 0b00100010 ;
	//uint8_t x;
	//

	//
	//while(1){
		//x = eeprom_read_byte((uint8_t *)(locLocMOV));
		//movCPU = (char)x;
		////UDR = 'O';
		//_delay_ms(10);
		//UDR = x;
		//locLocMOV--;
		//if(x != 0x46)break;
	//}
	
	//sei();
}

// TIMER
ISR (TIMER0_COMP_vect) 
{
	if (activado==true)
	{
		veces++;
		if (veces >= 4)
		{
			veces=0;
			paso++;
		}
		
		/*if(paso==1)
		{
			if (intento>=1)
			{
				escribirInsMov();
				//intento=0;
				resetPaso();
				//break;
			} else {
				escribirPaso();
				resetPaso();
				//break;
			}
		}*/
	}
}

// INTERRUPCION
ISR (INT0_vect)
{
	while(1){
		activado=false;
		intento++;
		resetPaso();
		detener();
		_delay_ms(1000);
		reversa();
		_delay_ms(1000);
		//eeprom_write_byte((uint8_t *)(250),(uint8_t)(locMOV));
		if (orientacion == 'N')
		{
			if (intento == 1)
			{
				vueltaDerecha('D','D','E');	
				break;
			}
			if (intento == 2)
			{
				vueltaIzquierda('I','I','O');
				break;
			}		
		}
		if (orientacion == 'S')
		{
			if (intento == 1)
			{
				vueltaDerecha('D','I','O');
				break;
			}
			if (intento == 2)
			{
				vueltaIzquierda('I','D','E');
				break;
			}
		}
		if (orientacion == 'E')
		{
			if (intento == 1)
			{
				vueltaDerecha('D','B','S');
				break;
			}
			if (intento == 2)
			{
				vueltaIzquierda('I','A','N');
				break;
			}
		}
		if (orientacion == 'O')
		{
			if (intento==1)
			{
				vueltaDerecha('D','A','N');
				break;
			}
			if (intento == 2)
			{
				vueltaIzquierda('I','B','S');
				break;
			}	
		}
		//_delay_ms(5000);
	}
}

void darPaso(){
	if (activado==true)
	{
		if(paso==1)
		{
			if (intento == 1)
			{
				escribirInsMov();
				intento=0;
				resetPaso();
			}
			else if (intento == 2)
			{
				escribirInsMov();
				intento=0;
				resetPaso();
			}
			else {
				escribirPaso();
				resetPaso();
			}
		}
	}
}

void darPasoSolo(){
	PORTA = 1;
	PORTB = 1;
	_delay_ms(1000);
	detener();
}

void recorrerSolo(){
	while (1)
	{
		uint8_t instCarro = eeprom_read_byte((uint8_t *)(loclocINST));
		sigInst = (char)instCarro;
		_delay_ms(100);
		if (sigInst!='F')
		{
			if (sigInst=='P')
			{
				darPasoSolo();
			}
			if (sigInst=='D')
			{
				vueltaDerechaSolo();
			}
			if (sigInst=='I')
			{
				vueltaIzquierdaSolo();
			}
		}
		loclocINST++;
	}
}

int main(void) 
{
	config();
	detener();
	locINST= 0;
	locMOV= 500;
	loclocINST = 0;
	locLocMOV = 500;
	veces = 0;
	paso = 0;
	orientacion = 'N';
	movimiento = 'A';
	intento = 0;
	activado = false;
	recivido=false;
	//limpiarEEPROM();
	
	escribirMOV(movimiento);
	
    while(1)
    {
		if (apretado(PINC,0))
		{
			recorrer();
		}
		if (apretado(PINC,1))
		{
			rutaCompletada();
		}
		if (apretado(PINC, 2))
		{
			recorrerSolo();
		}
		// REALIZAR PASO/CAMBIO DE MOVIMIENTO
		darPaso();
		
		if (recivido==true)
		{
			recivido=false;
			
			//UDR='R';
			x=0;
			while( x != 0xff ){
				x = (char)eeprom_read_byte((uint8_t *)(locLocMOV));
				//UDR = 'O';
				// Wait until last byte has been transmitted
				_delay_ms(10);
				while((UCSRA &(1<<UDRE)) == 0);
				UDR = x;
				//x = 'F';
				locLocMOV--;
			}
		}
    }
}