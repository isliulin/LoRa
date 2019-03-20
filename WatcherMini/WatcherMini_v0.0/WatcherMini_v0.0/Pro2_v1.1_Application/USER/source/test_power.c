#include "user_main.h"
#include "user_bq24195.h"
#include "user_sim.h"
#include "user_led.h"
#include "user_adc.h"
#include "user_flash_L072.h"
#include "user_sensor_pro2.h"
#include "user_gps.h"
#include "bootloader_config.h"
#include <string.h>

void UserMain()
{
    int32_t Battery;
    uint16_t currentResult;
    uint8_t r0,r8,charge,*chargeResult,current,succeed,result,aux;

    succeed=1;
    printf("����Դģ��\r\n\r\n");
    HAL_Delay(2000);
    printf("���ϵ�Դ�������͵��,�鿴����Դ���Ƿ�������������\r\n\r\n");
    HAL_Delay(2000);
    
    printf("���ڹرճ�磬�鿴����Դ���Ƿ����ˣ���������\r\n\r\n");
    Bq24195DisableCharge();
    HAL_Delay(3000);
    
    printf("���ڴ򿪳�磬�鿴����Դ���Ƿ������ˣ���������\r\n\r\n");
    Bq24195EnableCharge();
    HAL_Delay(3000);
    
    if(AdcInit()==0)
    {
        printf("оƬ��ʼ����������\r\n");
        HAL_Delay(1000);
        HAL_NVIC_SystemReset();
    }
    else
    {
        AdcGetBattery(&Battery);
//        if(Battery==0)
//        {
//            printf("����������\r\n\r\n");
//            return;
//        }
//        else
//        {
//            printf("��ǰ����:%u\r\n\r\n",Battery);
//            
//        }
    }
    
    SetMiniSysVoltage(SYSTEM_VOLTAGE_3000MV
    +SYSTEM_VOLTAGE_400MV
    +SYSTEM_VOLTAGE_200MV
    +SYSTEM_VOLTAGE_100MV);
    SetWdgTimer(WATCHER_DOG_TIMER_DISABLE);
    SetInputCurrentLimit(INPUT_CURRENT_2000MA);
    SetChargeType(CHARGER_BATTERY);
    
    Bq24195ReadByte(0,&r0);
    
    current = r0 & INPUT_CURRENT_LIMIT;	
    Bq24195ReadByte(8,&r8);
    charge = r8 & CHARGE_STATUE;
    
    AdcGetBattery(&Battery); 
    
    if(r0&ENABLE_HIGH_IMPEDANCE_MODE){
        printf("�Ȳ����أ��ٲ������USB��������������������\r\n");
    }
    chargeResult = (uint8_t*)(
        charge==CHARGE_PRE_CHARGING?"Ԥ���,����OK\r\n":
        charge==CHARGE_FAST_CHARGING?"���ٳ�磬����OK\r\n":	
        charge==CHARGE_DONE_CHARGING?"�����ɣ�����OK\r\n":"û��磬����error\r\n");
    currentResult = 
        current==INPUT_CURRENT_100MA?100:
        current==INPUT_CURRENT_150MA?150:
        current==INPUT_CURRENT_500MA?500:
        current==INPUT_CURRENT_900MA?900:
        current==INPUT_CURRENT_1200MA?1200:
        current==INPUT_CURRENT_1500MA?1500:
        current==INPUT_CURRENT_2000MA?2000:
        current==INPUT_CURRENT_3000MA?3000:0;
    if(current==INPUT_CURRENT_100MA)
    {
        succeed=0;
        printf("������\r\n");
    }
    printf("��緽ʽ:%s������:%dma\r\n��ص���:%d��\r\n"
            ,chargeResult,currentResult,Battery);

    Bq24195ReadByte(FAULT_REGISTER,&result);
    
    if(result!=0xff)
    {
        aux = result&CHARGE_FAULT;
        if(aux==CHARGE_NORMAL){
            printf("���״̬:����OK\r\n");
        }else if(aux==CHARGE_INPUT_FAULT)
        {
            printf("���״̬:���������ѹ����\r\n");
            succeed=0;
        }else{
            printf("���״̬:����error\r\n");
            succeed=0;
        }
        if(result&BATTERY_FAULT){
            printf("��أ�error��ص�ѹ����\r\n");
            succeed=0;
        }else{
            printf("���:����OK\r\n");
        }
    }
    printf("���ģ��:%s\r\n\r\n",succeed==1?"����OK":"����ERROR");

}
