#include <stdio.h>
#include <stdlib.h>
#include <esp_log.h>
#include "ui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "pn532_driver_i2c.h"
#include "pn532_driver_hsu.h"
#include "pn532.h"
#include "ui_helpers.h"
#include "database.h"
// select ONLY ONE interface for the PN532
#define PN532_MODE_I2C 0
#define PN532_MODE_HSU 1
#define PN532_MODE_SPI 0

#if PN532_MODE_I2C

// I2C mode needs only SDA, SCL and IRQ pins. RESET pin will be used if valid.
// IRQ pin can be used in polling mode or in interrupt mode. Use menuconfig to select mode.
#define SCL_PIN    (9)
#define SDA_PIN    (8)
#define RESET_PIN  (-1)
#define IRQ_PIN    (3)

#elif PN532_MODE_HSU

// HSU mode needs only RX/TX pins. RESET pin will be used if valid.
#define RESET_PIN      (-1)
#define IRQ_PIN        (-1)
#define HSU_HOST_RX    (44)
#define HSU_HOST_TX    (43)
#define HSU_UART_PORT  UART_NUM_0
#define HSU_BAUD_RATE  (921600)

#elif PN532_MODE_SPI

#error SPI is not implemented

#endif
static const char *TAGG = "HTTP";
char NFCDATA[8];
void read_nfc_card()
{
    pn532_io_t pn532_io;
    esp_err_t err;

    printf("APP MAIN\n");

#if 0
    // Enable DEBUG logging
    esp_log_level_set("PN532", ESP_LOG_DEBUG);
    esp_log_level_set("pn532_driver", ESP_LOG_DEBUG);
    esp_log_level_set("pn532_driver_i2c", ESP_LOG_DEBUG);
    esp_log_level_set("pn532_driver_hsu", ESP_LOG_DEBUG);
    esp_log_level_set("i2c.master", ESP_LOG_DEBUG);
#endif

    vTaskDelay(1000 / portTICK_PERIOD_MS);

#if PN532_MODE_I2C

    ESP_LOGI(TAG, "init PN532 in I2C mode");
    ESP_ERROR_CHECK(pn532_new_driver_i2c(SDA_PIN, SCL_PIN, RESET_PIN, IRQ_PIN, 0, &pn532_io));

#elif PN532_MODE_HSU

    ESP_ERROR_CHECK(pn532_new_driver_hsu(HSU_HOST_RX,
                                         HSU_HOST_TX,
                                         RESET_PIN,
                                         IRQ_PIN,
                                         HSU_UART_PORT,
                                         HSU_BAUD_RATE,
                                         &pn532_io));

#endif

    do {
        err = pn532_init(&pn532_io);
        if (err != ESP_OK) {
            pn532_release(&pn532_io);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    } while(err != ESP_OK);

    uint32_t version_data = 0;
    do {
        err = pn532_get_firmware_version(&pn532_io, &version_data);
        if (ESP_OK != err) {
            pn532_reset(&pn532_io);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    } while (ESP_OK != err);

    // Log firmware infos
    int tries = 0;
    char LASTNFCDATA[8];
    while (1)
    {
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uid_length;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

        // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
        // 'uid' will be populated with the UID, and uid_length will indicate
        // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
        err = pn532_read_passive_target_id(&pn532_io, PN532_BRTY_ISO14443A_106KBPS, uid, &uid_length, 0);

        if (ESP_OK == err)
        {
            // Display some basic information about the card
            ESP_LOG_BUFFER_HEX_LEVEL(TAGG, uid, uid_length, ESP_LOG_INFO);
            int len = 0;
			for (int i = 0; i < uid_length; i++) {
        		len += sprintf(NFCDATA + len, "%d", uid[i]); 
    		}
    		int len2 = 0;
    		for (int i = 0; i < uid_length; i++) {
        		len2 += sprintf(LASTNFCDATA + len2, "%d", uid[i]); 
    		}
    		if (strcmp(NFCDATA, LASTNFCDATA) == 0) {
				tries++;
			}else{
				tries = 0;
			}
			if (tries >= 5)
			{
				// lv_label_set_text(ui_LoginChooseLabel, NFCDATA);
				tries = 0;
				http_get_users_db();
				if(nfcTask != NULL)
				{
					vTaskSuspend(nfcTask);
					nfcTask = NULL;
				}
			}
			
    		//lv_label_set_text(ui_LoginChooseLabel,  NFCDATA);
            err = pn532_in_list_passive_target(&pn532_io);
            if (err != ESP_OK) {
                continue;
            }

            NTAG2XX_MODEL ntag_model = NTAG2XX_UNKNOWN;
            err = ntag2xx_get_model(&pn532_io, &ntag_model);
            if (err != ESP_OK)
                continue;

            int page_max;
            switch (ntag_model) {
                case NTAG2XX_NTAG213:
                    page_max = 45;
                    ESP_LOGI(TAGG, "found NTAG213 target (or maybe NTAG203)");
                    break;

                case NTAG2XX_NTAG215:
                    page_max = 135;
                    ESP_LOGI(TAGG, "found NTAG215 target");
                    break;

                case NTAG2XX_NTAG216:
                    page_max = 231;
                    ESP_LOGI(TAGG, "found NTAG216 target");
                    break;

                default:
                    ESP_LOGI(TAGG, "Found unknown NTAG target!");
                    continue;
            }

            for(int page=0; page < page_max; page+=4) {
                uint8_t buf[16];
                err = ntag2xx_read_page(&pn532_io, page, buf, 16);
                if (err == ESP_OK) {
                    ESP_LOG_BUFFER_HEXDUMP(TAGG, buf, 16, ESP_LOG_INFO);
                }
                else {
                    break;
                }
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}
