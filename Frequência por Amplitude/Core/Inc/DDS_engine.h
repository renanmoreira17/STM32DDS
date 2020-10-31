/*
 * DDS_engine.h
 *
 *  Created on: Oct 30, 2020
 *  Universidade Federal de Minas Gerais
 *  	Author: Renan Moreira (renanmoreira@ufmg.br), Rodolfo Lessa (rodolfoalvv@ufmg.br)
 *     Version: 1.0
 * 	   License: GPLv3.0
 */

#ifndef INC_DDS_ENGINE_H_
#define INC_DDS_ENGINE_H_

#include "stdint.h"

void DDS_init();
void DDS_start();
void DDS_stop();
void DDS_set_frequency_index(uint8_t index);


#endif /* INC_DDS_ENGINE_H_ */
