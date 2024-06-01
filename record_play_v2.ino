#include <Arduino.h>
#include <driver/i2s.h>
#include <hal/i2s_types.h>

// I2S 引脚定义 INMP441
// SCK
#define I2S_MICROPHONE_BCLK      7
// WS
#define I2S_MICROPHONE_WS        6
// SD
#define I2S_MICROPHONE_DATA      4

// max98357
// BCLK
#define I2S_SPEAKER_BCLK         17
// LRC
#define I2S_SPEAKER_WS           16
// DIN
#define I2S_SPEAKER_DATA         18

// I2S 配置
i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
  .sample_rate = 44100,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = I2S_COMM_FORMAT_I2S_MSB,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 8,
  .dma_buf_len = 64,
  .use_apll = false,
  .tx_desc_auto_clear = true,
  .fixed_mclk = 0
};

// I2S 麦克风引脚配置
i2s_pin_config_t i2s_mic_pin_config = {
  .bck_io_num = I2S_MICROPHONE_BCLK,
  .ws_io_num = I2S_MICROPHONE_WS,
  .data_in_num = I2S_MICROPHONE_DATA,
  .data_out_num = I2S_PIN_NO_CHANGE
};

// I2S 扬声器引脚配置
i2s_pin_config_t i2s_speaker_pin_config = {
  .bck_io_num = I2S_SPEAKER_BCLK,
  .ws_io_num = I2S_SPEAKER_WS,
  .data_in_num = I2S_PIN_NO_CHANGE,
  .data_out_num = I2S_SPEAKER_DATA
};

void setup() {
  Serial.begin(115200);

  // 安装并启动 I2S 驱动
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  
  // 设置麦克风 I2S 引脚
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pin_config);

  // 设置扬声器 I2S 引脚
  i2s_set_pin(I2S_NUM_1, &i2s_speaker_pin_config);

  // 配置麦克风 I2S 时钟
  i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  // 配置扬声器 I2S 时钟
  i2s_set_clk(I2S_NUM_1, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void loop() {
  const int bufferSize = 1024;
  int16_t i2s_read_buffer[bufferSize];
  size_t bytes_read;

  // 从麦克风读取数据
  i2s_read(I2S_NUM_0, &i2s_read_buffer, bufferSize, &bytes_read, portMAX_DELAY);

  // 将数据写入扬声器
  size_t bytes_written;
  i2s_write(I2S_NUM_1, i2s_read_buffer, bytes_read, &bytes_written, portMAX_DELAY);
}
