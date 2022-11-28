#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "sdkconfig.h"

#include "oled_utils.h"

void delay(uint32_t ms){
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void app_main(void)
{
	i2c_oled_master_init();
	oled_init();
	oled_clear();/*
	while(1){
		oled_clear();
		oled_fill_yellow();
		delay(1000);
		oled_fill_blue();
		delay(1000);
		oled_clear_yellow();
		delay(1000);
		oled_clear_blue();
		delay(1000);
		oled_write_str("User: Irvin", 0);
		delay(1000);
		oled_write_str("Temp: 12 C", 2);
		delay(1000);
		//Los saltos de linea hacen cambiar entre paginas
		oled_fill();
		oled_write_str("Usuario:Irvin\nPlanta:Girasolio\nTemp:12C\nH:50%\n", 0);
		delay(1000);
		oled_clear();
		oled_send_data(&pokemon[0][0],1024);
		delay(2000);
		oled_clear();
		oled_send_data(&caricatura[0][0],1024);
		delay(2000);
		oled_clear();
		oled_send_data(&caricatura_2[0][0],1024);
		delay(2000);

	}*/

	oled_write_str("Usuario: Irvin\nTemp: 12 C\n", 0);
	oled_send_data(&girasol[0],768);

	
}