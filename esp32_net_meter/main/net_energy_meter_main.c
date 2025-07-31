/*
	ESP32-IDF program to
	- Read JSON formatted object of timestamped power values from MATLAB Simulink via USB.
	- Calculate real and reactive energies and append to the incoming MATLAB object.
	- Publish the new JSON object to subscribed MQTT clients (entreprise services).
*/
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart_vfs.h"
#include "driver/uart.h"
#include "energy.h"
#include "mqtt.h"


#define RX_BUFFER_SIZE 4000

//initialize mqtt client
esp_mqtt_client_handle_t client;

static void uart_read_task(void *arg)
{
    const char *UART_READ_TASK_TAG = "UART_READ_TASK";
    esp_log_level_set(UART_READ_TASK_TAG, ESP_LOG_INFO);
    uint8_t *rx_buffer = (uint8_t *)malloc(RX_BUFFER_SIZE);
    if (uart_driver_install(UART_NUM_0, 2 * 1024, 0, 0, NULL, 0) != ESP_OK) {
        ESP_LOGE(UART_READ_TASK_TAG, "Driver installation failed");
        vTaskDelete(NULL);
    }

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_param_config(UART_NUM_0, &uart_config);

    while (1) {
    	const int received_bytes = uart_read_bytes(UART_NUM_0, rx_buffer, RX_BUFFER_SIZE, 1000 / portTICK_PERIOD_MS);
    	if(received_bytes){
    	    rx_buffer[received_bytes] = 0;
    	    esp_mqtt_client_publish(client, "meter_data",  estimate_energy((const char *)rx_buffer), 0, 1, 0);
    	    ESP_LOGI(UART_READ_TASK_TAG, "Processed %d bytes: %s\n", received_bytes, rx_buffer);    	    
    	}
    }
    free(rx_buffer);

    vTaskDelete(NULL);	//call vTaskDelete() before return. Otherwise program will crash
}

int app_main()
{
    ESP_LOGI(MQTT_CLIENT_TAG, "[APP] Startup..");
    ESP_LOGI(MQTT_CLIENT_TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(MQTT_CLIENT_TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
    esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("transport", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    mqtt_service_start(&client);
    xTaskCreate(uart_read_task, "uart_read_task", 4*1024, NULL, 5, NULL);
    return 0;
}
