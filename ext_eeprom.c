#define USE_AND_OR /* To enable AND_OR mask setting */
#include<i2c.h>
#include<ports.h>
#include<stdio.h>

#include "mcu_timer.h"
#include "ext_eeprom.h"
#include "variable.h"
#include "ext_lcd.h"
#include "main.h"
#include "peripherial.h"

//******************** Interrupt sevice rotuine for I2C2 Master ***************************
void __attribute__((interrupt,no_auto_psv)) _MI2C2Interrupt(void)
{
  MI2C2_Clear_Intr_Status_Bit;  //Clear Interrupt status of I2C2
}



void eeprom_write(unsigned int data)
{
    unsigned char eeprom_counter=0,tempcntr;
    unsigned int  eeprom_address=0;  //v3.1.2.D
    signed char eeprom_data_string[25]; //---------read_array[20] = {0};
    unsigned int offline_ftoi_temp=0;	//v2.2.9
    unsigned int coin_ftoi_temp=0;	//v2.2.9  //1.0.2

    for(tempcntr = 0; tempcntr<sizeof(eeprom_data_string); tempcntr++)
    {
	    eeprom_data_string[tempcntr] = '#';	//0x23;
    }
	
    eeprom_counter=0;
    eeprom_counter=0;
    i2c_bus_recovery();		//i2c for eeprom
    
switch(data)
{
    case SER_SIGNATURE_BYTE:					                                //0
        eeprom_address = EE_SIGNATURE_BYTE;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('S'));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('A'));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('R'));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('V'));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('A'));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('J'));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('A'));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)('L'));

        strcpy(lcd_data,"SIGNATURE:");
        lcd_display(1);
        //sprintf(lcd_data,"%s",eeprom_data_string);
        strcpy(lcd_data,"****************");
        lcd_display(2);
    break;

    case SER_ERASE_SIGNATURE:						                            //99
        eeprom_address = EE_SIGNATURE_BYTE;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(0));

	if(mode_flags.op_mode == DEBUG_MODE)
	{
		strcpy(lcd_data,"SIGNATURE:");
		lcd_display(1);
		sprintf(lcd_data,"%s",eeprom_data_string);
		lcd_display(2);
	}
	else{}
    break;

    case SER_TDS_CALIB:	//Calibrate TDS-1				                        //1
        eeprom_address = EE_TDS_CALIB;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)((tds_calib_fact & 0xFF00) >> 8));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(tds_calib_fact & 0x00FF));

	if(mode_flags.op_mode == DEBUG_MODE)
	{
		strcpy(lcd_data,"TDS CALIB FACT:");
		lcd_display(1);
		sprintf(lcd_data,"%u",tds_calib_fact);
		lcd_display(2);
	}
	else{}
	    break;

    case SER_OUT_FLOW_CALIB:	   //out_flow_calibration                       //2
	   
	    eeprom_address = EE_OUT_FLOW_CALIB;
	    eeprom_data_string[eeprom_counter++] = ((out_flow_calibration & 0xFF00) >> 8);  //3.1.1
	    eeprom_data_string[eeprom_counter++] = (out_flow_calibration & 0x00FF);    //3.1.1

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"OUT FLOW CALIB:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",out_flow_calibration);
		    lcd_display(2);
	    }
	    else{}
	    break;

    case SER_IN_FLOW_CALIB:	                                                    //3
	    //in_flow_calibration
	    eeprom_address = EE_IN_FLOW_CALIB;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((in_flow_calibration & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(in_flow_calibration & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"IN FLOW CALIB:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",in_flow_calibration);
		    lcd_display(2);
	    }
	    else{}
	    break;

    case SER_SERVER_IP:	 //server_ip[]                                          //4	
        
	    eeprom_address = EE_SERVER_IP;
	    strcpy(eeprom_data_string,server_ip);
        eeprom_counter = (sizeof(server_ip));
	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"SR IP:");
		    lcd_display(1);
		    sprintf(lcd_data,"%s",server_ip);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_SERVER_PORT:	//server_port[]                                     //5     
        
        eeprom_address = EE_SERVER_PORT;
        strcpy(eeprom_data_string,server_port);
        eeprom_counter = (sizeof(server_port));                                 //3.1.1
        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"SR PORT NO:");
            lcd_display(1);
            sprintf(lcd_data,"%s",server_port);
            lcd_display(2);
        }
        else{}
    break;

    case SER_SERVER_APN:	 //apn[]                	                    	//6	   
        
        eeprom_address = EE_SERVER_APN;
        strcpy(eeprom_data_string,apn);
        eeprom_counter = (sizeof(apn));                                         //3.1.1
        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"NW APN:");
            lcd_display(1);
            sprintf(lcd_data,"%.16s",apn);//v3.2.0.A  //%s to .16s
            lcd_display(2);
        }
        else{}
    break;

    case SER_TDS_MAX:    //max_tds               			                 	//7	 
        
	    eeprom_address = EE_TDS_MAX;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((max_tds & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(max_tds & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"MAX TDS VALUE:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",max_tds);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_TDS_MIN:	                        			                	//8	 
        
	    eeprom_address = EE_TDS_MIN;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((min_tds & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(min_tds & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"MIN TDS VALUE:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",min_tds);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_USER_TIMEOUT:	         //user_timeout                             //9	
        
	    eeprom_address = EE_USER_TIMEOUT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((user_timeout & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(user_timeout & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"USER TIMEOUT:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",user_timeout);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_DISP_TIMEOUT:	       //dispense_timeout                           //10	
        
	    eeprom_address = EE_DISP_TIMEOUT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((dispense_timeout & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(dispense_timeout & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"DISPENSE TIMEOUT"); //v3.1.1.B
		    lcd_display(1);
		    sprintf(lcd_data,"%u",dispense_timeout);
		    lcd_display(2);
	    }
	    else{}
    break;

    //case SER_LLS_DETECT_STATE:	                                                //11
    case SER_SLEEP_MODE_FLAG:
//@			eeprom_address = EE_SLEEP_MODE_FLAG;
//			eeprom_data_string[eeprom_counter++] = peripheral_flags.sleep_mode_flag;
//			
//			if(mode_flags.op_mode == DEBUG_MODE)
//			{
//				strcpy(lcd_data,"SLEEP MODE FLAG");
//				lcd_display(1);
//				if(peripheral_flags.sleep_mode_flag)
//					strcpy(lcd_data,"ON");
//				else
//					strcpy(lcd_data,"OFF");
//				lcd_display(2);
//			}
//@			else{}
    break;

    case SER_DEVICE_EN_DIS:	    //Enable/Disable Device 	              		//12
	   	
	    eeprom_address = EE_DEVICE_EN_DIS;
	    eeprom_data_string[eeprom_counter++] = peripheral_flags.dev_en_dis;

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"DEVICE EN/DIS:");
		    lcd_display(1);
		    if(peripheral_flags.dev_en_dis)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;

	case SER_TDS_EN_DIS:	//Enable/Disable TDS Sensor	                		//13
	   
	    eeprom_address = EE_TDS_EN_DIS;
	    eeprom_data_string[eeprom_counter++] = peripheral_flags.tds_en_dis;

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"TDS EN/DIS:");
		    lcd_display(1);				
		    if(peripheral_flags.tds_en_dis)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
	    break;

    case SER_LLS_HLS_EN_DIS:	//Enable/Disable LLS		             		//14
        
	    eeprom_address = EE_LLS_HLS_EN_DIS;
	    eeprom_data_string[eeprom_counter++] = peripheral_flags.lls_hls_en_dis;

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"LLS HLS EN/DIS:");
		    lcd_display(1);
		    if(peripheral_flags.lls_hls_en_dis)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
	    break;

    case SER_MASS_DISP_EN_DIS:	//Enable/Disable Mass Dispense                  //15
    break;

    case SER_SCHOOL_DISPENSE_AMT:   //max_dispense_litre	                    //16	
        
	    eeprom_address = EE_SCHOOL_DISPENSE_AMT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((school_disp_amount & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(school_disp_amount & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"SCHOOL DISP AMT:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",school_disp_amount);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_ER_DISPLAY_TIMER:		  //error_display_time 	                    //17
	   	
	    eeprom_address = EE_ER_DISPLAY_TIMER;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((error_display_time & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(error_display_time & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"ERROR DISP TIME:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",error_display_time);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_DISPENSE_LT_LC:		                                            //18
        
	    /*//DISPENSE_LEAST_COUNT		
	    //address_high_byte = ((unsigned char)(EE_DISPENSE_LT_LC >> 8));
	    //address_low_byte = ((unsigned char)(EE_DISPENSE_LT_LC & 0x00FF));
	    //if(!fresh_eeprom)
		    eeprom_address = EE_DISPENSE_LT_LC;
	    //else
	    //	eeprom_address = DFLT_DISPENSE_LT_LC;
	    tempvar = ((unsigned long)(DISPENSE_LEAST_COUNT));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((tempvar & 0xFF000000) >> 24));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((tempvar & 0x00FF0000) >> 16));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((tempvar & 0x0000FF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(tempvar & 0x000000FF));

	    strcpy(lcd_data,"DISP LEAST CNT:");
	    lcd_display(1);
	    sprintf(lcd_data,"%2.2f",DISPENSE_LEAST_COUNT);
	    lcd_display(2);*/
    break;

    case SER_NEW_PASSWORD_SET:	//current_password[]	                     		//19	    
	    eeprom_address = EE_NEW_PASSWORD_SET;
	    strcpy(eeprom_data_string,current_password);
        eeprom_counter = (sizeof(current_password));                            //3.1.1
	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"NEW PASSWORD:");
		    lcd_display(1);
		    strcpy(lcd_data,"****************");
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_LOGOUT_IDLE_TIMEOUT:		   //logout_idle_time					//20	
        
	    eeprom_address = EE_LOGOUT_IDLE_TIMEOUT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((logout_idle_time & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(logout_idle_time & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"LOGOUT IDLE TIME");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",logout_idle_time);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_LT_EMPTY_TANK:			   //lit_empty_tank  						//21
        
	    eeprom_address = EE_LT_EMPTY_TANK;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((lit_empty_tank & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(lit_empty_tank & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    //---strcpy(lcd_data,"SM SWITCH DELAY");
		    strcpy(lcd_data,"LT TO EMPTY TANK");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",lit_empty_tank);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_WATER_EXPIRED:             //Water Expired		         			//22
        
	    eeprom_address = EE_WATER_EXPIRED;
	    eeprom_data_string[eeprom_counter++] = error_flags.water_expired;

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    if(error_flags.water_expired)
			    strcpy(lcd_data,"WATER EXPIRED");
		    else
			    strcpy(lcd_data,"NO WATER EXPIRED");  //WATER NOT EXPIRED
		    lcd_display(1);
	    }
	    else{}
    break;

    case SER_FACT_PASSWORD_RESET:	//Factory Password Reset-	            	//23
	  
	    eeprom_address = EE_FACT_PASSWORD_RESET;
	    strcpy(eeprom_data_string,factory_password);
        eeprom_counter = (sizeof(factory_password));                            //3.1.1
	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"FACTORY PASSWORD:");
		    lcd_display(1);
		    sprintf(lcd_data,"****************");
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_RESET_OOO:	             //Reset Out of Order		      			//24
    break;

    case SER_REMOTE_DUMP:           //Remote Dump Water-		        		//25
    break;

    case SER_RQ_SYSTEM_SETTINGS:    //Request System Settings		            //26
    break;

    case SER_IDLE_MSG_FREQ:    	//Idle Message Frequency-		                //27
        
	    eeprom_address = EE_IDLE_MSG_FREQ;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((idle_msg_freq & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(idle_msg_freq & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"HB MSG FREQ:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",idle_msg_freq);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_RECONNECTION:	//force fully Request Reconnection-	            	//28
	    break;

    case SER_RESET_ALL_OOO:	//force fully clear all error-	            		//29				
	    break;

    case SER_DUMP_TIMEOUT:			 //dump_timeout 		     				//30
	   
	    eeprom_address = EE_DUMP_TIMEOUT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((dump_timeout & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(dump_timeout & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"DUMP TIMEOUT:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",dump_timeout);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_REFILL_TIMEOUT:	 //refill_timeout                               //31
	   		
	    eeprom_address = EE_REFILL_TIMEOUT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((refill_timeout & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(refill_timeout & 0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"REFILL TIMEOUT:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",refill_timeout);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_CLEANING_TIMEOUT:	 //cleaning_timeout	                            //32
	   	
	    eeprom_address = EE_CLEANING_TIMEOUT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((cleaning_timeout & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(cleaning_timeout&0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"CLEANING TIMEOUT"); //v3.1.1.B
		    lcd_display(1);
		    sprintf(lcd_data,"%u",cleaning_timeout);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_CLEAN_TANK:						                              	//35				
    break;

    case SER_MODEM_COMMAND:						                            	//36
    break;

    case SER_DEBUG_MODE:            // debug_mode			            		//39
	    
	    eeprom_address = EE_DEBUG_MODE;

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"DEBUG MODE:");
		    lcd_display(1);
		    if(mode_flags.op_mode == DEBUG_MODE)
		    {
			    strcpy(lcd_data,"ENABLED");
			    eeprom_data_string[eeprom_counter++] = 1;
		    }
		    else
		    {
			    strcpy(lcd_data,"DISABLED");
			    eeprom_data_string[eeprom_counter++] = 0;
		    }	
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_CHANGE_DEV_ID:	 //MACHINE_ID		                		    	//40
	   
	    eeprom_address = EE_CHANGE_DEVICE_ID;
	    strcpy(eeprom_data_string,MACHINE_ID);
        eeprom_counter = (sizeof(MACHINE_ID));                          //3.1.1
	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"DEVICE ID:");
		    lcd_display(1);
		    sprintf(lcd_data,"%s",MACHINE_ID);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_LEAK_TIMER:   //leak_timeout		                       			//41    
	    
	    eeprom_address = EE_LEAK_DETECT_TIMER;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((leak_timeout & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(leak_timeout&0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"LEAK TIME OUT:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",leak_timeout);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_BUZZER_CTRL:	 //system_flags.pag_buzzer_control		            //42
	    
	    eeprom_address = EE_BUZZER_CONTROL;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(peripheral_flags.pag_buzzer_control));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"BUZZER CONTROL:");
		    lcd_display(1);
		    if(peripheral_flags.pag_buzzer_control)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_DEV_SOFT_RESET:	                          						//43
    break;

    case SER_NETWORK_SIGNAL_STRENGTH:		                     				//44
    break;

    case SER_DIAGNOSTIC_TEST:	  //system_flags.diagnostic_test            	//46
	  
	    eeprom_address = EE_RUN_DIAG_TEST;
	    if(mode_flags.op_mode==DIAGNOSTIC_MODE)
	    {
		    mode_flags.run_diagnostic=TRUE;
		    eeprom_data_string[eeprom_counter++] = 1;
	    }
	    else
	    {
		    mode_flags.run_diagnostic=FALSE;
		    eeprom_data_string[eeprom_counter++] = 0;
	    }

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"DIAGNOSTIC TEST:");
		    lcd_display(1);
		    if(mode_flags.run_diagnostic)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;
	    
    case SER_SMS_PERMIT:		//gsm_flags.sms_permit         	 				//49
	    
	    eeprom_address = EE_SMS_PERMISSION;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(gsm_flags.sms_permit));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"SMS PERMIT:");
		    lcd_display(1);
		    if(gsm_flags.sms_permit)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;
	  
    case SER_SCHOOL_MODE:    // school_mode_en	  //3.1.2.I               	//52	         
	    eeprom_address = EE_SCHOOL_MODE;
	    eeprom_data_string[eeprom_counter++] = school_mode_en;	
            if(mode_flags.op_mode == DEBUG_MODE)
            {
                    strcpy(lcd_data,"SCHOOL MODE:");
                    lcd_display(1);
                    if(school_mode_en)
                            strcpy(lcd_data,"ENABLED");
                    else
                            strcpy(lcd_data,"DISABLED");
                    lcd_display(2);
            }
            else{}
    break;	    


    case SER_MASTER_CONT_NO:	    // master_contact		                  	//56	
        
        eeprom_address = EE_MASTER_CONT_NO;
        strcpy(eeprom_data_string,master_contact);
        eeprom_counter = (sizeof(master_contact));                              //3.1.1
        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"MASTER CONTACT:");
            lcd_display(1);
            sprintf(lcd_data,"%s",master_contact);
            lcd_display(2);
        }
        else{}
	break;
    
    case SER_DATA_ER_AUTO_CORRECT:	// error_flags.data_error_auto_correct	    //57  
        
        eeprom_address = EE_DATA_ER_AUTO_CORRECT;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(error_flags.data_error_auto_correct));

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"DATA ER CORRECT");
            lcd_display(1);
            if(error_flags.data_error_auto_correct)
                strcpy(lcd_data,"ENABLED");
            else
                strcpy(lcd_data,"DISABLED");
            lcd_display(2);
        }
        else{}
	break;
	
    case SER_HYST_COUNT:	// hyst_count		                    			//58	    
	
    break;
	
    case SER_TEMPERATURE_CALIB:				                           			//63
        
        eeprom_address = EE_TEMPERATURE_CALIB;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)((temp_calib_fact & 0xFF00) >> 8));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(temp_calib_fact & 0x00FF));

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"TEMP. CALIB FACT");
            lcd_display(1);
            sprintf(lcd_data,"%u",temp_calib_fact);
            lcd_display(2);
        }
        else{}
    break;

  	case SER_WATER_TEMPERATURE_LEVEL:               			//v2.3.6       //64
			eeprom_address = EE_WATER_TEMPERATURE_LEVEL;			//v2.3.6
			eeprom_data_string[eeprom_counter++] = ((unsigned char)(water_temperature_level));			//v2.3.6	
						
			if(mode_flags.op_mode == DEBUG_MODE)
			{
				strcpy(lcd_data,"WATER TEMP. LEVEL");			//v2.3.6
				lcd_display(1);
				sprintf(lcd_data,"%u",water_temperature_level);			//v2.3.6
				lcd_display(2);
			}
			else{}
			break;
    
   case SER_TEMPERATURE_EN_DIS:	     // peripheral_flags.temp_en_dis            //65
	
        eeprom_address = EE_TEMPERATURE_EN_DIS;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(peripheral_flags.temp_en_dis));

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"TEMPERATURE SENS");
            lcd_display(1);
            if(peripheral_flags.temp_en_dis)
                strcpy(lcd_data,"ENABLED");
            else
                strcpy(lcd_data,"DISABLED");
            lcd_display(2);
        }
        else{}
    break;	    
	
    case SER_OUT_FLOW_FREQ_LIMIT:			                           			//66		
	   
	    eeprom_address = EE_OUT_FLOW_FREQ_LIMIT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((output_flow_freq_limit & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(output_flow_freq_limit&0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"OUT FLOW FREQ:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",output_flow_freq_limit);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_IN_FLOW_FREQ_LIMIT:	 //cleaning_timeout		                   	//67
	   
	    eeprom_address = EE_IN_FLOW_FREQ_LIMIT;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((input_flow_freq_limit & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(input_flow_freq_limit&0x00FF));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"IN FLOW FREQ:");
		    lcd_display(1);
		    sprintf(lcd_data,"%u",input_flow_freq_limit);
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_AUTO_REFILL_FLAG:		//refill_flags.auto_refill_flag      		//69
	    
	    eeprom_address = EE_AUTO_REFILL_FLAG;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(refill_flags.auto_refill_flag));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"AUTO REFILL FLAG");
		    lcd_display(1);
		    if(refill_flags.auto_refill_flag)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;


    case SER_FRANCHISEE_RFID:          //v2.2.5					                //70

	    eeprom_address = EE_FRANCHISEE_RFID;
	    strcpy(eeprom_data_string,franchisee_rfid_tag);
        eeprom_counter = (sizeof(franchisee_rfid_tag));                         //3.1.1
	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    lcd_init();
		    strcpy(lcd_data,"FR RFID CARD NO.");
		    lcd_display(1);
		    strcpy(lcd_data,franchisee_rfid_tag);
		    lcd_display(2);
	    }
	    else{}
    break;


    case SER_CORPORATE_MODE:			//v2.2.5                                //71
        
	    eeprom_address = EE_CORPORATE_MODE;
	    eeprom_data_string[eeprom_counter++] = corporate_mode_en;

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"CORPORATE MODE:");
		    lcd_display(1);
		    if(corporate_mode_en)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_CORPORATE_MODE_ACC_ID:			//v2.2.5                            //72
        
	    eeprom_address = EE_CORPORATE_MODE_ACC_ID;
	    for(tempcntr = 0; tempcntr < (sizeof(corp_mode_acc_id)); tempcntr++)
	    {
		    eeprom_data_string[eeprom_counter++] = corp_mode_acc_id[tempcntr];
	    }

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"CORPORATE ACC");
		    lcd_display(1);
		    sprintf(lcd_data,"%s", corp_mode_acc_id);
		    lcd_display(2);
	    }
	    else{}
    break;
    
    case SER_CHILLER_HEATER_EN_DIS:   //V2.2.6	//v2.3.6

    eeprom_address = EE_CHILLER_HEATER_EN_DIS;	//v2.3.6
    eeprom_data_string[eeprom_counter++] = ((unsigned char)(peripheral_flags.chiller_heater_en_dis));

    if(mode_flags.op_mode == DEBUG_MODE)
    {
        if(peripheral_flags.chiller_heater_en_dis==1)		//v2.3.6
            strcpy(lcd_data,"CHILLER IS");
        else if(peripheral_flags.chiller_heater_en_dis==2)	//v2.3.6
            strcpy(lcd_data,"HEATER IS");
        else{}
        lcd_display(1);
        if(peripheral_flags.chiller_heater_en_dis)
            strcpy(lcd_data,"ENABLED");
        else
            strcpy(lcd_data,"DISABLED");
        lcd_display(2);
    }
    else{}
    break;

   
	    
    case SER_OFFLINE_WATER_TARIFF:		//v2.2.9		                    	//87
          
	    eeprom_address = EE_OFFLINE_WATER_TARIFF;
	    offline_ftoi_temp = (unsigned int)offline_water_tariff;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)((offline_ftoi_temp & 0xFF00) >> 8));
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(offline_ftoi_temp & 0x00FF));
    break;

    case SER_MENU_PASSWORD_SET:	//Change SMS Password		             		//88

	    eeprom_address = EE_MENU_PASSWORD_SET;
        strcpy(eeprom_data_string,menu_franchisee_password);  //v3.1.1.H
	    //memcpy(eeprom_data_string,menu_franchisee_password,4);
        eeprom_counter = (sizeof(menu_franchisee_password));                    //3.1.1
	    if(mode_flags.op_mode == DEBUG_MODE)  //ask
	    {
		    strcpy(lcd_data,"NEW PASSWORD:");
		    lcd_display(1);
		    strcpy(lcd_data,"****************");
		    lcd_display(2);
	    }
	    else{}
    break;	
	    
    case SER_TDS_VERSION:   //v2.2.9				                    		//91

        eeprom_address = EE_TDS_VERSION;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(tds_header_version));

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"TDS HEADER:");
            lcd_display(1);
            if(tds_header_version=='C')
                strcpy(lcd_data,"VER. C");
            else
                strcpy(lcd_data,"VER. B");
            lcd_display(2);
        }
        else{}
	break;

    case SER_TEMP_COMPEN_EN_DIS:   //v2.2.9			                     		//92

	    eeprom_address = EE_TEMP_COMPEN_EN_DIS;
	    eeprom_data_string[eeprom_counter++] = ((unsigned char)(peripheral_flags.temp_compen_en_dis));

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"TEMP COMPENSE   "); //TEMP. COMPENSATION //v3.1.1.B
		    lcd_display(1);
		    if(peripheral_flags.temp_compen_en_dis)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;

    case SER_AUTO_SYNC_EN_DIS:      	                                  		//93
	   
	    eeprom_address = EE_AUTO_SYNC_EN_DIS;
	    eeprom_data_string[eeprom_counter++] = system_flags.auto_sync_flag;

	    if(mode_flags.op_mode == DEBUG_MODE)
	    {
		    strcpy(lcd_data,"SYNC EN/DIS:");
		    lcd_display(1);				
		    if(system_flags.auto_sync_flag)
			    strcpy(lcd_data,"ENABLED");
		    else
			    strcpy(lcd_data,"DISABLED");
		    lcd_display(2);
	    }
	    else{}
    break;   
    //  -----------------   v3.1.2.J   ----write 
     case SER_MODEM_CHECK_ROUTINE:	//Calibrate TDS-1				                        //1
        eeprom_address = EE_MODEM_CHECK_ROUTINE;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)((modem_status_check_hour & 0xFF00) >> 8));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(modem_status_check_hour & 0x00FF));

