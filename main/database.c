#include "arch/sys_arch.h"
#include "database.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_client.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sec1.pb-c.h"
#include "string.h"
#include "read_nfc.h"
#include "esp_err.h"
#include "freertos/event_groups.h"

#include "cJSON.h"

#include "ui.h"
#include "widgets/lv_label.h"

// URL for HTTP POST request
#define POST_URL "https://products-010b.restdb.io/rest/clients"
#define API_KEY "3066f1ec7cfcb3db4b30de1cb620633608e8d"
#define BUFFERSIZE 512


void http_get_users_db();
void http_get_user_orders();

static const char *TAG = "HTTP";

char* username;
float balance;
float BTCPRICE;

esp_err_t _my_http_event_handler(esp_http_client_event_t *evt) {
	static char *output_buffer;  // Buffer to store response of http request from event handler
	static int output_len; 
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "Header");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "Redirect");
            break;
        case HTTP_EVENT_ON_DATA:    
        if (evt->data_len > 0 && evt->data) {
                ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
                if (evt->user_data) {
	                memcpy(evt->user_data + output_len, evt->data, evt->data_len);
	            } else {
	                if (output_buffer == NULL) {
	                    output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
	                    output_len = 0;
	                    if (output_buffer == NULL) {
	                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
	                        return ESP_FAIL;
	                    }
	                }
	                memcpy(output_buffer + output_len, evt->data, evt->data_len);
	            }
	            output_len += evt->data_len;
	        }
 		break;	            
	    case HTTP_EVENT_ON_FINISH:
	        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
	        if (output_buffer != NULL) {
	            ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
	            free(output_buffer);
	            output_buffer = NULL;
	        }
	        output_len = 0;
	        
	        break;
	        
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


