/*
 * @Author:
 *  #Jiabin Hsu  | zsiothsu(at)zhishan-iot.tk
 *  #Weilun Fong | wlf(at)zhishan-iot.tk
 * @E-mail:mcu(at)zhishan-iot.tk
 * @File-description:operations for timer resource
 * @Required-compiler:SDCC
 * @Support-mcu:STC micro STC90-RC/RD+ series
 * @Version:V0
 */

#include "tim.h"

#ifdef ___COMPILE_TIM___

/*
 * @Prototype:unsigned int TIM_calcValue(unsigned int time,TIM_mode m)
 * @Parameter:
 *  (1)time:initial value of time (time/us)
 *  (2)m:work mode(refer to header file)
 * @Ret-val:initial value of timer counter register(if return 0x0000,it means that the time has over the limit)
 * @Note:calculate timer counter register value
 */
unsigned int TIM_calculateValue(unsigned int time,TIM_mode m)
{
    /* MachineCycle:12/_FRE_OSC_ */
    unsigned int maxTick = 0x0000;
    
    switch(m)
    {
        case TIM_mode_0:maxTick = 0x1FFF;break;
        case TIM_mode_1:maxTick = 0xFFFF;break;
        case TIM_mode_2:maxTick = 0x00FF;break;
        case TIM_mode_3:maxTick = 0x00FF;break;
        default:break;
    }
    if((time*12)/(_FRE_OSC_/1000000) >= maxTick )
    {
        return 0;
    }
    else
    {
        return (maxTick+1-((time*12)/(_FRE_OSC_/1000000)));
    }
}

/*
 * @Prototype:void TIM_cmd(PERIPH_TIM tim,Action a)
 * @Parameter:
 *  (1)tim:target timer module
 *  (2)a:expected action
 * @Ret-val:
 * @Note:launch or stop timer
 */
void TIM_cmd(PERIPH_TIM tim,Action a)
{
    switch(tim)
    {
        case PERIPH_TIM_0:TR0 = a;break;
        case PERIPH_TIM_1:TR1 = a;break;
        default:break;
    }
}

/*
 * @Prototype:void TIM_config(PERIPH_TIM tim,TIM_configTypeDef *tc)
 * @Parameter:
 *  (1)tim:target timer module
 *  (2)tc:custom configure reference info.,look up details in header file
 * @Ret-val:
 * @Note:configure target timer
 */
void TIM_config(PERIPH_TIM tim,TIM_configTypeDef *tc)
{
    TIM_setFunction(tim,tc->function);
    TIM_setMode(tim,tc->mode);
    TIM_setValue(tim,tc->value);
    TIM_INT_cmd(tim,tc->interruptState);
    TIM_INT_setPriority(tim,tc->interruptPriority);
}

/*
 * @Prototype:unsigned int TIM_getValue(PERIPH_TIM tim)
 * @Parameter:(1)tim:target timer module
 * @Ret-val:value(if return 0,means user call this function with error ways)
 * @Note:get timer's value
 */
unsigned int TIM_getValue(PERIPH_TIM tim)
{
    switch(tim)
    {
        case PERIPH_TIM_0:return ((TH0 << 0x08) | TL0);
        case PERIPH_TIM_1:return ((TH1 << 0x08) | TL1);
        default:return 0;
    }
}

/*
 * @Prototype:bool TIM_isOverflow(PERIPH_TIM tim)
 * @Parameter:(1)tim:target timer module
 * @Ret-val:(1)false:not overflow;(2)true:overflow;
 * @Note:check value register timer if is overflow
 */
bool TIM_isOverflow(PERIPH_TIM tim)
{
    switch(tim)
    {
        case PERIPH_TIM_0:return (bool)TF0;
        case PERIPH_TIM_1:return (bool)TF1;
        default:return false;
    }
}

/*
 * @Prototype:void TIM_setFunction(PERIPH_TIM tim,TIM_function f)
 * @Parameter:
 * (1)tim:target timer module
 * (2)f:expected function
 * @Ret-val:
 * @Note:configure the module as counter mode or timer mode
 */
void TIM_setFunction(PERIPH_TIM tim,TIM_function f)
{
    switch(tim)
    {
        case PERIPH_TIM_0:CONFB(TMOD,BIT_NUM_T0_CT,f);break;
        case PERIPH_TIM_1:CONFB(TMOD,BIT_NUM_T1_CT,f);break;
        default:break;
    }
}

/*
 * @Prototype:void TIM_setMode(PERIPH_TIM tim,TIM_mode m)
 * @Parameter:
 * (1)tim:target timer module
 * (2)m:work mode(refer to header file)
 * @Ret-val:
 * @Note:set work mode of timer
 */
void TIM_setMode(PERIPH_TIM tim,TIM_mode m)
{
    switch(tim)
    {
        case PERIPH_TIM_0:TMOD = (TMOD & 0xFC) | m;break;
        case PERIPH_TIM_1:TMOD = (TMOD & 0xCF) | (m << 0x04);break;
        default:break;
    }
}

/*
 * @Prototype:void TIM_setValue(PERIPH_TIM tim,unsigned int val)
 * @Parameter:
 * (1)tim:target timer module
 * (2)val:expected value
 * @Ret-val:
 * @Note:set value in value register of specified timer module
 */
void TIM_setValue(PERIPH_TIM tim,unsigned int val)
{
    switch(tim)
    {
        case PERIPH_TIM_0:
        {
            TH0 = (u8)((val >> 0x8) & 0x00FF);
            TL0 = (u8)(val & 0x00FF);
        } break;
        case PERIPH_TIM_1:
        {
            TH1 = (u8)((val >> 0x8) & 0x00FF);
            TL1 = (u8)(val & 0x00FF);
        } break;
        default:break;
    }
}

/*
 * @Prototype:void TIM_INT_cmd(PERIPH_TIM tim,Action a)
 * @Parameter:
 * (1)tim:target timer module
 * (2)a:expected action,decide disable or enable timer interrupt
 * @Ret-val:
 * @Note:disable or enable timer interrupt
 */
void TIM_INT_cmd(PERIPH_TIM tim,Action a)
{
    switch(tim)
    {
        case PERIPH_TIM_0:ET0 = a;break;
        case PERIPH_TIM_1:ET1 = a;break;
        default:break;
    }
}

/*
 * @Prototype:void TIM_INT_setPriority(PERIPH_TIM tim,INTR_priority p)
 * @Parameter:
 * (1)tim:target timer module
 * (2)p:expected interrupt priority class
 * @Ret-val:
 * @Note:set priority of TIM module
 */
void TIM_INT_setPriority(PERIPH_TIM tim,INTR_priority p)
{
    switch(tim)
    {
        case PERIPH_TIM_0:
        {
            PT0 = TESTB(p,0);
            CONFB(IPH,BIT_NUM_PT0H,TESTB(p,1));
        } break;
        case PERIPH_TIM_1:
        {
            PT1 = TESTB(p,0);
            CONFB(IPH,BIT_NUM_PT1H,TESTB(p,1));
        } break;
        default:break;
    }
}

#endif
