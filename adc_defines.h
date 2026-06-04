#define FOSC 12000000
#define CCLK (FOSC*5)
#define PCLK (CCLK/4)
#define ADC_CLK 3000000

//defines for ADCR sfr
#define CLKDIV_BITS 8//8-15
#define PDN_BIT 21
#define ADC_CONV_START_BIT 24

//defines for ADDR sfr
#define DIGITAL_DATA_BIT 6//6-15
#define DONE_BIT 31

#define CLKDIV ((PCLK/ADC_CLK)-1)
#define AIN0_FUNC 0X00400000     