esp_err_t _my_http_POST_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "Header");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "Redirect");
            break;
        case HTTP_EVENT_ON_DATA:    
        if (evt->data_len > 0 && evt->data) {
                ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
              }
 		break;	            
	    case HTTP_EVENT_ON_FINISH:
	        break;
	        
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void http_register_user() {
    esp_http_client_config_t config = {
        .url = POST_URL,  
        .event_handler = _my_http_POST_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
	
	esp_http_client_set_method(client, HTTP_METHOD_POST);
    // Додавання заголовків
    esp_http_client_set_header(client, "content-type", "application/json");
    esp_http_client_set_header(client, "x-apikey", API_KEY);
	esp_http_client_set_header(client, "cache-control", "no-cache");
	
    const char* login = lv_textarea_get_text(ui_RegisterScreenNameInputArea);
	const char* password = NFCDATA;
	
	
	
	cJSON *json = cJSON_CreateObject();
	cJSON_AddStringToObject(json, "username", login);
	cJSON_AddStringToObject(json, "password", password);
	cJSON_AddStringToObject(json, "balance", "0");

	char *postdata = cJSON_PrintUnformatted(json);

	esp_http_client_set_post_field(client, postdata, strlen(postdata));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
	free(postdata);
	cJSON_Delete(json);
    esp_http_client_cleanup(client);
   
}

void http_get_users_db()
{
	bool registered = false;
	char responseBuffer[BUFFERSIZE];
	esp_http_client_config_t config = {
        .url = POST_URL,  
        .event_handler = _my_http_event_handler,
        .user_data = responseBuffer,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
	
	esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_http_client_set_header(client, "content-type", "application/json");
    esp_http_client_set_header(client, "x-apikey", API_KEY);
	esp_http_client_set_header(client, "cache-control", "no-cache");
	

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d", esp_http_client_get_status_code(client));
        
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    
    cJSON *json = cJSON_Parse(responseBuffer);
            if (json == NULL) {
                ESP_LOGE(TAG, "bad JSON!");
            }

            if (cJSON_IsArray(json)) {
                int user_count = cJSON_GetArraySize(json);
                for (int i = 0; i < user_count; i++) {
                    cJSON *user = cJSON_GetArrayItem(json, i);
                    cJSON *password = cJSON_GetObjectItem(user, "password");
        			if (strncmp(password->valuestring, NFCDATA, 6) == 0 ){
							cJSON *db_username = cJSON_GetObjectItem(user, "username");
							cJSON *db_balance = cJSON_GetObjectItem(user, "balance");
							username = db_username->valuestring;
							char printusername[32] = "Welcome, ";
							strcat(printusername, username);
							lv_label_set_text(ui_MainScreenHeaderLabel, printusername);
							
							balance = db_balance->valuedouble;
							char strbal[20];
							sprintf(strbal, "%.2f", balance);
							char printbalance[20] = "Your balance: ";
							strcat(printbalance, strbal);
							lv_label_set_text(ui_MainScreenBalanceLabel, printbalance);
            				_ui_screen_change(&ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_MainScreen_screen_init);
            				
            				registered = true;
        			}

                }
                
            } else {
                ESP_LOGE(TAG, "json not arr!");
            }
			
            // Очищаємо JSON-об'єкт
            cJSON_Delete(json);

    esp_http_client_cleanup(client);
    
    if(!registered){
		_ui_screen_change(&ui_RegisterScreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_RegisterScreen_screen_init);
	}else{
		http_get_user_orders();
	}
}
	
void http_get_BTC_price()
{

	char responseBuffer[BUFFERSIZE];
	esp_http_client_config_t config = {
        .url = "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT",  
        .event_handler = _my_http_event_handler,
        .user_data = responseBuffer,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "content-type", "application/json");
	
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d", esp_http_client_get_status_code(client));
        
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    
    cJSON *json = cJSON_Parse(responseBuffer);
    if (json == NULL) {
    	ESP_LOGE(TAG, "bad JSON!");
	}
	if (cJSON_IsArray(json)) {
		cJSON *BTCprice = cJSON_GetObjectItem(json, "price");
		BTCPRICE = BTCprice->valuedouble;
	}
	cJSON_Delete(json);
    esp_http_client_cleanup(client);
}

void http_get_user_orders() {
	char responseBuffer[BUFFERSIZE];
	esp_http_client_config_t config = {
        .url = "https://products-010b.restdb.io/rest/orders", 
        .event_handler = _my_http_event_handler,
        .user_data = responseBuffer,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
	esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_http_client_set_header(client, "content-type", "application/json");
    esp_http_client_set_header(client, "x-apikey", API_KEY);
	esp_http_client_set_header(client, "cache-control", "no-cache");
	
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d", esp_http_client_get_status_code(client));
        
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    
    cJSON *json = cJSON_Parse(responseBuffer);
    if (cJSON_IsArray(json)) {
                int order_count = cJSON_GetArraySize(json);
                int current_order = 0;
                char result[32];
                for (int i = 0; i < order_count; i++) {
                    cJSON *order = cJSON_GetArrayItem(json, i);
                    cJSON *password = cJSON_GetObjectItem(order, "password");
        			if (strncmp(password->valuestring, NFCDATA, 6) == 0 ){
							cJSON *db_orderprice = cJSON_GetObjectItem(order, "orderprice");
							cJSON *db_ordervalue = cJSON_GetObjectItem(order, "ordervalue");
							float orderprice = db_orderprice->valuedouble;
							float ordervalue = db_ordervalue->valuedouble;
							sprintf(result, "BTCUSDT - %.2f - %.2f", orderprice, ordervalue);
							lv_table_set_cell_value(table, current_order, 0, result);
        					lv_obj_t * btn = lv_btn_create(table);
        					lv_obj_set_size(btn, 110, 30);
        					lv_obj_t * label = lv_label_create(btn);
        					lv_label_set_text(label, "Close deal");
        					lv_obj_center(label); 
        					lv_obj_align(btn, LV_ALIGN_CENTER, 200, current_order * 55 - 115);
        					current_order++;
    				}
                }
                
            } else {
                ESP_LOGE(TAG, "json not arr!");
            }
	cJSON_Delete(json);
    esp_http_client_cleanup(client);
   
}

