#include <Arduino.h>
#include <driver/i2s.h>

// I2S 配置
#define I2S_NUM         I2S_NUM_0
#define I2S_BCK_PIN     17
#define I2S_WS_PIN      16
#define I2S_DATA_PIN    18

void setup() {
  Serial.begin(115200);
  // I2S 配置结构体
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  // I2S 引脚配置结构体
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_DATA_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  // 安装和启动 I2S 驱动程序
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
  i2s_set_clk(I2S_NUM, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}

void loop() {
  // 示例：播放简单的正弦波
  const int frequency = 440;
  const int amplitude = 30000;
  const int sampleRate = 44100;
  const int samplesPerCycle = sampleRate / frequency;
  int16_t samples[samplesPerCycle];

  for (int i = 0; i < samplesPerCycle; ++i) {
    samples[i] = amplitude * sin(2 * PI * i / samplesPerCycle);
  }

  while (true) {
    size_t bytesWritten;
    i2s_write(I2S_NUM, samples, sizeof(samples), &bytesWritten, portMAX_DELAY);
  }
}