//	if(mode_flags.op_mode == DEBUG_MODE)
//	{
//		strcpy(lcd_data,"MODEM CHECK FREQ:");
//		lcd_display(1);
//		sprintf(lcd_data,"%u",modem_status_check_hour);
//		lcd_display(2);
//	}
//	else{}
	    break;
     //  -----------------   v3.1.2.J   ----write 
	    
    case SER_OP_MODE:			 // mode_flags.op_mode                          //100
       
        eeprom_address = EE_OP_MODE;
        eeprom_data_string[eeprom_counter++] = mode_flags.op_mode;

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"OP MODE:");
            lcd_display(1);
            sprintf(lcd_data,"%u", mode_flags.op_mode);
            lcd_display(2);
        }
        else{}
	break;	    
	    
    case SER_ERROR_STACK:		 // error_stack[16]                             //101
        
        eeprom_address = EE_ERROR_STATE;
        //for(tempcntr = 0; tempcntr <= (sizeof(error_stack)); tempcntr++)
        for(tempcntr = 0; tempcntr < TOTAL_ERRORS; tempcntr++)
        {
            eeprom_data_string[eeprom_counter++] = error_stack[tempcntr];
        }

//        if(mode_flags.op_mode == DEBUG_MODE)
//        {
//            strcpy(lcd_data,"ERROR STACK.:");   //1.0.1
//            lcd_display(1);
//            sprintf(lcd_data,"%s", error_stack);
//            lcd_display(2);
//        }
//        else{}
    break;    
    
    case SER_SCHOOL_MODE_ACC_ID:		              	                        //102
        
       	eeprom_address = EE_SCHOOL_MODE_ACC_ID;
        for(tempcntr = 0; tempcntr < (sizeof(sch_mode_acc_id)); tempcntr++)
        {
            eeprom_data_string[eeprom_counter++] = sch_mode_acc_id[tempcntr];
        }

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"SCHOOL AC ID:");
            lcd_display(1);
            sprintf(lcd_data,"%s", sch_mode_acc_id);
            lcd_display(2);
        }
        else{}
    break; 

    case SER_TANK_LOW_LEVEL_STATUS:		                                        //108
        
        eeprom_address = EE_TANK_LOW_LEVEL_STATUS;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)((tank_low_level_litre & 0xFF00) >> 8));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(tank_low_level_litre & 0x00FF));

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"TANK LOW STATUS ");
            lcd_display(1);
            sprintf(lcd_data,"%u",tank_low_level_litre);
            lcd_display(2);
        }
        else{}
    break;
    
    case SER_CARD_INIT_BAL:	                                                    //112		
        
        eeprom_address = EE_CARD_INIT_BAL;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)((card_init_balance & 0xFF00) >> 8));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(card_init_balance & 0x00FF));
        if(mode_flags.op_mode == DEBUG_MODE)                                    //v3.1.1.G
        {
            strcpy(lcd_data,"CARD INIT BAL");
            lcd_display(1);
            sprintf(lcd_data,"%u",card_init_balance);
            lcd_display(2);
        }else{}
    break;	    
	    
    case SER_PREV_DATE:			                                                //113	  
        
        eeprom_address = EE_PREV_DATE;
        eeprom_data_string[eeprom_counter++] = P_date;
    break;

    case SER_PREV_MONTH:		                                                //114	  
        
        eeprom_address = EE_PREV_MONTH;
        eeprom_data_string[eeprom_counter++] = P_month;
    break;

    case SER_PREV_YEAR:				                                            //115 
        
        eeprom_address = EE_PREV_YEAR;
        eeprom_data_string[eeprom_counter++] = P_yr;
    break;
	    
    case SER_ULTRA_SUPER_ADMIN_MODE:			                                //116
        
        eeprom_address = EE_ULTRA_SUPER_ADMIN_MODE;

        if(mode_flags.op_mode == ULTRA_SUPER_ADMIN_MODE)
        {	
            eeprom_data_string[eeprom_counter++] = 1;
        }
        else{
            eeprom_data_string[eeprom_counter++] = 0;
        }	
    break;			
 	
