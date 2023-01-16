/*
 * TIMER.c
 *
 *  Created on: 26 dic. 2022
 *      Author: Ashley Roxana Reséndiz Sánchez
 */
#include "lib/include.h"

  /*Configurar el timer 0 y timer 1 en modo de 32 bits para hacer la siguiente rutina
    una función que active el convertidor analógico digital*/

void (*PeriodicTask)(void); //se crea el puntero PeriodcTask para una función que no recibe ni retorna nada
extern void Configurar_Timer0A(void(*task)(void), unsigned long int periodo)
{
    //Encender el Periferico Timer 0  pag 338
    SYSCTL->RCGCTIMER |= (1<<0); 

    //Asignar la direccion de memoria de la tarea a temporizar
    PeriodicTask = task; 

    // Desabilitar el temporizador a usar antes de hacer cambios
    TIMER0->CTL = (0<<0);  

    // Se selecciona el modo de operacion de 16 o 32 bits pag. 728
    TIMER0->CFG = 0x00000000; 

    // Se seleciona el modo del timer pag. 732:  32 bits con 0x0
    TIMER0->TAMR = (0x2<<0); // modo periodico, la A de "tAmr" es porque se trabaja con el timer A 

    // Para establecer la carga 
    TIMER0->TAILR = periodo - 1; 

    //Se limpia el timer 0
    TIMER0->ICR = 1<<0;

    //Se habilita la mascara de la interrupción
    TIMER0->IMR = 1<<0;

    //*** PREGUNTAR DE QUE SE TRATA EL 0X2
    NVIC->IP[4] = (NVIC->IP[4]&0x00FFFFFF) | 0x20000000; //4 es el numero de prioridad (n) y el 00FFFFFF es porque se suma 3

    // Se se usa la int. 19 porque es la que corresponde al timer 0A
    NVIC->ISER[0] = (1UL << 19); 
    // numero de interrupcion TIMER0A = 19 
    // n=4 ----> [4n+3] [4n+2] [4n+1] [4n] ---> [4n+3]
    
    // Se habilita el timer 
    TIMER0->CTL = (1<<0);

}

extern void Timer0A_Handler(void)
{
  TIMER0->ICR = 1<<0;// acknowledge TIMER0A timeout
  (*PeriodicTask)();           
}


