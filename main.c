#include "lib/include.h"

volatile uint16_t Result[3];
uint16_t duty[3];

void Hilo_1(void)
{
    /*
    Configurar el timer 0 y timer 1 en modo de 32 bits para hacer la siguiente rutina
    una función que active el convertidor analógico digital y muestre dos señales de 
    cierta frecuencia y amplitud definida y los guarde en dos arreglos de tamaño 1024
    y la envíe por el protocolo de comunicación asíncrono para ser procesadas y regresadas
    al microcontrolador en valores binarios las cuales modificaran el ancho de pulso y 
    reconstruirán la señal enviada a un transistor o opam 
    **Si se ocupa PWM para el modificar el ancho de pulso 

    b) 2kHz, -5 a 5
    c) 10kHz, -1 a 1*/

    GPIOB_AHB->DATA = (1<<0);
    ADC0_InSeq2(Result,duty);
    GPIOB_AHB->DATA = (0<<0);

}

int main(void)
{

    Configurar_PLL(_20MHZ);  //Confiuracion de velocidad de reloj
    Configurar_UART0();
    Configura_Reg_ADC0();
    Configura_Reg_PWM1(8);
    Configurar_Timer0A(&Hilo_1,10000); // 10000 = periodo ***Checar valor del periodo 
   

    while (1)
    {
        //ADC0_InSeq2(Result,duty); //llamada a la conversion por procesador
        //PWM0->_0_CMPB = duty[0];
        //PWM0->_1_CMPA = duty[1];
        //PWM0->_2_CMPA = duty[2];
    }
    
}