//  ------------------------------   COIN DATA   ----------------------------   //v2.2.9 
    
    case SER_COIN_EN_DIS:	//v2.2.9                                            //74
        
        eeprom_address = EE_COIN_EN_DIS;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(peripheral_flags.coin_en_dis));

        if(mode_flags.op_mode == DEBUG_MODE)
        {
            strcpy(lcd_data,"COIN BOX");
            lcd_display(1);
            if(peripheral_flags.coin_en_dis)
                strcpy(lcd_data,"ENABLED");
            else
                strcpy(lcd_data,"DISABLED");
            lcd_display(2);
        }
        else{}
    break;

    case SER_COIN_CHN_EN_DIS:		//v2.2.9                                    //75
        
    	eeprom_address = EE_COIN_CHN_EN_DIS;
    	eeprom_data_string[eeprom_counter++] = ((unsigned char)(channel_cmd));
    break;

    case SER_TOTAL_COIN_AMOUNT:	//v2.2.9	                                    //76
        
    	eeprom_address = EE_TOTAL_COIN_AMOUNT; 
    	eeprom_data_string[eeprom_counter++] = ((unsigned char)((total_coin_amount & 0xFF00) >> 8));
    	eeprom_data_string[eeprom_counter++] = ((unsigned char)(total_coin_amount & 0x00FF));
    break;

    case SER_TOTAL_COIN_COUNT:	//v2.2.9                                        //77 
        
    	eeprom_address = EE_TOTAL_COIN_COUNT;
    	eeprom_data_string[eeprom_counter++] = ((unsigned char)((total_coin_count & 0xFF00) >> 8));
    	eeprom_data_string[eeprom_counter++] = ((unsigned char)(total_coin_count & 0x00FF));
    break;

    case SER_COIN_WATER_TARIFF:		//v2.2.9	                                //78	
        
    	eeprom_address = EE_COIN_WATER_TARIFF;
    	coin_ftoi_temp = (unsigned int)coin_water_tariff;
    	eeprom_data_string[eeprom_counter++] = ((unsigned char)((coin_ftoi_temp & 0xFF00) >> 8));
    	eeprom_data_string[eeprom_counter++] = ((unsigned char)(coin_ftoi_temp & 0x00FF));
    break;

    case SER_COIN_COLLECT_CAPACITY:	//v2.2.9                                    //79	
        
        eeprom_address = EE_COIN_COLLECT_CAPACITY;
        eeprom_data_string[eeprom_counter++] = ((unsigned char)((coin_collector_capacity & 0xFF00) >> 8));
        eeprom_data_string[eeprom_counter++] = ((unsigned char)(coin_collector_capacity & 0x00FF));
    break;

    case SER_COIN_ID:	//v2.2.9                                                //81
        
	    switch(coin_id_channel_no)
	    {
	    case 1:
		    eeprom_address = EE_COIN_ID_CHN1;
		    strcpy(coin_id.channel_1,temp_coin_id);
		    break;
	    case 2:
		    eeprom_address = EE_COIN_ID_CHN2;
		    strcpy(coin_id.channel_2,temp_coin_id);
		    break;
	    case 3:
		    eeprom_address = EE_COIN_ID_CHN3;
		    strcpy(coin_id.channel_3,temp_coin_id);
		    break;
	    case 4:
		    eeprom_address = EE_COIN_ID_CHN4;
		    strcpy(coin_id.channel_4,temp_coin_id);
		    break;
	    case 5:
		    eeprom_address = EE_COIN_ID_CHN5;
		    strcpy(coin_id.channel_5,temp_coin_id);
		    break;
	    case 6:
		    eeprom_address = EE_COIN_ID_CHN6;
		    strcpy(coin_id.channel_6,temp_coin_id);
		    break;
	    case 7:
		    eeprom_address = EE_COIN_ID_CHN7;
		    strcpy(coin_id.channel_7,temp_coin_id);
		    break;
	    case 8:
		    eeprom_address = EE_COIN_ID_CHN8;
		    strcpy(coin_id.channel_8,temp_coin_id);
		    break;
	    default:
		break;
	    }
	    strcpy(eeprom_data_string,temp_coin_id);
      eeprom_counter = (sizeof(temp_coin_id)); 
    break;

    case SER_COIN_COLLECTOR_ERROR:			                                //109
    	
    	eeprom_address = EE_COIN_COLLECTOR_ERROR;
    	eeprom_data_string[eeprom_counter++] = coin_flags.coin_collector_error;

	break;


