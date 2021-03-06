/* Includes ------------------------------------------------------------------*/
#include "IOLib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*********************************************************************************************

  * @brief  IoPwm_RunAsSpecify
  * @param  ioPwm：ioPwm实例指针
            status：状态
  * @return 
  * @remark -1：到时间了，没有新的时间进入
            0：运行期间
            1：到时瞬间

  ********************************************************************************************/
int IoPwm_RunAsSpecify(struct IOPwmStruct *ioPwm, struct IOPwmStatusStruct *status)
{
     /* 持续时间为0则报-1，一般默认的才会为0 */
    if(status->keepInterval == 0)
    {   return -1;  }
    
     /* 超时则报-1 */
    if((ioPwm->startTime + status->keepInterval) < REALTIME && status->keepInterval != -1)
    {   
        status->keepInterval = 0;               // 持续时间置0
        ioPwm->CallBack_IOOperation(false);     // 发现超时则直接非使能
        return 1;  
    }
  
    /* 计算出占空比 */
    uint16_t toggleInterval = (ioPwm->CallBack_IsIOActive() == false)? (status->totalInterval - status->activeInterval): status->activeInterval;

    
    /* 按照占空比实际处理*/
    if((ioPwm->__runTime + toggleInterval) < SYSTIME)
    {
        ioPwm->__runTime = SYSTIME;     
        ioPwm->CallBack_IOOperation((ioPwm->CallBack_IsIOActive() == true)? false:true);       // 根据IO状态反转切换
    }
    return 0;
}

/*********************************************************************************************

  * @brief  IOPwm_Handle
  * @param   
  * @return 
  * @remark 

  ********************************************************************************************/
void IOPwm_Handle(struct IOPwmStruct *ioPwm)
{
    switch(ioPwm->process.current)
    {
    case Process_Init:  // 初始阶段，执行
        if(ioPwm->CallBack_Init != 0)
        {   ioPwm->CallBack_Init(); }
        PROCESS_CHANGE(ioPwm->process, Process_Idle);
        break;
        
    case Process_Idle:  // 默认状态执行阶段，当默认状态keepInterval不为0，则当超时自动切换到当前状态
        if(IoPwm_RunAsSpecify(ioPwm, &(ioPwm->defaultStatus)) == 1)
        {
            PROCESS_CHANGE(ioPwm->process, Process_Run);
            ioPwm->startTime = REALTIME;
            ioPwm->__runTime = SYSTIME;
        }
        break;
        
    case Process_Run:   // 运行阶段，按照currentStatus执行，并返回非0（）时切换为初始化
        if(IoPwm_RunAsSpecify(ioPwm, &(ioPwm->currentStatus)) != 0)
        {   PROCESS_CHANGE(ioPwm->process, Process_Idle);   }
        break;
    }
}
/*********************************************************************************************

  * @brief  IOPwm_ChangeStatus
  * @param  ioPwm：ioPwm实例指针
            status：状态
  * @return 
  * @remark 

  ********************************************************************************************/
void IOPwm_ChangeStatus(struct IOPwmStruct *ioPwm, struct IOPwmStatusStruct *status)
{
    memcpy(&(ioPwm->currentStatus), status, sizeof(struct IOPwmStatusStruct));
    ioPwm->__runTime = SYSTIME;
    ioPwm->startTime = REALTIME;
    PROCESS_CHANGE(ioPwm->process, Process_Run);
}
/*********************************************************************************************

  * @brief  IOPwm_ChangeDefault
  * @param  ioPwm：ioPwm实例指针
            status：状态
  * @return 
  * @remark 

  ********************************************************************************************/
void IOPwm_ChangeDefault(struct IOPwmStruct *ioPwm, struct IOPwmStatusStruct *status)
{
    memcpy(&(ioPwm->defaultStatus), status, sizeof(struct IOPwmStatusStruct));
    ioPwm->__runTime = SYSTIME;
    ioPwm->startTime = REALTIME;
}
/*********************************************************************************************

  * @brief  IOPwm_ChangeStatus
  * @param  status：状态实例指针
            dutyRatio：占空比
            totalInterval：总时长
            keepInterval：保持时长
  * @return 
  * @remark 

  ********************************************************************************************/
void IOPwm_StatusModify(struct IOPwmStatusStruct *status, uint8_t dutyRatio, uint16_t totalInterval, uint16_t keepInterval)
{
    status->activeInterval = (totalInterval * dutyRatio) / 100;
    status->totalInterval = totalInterval;
    status->keepInterval = keepInterval;
}
/*********************************************************************************************

  * @brief  IOPwm_IsInDefault
  * @param  ioPwm：ioPwm实例指针
  * @return 
  * @remark 是否处于默认运行下

  ********************************************************************************************/
bool IOPwm_IsInDefault(struct IOPwmStruct *ioPwm)
{
    return (ioPwm->process.current == Process_Idle);
}
/*********************************************************************************************

  * @brief  IOPwm_IsIdle
  * @param  ioPwm：ioPwm实例指针
  * @return 
  * @remark 是否为空闲

  ********************************************************************************************/
bool IOPwm_IsIdle(struct IOPwmStruct *ioPwm)
{
    return (ioPwm->process.current == Process_Idle && ioPwm->defaultStatus.keepInterval == 0);
}
/*********************************************************************************************

  * @brief  KeyDetect_PressCheck
  * @param  
  * @return 
  * @remark 按键按下检测

  ********************************************************************************************/
void KeyDetect_PressCheck(struct KeyDetectStruct *key, bool isInterrupted)
{
    if((isInterrupted == true || key->CallBack_IsKeyPressed(key) == true)
       && key->_isTrigged == false)
	{	
		key->_isTrigged = true;	
		key->__time = SYSTIME;
	}
}
/*********************************************************************************************

  * @brief  KeyDetect_Handle
  * @param  
  * @return 
  * @remark 按键抬起检测

  ********************************************************************************************/
void KeyDetect_Handle(struct KeyDetectStruct *key)
{
	// 检测到按键抬起
    if(key->CallBack_IsKeyPressed(key) == false && key->_isTrigged == true)
	{	
		key->_isTrigged = false;
        if(key->CallBack_KeyRaiseHandle != NULL)
		{   key->CallBack_KeyRaiseHandle(key, SYSTIME - key->__time); }
	}
}
/*********************************************************************************************

  * @brief  OptKeep_Start
  * @param  operation: 操作指针
            interval: 持续时间
  * @return 
  * @remark  操作保持开始

  ********************************************************************************************/
void OperationKeep_Start(struct OperationKeep *operation, uint32_t interval)
{
    operation->CallBack_IOOperation(true);
    operation->time = *(operation->referenceTime);
    operation->interval = interval;
    operation->isRunning = true;
}
/*********************************************************************************************

  * @brief  OptKeep_Handle
  * @param  operation: 操作指针
  * @return 
  * @remark 

  ********************************************************************************************/
void OperationKeep_Handle(struct OperationKeep *operation)
{
    if(operation->isRunning == true
       && (operation->time + operation->interval) < *(operation->referenceTime))
    {
        operation->CallBack_IOOperation(false);
        operation->isRunning = false;
    }
}
