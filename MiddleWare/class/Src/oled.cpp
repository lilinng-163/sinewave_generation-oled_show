#include <iostream>
#include <cstdio>
#include "main.h"
#include "oled.hpp"
#include "i2c.hpp"
#include "oled_font.hpp"

int oled::write_cmd(unsigned char cmd)
{
    i2c.start();
    i2c.send_byte(0x78);
    i2c.receive_ack();
    i2c.send_byte(0x00);
    i2c.receive_ack();
    i2c.send_byte(cmd);
    i2c.receive_ack();
    i2c.stop();

    return 0;
}
int oled::write_data(unsigned char data)
{
    i2c.start();
    i2c.send_byte(0x78);
    i2c.receive_ack();
    i2c.send_byte(0x40);
    i2c.receive_ack();
    i2c.send_byte(data);
    i2c.receive_ack();
    i2c.stop();

    return 0;
}
int oled::setcursor(unsigned char x, unsigned char y)
{
    write_cmd(0xB0 | y);
    write_cmd(0x10 | ((x & 0xF0) >> 4));
    write_cmd(0x00 | (x & 0x0F));

    return 0;
}
oled::oled(GPIO_TypeDef* port, uint16_t scl, uint16_t sda) : i2c(port, scl, sda)
{
	write_cmd(0xAE);	//关闭显示
	
	write_cmd(0xD5);	//设置显示时钟分频比/振荡器频率
	write_cmd(0x80);
	
	write_cmd(0xA8);	//设置多路复用率
	write_cmd(0x3F);
	
	write_cmd(0xD3);	//设置显示偏移
	write_cmd(0x00);
	
	write_cmd(0x40);	//设置显示开始行
	
	write_cmd(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	write_cmd(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	write_cmd(0xDA);	//设置COM引脚硬件配置
	write_cmd(0x12);
	
	write_cmd(0x81);	//设置对比度控制
	write_cmd(0xCF);

	write_cmd(0xD9);	//设置预充电周期
	write_cmd(0xF1);

	write_cmd(0xDB);	//设置VCOMH取消选择级别
	write_cmd(0x30);

	write_cmd(0xA4);	//设置整个显示打开/关闭

	write_cmd(0xA6);	//设置正常/倒转显示

	write_cmd(0x8D);	//设置充电泵
	write_cmd(0x14);

	write_cmd(0xAF);	//开启显示

    clear();
}
int oled::get_id(void)
{
    //以后实现
    return 0;
}
int oled::set_pixel(unsigned char x, unsigned char y)
{
    if(x > 127 || y > 63) 
    {
        return 0;
    }

    unsigned short idx = (y / 8) * 128 + x;
    buffer[idx] |= (1 << (y % 8));

    return 0;
}
int oled::clear_pixel(unsigned char x, unsigned char y)
{
    if(x > 127 || y > 63) return 0;

    // OLED 帧缓冲排布: buffer[1024]
    //
    //   每页 128 字节, 共 8 页 (128列 × 8页 = 1024字节)
    //
    //   页 0: buffer[  0 ~ 127]   ←  行  0 ~  7
    //   页 1: buffer[128 ~ 255]   ←  行  8 ~ 15
    //   页 2: buffer[256 ~ 383]   ←  行 16 ~ 23
    //   页 3: buffer[384 ~ 511]   ←  行 24 ~ 31
    //   页 4: buffer[512 ~ 639]   ←  行 32 ~ 39
    //   页 5: buffer[640 ~ 767]   ←  行 40 ~ 47
    //   页 6: buffer[768 ~ 895]   ←  行 48 ~ 55
    //   页 7: buffer[896 ~ 1023]  ←  行 56 ~ 63
    //
    //   每列一个字节, 字节内 bit 0=最上行, bit 7=最下行
    //
    //   例: (x=10, y=35)
    //     页号 = y / 8 = 35 / 8 = 4 → buffer[512~639]
    //   列号 = x = 10
    //     索引 = 页号*128 + 列号 = 4*128 + 10 = 522 → buffer[522]
    //     位   = y % 8 = 3 → bit 3
    //     buffer[522] |= (1 << 3)  置第 3 位为 1

    unsigned short page  = y / 8;          // 所在页 (0~7)
    unsigned short col   = x;              // 所在列 (0~127)
    unsigned short bit   = 1 << (y % 8);   // 页内行号对应的 bit 掩码
    unsigned short idx   = page * 128 + col;

    buffer[idx] |= bit;
    return 0;
}
int oled::draw_wave(const uint16_t *data, unsigned char len)
{
    unsigned char n = (len > 128) ? 128 : len;
    for(unsigned char x = 0; x < n; x++)
    {
        unsigned char y = 63 - (data[x] * 64 / 4096);
        set_pixel(x, y);
    }
    return 0;
}
int oled::refresh(void)
{
    for(auto page = 0; page < 8; page++)
    {
        setcursor(0, page);
        for(auto col = 0; col < 128; col++)
            write_data(buffer[page * 128 + col]);
    }
    return 0;
}
int oled::clear(void)
{
    for(auto i = 0; i < 1024; i++)
        buffer[i] = 0x00;
    
    refresh();
    return 0;
}

int oled::show_num(int num, int x, int y)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", num);
    return show_string(buf, x, y);
}

int oled::show_string(std::string str, int x, int y)
{
    for(size_t i = 0; i < str.length(); i++)
    {
        auto idx = str[i] - 32;
        setcursor(x + i * 8, y);
        for(auto j = 0; j < 8; j++)
            write_data(OLED_F8x16[idx][j]);

        setcursor(x + i * 8, y + 1);
        for(auto j = 8; j < 16; j++)
            write_data(OLED_F8x16[idx][j]);
    }
    return 0;
}