//		case SER_SERVER_LIVE_CHECK_EN_DIS:   //V2.3.5
     
//			eeprom_address = EE_SERVER_LIVE_CHECK_EN_DIS;
//			eeprom_data_string[eeprom_counter++] = ((unsigned char)(system_flags.server_live_check_en_dis));
//			break;

	default:
		break;
} //end of switch case
    if(eeprom_counter>=1)  //v3.1.2.D
    {
    write_eeprom_multiple_bytes(eeprom_data_string, eeprom_address,eeprom_counter);
    i2c_wait(10);
    }
    timer2_sec_delay(1);
}

void eeprom_config(unsigned int address, unsigned char rw_bit)	//
{
	unsigned char eeprom_dev_address,local_cnt=0;	//---eeprom_cmd_address,
	unsigned char data[3]={0};

	StartI2C2();	//Send the Start Bit
	IdleI2C2();		//Wait to complete

	eeprom_dev_address = 0x50;	//0b1010000 Serial EEPROM address

	if(rw_bit==EE_RD_BIT)
		data[0] = (eeprom_dev_address << 1) | 1;	//Device Address & RD=1 & WR=0
	else
		data[0] = (eeprom_dev_address << 1) | 0;	//Device Address & RD=1 & WR=0

	data[1] = (address >> 8) & 0x00FF;				// Assigning MSByte of the address
	data[2] = (address & 0x00FF);					// Assigning LSByte of the address

	while(local_cnt<=2)		//write single byte
	{
		MasterWriteI2C2(data[local_cnt++]);
		IdleI2C2();		//Wait to complete

		if(rw_bit==EE_RD_BIT)
			break;
		else{}
		//ACKSTAT is 0 when slave acknowledge, 
		//if 1 then slave has not acknowledge the data.
		if(I2C2STATbits.ACKSTAT)
		{
			strcpy(lcd_data,"EEPROM ERROR");
			lcd_display(2);
			timer2_ms_delay(1000);
			break;
		}
		else{}
	}
}

void i2c_wait(unsigned int cnt)
{
	while(--cnt)
	{
		asm( "nop" );
		asm( "nop" );
	}
}

void init_i2c(void)
{
	UINT config1 = 0;//---,i=0;
	UINT config2 = 0;
	
    /* Turn off I2C modules */
    CloseI2C1();    //Disbale I2C1 mdolue if enabled previously
	
	
	//************ I2C interrupt configuration ******************************************************
	    ConfigIntI2C1(MI2C_INT_OFF);  //Disable I2C interrupt
	
	//***************** I2C1 configuration **********************************************************
	/**********************************************************************************************
	*
	*        I2C1 enabled
	*        continue I2C module in Idle mode
	*        IPMI mode not enabled
	*        I2CADD is 7-bit address
	*        Disable Slew Rate Control for 100KHz
	*        Enable SM bus specification
	*        Disable General call address
	*        Baud @ 8MHz = 39 into I2CxBRG
	**********************************************************************************************/
	   //---config1 = (I2C_ON  | I2C_7BIT_ADD );
	   config1 = (I2C_ON  | I2C_7BIT_ADD|I2C_IDLE_STOP);	//v2.2.4
	   config2 = 255;	//config=39;
	   OpenI2C2(config1,config2);   //configure I2C1
}

