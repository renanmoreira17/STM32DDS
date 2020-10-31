/*
 * DDS_engine.c
 *
 *  Created on: Oct 30, 2020
 *  Universidade Federal de Minas Gerais
 *  	Author: Renan Moreira (renanmoreira@ufmg.br), Rodolfo Lessa (rodolfoalvv@ufmg.br)
 *     Version: 1.0
 * 	   License: GPLv3.0
 */

#include "DDS_engine.h"
#include "DDS_tables.h"

#include "stm32f1xx_hal.h"

volatile uint32_t current_FTW;
volatile uint32_t phase_accumulator;

extern TIM_HandleTypeDef htim2; //acesso ao timer2 (DDS) definido na main.c
extern TIM_HandleTypeDef htim3; //acesso ao timer3 (PWM) definido na main.c

void DDS_init() {
	//zera o FTW atual até ser fornecido um outro pelo DDS_set_frequency_index(uint8_t);
	current_FTW = 0;
	//zera o acumulador de fase
	phase_accumulator = 0;
	//para o DDS até o usuário chamar DDS_start() pela primeira vez
	DDS_stop();
}

void DDS_start() {
	//ativa o contador do timer de controle do DDS
	htim2.Instance->CR1 |= TIM_CR1_CEN;
}

void DDS_stop() {
	//desativa o contador do timer de controle do DDS
	htim2.Instance->CR1 &= ~TIM_CR1_CEN;
	//reseta o contador do timer de controle do DDS para o início
	htim2.Instance->CNT = 0;
	//zera o registrador de comparacão do PWM, fazendo duty-cyle de 0%.
	htim3.Instance->CCR4 = 0;
}

void DDS_set_frequency_index(uint8_t index) {
	//busca a frequência correspondente ao índice passado na LUT e o atribui ao FTW atual
	current_FTW = FTW_LUT[index];
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//verifica se a origem da interrupcão é o timer de controle do DDS
	if (htim != &htim2) return;

	//incrementa o phase accumulator com o valor de FTW atual.
	phase_accumulator += current_FTW;

	//pega apenas os 10 bits mais significantes do phase accumulator.
	//isso porque nossa LUT da forma de onda tem 2^10 posicões
	uint16_t LUT_index = (phase_accumulator & 0xFFC00000) >> 22;

	//busca essa posicao na lookup table da nossa forma de onda
	uint16_t output_value = SINE_LUT[LUT_index];

	//como a resolucao do nosso PWM foi calculada como 2^12, e a saída
	//da LUT de forma de onda é de 16 bits, truncamos esse valor
	//novamente para caber no registrador de comparacão do PWM.
	output_value = output_value >> 4;

	//seta esse valor calculado no registrador de comparacão
	htim3.Instance->CCR1 = output_value;
	/*
	 * de maneira alternativa, pode-se fazer essa mesma lógica em apenas uma linha:
	 * htim3->Instance->CCR1 = SINE_LUT[((phase_accumulator+=current_FTW)&0xFFC00000)>>22]>>4;
	 */
}
