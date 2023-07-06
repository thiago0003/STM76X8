#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <string.h>

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

#define UART UART_NUM_2

int num = 0;

void init(void) 
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_BITS_MAX,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // We won't use a buffer for sending data.
    uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

static void tx_task(void *arg)
{
	char* Txdata = (char*) malloc(100);
    while (1) {
    	sprintf (Txdata, "0x00");
        uart_write_bytes(UART, Txdata, strlen(Txdata));
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART, data, RX_BUF_SIZE, 500 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            //data[rxBytes] = 0;

            for(int i = 0; i < rxBytes; i++)
                ESP_LOGI(RX_TASK_TAG, "Read %d'%02X'", rxBytes, data[i]);

            ESP_LOGI(RX_TASK_TAG, "-----------------------------------");
        }
    }
    free(data);
}

void app_main(void)
{
    init();
    //xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);
}