void write_ext_eeprom_byte(unsigned char ee_data)
{
	MasterWriteI2C2(ee_data);       //Write Slave address and set master for transmission
	while(I2C2STATbits.TBF);     //Wait till address is transmitted
	while(!IFS3bits.MI2C2IF);    //Wait for ninth clock cycle
	//while((!IFS3bits.MI2C2IF) ||(temp==0))    //Wait for ninth clock cycle
	//	temp++;
	MI2C2_Clear_Intr_Status_Bit; //Clear interrupt flag
	while(I2C2STATbits.TRSTAT);
}

void write_eeprom_int(unsigned int ee_data_int)
{
	write_ext_eeprom_byte(ee_data_int >> 8);	// Writing MSB
	write_ext_eeprom_byte(ee_data_int);			// Writing LSB
}

/*---------------------------------------------------------------------------------------------------
Function Name:	void write_eeprom_multiple_bytes(unsigned char *ee_bytes, unsigned int ee_address)
Arguments:		*ee_bytes = pointer to an array from which data is to be written
				ee_address = address of the eeprom register from where the data write should start
Return Val:		None
Description:	This function takes bytes from pointed array and writes sequentially on the address
				starting from the given address. It stops writting as soon as it encounters '#' character.
---------------------------------------------------------------------------------------------------*/
//void write_eeprom_multiple_bytes(unsigned char *ee_bytes, unsigned int ee_address)
void write_eeprom_multiple_bytes(unsigned char *ee_bytes, unsigned int ee_address,unsigned char ee_length)
{
    unsigned char no_of_bytes=0;
	IdleI2C2();		//Wait to complete
	StartI2C2();	//Send the Start Bit

    while(I2C2CONbits.SEN);  //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag
 
    MasterWriteI2C2(0xA0);       //Write Slave address and set master for transmission
    while(I2C2STATbits.TBF);     //Wait till address is transmitted
    while(!IFS3bits.MI2C2IF);    //Wait for ninth clock cycle
    MI2C2_Clear_Intr_Status_Bit; //Clear interrupt flag
    while(I2C2STATbits.TRSTAT);
    
    write_ext_eeprom_byte((unsigned char)((ee_address & 0xFF00)>>8));
    write_ext_eeprom_byte((unsigned char)(ee_address & 0x00FF));
//    while(*ee_bytes != '#')
//    {
//    	write_ext_eeprom_byte(*ee_bytes++);
//    }
    
    while(no_of_bytes<ee_length)
   {
        no_of_bytes++;
        write_ext_eeprom_byte(*ee_bytes++);
   }
    
    IdleI2C2();              //wait for the I2C to be Idle
    StopI2C2();              //Terminate communication protocol with stop signal
    while(I2C2CONbits.PEN);  //Wait till stop sequence is completed
	timer2_ms_delay(5);	
}

/*---------------------------------------------------------------------------------------------------
Function Name:	void erase_ext_eeprom(void)
Arguments:		None
Return Val:		None
Description:	This function erases the whole EEPROM accept last byte. Considering size of EEPROM is 64KBytes(24C512)
---------------------------------------------------------------------------------------------------*/
void erase_ext_eeprom(void)
{
	unsigned int loop_cnt = 0, erase_add = 0;
	for(loop_cnt=0; loop_cnt<513; loop_cnt++)
	{
		IdleI2C2();		//Wait to complete
		StartI2C2();	//Send the Start Bit
	
	    while(I2C2CONbits.SEN);  //Wait till Start sequence is completed
	    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag
	 
	    MasterWriteI2C2(0xA0);       //Write Slave address and set master for transmission
	    while(I2C2STATbits.TBF);     //Wait till address is transmitted
	    while(!IFS3bits.MI2C2IF);    //Wait for ninth clock cycle
	    MI2C2_Clear_Intr_Status_Bit; //Clear interrupt flag
	    while(I2C2STATbits.TRSTAT);
	    
	    // Sending Starting Address to be written
	    write_ext_eeprom_byte((unsigned char)((erase_add & 0xFF00)>>8));
	    write_ext_eeprom_byte((unsigned char)(erase_add & 0x00FF));
	    unsigned int ee_cnt = 0;
	    for(ee_cnt = 0; ee_cnt<128; ee_cnt++)
	    {
	    	write_ext_eeprom_byte(0x00);
	    }
	    
	    IdleI2C2();              //wait for the I2C to be Idle
	    StopI2C2();              //Terminate communication protocol with stop signal
	    while(I2C2CONbits.PEN);  //Wait till stop sequence is completed
		timer2_ms_delay(5);	
		erase_add += 128;
	}
}

/*---------------------------------------------------------------------------------------------------
Function Name:	void read_eeprom_parameter(unsigned char *ee_bytes, unsigned int ee_address)
Arguments:		*ee_bytes = pointer to an array from which data is to be written
				ee_address = address of the eeprom register from where the data write should start
Return Val:		None
Description:	This function takes bytes from pointed array and writes sequentially on the address
				starting from the given address. It stops writting as soon as it encounters '#' character.
---------------------------------------------------------------------------------------------------*/
void read_eeprom_parameter(unsigned char parameter_name)  
{
    unsigned char temp_ee_bytes[25], tempcntr;  //temp_ee_bytes 20 to 25  //v3.2.0.A
    unsigned int eeprom_address=0; //v3.1.2.D
    unsigned int offline_ftoi_temp=0;	//v2.2.9
    unsigned int coin_ftoi_temp=0;	//v2.2.9 //1.0.2
	
    for(tempcntr = 0; tempcntr<sizeof(temp_ee_bytes); tempcntr++)
	    temp_ee_bytes[tempcntr] = 0;

    i2c_bus_recovery();		//i2c for eeprom

switch(parameter_name)
{
    case SER_SIGNATURE_BYTE:		                                        //0		
	eeprom_address = EE_SIGNATURE_BYTE;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 8);	//Reading data from EEPROM
	signature_byte[0] = temp_ee_bytes[0];								//Updating parameter
	signature_byte[1] = temp_ee_bytes[1];
	signature_byte[2] = temp_ee_bytes[2];
	signature_byte[3] = temp_ee_bytes[3];
	signature_byte[4] = temp_ee_bytes[4];
	signature_byte[5] = temp_ee_bytes[5];
	signature_byte[6] = temp_ee_bytes[6];
	signature_byte[7] = temp_ee_bytes[7];
    break;

    case SER_TDS_CALIB:	//Calibrate TDS-1	                                //1		
	eeprom_address = EE_TDS_CALIB;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	tds_calib_fact = ((temp_ee_bytes[0]<<8));// & 0xFF00);					//Updating parameter
	tds_calib_fact |= (temp_ee_bytes[1]);// & 0x00FF);						//Updating parameter
    break;
       
    case SER_OUT_FLOW_CALIB:	                                                //2	
	eeprom_address = EE_OUT_FLOW_CALIB;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	out_flow_calibration = ((temp_ee_bytes[0]<<8));// & 0xFF00);
	out_flow_calibration |= (temp_ee_bytes[1]);// & 0x00FF);
    break;

    case SER_IN_FLOW_CALIB:	                                         	//3	
	eeprom_address = EE_IN_FLOW_CALIB;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	//in_flow_calibration = ((temp_ee_bytes[0]));	// <<8) & 0xFF00);
	in_flow_calibration = ((temp_ee_bytes[0]<<8));	// <<8) & 0xFF00);
	in_flow_calibration |= (temp_ee_bytes[1]);		// & 0x00FF);
    break;

    case SER_SERVER_IP:	//Change destination IP                                 //4	
	eeprom_address = EE_SERVER_IP;
	for(tempcntr=0; tempcntr<sizeof(server_ip); tempcntr++)
		server_ip[tempcntr] = 0;

	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(server_ip));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr<sizeof(server_ip); tempcntr++)
		server_ip[tempcntr] = temp_ee_bytes[tempcntr];				//Updating parameter
    break;

    case SER_SERVER_PORT:	//server_port[]//Change destination Port        //5	
	eeprom_address = EE_SERVER_PORT;
	for(tempcntr=0; tempcntr<sizeof(server_port); tempcntr++)
		server_port[tempcntr] = 0;

	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(server_port));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr<sizeof(server_port); tempcntr++)
		server_port[tempcntr] = temp_ee_bytes[tempcntr];				//Updating parameter
    break;

    case SER_SERVER_APN:	//Change APN Name                               //6	  
	eeprom_address = EE_SERVER_APN;
	for(tempcntr=0; tempcntr<sizeof(apn); tempcntr++)
		apn[tempcntr] = 0;

	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address,sizeof(apn));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr<sizeof(apn); tempcntr++)
		apn[tempcntr] = temp_ee_bytes[tempcntr];				//Updating parameter
    break;

    case SER_TDS_MAX:	//Max TDS Set Point                                     //7	
	eeprom_address = EE_TDS_MAX;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	max_tds = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	max_tds |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_TDS_MIN:	//Min TDS Set Point					//8	   
	eeprom_address = EE_TDS_MIN;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	min_tds = ((temp_ee_bytes[0]<<8));// &  0xFF00);				//Updating parameter
	min_tds |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_USER_TIMEOUT:							//9
	eeprom_address = EE_USER_TIMEOUT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	user_timeout = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	user_timeout |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_DISP_TIMEOUT:	                                        	//10	  
	eeprom_address = EE_DISP_TIMEOUT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	dispense_timeout = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	dispense_timeout |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    //case SER_LLS_DETECT_STATE:                                        	//11
    case SER_SLEEP_MODE_FLAG:
