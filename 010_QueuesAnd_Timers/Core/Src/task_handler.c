/*
 * task_handler.c
 *
 *  Created on: Apr 20, 2026
 *      Author: nachiket
 */

#include "main.h"

/* Function definations START */
int extract_command(command_t *cmd);
void process_command(command_t *cmd);
/* Function definations END */

/* Global Variables START */
const char *msg_inv="\n///// INVALID OPTION /////\n";
/* Global Variables END */

void menu_task(void* parameters)
{
	uint32_t cmd_addr=0;
	//here the cmd variable is of command_t * or command_t* type both are same and the value which cmd holds is of command_t type
	//and command_t* or command_t * just means that cmd holds "an address"
	command_t *cmd=0;

	int option;

	const char* msg_menu="=============================\n"
						 "|            MENU            |\n"
			             "=============================\n"
						 "LED effect    --------> 0\n"
			             "Date and Time --------> 1\n"
			             "Exit          --------> 2\n"
			             "Enter your choice : \n";
	while(1)
	{
		//Sending menu to the print queue
		xQueueSend(q_print, &msg_menu, portMAX_DELAY);

		//Waiting until the command is entered by the user
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);

		//typecasting the cmd_addr pointer into command_t pointer to read the struct's content properly
		cmd=(command_t*)cmd_addr;

		if(cmd->len==1)
		{
			option=cmd->payload[0]-48; //conv ascii to no. by subtracting 48
			switch(option)
			{
			case 0:
				curr_state=sLedEffect;
				xTaskNotify(led_task_handle, 0, eNoAction);
				break;
			case 1:
				curr_state=sRtcMenu;
				xTaskNotify(rtc_task_handle, 0, eNoAction);
				break;
			case 2: //exit
				break;
			default:
				xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				continue;
			}
		}
		else{
			xQueueSend(q_print, &msg_inv, portMAX_DELAY); //invalid option selected
			continue;
		}
		//waits to run the while loop again until another task notifies the menu_task
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	}
}

void led_task(void* parameters)
{
	uint32_t cmd_addr;
	command_t* cmd;
	const char* msg_led= "=============================\n"
						 "|         LED EFFECTS        |\n"
			             "=============================\n"
						 "(none, e1, e2, e3, e4)\n"
			             "Enter your choice : \n";
	while(1)
	{
		//wait for notif from menu task
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		//after receiving notif print the led menu
		xQueueSend(q_print, &msg_led, portMAX_DELAY);

		//waiting for user input
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd=(command_t*)cmd_addr;

		if(cmd->len <= 4)
		{
			//strcmp compares both the strings if equal return 0
			if(! strcmp((char*)cmd->payload,"none"))
				led_effect_stop();
			else if(! strcmp((char*)cmd->payload,"e1"))
				led_effect(1);
			else if(! strcmp((char*)cmd->payload,"e2"))
				led_effect(2);
			else if(! strcmp((char*)cmd->payload,"e3"))
				led_effect(3);
			else if(! strcmp((char*)cmd->payload,"e4"))
				led_effect(4);
			else
				xQueueSend(q_print, &msg_inv, portMAX_DELAY);
		}
		else
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);

		//update the state variable
		curr_state=sMainMenu;

		//notify task menu so it can take over the screen again
		xTaskNotify(menu_task_handle, 0, eNoAction);
	}
}

void command_handler_task(void* parameters)
{
	BaseType_t ret;
	command_t cmd;

	while(1)
	{
		//wait for notification to arrive
		ret=xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		if(ret==pdTRUE)
			process_command(&cmd);
	}
}

uint8_t getnumber(uint8_t *p , int len)
{
	int value ;

	if(len > 1)
	   value =  ( ((p[0]-48) * 10) + (p[1] - 48) );
	else
		value = p[0] - 48;

	return value;
}

