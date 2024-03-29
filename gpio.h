/*
 * gpio.h
 *
 *  Created on: 11.07.2019
 *      Author: slawek
 *
 *  Macros for configuration and using GPIO for STM32F030 and alike
 *
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f0xx.h"
#include <stdint.h>


#define INLINE	inline __attribute__((always_inline))

typedef enum
{
	//mode								//can be ORed with:
	INPUT 		= 	0U,			//pull
	GP_OUTPUT	=	(1U<<9U),	//type, pull, speed
	ALTERNATE	=	(2U<<9U),	//type, pull, speed, alt
	ANALOG		=	(3U<<9U),	//-

	//type
	PUSH_PULL	=	(0U),
	OPEN_DRAIN	=	(1U<<8U),

	//pull
	NO_PULL_UP_DOWN	= (0U),
	PULL_UP		=	(1U<<4U),
	PULL_DOWN	=	(2U<<4U),

	//speed
	LOW_SPEED		=	0U,
	MEDIUM_SPEED	=	(1U<<6U),
	HIGH_SPEED		=	(3U<<6U),

	//alt
	AF0			=	0U,
	AF1			=	1U,
	AF2			=	2U,
	AF3			=	3U,
	AF4			=	4U,
	AF5			=	5U,
	AF6			=	6U,
	AF7			=	7U,

	//Alternate mode+alt. Can be ORed with type, pull, speed
	ALT_0	=	ALTERNATE + AF0,
	ALT_1	=	ALTERNATE + AF1,
	ALT_2	=	ALTERNATE + AF2,
	ALT_3	=	ALTERNATE + AF3,
	ALT_4	=	ALTERNATE + AF4,
	ALT_5	=	ALTERNATE + AF5,
	ALT_6	=	ALTERNATE + AF6,
	ALT_7	=	ALTERNATE + AF7

} GPIO_Mode_T;


typedef enum {

	GPIO_LO,
	GPIO_HI

} GPIO_Level_T;


static INLINE void GPIO_Config(GPIO_TypeDef * const restrict gpio, unsigned int pin, GPIO_Mode_T mode)
{
	//require DEFINE_THIS_FILE from massert.h
	//ASSERT(pin<16);

	if (mode & 0x100u)
		gpio->OTYPER |= (1<<pin);
	else
		gpio->OTYPER &= (uint32_t)~(1<<pin);

	pin *= 2;

	uint32_t reset_mask = ~(0x03u << pin);

	gpio->MODER &= reset_mask;
	gpio->MODER |= (((mode & 0x600u) >> 9u) << pin );

	gpio->PUPDR &= reset_mask;
	gpio->PUPDR |= (((mode & 0x30u) >> 4u) << pin );

	gpio->OSPEEDR &= reset_mask;
	gpio->OSPEEDR |= (((mode & 0xC0u) >> 6u) << pin);

	pin *= 2;
	volatile uint32_t * reg_adr = (pin<32)? &gpio -> AFR[0] : &gpio -> AFR[1];
	pin &= 0x1f;

	*reg_adr &= ~(0x0fu << pin);
	*reg_adr |= (uint32_t)(mode & 0x0ful) << pin;

}

static INLINE void GPIO_Set(GPIO_TypeDef * const restrict gpio, int pin, GPIO_Level_T level)
{
	gpio -> BSRR = (1 << (pin + !(level) * 16));
}

static INLINE void GPIO_Hi(GPIO_TypeDef * const restrict gpio, int pin)
{
	gpio -> BSRR = (1 << pin);
}

static INLINE void GPIO_Low(GPIO_TypeDef * const restrict gpio, int pin)
{
	gpio -> BRR = 1 << pin;
}

static INLINE void GPIO_HiMsk(GPIO_TypeDef * const restrict gpio, uint16_t mask)
{
	gpio -> BSRR = (mask);
}

static INLINE void GPIO_LowMsk(GPIO_TypeDef * const restrict gpio, uint16_t mask)
{
	gpio -> BRR = mask;
}

static INLINE void GPIO_HiLowMsk(GPIO_TypeDef * const restrict gpio, uint16_t hiMask, uint16_t lowMask)
{
	gpio -> BSRR = ((uint32_t)hiMask) | (((uint32_t)lowMask)<<16);
}

static INLINE int GPIO_Get(GPIO_TypeDef * const restrict gpio, int pin)
{
	return (gpio -> IDR & (1 << pin));
}

static INLINE int GPIO_Get01(GPIO_TypeDef * const restrict gpio, int pin)
{
	return !!(gpio -> IDR & (1 << pin));
}

#define GPIO(b_) GPIO_(b_)
#define GPIO_(b_) (GPIO##b_)

#define GPIOxEN(b_) (RCC->AHBENR |= AHBEN_(b_))
#define AHBEN_(b_) (RCC_AHBENR_GPIO##b_##EN)

#define _BV(b_) (1<<(b_))

#endif /* GPIO_H_ */