//	eeprom_address = EE_SLEEP_MODE_FLAG;
//	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
//	peripheral_flags.sleep_mode_flag = (temp_ee_bytes[0]);				//Updating parameter
    break;

    case SER_DEVICE_EN_DIS:	//Enable/Disable Device                         //12			
	eeprom_address = EE_DEVICE_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	peripheral_flags.dev_en_dis = (temp_ee_bytes[0]);				//Updating parameter
    break;

    case SER_TDS_EN_DIS:	//Enable/Disable TDS Sensor                     //13
	eeprom_address = EE_TDS_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	peripheral_flags.tds_en_dis = (temp_ee_bytes[0]);// & 0x01);				//Updating parameter
    break;

    case SER_LLS_HLS_EN_DIS:	//Enable/Disable LLS-                           //14	
	eeprom_address = EE_LLS_HLS_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	peripheral_flags.lls_hls_en_dis = (temp_ee_bytes[0]);// & 0x01);				//Updating parameter
    break;

    case SER_MASS_DISP_EN_DIS:	//Enable/Disable Mass Dispense-15	
    break;

    case SER_SCHOOL_DISPENSE_AMT:						//16
	
	eeprom_address = EE_SCHOOL_DISPENSE_AMT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	school_disp_amount = ((temp_ee_bytes[0] & 0x00FF) << 8);
	school_disp_amount |= (temp_ee_bytes[1] & 0x00FF);
	sch_disp_fraction = ((float)school_disp_amount)/1000.0;	
    break;

    case SER_ER_DISPLAY_TIMER:							//17
	eeprom_address = EE_ER_DISPLAY_TIMER;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	error_display_time = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	error_display_time |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_DISPENSE_LT_LC:							//18
	    /*//DISPENSE_LEAST_COUNT		
	    //if(!fresh_eeprom)
		    eeprom_address = EE_DISPENSE_LT_LC;
	    //else
	    //	eeprom_address = DFLT_DISPENSE_LT_LC;
	    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 4);	//Reading data from EEPROM
	    //tempread = (temp_ee_bytes[0]<<24);
	    //tempread |= (temp_ee_bytes[1]<<16);
	    //tempread |= (temp_ee_bytes[2]<<8);
	    //tempread |= temp_ee_bytes[3];
	    templong = (temp_ee_bytes[0]<<24);
	    templong |= (temp_ee_bytes[1]<<16);
	    templong |= (temp_ee_bytes[2]<<8);
	    templong |= temp_ee_bytes[3];
	    DISPENSE_LEAST_COUNT = ((float)(templong));
	    //DISPENSE_LEAST_COUNT = (temp_ee_bytes[0]<<8);				//Updating parameter
	    //DISPENSE_LEAST_COUNT |= temp_ee_bytes[1];				//Updating parameter*/
    break;

    case SER_NEW_PASSWORD_SET:	//Change SMS Password                           //19
	eeprom_address = EE_NEW_PASSWORD_SET;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(current_password));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr <sizeof(current_password); tempcntr++)
		current_password[tempcntr] = temp_ee_bytes[tempcntr];				//Updating parameter
    break;

    case SER_LOGOUT_IDLE_TIMEOUT:						//20		
	eeprom_address = EE_LOGOUT_IDLE_TIMEOUT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	logout_idle_time = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	logout_idle_time |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_LT_EMPTY_TANK:							//21
	eeprom_address = EE_LT_EMPTY_TANK;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	lit_empty_tank = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	lit_empty_tank |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_WATER_EXPIRED:	//Water Expired                                 //22	
	eeprom_address = EE_WATER_EXPIRED;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	error_flags.water_expired = (temp_ee_bytes[0]);					//Updating parameter
    break;

    case SER_FACT_PASSWORD_RESET:	//Factory Password Reset                //23
	eeprom_address = EE_FACT_PASSWORD_RESET;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(factory_password));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr<sizeof(factory_password); tempcntr++)
		factory_password[tempcntr] = temp_ee_bytes[tempcntr];				//Updating parameter
    break;

    case SER_RESET_OOO:	//Reset Out of Order					//24
    break;

    case SER_REMOTE_DUMP:	//Remote Dump Water				//25
    break;

    case SER_RQ_SYSTEM_SETTINGS:	//Request System Settings		//26
    break;

    case SER_IDLE_MSG_FREQ:	//Idle Message Frequency			//27	
	eeprom_address = EE_IDLE_MSG_FREQ;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	idle_msg_freq = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	idle_msg_freq |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_RECONNECTION:	//force fully Request Reconnection		//28
	    break;

    case SER_RESET_ALL_OOO:	//force fully clear all error			//29				
	    break;

    case SER_DUMP_TIMEOUT:							//30
	    //dump_timeout 		
	    eeprom_address = EE_DUMP_TIMEOUT;
	    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	    dump_timeout = ((temp_ee_bytes[0]<<8));// & 0xFF00);				//Updating parameter
	    dump_timeout |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
	    break;

    case SER_REFILL_TIMEOUT:							//31	
	eeprom_address = EE_REFILL_TIMEOUT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	refill_timeout = ((temp_ee_bytes[0] << 8));// & 0xFF00);				//Updating parameter
	refill_timeout |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;

    case SER_CLEANING_TIMEOUT:							//32	
	eeprom_address = EE_CLEANING_TIMEOUT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	cleaning_timeout = ((temp_ee_bytes[0] << 8));// & 0xFF00);				//Updating parameter
	cleaning_timeout |= (temp_ee_bytes[1]);// & 0x00FF);				//Updating parameter
    break;
    
    case SER_CLEAN_TANK:							//35				
	    break;

    case SER_MODEM_COMMAND:							//36
	    break;

    case SER_DEBUG_MODE:							//39	
	eeprom_address = EE_DEBUG_MODE;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	if(temp_ee_bytes[0])
		mode_flags.op_mode = DEBUG_MODE;// & 0x00FF);
	else{}
    break;
    
   case SER_CHANGE_DEV_ID:	                                 		//40
	eeprom_address = EE_CHANGE_DEVICE_ID;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, (sizeof(MACHINE_ID)));
	for(tempcntr = 0; tempcntr <(sizeof(MACHINE_ID)); tempcntr++)
		MACHINE_ID[tempcntr] = temp_ee_bytes[tempcntr];
    break;
    
    case SER_LEAK_TIMER:							//41
	eeprom_address = EE_LEAK_DETECT_TIMER;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	leak_timeout = ((temp_ee_bytes[0]<<8));// & 0xFF00);						//Updating parameter
	leak_timeout |= (temp_ee_bytes[1]);// & 0x00FF);							//Updating parameter
    break;

    case SER_BUZZER_CTRL:			                                //42
	eeprom_address = EE_BUZZER_CONTROL;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	peripheral_flags.pag_buzzer_control = (temp_ee_bytes[0]);// & 0x01);
    break;

    case SER_DIAGNOSTIC_TEST:			                                //46
	eeprom_address = EE_RUN_DIAG_TEST;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	mode_flags.run_diagnostic = (temp_ee_bytes[0]);// & 0x01);
	if(mode_flags.run_diagnostic)			//addded by Manish
		mode_flags.op_mode = DIAGNOSTIC_MODE;
	else{}
    break;
    
    case SER_SMS_PERMIT:			                                //49
	eeprom_address = EE_SMS_PERMISSION;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	gsm_flags.sms_permit = (temp_ee_bytes[0]);// & 0x01);
    break;

    case SER_SCHOOL_MODE:			                                //52
	eeprom_address = EE_SCHOOL_MODE;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM	
	if(temp_ee_bytes[0])
	{
		mode_flags.op_mode = SCHOOL_MODE;
		school_mode_en = 1;
	}	
	else
	{
		school_mode_en = 0;
	}
     break;

    case SER_MASTER_CONT_NO:		                                 	//56
	eeprom_address = EE_MASTER_CONT_NO;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(master_contact));	//Reading data from EEPROM
	//strcpy(master_contact, temp_ee_bytes);
	for(tempcntr = 0; tempcntr<sizeof(master_contact); tempcntr++)
	{
		master_contact[tempcntr] = temp_ee_bytes[tempcntr];
	}
    break;
    
    case SER_DATA_ER_AUTO_CORRECT:						//57
	eeprom_address = EE_DATA_ER_AUTO_CORRECT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	error_flags.data_error_auto_correct = (temp_ee_bytes[0]);	// & 0x01);
    break;

    case SER_HYST_COUNT:							//58
	
    break;

    case SER_TEMPERATURE_CALIB:                                                 //63
	eeprom_address = EE_TEMPERATURE_CALIB;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	temp_calib_fact = ((temp_ee_bytes[0]<<8));// & 0xFF00);					//Updating parameter
	temp_calib_fact |= (temp_ee_bytes[1]);// & 0x00FF);						//Updating parameter
    break;

    case SER_WATER_TEMPERATURE_LEVEL:			//-64				//v2.3.6
        eeprom_address = EE_WATER_TEMPERATURE_LEVEL;				//v2.3.6
        read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
        water_temperature_level = (temp_ee_bytes[0]);// & 0x00FF);
    break;

    case SER_TEMPERATURE_EN_DIS:                                                //65
	eeprom_address = EE_TEMPERATURE_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	peripheral_flags.temp_en_dis = (temp_ee_bytes[0]);	// & 0x01);
    break;

    case SER_OUT_FLOW_FREQ_LIMIT:                                               //66
	eeprom_address = EE_OUT_FLOW_FREQ_LIMIT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	output_flow_freq_limit = ((temp_ee_bytes[0]<<8));// & 0xFF00);					//Updating parameter
	output_flow_freq_limit |= (temp_ee_bytes[1]);// & 0x00FF);						//Updating parameter
    break;

    case SER_IN_FLOW_FREQ_LIMIT:                                                //67
	eeprom_address = EE_IN_FLOW_FREQ_LIMIT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	input_flow_freq_limit = ((temp_ee_bytes[0]<<8));// & 0xFF00);					//Updating parameter
	input_flow_freq_limit |= (temp_ee_bytes[1]);// & 0x00FF);						//Updating parameter
    break; 

    case SER_AUTO_REFILL_FLAG:                                                  //69
	eeprom_address = EE_AUTO_REFILL_FLAG;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	refill_flags.auto_refill_flag = (temp_ee_bytes[0]);	// & 0x01);
    break;

    case SER_FRANCHISEE_RFID:               //v2.2.5                            //70
	eeprom_address = EE_FRANCHISEE_RFID ;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(franchisee_rfid_tag));
	for(tempcntr = 0; tempcntr<sizeof(franchisee_rfid_tag); tempcntr++)
	{
	    franchisee_rfid_tag[tempcntr] = temp_ee_bytes[tempcntr];
	}	
    break;

    case SER_CORPORATE_MODE:			//v2.2.5                        //71
	eeprom_address = EE_CORPORATE_MODE;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	if(temp_ee_bytes[0])
	{
	    mode_flags.op_mode = CORPORATE_MODE;
	    corporate_mode_en = 1;
	}	
	else{
	     corporate_mode_en = 0;
	}
    break;
    
    case SER_CORPORATE_MODE_ACC_ID:			//v2.2.5                //72

	eeprom_address = EE_CORPORATE_MODE_ACC_ID;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(corp_mode_acc_id));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr<sizeof(corp_mode_acc_id); tempcntr++)
	{
	    corp_mode_acc_id[tempcntr] = temp_ee_bytes[tempcntr];
	}
    break;

    case SER_CHILLER_HEATER_EN_DIS:                 //v2.2.6	//v2.3.6  //73
        eeprom_address = EE_CHILLER_HEATER_EN_DIS;	//v2.3.6
        read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
        peripheral_flags.chiller_heater_en_dis = (temp_ee_bytes[0]);	// & 0x01);			//v2.3.6
    break;
    
    case SER_OFFLINE_WATER_TARIFF:		//v2.2.9                        //87			
	eeprom_address = EE_OFFLINE_WATER_TARIFF;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	offline_ftoi_temp = (((temp_ee_bytes[0]) & 0x00FF) << 8);
	offline_ftoi_temp |= ((temp_ee_bytes[1]) & 0x00FF);
	offline_water_tariff = (float)offline_ftoi_temp;
    break;
    
    case SER_MENU_PASSWORD_SET:		                                        //88
         for(tempcntr=0; tempcntr<sizeof(menu_franchisee_password); tempcntr++)
		menu_franchisee_password[tempcntr] = 0;
	eeprom_address = EE_MENU_PASSWORD_SET;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(menu_franchisee_password));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr <sizeof(menu_franchisee_password); tempcntr++)
	    menu_franchisee_password[tempcntr] = temp_ee_bytes[tempcntr];			
    break;
    
    case SER_TDS_VERSION:                 //v2.2.9                              //91
	eeprom_address = EE_TDS_VERSION;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	tds_header_version = (temp_ee_bytes[0]);	// & 0x01);
    break;

    case SER_TEMP_COMPEN_EN_DIS:                 //v2.2.6                       //92
	eeprom_address = EE_TEMP_COMPEN_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	peripheral_flags.temp_compen_en_dis = (temp_ee_bytes[0]);	// & 0x01);
    break;
	    
    case SER_AUTO_SYNC_EN_DIS:	//Enable/Disable server_sync_flag               //93
	eeprom_address = EE_AUTO_SYNC_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	system_flags.auto_sync_flag = (temp_ee_bytes[0]);// & 0x01);				//Updating parameter
    break;	
    
     //  -----------------   v3.1.2.J   ----read
     case SER_MODEM_CHECK_ROUTINE:	                                //96
	eeprom_address = EE_MODEM_CHECK_ROUTINE;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	modem_status_check_hour = ((temp_ee_bytes[0]<<8));// & 0xFF00);					//Updating parameter
	modem_status_check_hour |= (temp_ee_bytes[1]);// & 0x00FF);						//Updating parameter
    break;
    //  -----------------   v3.1.2.J   ----read
    
    case SER_OP_MODE:								//100
	eeprom_address = EE_OP_MODE;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	//mode_flags.op_mode = (temp_ee_bytes[0] && 0x07);
	mode_flags.op_mode = temp_ee_bytes[0];
    break;
	    
   case SER_ERROR_STACK:		                                	//101
	eeprom_address = EE_ERROR_STATE;
	//---read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, (sizeof(error_stack)));	//Reading data from EEPROM
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, TOTAL_ERRORS);	//Reading data from EEPROM
	//for(tempcntr = 0; tempcntr <= (sizeof(error_stack)); tempcntr++)
	for(tempcntr = 0; tempcntr < TOTAL_ERRORS; tempcntr++)
	{
		error_stack[tempcntr] = temp_ee_bytes[tempcntr];
	}
    break;
    
    case SER_SCHOOL_MODE_ACC_ID:			                        //102 
	eeprom_address = EE_SCHOOL_MODE_ACC_ID;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(sch_mode_acc_id));	//Reading data from EEPROM
	for(tempcntr = 0; tempcntr<sizeof(sch_mode_acc_id); tempcntr++)
	{
		sch_mode_acc_id[tempcntr] = temp_ee_bytes[tempcntr];
	}
    break;
	    
    case SER_TANK_LOW_LEVEL_STATUS:                                             //108
	eeprom_address = EE_TANK_LOW_LEVEL_STATUS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	tank_low_level_litre = ((temp_ee_bytes[0]<<8));// & 0xFF00);					//Updating parameter
	tank_low_level_litre |= (temp_ee_bytes[1]);// & 0x00FF);						//Updating parameter
    break;
 
    case SER_CARD_INIT_BAL:			                                //112
	eeprom_address = EE_CARD_INIT_BAL;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	card_init_balance  = ((temp_ee_bytes[0]<<8));// & 0xFF00);					//Updating parameter
	card_init_balance |= (temp_ee_bytes[1]);// & 0x00FF);						//Updating parameter
    break;
  
    case SER_PREV_DATE:			                                        //113
	eeprom_address = EE_PREV_DATE;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	//mode_flags.op_mode = (temp_ee_bytes[0] && 0x07);
	P_date = temp_ee_bytes[0];
    break;

    case SER_PREV_MONTH:			                                //114
	eeprom_address = EE_PREV_MONTH;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	//mode_flags.op_mode = (temp_ee_bytes[0] && 0x07);
	P_month = temp_ee_bytes[0];
    break;

    case SER_PREV_YEAR:			                                        //115
	eeprom_address = EE_PREV_YEAR;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	//mode_flags.op_mode = (temp_ee_bytes[0] && 0x07);
	P_yr = temp_ee_bytes[0];
    break;

    case SER_ULTRA_SUPER_ADMIN_MODE:			                        //116
	    eeprom_address = EE_ULTRA_SUPER_ADMIN_MODE;
	    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	    system_flags.ultra_super_admin_state = temp_ee_bytes[0];			
    break;

 
  	
