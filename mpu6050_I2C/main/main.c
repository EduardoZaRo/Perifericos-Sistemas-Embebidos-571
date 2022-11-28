#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "sdkconfig.h"
static const char *TAG = "Practica 3";

#define I2C_MASTER_SCL_IO           22         /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21         /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

/**** CONSTANTES PRACTICA 3****/
#define WHO_AM_I        0x75
#define GYRO_XOUT_H         0x43
#define GYRO_XOUT_L         0x44
#define GYRO_YOUT_H         0x45
#define GYRO_YOUT_L         0x46
#define GYRO_ZOUT_H         0x47
#define GYRO_ZOUT_L         0x48

#define ACCEL_XOUT_H         0x3B
#define ACCEL_XOUT_L         0x3C
#define ACCEL_YOUT_H         0x3D
#define ACCEL_YOUT_L         0x3E
#define ACCEL_ZOUT_H         0x3F
#define ACCEL_ZOUT_L         0x40

#define TEMP_OUT_H           0x41
#define TEMP_OUT_L           0x42

#define LEN             8
#define ADDR            0x68 //MPU6050 slave register
#define PWR_MGMT_1      0x6B
#define ACCEL_CONFIG    0x1C
#define GYRO_CONFIG     0x1B
void delay(uint32_t ms){
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

static esp_err_t device_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ADDR << 1), 0);
    i2c_master_write_byte(cmd, reg_addr, 0);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ADDR << 1) | 1, 0);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t device_register_write(uint8_t reg_addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ADDR << 1), 0);
    i2c_master_write_byte(cmd, reg_addr, 0);
    i2c_master_write_byte(cmd, data, 0);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

void print_data(char* str, uint8_t addr){
    uint8_t data = 0;
    char test[40];

    if (device_register_read(addr, &data, LEN) == ESP_OK)
        sprintf(test,"%s \t= 0x%x",str, data);
    else 
        sprintf(test,"ERROR %s \t= 0x%x",str,data);
    
    ESP_LOGI(TAG, "%s", test);
}

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}
void getAccels(float * accelx, float * accely, float * accelz){
    uint8_t accxh, accxl, accyh, accyl, acczh, acczl;
    device_register_read(ACCEL_XOUT_H, &accxh, LEN); 
    device_register_read(ACCEL_XOUT_L, &accxl, LEN); 
    device_register_read(ACCEL_YOUT_H, &accyh, LEN); 
    device_register_read(ACCEL_YOUT_L, &accyl, LEN); 
    device_register_read(ACCEL_ZOUT_H, &acczh, LEN); 
    device_register_read(ACCEL_ZOUT_L, &acczl, LEN); 
    *accelx = (int16_t)(accxh << 8|accxl)/16384.00 * 9.806;
    *accely = (int16_t)(accyh << 8|accyl)/16384.00 * 9.806;
    *accelz = (int16_t)(acczh << 8|acczl)/16384.00 * 9.806;
}
void getGyros(float * gyrox, float * gyroy, float * gyroz){
    uint8_t gyroxh, gyroxl, gyroyh, gyroyl, gyrozh, gyrozl;
    device_register_read(GYRO_XOUT_H, &gyroxh, LEN); 
    device_register_read(GYRO_XOUT_L, &gyroxl, LEN); 
    device_register_read(GYRO_YOUT_H, &gyroyh, LEN); 
    device_register_read(GYRO_YOUT_L, &gyroyl, LEN); 
    device_register_read(GYRO_ZOUT_H, &gyrozh, LEN); 
    device_register_read(GYRO_ZOUT_L, &gyrozl, LEN); 
    *gyrox = (int16_t)(gyroxh << 8 | gyroxl)/131.00;
    *gyroy = (int16_t)(gyroyh << 8 | gyroyl)/131.00;
    *gyroz = (int16_t)(gyrozh << 8 | gyrozl)/131.00;
}
void getTemp(float * temp){
    uint8_t temph, templ;
    device_register_read(TEMP_OUT_H, &temph, LEN);  
    device_register_read(TEMP_OUT_L, &templ, LEN);
    *temp = (int16_t)(temph << 8 | templ)/340.0 + 36.53;
}
void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
    device_register_write(PWR_MGMT_1, 0x00); //Quita modo sleep por default
    delay(100);
    device_register_write(ACCEL_CONFIG, 0x00);//± 2g
    delay(100);
    device_register_write(GYRO_CONFIG, 0x00); //± 250 °/s
    delay(100);

    float accx, accy, accz, gyrox, gyroy, gyroz, temp;
    while (1)
    {
        print_data("WHO_I_AM", WHO_AM_I);

        getAccels(&accx, &accy, &accz);
        getGyros(&gyrox, &gyroy, &gyroz);
        getTemp(&temp);
        printf("ACCX: %f | ACCY: %f | ACCZ: %f |\n", accx, accy, accz);
        printf("GYROX: %f | GYROY: %f | GYROZ: %f |\n", gyrox, gyroy, gyroz);
        printf("TEMP: %f |\n", temp);
        ESP_LOGI(TAG, "\n");
        delay(1000);
    }

    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C unitialized successfully");
}
