//#include "lcd_func.h"
#include "sw_func.h"

void TIM1_UP_IRQHandler(void)
{
	// Clear the TIM1 Update pending bit
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

	suliCurrent_Tick++;
	fnSw_Tick();
	//	fnADC_Tick();
	//fnJS_Tick();

}


void main()
{
fnGR_GPIO_Configuration();
fnGR_Memory_Configuration();
fnDisplay_Initialize();
fnDisplay_Clear_Screen(RGB(255,255,255));
//fnDisplay_Reg_Write(0x007, 0x0173);   //Display Control1 

fnTimer_Initialize();
fnSw_Initialize();
//fnGr_SelectFont("Times13", 2);
//	fnGr_PutString(20,250,"SWITCH2", RGB(0,0,0), RGB(255,255,255), ORIENTATION_90);
//ADC_Initialize();
	//JS_Initialize();


	while(1)
	{
fnSw_Foreground_Service();
		//	ADC_Foreground_Service();
		//JS_Foreground_Service();

	}
	
}