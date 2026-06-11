#include "bsp_beep.h"

// 报警参数
#define ALARM_BASE_TIMER_US    500      // 基础定时器周期 0.5ms（微秒）
#define ALARM_TONE_SWITCH_MS   600      // 每 600ms 切换一次音调
#define ALARM_TONE_HIGH        1500     // 高音频率 (Hz)
#define ALARM_TONE_LOW         500      // 低音频率 (Hz)

static uint8_t timer_inited = 0;        // 定时器是否已初始化
static uint32_t tone_switch_counter = 0; // 音调切换计数器
static uint32_t half_period_counter = 0; // 半周期计数器
static uint32_t half_period_ticks = 0;   // 当前音调半周期对应的中断次数
static uint8_t current_tone = 0;         // 0=低音，1=高音
static uint8_t alarm_running = 0;        // 报警是否运行中

static void TIM3_Init(void);
static void update_half_period_ticks(void);

/**
 * @brief 初始化蜂鸣器GPIO（推挽输出，初始低电平）
 */
void BEEP_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);

    GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}

/**
 * @brief 启动报警（开启定时器，开始产生交替警报声）
 */
void BEEP_StartAlarm(void)
{
    if (!timer_inited)
    {
        TIM3_Init();
        timer_inited = 1;
    }

    current_tone = 0;
    tone_switch_counter = 0;
    half_period_counter = 0;
    update_half_period_ticks();
    alarm_running = 1;
    GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief 停止报警（关闭定时器，拉低引脚）
 */
void BEEP_StopAlarm(void)
{
    alarm_running = 0;
    TIM_Cmd(TIM3, DISABLE);
    GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}

/**
 * @brief 根据当前音调更新半周期中断计数
 */
static void update_half_period_ticks(void)
{
    uint32_t freq = current_tone ? ALARM_TONE_HIGH : ALARM_TONE_LOW;
    // 半周期时间（微秒）= 1e6 / (2 * freq)
    // 需要的中断次数 = 半周期时间 / 基础中断周期(微秒)
    half_period_ticks = (1000000 / (2 * freq)) / ALARM_BASE_TIMER_US;
    if (half_period_ticks < 1) half_period_ticks = 1;
}

/**
 * @brief 定时器3初始化（0.5ms中断）
 */
static void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 72MHz / 72 = 1MHz，计数 500 = 0.5ms
    TIM_TimeBaseStructure.TIM_Period = 500 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, DISABLE);
}

/**
 * @brief 蜂鸣器中断处理函数（由 stm32f10x_it.c 的 TIM3_IRQHandler 调用）
 */
void BEEP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        if (!alarm_running) return;

        // 音调切换
        tone_switch_counter++;
        if (tone_switch_counter >= (ALARM_TONE_SWITCH_MS * 1000 / ALARM_BASE_TIMER_US))
        {
            tone_switch_counter = 0;
            current_tone = !current_tone;
            update_half_period_ticks();
        }

        // 产生方波
        half_period_counter++;
        if (half_period_counter >= half_period_ticks)
        {
            half_period_counter = 0;
            GPIO_WriteBit(BEEP_GPIO_PORT, BEEP_GPIO_PIN,
                (BitAction)(1 - GPIO_ReadOutputDataBit(BEEP_GPIO_PORT, BEEP_GPIO_PIN)));
        }
    }
}
