#include "stm32f10x.h" // Device header

void Timer_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 开启时钟

    TIM_InternalClockConfig(TIM2); // 使用内部时钟源

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;       // 滤波器分频系数
    TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInitStructure.TIM_Period            = 100 - 1;            // 自动重装器
    TIM_TimeBaseInitStructure.TIM_Prescaler         = 720 - 1;            // 预分频器
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                  // 重复计数器
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);                   // 配置时基单元

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);      // 避免初始化后立即进入中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 使能更新中断

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // NVIC分组

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_Init(&NVIC_InitStructure); // NVIC配置

    TIM_Cmd(TIM2, ENABLE); // 启动定时器
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        // do something

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
