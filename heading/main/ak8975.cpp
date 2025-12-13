#include <cstring>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"

extern MessageBufferHandle_t xMessageBufferToClient;

static const char *TAG = "MAG";

// I2Cdev and AK9875 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "AK8975.h"

#define RAD_TO_DEG (180.0/M_PI)
#define DEG_TO_RAD 0.0174533

#if 0
// Arduino macro
#define micros() (unsigned long) (esp_timer_get_time())
#define delay(ms) esp_rom_delay_us(ms*1000)
#endif

AK8975 mag(CONFIG_I2C_ADDR);

// MAG Data Sensitivity adjustment data
// Sensitivity adjustment data for each axis is stored to fuse ROM on shipment.
uint8_t MagAdjustment[3];
float MagCalibration[3];

void ak8975(void *pvParameters){
	// Initialize device
	mag.initialize();
	
	if (!mag.testConnection()) {
		ESP_LOGE(TAG, "testConnection fail");
		vTaskDelete(NULL);
	}

	// Goto Powerdown Mode
	mag.setMode(AK8975_MODE_POWERDOWN);
	// After power-down mode is set, at least 100ms(Twat) is needed before setting another mode.
	delay(200);
  
	// Goto Fuse ROM access mode
	mag.setMode(AK8975_MODE_FUSEROM);
	delay(200);
  
	// Get Sensitivity adjustment value from fuse ROM
	// Sensitivity adjustment values for each axis are written in the fuse ROM at the time of shipment.
	MagAdjustment[0] = mag.getAdjustmentX();
	MagAdjustment[1] = mag.getAdjustmentY();
	MagAdjustment[2] = mag.getAdjustmentZ();
	ESP_LOGI(TAG, "MagAdjustment: %d %d %d", MagAdjustment[0], MagAdjustment[1], MagAdjustment[2]);
  
	// Calculate sensitivity
	// from datasheet 8.3.11
	for (int i=0;i<3;i++) {
	  MagCalibration[i] = (float)(MagAdjustment[i] - 128)/256.0f + 1.0f;
	  ESP_LOGI(TAG, "MagCalibration[%d]=%f", i, MagCalibration[i]);
	}
  
	// Goto Powerdown Mode
	mag.setMode(AK8975_MODE_POWERDOWN);
	delay(200);
  
	while(1){
		// Goto Single Mode
		mag.setMode(AK8975_MODE_SINGLE);
	
		// Wait fo Data Ready
		int counter = 0;
		while(1) {
			if (mag.getDataReady()) break;
			counter++;
			vTaskDelay(1);
		}
		ESP_LOGD(TAG, "counter=%d", counter);

		// Read Measurement data
		int16_t mx, my, mz; 
		mag.getHeading(&mx, &my, &mz);
		bool OverflowStatus = mag.getOverflowStatus();
		bool DataError = mag.getDataError();
		ESP_LOGD(TAG, "OverflowStatus=%d DataError=%d", OverflowStatus, DataError);
		if (OverflowStatus) {
			ESP_LOGW(TAG, "Magnetic sensor overflow occurred");
		} else if (DataError) {
			ESP_LOGW(TAG, "Data read error occurred");
		} else {
			ESP_LOGD(TAG, "mag=%d %d %d", mx, my, mz);
			mx = mx + CONFIG_MAGX;
			my = my + CONFIG_MAGY;
			mz = mz + CONFIG_MAGZ;
			float _mx, _my, _mz; 
			_mx = (float)mx * MagCalibration[0];
			_my = (float)my * MagCalibration[1];
			_mz = (float)mz * MagCalibration[2];
			ESP_LOGD(TAG, "mag=%f %f %f", _mx, _my, _mz);

			float heading = atan2((double)_my, (double)_mx) * 180.0/3.14159265 + 180;
			while (heading < 0) heading += 360;
			while (heading > 360) heading -= 360;
			ESP_LOGI(TAG, "heading=%f", heading);

			// Send WEB request
			cJSON *request;
			request = cJSON_CreateObject();
			cJSON_AddStringToObject(request, "id", "data-request");
			cJSON_AddNumberToObject(request, "heading", heading);
			char *my_json_string = cJSON_Print(request);
			ESP_LOGD(TAG, "my_json_string\n%s",my_json_string);
			size_t xBytesSent = xMessageBufferSend(xMessageBufferToClient, my_json_string, strlen(my_json_string), 100);
			if (xBytesSent != strlen(my_json_string)) {
				ESP_LOGE(TAG, "xMessageBufferSend fail");
			}
			cJSON_Delete(request);
			cJSON_free(my_json_string);
		}

		vTaskDelay(10);
	}

	// Never reach here
	vTaskDelete(NULL);
}