//  ***********************************************************************************************/	//v2.2.9
    case SER_COIN_EN_DIS:	//v2.2.9                                        //74		
	eeprom_address = EE_COIN_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	peripheral_flags.coin_en_dis = (temp_ee_bytes[0]);				// & 0x01);
    break;

    case SER_COIN_CHN_EN_DIS:	//v2.2.9                                        //75
	eeprom_address = EE_COIN_CHN_EN_DIS;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
	channel_cmd = (temp_ee_bytes[0]);									// & 0x01);
    break;

    case SER_TOTAL_COIN_AMOUNT:	//v2.2.9                                        //76				
	eeprom_address = EE_TOTAL_COIN_AMOUNT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	total_coin_amount = (((temp_ee_bytes[0]) & 0x00FF) << 8);
	total_coin_amount |= ((temp_ee_bytes[1]) & 0x00FF);
    break;

    case SER_TOTAL_COIN_COUNT:	//v2.2.9                                        //77				
	eeprom_address = EE_TOTAL_COIN_COUNT;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	total_coin_count = (((temp_ee_bytes[0]) & 0x00FF) << 8);
	total_coin_count |= ((temp_ee_bytes[1]) & 0x00FF);
    break;

    case SER_COIN_WATER_TARIFF:	//v2.2.9                                        //78				
	eeprom_address = EE_COIN_WATER_TARIFF;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	coin_ftoi_temp = (((temp_ee_bytes[0]) & 0x00FF) << 8);
	coin_ftoi_temp |= ((temp_ee_bytes[1]) & 0x00FF);
	coin_water_tariff = (float)coin_ftoi_temp;
    break;

    case SER_COIN_COLLECT_CAPACITY:	//v2.2.9                                //79				
	eeprom_address = EE_COIN_COLLECT_CAPACITY;
	read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 2);	//Reading data from EEPROM
	coin_collector_capacity = (((temp_ee_bytes[0]) & 0x00FF) << 8);
	coin_collector_capacity |= ((temp_ee_bytes[1]) & 0x00FF);
    break;

    case SER_COIN_ID:	//v2.2.9                                                //81
			    
        for(tempcntr = 0; tempcntr<sizeof(temp_coin_id); tempcntr++)
        {
            temp_coin_id[tempcntr] = 0;
        }
            switch(eeprom_channel_read)
            {
                case 1:
                    eeprom_address = EE_COIN_ID_CHN1;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_1));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_1); tempcntr++)
                    {
                        coin_id.channel_1[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }
                break;
                case 2:
                    eeprom_address = EE_COIN_ID_CHN2;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_2));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_2); tempcntr++)
                    {
                        coin_id.channel_2[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }						
                break;
                case 3:
                    eeprom_address = EE_COIN_ID_CHN3;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_3));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_3); tempcntr++)
                    {
                        coin_id.channel_3[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }
                break;
                case 4:
                    eeprom_address = EE_COIN_ID_CHN4;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_4));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_4); tempcntr++)
                    {
                        coin_id.channel_4[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }						
                break;
                case 5:
                    eeprom_address = EE_COIN_ID_CHN5;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_5));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_5); tempcntr++)
                    {
                        coin_id.channel_5[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }
                break;
                    case 6:
                    eeprom_address = EE_COIN_ID_CHN6;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_6));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_6); tempcntr++)
                    {
                        coin_id.channel_6[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }						
                break;
                case 7:
                    eeprom_address = EE_COIN_ID_CHN7;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_7));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_7); tempcntr++)
                    {
                        coin_id.channel_7[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }
                break;
                case 8:
                    eeprom_address = EE_COIN_ID_CHN8;
                    read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, sizeof(coin_id.channel_8));
                    for(tempcntr = 0; tempcntr<sizeof(coin_id.channel_8); tempcntr++)
                    {
                        coin_id.channel_8[tempcntr] = temp_ee_bytes[tempcntr];
                        temp_coin_id[tempcntr] = temp_ee_bytes[tempcntr];
                    }
                break;
                
                default:
                break;
            //	      strcpy(temp_coin_id,temp_ee_bytes);

            }
    break;

    case SER_COIN_COLLECTOR_ERROR:						//109
            eeprom_address = EE_COIN_COLLECTOR_ERROR;
            read_eeprom_multiple_bytes(temp_ee_bytes, eeprom_address, 1);	//Reading data from EEPROM
            coin_flags.coin_collector_error = temp_ee_bytes[0];
    break;	 	 
					
    default:	
	    break;
} //end of switch	
} //end of function

