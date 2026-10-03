#include "oled.h"
#include "oledfont.h"

// 声明在 main.c 里写的 DWT 微秒延时函数
extern void delay_us(uint32_t us);

#define OLED_ADDR 0x78 // 0x3C << 1，如果屏幕不亮，改成 0x7A 试试

// ==========================================
// 1. 软件 I2C 底层时序 (全部加 static，私有化)
// ==========================================
static void I2C_Delay(void) {
    delay_us(2); // 8MHz 下，精确延时 2 微秒，屏幕不亮可改成 5
}

static void I2C_Start(void) {
    SDA_H(); SCL_H(); I2C_Delay();
    SDA_L(); I2C_Delay(); // SCL 高时拉低 SDA 为起始信号
    SCL_L(); I2C_Delay();
}

static void I2C_Stop(void) {
    SDA_L(); SCL_H(); I2C_Delay();
    SDA_H(); I2C_Delay(); // SCL 高时拉高 SDA 为停止信号
}

static void I2C_SendByte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        SCL_L();
        if (byte & 0x80) SDA_H(); else SDA_L(); // 发送最高位
        byte <<= 1;
        I2C_Delay();
        SCL_H(); I2C_Delay(); // 拉高 SCL，从机读取数据
    }
    SCL_L(); I2C_Delay();
}

uint8_t I2C_WaitAck(void) {
    uint8_t ack;
    uint32_t timeout = 0;
    SDA_H(); // 释放 SDA
    I2C_Delay();
    SCL_H(); I2C_Delay();
    while (SDA_READ() == 1) {
           timeout++;
           if (timeout > 5000) { // 超时强制退出，防止卡死
               I2C_Stop();
               return 1; // 返回 1 表示没收到应答
           }
        }
    
    // 读到了 0，说明从机应答了
    ack = SDA_READ(); 
    SCL_L();
    I2C_Delay();
    return ack;
}

// ==========================================
// 2. OLED 协议层 (加 static)
// ==========================================
static void OLED_WriteCmd(uint8_t cmd) {
    I2C_Start();
    I2C_SendByte(OLED_ADDR); I2C_WaitAck();
    I2C_SendByte(0x00); I2C_WaitAck(); // 0x00 代表命令
    I2C_SendByte(cmd); I2C_WaitAck();
    I2C_Stop();
}

void OLED_WriteData(uint8_t data) {
    I2C_Start();
    I2C_SendByte(OLED_ADDR); I2C_WaitAck();
    I2C_SendByte(0x40); I2C_WaitAck(); // 0x40 代表数据
    I2C_SendByte(data); I2C_WaitAck();
    I2C_Stop();
}

void OLED_SetCursor(uint8_t x, uint8_t y) {
    OLED_WriteCmd(0xB0 + y); // 设置页地址（0-7）
    OLED_WriteCmd(0x00 + (x & 0x0F)); // 列低4位
    OLED_WriteCmd(0x10 + ((x >> 4) & 0x0F)); // 列高4位
}

// ==========================================
// 3. OLED 对外接口 (不加 static，在 oled.h 里声明)
// ==========================================
void OLED_Init(void) {
    HAL_Delay(200); // 上电等屏幕准备好
    
    OLED_WriteCmd(0xAE); // 关显示
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14); // 电荷泵（必须开，不然不亮）
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x02); // 页寻址模式
    OLED_WriteCmd(0xA1); // 左右方向
    OLED_WriteCmd(0xC8); // 上下方向
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x40);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xAF); // 开显示
    
    OLED_Clear();
}

void OLED_Clear(void) {
    for (uint8_t i = 0; i < 8; i++) {
        OLED_SetCursor(0, i);
        for (uint8_t n = 0; n < 128; n++) {
            OLED_WriteData(0x00);
        }
    }
}

// 显示 8x16 ASCII 字符 (适配你的二维数组)
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr) {
    uint8_t c = chr - ' '; // 计算偏移
    
    // 上半页 (8列)
    OLED_SetCursor(x, y);
    for (uint8_t i = 0; i < 8; i++) {
        OLED_WriteData(F8X16[c][i]); // 二维数组读取
    }
    
    // 下半页 (8列)
    OLED_SetCursor(x, y + 1);
    for (uint8_t i = 0; i < 8; i++) {
        OLED_WriteData(F8X16[c][i + 8]); // 二维数组读取
    }
}

// 显示 16x16 汉字/方块
void OLED_ShowChinese(uint8_t x, uint8_t y, const uint8_t *hz) {
    OLED_SetCursor(x, y);
    for (uint8_t i = 0; i < 16; i++) {
        OLED_WriteData(hz[i * 2]); // 上半页
    }
    OLED_SetCursor(x, y + 1);
    for (uint8_t i = 0; i < 16; i++) {
        OLED_WriteData(hz[i * 2 + 1]); // 下半页
    }
}

// 拼凑法显示纯英文/数字字符串
void OLED_ShowString(uint8_t x, uint8_t y, const char *str) {
    while (*str != '\0') {
        OLED_ShowChar(x, y, *str);
        x += 8; // 英文宽 8 像素
        str++;
        if (x > 120) { // 简单换行
            x = 0;
            y += 2;
        }
    }
}
