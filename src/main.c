#include "stm32f4xx.h"

void clock_init_100mhz_hse(void)
{
/* 1) liga o cristal de 100 MHz e espera estabilizar */
RCC->CR |= RCC_CR_HSEON;
while (!(RCC->CR & RCC_CR_HSERDY));
/* 2) prepara a Flash ANTES de subir a frequencia */
FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN

| FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS;

/* 3) prescalers: AHB /1, APB1 /2 (42 MHz max), APB2 /1 */
RCC->CFGR = RCC_CFGR_HPRE_DIV1
| RCC_CFGR_PPRE1_DIV2
| RCC_CFGR_PPRE2_DIV1;
/* 4) fatores do PLL — so com o PLL desligado */
RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE

| (25U << RCC_PLLCFGR_PLLM_Pos)
| (400U << RCC_PLLCFGR_PLLN_Pos)
| (1U << RCC_PLLCFGR_PLLP_Pos) // P = 4
| (9U << RCC_PLLCFGR_PLLQ_Pos);

/* 5) liga o PLL e espera o laco travar */
RCC->CR |= RCC_CR_PLLON;
while (!(RCC->CR & RCC_CR_PLLRDY));
/* 6) comuta o SYSCLK e confirma pelos bits SWS */
RCC->CFGR &= ~RCC_CFGR_SW;
RCC->CFGR |= RCC_CFGR_SW_PLL;
while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

volatile uint32_t ticks = 0;
void SysTick_Handler(void)
{
ticks++; // 1 tick a cada 1 ms
}
void delay_ms(uint32_t ms)
{
uint32_t inicio = ticks;
while ((ticks - inicio) < ms) { }
}

int main(void)
{
    clock_init_100mhz_hse(); // inicializa o clock em 100 MHz
    
SysTick_Config(100000); // base de tempo de 1 ms (HSE 100 MHz)
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // liga o clock do GPIOC
GPIOC->MODER &= ~(3 << (13 * 2)); // limpa os bits de PC13
GPIOC->MODER |= (1 << (13 * 2)); // PC13 como saida
while (1)
{
GPIOC->ODR ^= (1 << 13); // inverte o LED
delay_ms(500); // espera 500 ms
}
}