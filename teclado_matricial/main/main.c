#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "inttypes.h"
/*
Tener cuidado con usar GPIO, pueden tener configuracion fija (Solo input, solo pullup, etc)

https://www.luisllamas.es/arduino-teclado-matricial/
https://controlautomaticoeducacion.com/arduino/teclado-matricial-keypad/
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html#_CPPv410gpio_num_t
*/
static int fila_pulsada = 0, col_pulsada = 0;

static char teclado[4][4] = {
        { '1','2','3', 'A' },
        { '4','5','6', 'B' },
        { '7','8','9', 'C' },
        { '*','0','#', 'D' }
    };

#define GPIO_21    GPIO_NUM_21
#define GPIO_2     GPIO_NUM_2
#define GPIO_4     GPIO_NUM_4
#define GPIO_16    GPIO_NUM_16

#define GPIO_17    GPIO_NUM_17
#define GPIO_5     GPIO_NUM_5
#define GPIO_18    GPIO_NUM_18
#define GPIO_19    GPIO_NUM_19

static gpio_num_t filas[] = {GPIO_21,GPIO_2,GPIO_4,GPIO_16};
static gpio_num_t cols[]  = {GPIO_17,GPIO_5,GPIO_18,GPIO_19};
void delayMs(uint16_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}
void setupGPIOS(){
    for (int f = 0; f < 4; f++) {
        gpio_set_direction(filas[f], GPIO_MODE_OUTPUT);
        gpio_set_level(filas[f], !1);
    }
    for (int c = 0; c < 4; c++) {
        gpio_set_direction(cols[c], GPIO_MODE_INPUT);
        gpio_pullup_en(cols[c]);
    } 
}
bool leerTeclado(){
    bool pulsado = false;
    for (int f = 0; f < 4; f++)
    {
        gpio_set_level(filas[f], 0);

        for (int c = 0; c < 4; c++) {
            if (gpio_get_level(cols[c]) == 0)
            {
                fila_pulsada = f;
                col_pulsada = c;
                pulsado = true;
                while(gpio_get_level(cols[c]) == 0);
            }
        }
        gpio_set_level(filas[f], 1);
    }
    return pulsado;
}
void app_main(void)
{
    setupGPIOS();
    while(1){
        if(leerTeclado()){
            printf("%d | %d = %c\n",fila_pulsada,col_pulsada,teclado[fila_pulsada][col_pulsada]);
        }
        delayMs(100);
    }
}