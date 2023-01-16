/*
 * ADC.c
 *
 *  Created on: 23/12/2022
 *      Author: Ashley Roxana Reséndiz Sánchez
 */
#include "lib/include.h"

// Para controlar 3 potenciometros se habilitan los 3 canales 
/* Canal 1 -> PE2, Canal 2 -> PE1, Canal 8 -> PE5 en el modulo 0  
    a una velocidad de conversion de 250ksps dandole la mayor 
    prioridad al secuenciador 2 con evento de procesador 
    */


extern void Configura_Reg_ADC0(void)
{
    //Pag 352 para inicializar el modulo de reloj del adc RCGCADC, Se inicializa modulo 0
    SYSCTL->RCGCADC = (1<<0); 

    //Pag 340 (RGCGPIO) Puertos base habilitación del reloj
    //                     F     E      D       C      B     A
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|(1<<0);
    
    //Pag 663 (GPIODIR) Habilta los pines como I/O un cero para entrada y un uno para salida
    GPIOE_AHB->DIR = (0<<1) | (0<<2) | (0<<5); //PE1, PE2 y PE5
    
    //(GPIOAFSEL) pag.672 Enable alternate función para que el modulo analógico tenga control de esos pines
    GPIOE_AHB->AFSEL =  (1<<1) | (1<<2) | (1<<5);

    //(GPIODEN) pag.683 desabilita el modo digital
    GPIOE_AHB->DEN = (0<<1) | (0<<2) | (0<<5);

    //la tabla pag 1351, Se indica la función alternativa de los pines
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF0FF00F); //Poner 0 en los bits de los pines 1, 2, 5

    //(GPIOAMSEL) pag.687 habilitar analogico
    GPIOE_AHB->AMSEL = (1<<1) | (1<<2) | (1<<5);

    //Pag 891 El registro (ADCPC) establece la velocidad de conversión por segundo
    ADC0->PC = (0<<2)|(1<<1)|(1<<0);///Se pone 3 en binario porque trabaja con la velocidad 250ksps

    //Pag 841 Este registro (ADCSSPRI) configura la prioridad de los secuenciadores
    ADC0->SSPRI = 0x3012; //se pone mayor prioridad en el sec. 2 

    ////Pag 821 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0);
    
    //Pag 834 Este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger)
    ADC0->EMUX  = (0x0000); //se configura por procesador 

    //Pag 867 Este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado
    ADC0->SSMUX2 = 0x0128; 

    //pag 868 Este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción
    //    Sec. 2        mux 0            mux 1             mux 2      son donde se establecen los canales en la configuración del SSMUX2 -> linea 53 
    ADC0->SSCTL2 = (1<<2) | (1<<1)| (1<<6) | (1<<5) | (1<<10) | (1<<9); // Para establecer configuración de muestro y finalizar muestro acorde a la pag. 868
    
    //825 Enable ADC Interrupt */
    ADC0->IM |= (1<<2); // Al sec. 2 se le asigna un 1 para indicar que manda la señal pura al controlador de interrupciones
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;

    //Pag 821 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS = (0<<3) | (1<<2) | (0<<1) | (0<<0);  //se activa sec. 2
    ADC0->PSSI |= (1<<2); //conf. por procesador 
}


extern void ADC0_InSeq2(uint16_t *Result,uint16_t *duty)
{
        //uint16_t Rojo;
    //ADC Processor Sample Sequence Initiate (ADCPSSI)
       ADC0->PSSI = 0x00000004; //habilita por procesador para iniciar 
       while((ADC0->RIS&0x04)==0){}; // espera a que el convertidor termine  (RIS = registro de edo. muestra el edo de la señal de interrupción sin procesar) (Se pone 4 en hex. para asignar un 1 al sec. 2)
       Result[2] = ADC0->SSFIFO2&0xFFF; //  Leer  el resultado almacenado en el FIF02 , es 02 porque se usa sec. 2, pag. 860
       Result[1] = ADC0->SSFIFO2&0xFFF;
       Result[0] = ADC0->SSFIFO2&0xFFF;
       //Rojo = (uint16_t)readString('%');
       //printChar('A');
    
       duty[0] = 50000-(Result[0]*5000)/4096;
       duty[1] = 50000-(Result[1]*5000)/4096;
       duty[2] = 50000-(Result[2]*5000)/4096;
       ADC0->ISC = 0x0004;  //Conversion finalizada

}


