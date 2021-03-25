#include "xy_sys_headfile.h"

static unsigned char  errcod;

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_old_dis(unsigned char* table)
{
	static unsigned char s_bit = 0; 

	table[0] = 0x68;
	table[1] = 0x02;
	switch(errcod)
	{
		case 0:
			if(s_bit == 0)
			{
				table[2] = 0x07;
				table[3] = 0x7F;
			}
			else
			{
				table[2] = s_bit <  4 ? 0x01 << (s_bit-1) : 0x00;
				table[3] = s_bit >= 4 ? 0x01 << (s_bit-4) : 0x00;
			}
			s_bit++;
			if(s_bit > 10)
				s_bit = 0;
			break;
		case 1:
			table[2] = 0x00;
			table[3] = 0x5C;
			break;
		case 2:
			table[2] = 0x00;
			table[3] = 0x7C;
			break;
		case 3:
			table[2] = 0x00;
			table[3] = 0x58;
			break;
		case 4:
			table[2] = 0x07;
			table[3] = 0x7F;
			s_bit = 0;
			break;
		default:
			table[2] = 0x00;
			table[3] = 0x00;
			break;
	}
	table[4] = table[0] + table[1] + table[2] + table[3];
	table[5] = 0x16;	
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_uart(unsigned char mode)
{
	static unsigned char s_table[16] = {0};

	switch(mode)
	{
		case 0:				///< old protocol
			fml_test_old_dis(s_table);
		
			hal_serial_uart_tx_display(s_table, 6);
			break;
		default:
			break;
	}
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_delay(void)
{
	unsigned int i, j;

	watchdog();
	for(i=0; i<200; i++)
	{	
		for(j=0; j<1000; j++)
		{
			;
		}
	}
	watchdog();
	fml_test_uart(errcod);	
	for(i=0; i<200; i++)
	{
		for(j=0; j<1000; j++)
		{
			;
		}
	}	
	watchdog();
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_port(void)
{
	/*1*/
	#ifdef TEST_FUNC_NUMB_01
	PTC_on();
	fml_test_delay();
	PTC_off();
	#endif

	/*2*/
	#ifdef TEST_FUNC_NUMB_02
	rav_on();
	fml_test_delay();
	rav_off();
	#endif

	/*4*/
	#ifdef TEST_FUNC_NUMB_04
	blow_fan_on();
	fml_test_delay();
	blow_fan_off();
	#endif

	/*5*/
	#ifdef TEST_FUNC_NUMB_05
	light_on();
	fml_test_delay();
	light_off();
	#endif
	
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_motor(void)
{
	/*6*/
	#ifdef TEST_FUNC_NUMB_06
	blow_motor_step4_on();
	fml_test_delay();
	blow_motor_step4_off();
	#endif
	
	/*7*/
	#ifdef TEST_FUNC_NUMB_07
	blow_motor_step3_on();
	fml_test_delay();
	blow_motor_step3_off();
	#endif
	
	/*8*/
	#ifdef TEST_FUNC_NUMB_08
	blow_motor_step2_on();
	fml_test_delay();
	blow_motor_step2_off();
	#endif

	/*9*/
	#ifdef TEST_FUNC_NUMB_09
	blow_motor_step1_on();
	fml_test_delay();
	blow_motor_step1_off();
	#endif
	fml_test_delay();
	fml_test_delay();
	fml_test_delay();
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_temp(datall* p_data)
{
	unsigned int timeout = 0;
	
	while(1)
	{
		if(p_data->uart.rec_uart1_ok)
		{
			break;
		}
		timeout++;
		if(timeout > 60000)
		{
			timeout = 60000;
		}
		watchdog();
	}
	if(p_data->temperature.value == 0)			///< short circuit
	{
		errcod = 1;
	}
	else if(p_data->temperature.value > 80)		///< cut circuit
	{
		errcod = 2;
	}
	else if(timeout == 60000)					///< lost uart from display
	{
		errcod = 3;
	}
	else
	{
		errcod = 0;
	}
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_init(void)
{	
	blow_fan_off();
	light_off();	
	rav_off();
	PTC_off();

	blow_motor_step1_off();
	blow_motor_step2_off();
	blow_motor_step3_off();
	blow_motor_step4_off();
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
switchstate fml_test_check(void) 
{
	static unsigned char s_count = 0;
	
	if(self_test() == 0)
	{
		s_count++;
		if(s_count > 2)
		{
			fml_test_init();
			return YES;
		}
	}
	else
	{
		s_count = 0;
	}
	return NO;
}

/***********************************************************************************************************************
* Function Name: 
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void fml_test_logic(datall* p_data)
{
	static unsigned char s_step = 0;

	if(YES == p_data->testmode)
	{
		watchdog();
		switch(s_step)
		{
			case 0:
				s_step++;
				p_data->buzzer.burn_bee_on = ON;	
				fml_test_uart(0);
				fml_test_temp(p_data);
				break;
			case 1:
				if(OFF == p_data->buzzer.burn_bee_on)	///< start test beep
				{
					fml_test_port();
					fml_test_motor();
					s_step = 0;
				}
				break;
			default:
				break;
		}
	}	
}