/*---------------------------------------------------------------------------------------------------
Function Name:	void read_eeprom_multiple_bytes(unsigned char *ee_bytes, unsigned int ee_read_address, unsigned char no_of_bytes)
Arguments:		*ee_bytes = pointer to an array in which data is to be written
				ee_read_address = address of the eeprom register from where the data read should start
				no_of_bytes = no of bytes to be read from eeprom
Return Val:		None
Description:	This function takes bytes from pointed array and writes sequentially on the address
				starting from the given address. It stops writting as soon as it encounters '#' character.
---------------------------------------------------------------------------------------------------*/
void read_eeprom_multiple_bytes(unsigned char *ee_bytes, unsigned int ee_read_address, unsigned char no_of_bytes)
{
	//unsigned int temp=1;

	IdleI2C2();
	StartI2C2();
	while(I2C2CONbits.SEN );  //Wait till Start sequence is completed
 	MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

	MasterWriteI2C2(0xA0);       //Write Slave address and set master for transmission
	while(I2C2STATbits.TBF);     //Wait till address is transmitted
	while(!IFS3bits.MI2C2IF);    //Wait for ninth clock cycle
	
	MI2C2_Clear_Intr_Status_Bit; //Clear interrupt flag
	while(I2C2STATbits.TRSTAT);
	 
	MasterWriteI2C2((ee_read_address>>8)&0x00FF);       //Write Slave address and set master for transmission
	while(I2C2STATbits.TBF);     //Wait till address is transmitted
	while(!IFS3bits.MI2C2IF);    //Wait for ninth clock cycle
		
	MI2C2_Clear_Intr_Status_Bit; //Clear interrupt flag
	while(I2C2STATbits.TRSTAT);
	 
	MasterWriteI2C2(ee_read_address&0x00FF);       //Write Slave address and set master for transmission
	while(I2C2STATbits.TBF);     //Wait till address is transmitted
	while(!IFS3bits.MI2C2IF);    //Wait for ninth clock cycle

	MI2C2_Clear_Intr_Status_Bit; //Clear interrupt flag
	while(I2C2STATbits.TRSTAT);
	 
	IdleI2C2();              //wait for the I2C to be Idle
	
	StartI2C2();              //Restart signal
	while(I2C2CONbits.SEN );   //Wait till Restart sequence is completed
	//for(i=0;i<1000;i++);
	//*************** Master sends Slave Address with read signal *****************************************
    Nop();
  	MI2C2_Clear_Intr_Status_Bit;
	MasterWriteI2C2(0xA1);        //Write Slave address and set master for reception
	while(!IFS3bits.MI2C2IF);     //Wait for ninth clock cycle
	while(I2C1STATbits.TRSTAT);  //check for ACK from slave
	MI2C2_Clear_Intr_Status_Bit;
	//************** Master Recieves from slave ********************************************************
	MastergetsI2C2(no_of_bytes,ee_bytes,1000); //Master recieves from Slave upto 10 bytes


	//************** Stop condition *******************************************************************
	IdleI2C2();              //wait for the I2C to be Idle
	StopI2C2();              //Terminate communication protocol with stop signal
	while(I2C2CONbits.PEN);  //Wait till stop sequence is completed
}	
/*-------------------------------------------------------------------------------
I2C  Bus recovery
• Typical case is when masters  fails when  doing a read operationin a slave
• SDA  line is then non usable anymore because of the “Slave-Transmitter” mode.
• Methods to recover  the SDA line are :
– Reset the  slave device (assuming the device has a Reset  pin)
– Use a bus recovery sequence to leave  the “Slave-Transmitter” mode
• Bus recovery sequence is done as following:
1 - Send 9 clock pulses on SCL line
2 - Ask the master to keep SDA High until the “Slave-Transmitter” releases
	the SDA line to perform the ACK operation
3 - Keeping SDA High during the ACK means that the “Master-Receiver”
	does not acknowledge the previous byte receive
4 - The “Slave-Transmitter” then goes in an idle state
5 - The master then sends a STOP command initializing completely the bus
-------------------------------------------------------------------------------*/
void i2c_bus_recovery(void)
{
	mPORTAOutputConfig(0x000C); // Make pins RA2 and RA3 as Outputs
	mPORTASetBits(0x0008); // SDA High for 9 Clock cycles
	unsigned char tempcntr;
	for(tempcntr = 0; tempcntr < 9; tempcntr++)
	{
		mPORTASetBits(0x0004); // SCL High
		asm( "REPEAT #40\n\t"
		"NOP"); // delay 5 microsecond (us)
		mPORTAClearBits (0x0004); // SCL Low
		asm( "REPEAT #40\n\t"
		"NOP"); // delay 5 microsecond (us)
	}
	mPORTAClearBits(0x0004); // SCL Low
	mPORTAClearBits(0x0008); // SDA High for 9 Clock cycles
	asm( "REPEAT #40\n\t"
	"NOP"); // delay 5 microsecond (us)
	//----------- Generating Stop Condition
	mPORTASetBits(0x0004); // SCL High
	asm( "REPEAT #40\n\t"
	"NOP"); // delay 5 microsecond (us)
	mPORTASetBits(0x0008); // SDA High
	//---timer2_ms_delay(100);
}