void rtc_task(void *param)
{
	const char* msg_rtc1 =  "========================\n"
							"|          RTC         |\n"
							"========================\n";

	const char* msg_rtc2 =  "Configure Time            ----> 0\n"
							"Configure Date            ----> 1\n"
							"Enable reporting          ----> 2\n"
							"Exit                      ----> 3\n"
							"Enter your choice here : \n";


	const char *msg_rtc_hh = "Enter hour    (1-12):\n";
	const char *msg_rtc_mm = "Enter minutes (0-59):\n";
	const char *msg_rtc_ss = "Enter seconds (0-59):\n";
	const char *msg_rtc_tf = "Enter the time format (for AM input 1/for PM input 2):\n";

	const char *msg_rtc_dd  = "Enter date(1-31):\n";
	const char *msg_rtc_mo  ="Enter month(1-12):\n";
	const char *msg_rtc_dow  = "Enter day(1-7 sun:1):\n";
	const char *msg_rtc_yr  = "Enter year(0-99):\n";

	const char *msg_conf = "Configuration successful\n";
	const char *msg_rtc_report = "Enable time and date reporting(y/n)?:\n ";


	uint32_t cmd_addr;
	command_t *cmd;

	static int rtc_state = 0;
	int menu_code;

#define HH_CONFIG      0
#define MM_CONFIG      1
#define SS_CONFIG      2
#define TF_CONFIG      3

#define DATE_CONFIG    0
#define MONTH_CONFIG   1
#define YEAR_CONFIG    2
#define DAY_CONFIG     3

#define AM             1
#define PM             2


	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;


	while(1)
	{
		//Wait till the rtc task is notified
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		//Print the menu and show current date and time information
		xQueueSend(q_print, &msg_rtc1, portMAX_DELAY);
		show_time_date();
		xQueueSend(q_print, &msg_rtc2, portMAX_DELAY);


		while(curr_state != sMainMenu)
		{
			//Wait for command notification (Notify wait)
			xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
			cmd=(command_t*)cmd_addr;

			switch(curr_state)
			{
			case sRtcMenu:
			{
				//process RTC menu commands
				if(cmd->len==1)
				{
					menu_code=cmd->payload[0]-48;
					switch(menu_code)
					{
						case 0:
							curr_state=sRtcTimeConfig;
							xQueueSend(q_print, &msg_rtc_hh, portMAX_DELAY);
							break;
						case 1:
							curr_state=sRtcDateConfig;
							xQueueSend(q_print, & msg_rtc_dd, portMAX_DELAY);
							break;
						case 2:
							curr_state=sRtcReport;
							xQueueSend(q_print, &msg_rtc_report, portMAX_DELAY);
							break;
						case 3:
							curr_state=sMainMenu;
							break;
						default:
							curr_state=sMainMenu;
							xQueueSend(q_print, &msg_inv, portMAX_DELAY);
					}
				}
				else
				{
					curr_state=sMainMenu;
					xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				}
				break;
			}

			case sRtcTimeConfig:
			{
				switch(rtc_state)
				{
				case HH_CONFIG:
					uint8_t hour=getnumber(cmd->payload, cmd->len);
					time.Hours=hour;
					rtc_state=MM_CONFIG;
					xQueueSend(q_print, &msg_rtc_mm, portMAX_DELAY);
					break;
				case MM_CONFIG:
					uint8_t min=getnumber(cmd->payload, cmd->len);
					time.Minutes=min;
					rtc_state=SS_CONFIG;
					xQueueSend(q_print, &msg_rtc_ss, portMAX_DELAY);
					break;
				case SS_CONFIG:
					uint8_t sec=getnumber(cmd->payload, cmd->len);
					time.Seconds=sec;
					rtc_state=TF_CONFIG; //time format
					xQueueSend(q_print, &msg_rtc_tf, portMAX_DELAY);
					break;
				case TF_CONFIG:
					uint8_t tf=getnumber(cmd->payload, cmd->len);
					if(tf==AM)
						time.TimeFormat=RTC_HOURFORMAT12_AM;
					else if(tf==PM)
						time.TimeFormat=RTC_HOURFORMAT12_PM;
					else
					{
						xQueueSend(q_print, &msg_inv, portMAX_DELAY);
						curr_state=sMainMenu;
						rtc_state=0;
						break; //breaking the rtc_state switch
					}

					//Validating the info entered
					if(!validate_rtc_information(&time,NULL))
					{
						rtc_configure_time(&time);
						xQueueSend(q_print,&msg_conf,portMAX_DELAY);
						show_time_date();
					}
					else
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);

					curr_state = sMainMenu;
					rtc_state = 0;
					break; //breaking the rtc_state switch
				}
				break; //breaking the curr_state switch
			}

			case sRtcDateConfig:
			{
				switch(rtc_state)
				{
				case DATE_CONFIG:
					uint8_t d=getnumber(cmd->payload, cmd->len);
					date.Date=d;
					rtc_state=MONTH_CONFIG;
					xQueueSend(q_print, &msg_rtc_mo, portMAX_DELAY);
					break;
				case MONTH_CONFIG:
					uint8_t m=getnumber(cmd->payload, cmd->len);
					date.Month=m;
					rtc_state=YEAR_CONFIG;
					xQueueSend(q_print, &msg_rtc_yr, portMAX_DELAY);
					break;
				case YEAR_CONFIG:
					uint8_t y=getnumber(cmd->payload, cmd->len);
					date.Year=y;
					rtc_state=DAY_CONFIG;
					xQueueSend(q_print, &msg_rtc_dow, portMAX_DELAY);
					break;
				case DAY_CONFIG:
					uint8_t day=getnumber(cmd->payload, cmd->len);
					date.WeekDay=day;
					if(!validate_rtc_information(NULL,&date))
					{
						rtc_configure_date(&date);
						xQueueSend(q_print,&msg_conf,portMAX_DELAY);
						show_time_date();
					}
					else
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);

					curr_state=sMainMenu;
					rtc_state=0;
					break; //breaking the rtc_state switch
				}
				break; //breaking the curr_state switch
			}

			case sRtcReport:
			{
				//enable or disable RTC current time reporting over ITM printf
				if(cmd->len==1)
				{
					if(cmd->payload[0]=='y')
					{
						if(xTimerIsTimerActive(rtc_timer)==pdFALSE)
							xTimerStart(rtc_timer, portMAX_DELAY);
					}
					else if(cmd->payload[0]=='n')
					{
						xTimerStop(rtc_timer, portMAX_DELAY);
					}
					else
					{
						xQueueSend(q_print, &msg_inv, portMAX_DELAY);
					}
				}
				else
					xQueueSend(q_print, &msg_inv, portMAX_DELAY);

				curr_state=sMainMenu;

				break;
			}
			default:
				break;
			}// switch end

		} //while end

		//Notify menu task
		xTaskNotify(menu_task_handle, 0, eNoAction);
		}//while super loop end
}

