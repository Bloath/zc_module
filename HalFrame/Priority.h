#ifndef _PRIORITY_H_
#define _PRIORITY_H_
/* Includes ------------------------------------------------------------------*/
#include "Module/Module.h"
#include "Priority_Conf.h"
/* typedef -------------------------------------------------------------------*/
struct TaskUnitStruct
{   
    bool (*CallBack_IsNeedHandle)();    // 判断是否需要执行
    void (*CallBack_Init)();            // 初始化
    void (*CallBack_Handle)();          // 执行
    void (*CallBack_Finish)();          // 完成
    void (*CallBack_Pause)();           // 暂停
    void (*CallBack_Unpause)();         // 恢复暂停
    uint32_t time;                      // 时间
    uint16_t flag;                      // 标志位
    uint8_t level;                      // 等级
};

struct PriorityStruct
{
    struct TaskUnitStruct* __taskList[PRIORITY_TASK_COUNT];    // 任务队列
    int currentLevel;                                   // 当前最高任务等级, -1就是啥事没有
    uint8_t _usedTaskCount;                             // 当前使用的任务数量
    struct ProcessStruct process;                       // 流程
};
/* define --------------------------------------------------------------------*/
#define TASK_FLAG_INIT      (1<<0)
#define TASK_FLAG_HANDLING  (1<<1)
#define TASK_FLAG_PAUSING   (1<<2)

/* macro ---------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* function prototypes -------------------------------------------------------*/
/* functions -----------------------------------------------------------------*/
void Priority_AddTask(struct PriorityStruct *priorityObj, struct TaskUnitStruct *taskObj, uint8_t level);
void Priority_Handle(struct PriorityStruct *priorityObj);
int Priority_TaskkeepTime(struct TaskUnitStruct* task);
#endif