#include <Arduino.h>
#include <driver/i2s.h>
#include <hal/i2s_types.h>

#define INMP441_WS 6
#define INMP441_SCK 7
#define INMP441_SD 4



#define MAX98357_LRC 16
#define MAX98357_BCLK 17
#define MAX98357_DIN 18

// #define SAMPLE_RATE 44100
#define SAMPLE_RATE 16000

i2s_config_t inmp441_i2s_config = {
  .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = SAMPLE_RATE,
  .bits_per_sample = i2s_bits_per_sample_t(16),
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_MSB),
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 8,   // buffer 的数量
  .dma_buf_len = 128    // buffer的大小，单位是i2s_bits_per_sample_t 采样位数，越小播放需要越及时时延越小，否则相反
};


const i2s_pin_config_t inmp441_gpio_config = {
  .bck_io_num = INMP441_SCK,
  .ws_io_num = INMP441_WS,
  .data_out_num = -1,
  .data_in_num = INMP441_SD
};

i2s_config_t max98357_i2s_config = {
  .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate = SAMPLE_RATE,
  .bits_per_sample = i2s_bits_per_sample_t(16),
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_MSB),
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 8,
  .dma_buf_len = 128
};


const i2s_pin_config_t max98357_gpio_config = {
  .bck_io_num = MAX98357_BCLK,
  .ws_io_num = MAX98357_LRC,
  .data_out_num = MAX98357_DIN,
  .data_in_num = -1
};

void setup()
{

  i2s_driver_install(I2S_NUM_0, &inmp441_i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &inmp441_gpio_config);

  i2s_driver_install(I2S_NUM_1, &max98357_i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_1, &max98357_gpio_config);

}

void loop()
{

  size_t bytes_read;

  uint16_t data[256];

  esp_err_t result = i2s_read(I2S_NUM_0, &data, sizeof(data), &bytes_read, portMAX_DELAY);

  size_t bytes_write;

  result = i2s_write(I2S_NUM_1, &data, sizeof(data), &bytes_write, portMAX_DELAY);

}