void printf_task(void* parameters)
{
	uint32_t *msg;
	while(1)
	{
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg), HAL_MAX_DELAY);
	}
}

/* Helper Functions */
void process_command(command_t *cmd)
{
	extract_command(cmd);

	switch(curr_state)
	{
	case sMainMenu:
		xTaskNotify(menu_task_handle, (uint32_t)cmd, eSetValueWithOverwrite);
	break;

	case sLedEffect:
		xTaskNotify(led_task_handle, (uint32_t)cmd, eSetValueWithOverwrite);
	break;

	case sRtcMenu:
	case sRtcTimeConfig:
	case sRtcDateConfig:
	case sRtcReport:
		xTaskNotify(rtc_task_handle, (uint32_t)cmd, eSetValueWithOverwrite);
	break;
	}
}

int extract_command(command_t *cmd)
{

	uint8_t item=0;
	BaseType_t status;

	status=uxQueueMessagesWaiting(q_data);
	if(!status) //checks if there is any item waiting in queue
		return -1;

	uint8_t i=0;
	do
	{
		status=xQueueReceive(q_data, &item, 0);
		if(status==pdTRUE)
			cmd->payload[i++]=item;
	}while(item!='\n');

	cmd->payload[i-1]='\0';
	cmd->len=i-1; //saving the len of command excluding the null char

	return 0;
}
