
#include <Arduino.h>
#include <driver/i2s.h>
#include <SPIFFS.h>

// I2S 引脚定义
#define I2S_MIC_BCLK      7
#define I2S_MIC_LRCK      6
#define I2S_MIC_SD        5
#define I2S_SPK_BCLK      17
#define I2S_SPK_LRCK      16
#define I2S_SPK_SD        18

#define RECORD_TIME 10 // 录音时间 (秒)
#define FILE_NAME "/recording.raw" // 保存文件名

// I2S 配置
i2s_config_t i2s_mic_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
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

i2s_config_t i2s_spk_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
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

// I2S 引脚配置
i2s_pin_config_t i2s_mic_pin_config = {
  .bck_io_num = I2S_MIC_BCLK,
  .ws_io_num = I2S_MIC_LRCK,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = I2S_MIC_SD
};

i2s_pin_config_t i2s_spk_pin_config = {
  .bck_io_num = I2S_SPK_BCLK,
  .ws_io_num = I2S_SPK_LRCK,
  .data_out_num = I2S_SPK_SD,
  .data_in_num = I2S_PIN_NO_CHANGE
};

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin())
  {
  	// 初始化失败时处理
    Serial.println("SPIFFS-An error occurred while mounting SPIFFS");

    // 格式化SPIFFS分区
    if (SPIFFS.format())
    {
      // 格式化成功
      Serial.println("SPIFFS partition formatted successfully");
      // 重启
      ESP.restart();
    }
    else
    {
      Serial.println("SPIFFS partition format failed");
    }
    return;
  }

  // // 初始化 SPIFFS
  // if(!SPIFFS.begin(true)) {
  //   Serial.println("SPIFFS 初始化失败");
  //   return;
  // }

  // 安装并启动 I2S 驱动
  i2s_driver_install(I2S_NUM_0, &i2s_mic_config, 0, NULL);
  i2s_driver_install(I2S_NUM_1, &i2s_spk_config, 0, NULL);
  
  // 设置麦克风 I2S 引脚
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pin_config);

  // 设置扬声器 I2S 引脚
  i2s_set_pin(I2S_NUM_1, &i2s_spk_pin_config);

  // 配置麦克风 I2S 时钟
  i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  // 配置扬声器 I2S 时钟
  i2s_set_clk(I2S_NUM_1, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  // 录制音频
  recordAudio();
  // 播放录制的音频
  playAudio();
}

void recordAudio() {
  File file = SPIFFS.open(FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.println("无法打开文件进行写入");
    return;
  }

  const int bufferSize = 1024;
  int16_t i2s_buffer[bufferSize];
  size_t bytes_read;

  unsigned long startMillis = millis();
  while (millis() - startMillis < RECORD_TIME * 1000) {
    // 从麦克风读取数据
    i2s_read(I2S_NUM_0, i2s_buffer, bufferSize, &bytes_read, portMAX_DELAY);
    // 将数据写入文件
    file.write((const uint8_t*)i2s_buffer, bytes_read);

    // 让出CPU时间
    // vTaskDelay(1);  // 添加这一行
  }

  file.close();
  Serial.println("录音完成");
}

void playAudio() {
  File file = SPIFFS.open(FILE_NAME, FILE_READ);
  if (!file) {
    Serial.println("无法打开文件进行读取");
    return;
  }

  const int bufferSize = 1024;
  int16_t i2s_buffer[bufferSize];
  size_t bytes_written;

  while (file.available()) {
    // 从文件读取数据
    int bytesRead = file.read((uint8_t*)i2s_buffer, bufferSize);
    // 将数据写入扬声器
    i2s_write(I2S_NUM_1, i2s_buffer, bytesRead, &bytes_written, portMAX_DELAY);

    // 让出CPU时间
    // vTaskDelay(1);  // 添加这一行
  }

  file.close();
  Serial.println("播放完成");
}

void loop() {
  // 空循环
}
