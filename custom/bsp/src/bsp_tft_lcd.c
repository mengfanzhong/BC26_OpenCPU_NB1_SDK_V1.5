/*
*********************************************************************************************************
*
*	模块名称 : TFT液晶显示器驱动模块
*	文件名称 : bsp_tft_lcd.c
*	版    本 : V1.0
*	说    明 : 支持1.54寸显示模块.
*			  支持的LCD内部驱动芯片型号有: st3375
*	修改记录 :
*		版本号  日期        作者    说明
*       v1.0   2020-04-29 孟凡忠  正式发布
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "fonts.h"

#define PINNAME_LCD_CS		PINNAME_SPI_CS
#define PINNAME_LCD_DC   	PINNAME_GPIO3
#define PINNAME_LCD_RES  	PINNAME_GPIO4
/* PWM背光引脚 */
#define PINNAME_LCD_PWM  	PINNAME_NETLIGHT


/* 下面3个变量，主要用于使程序同时支持不同的屏 */
u32 g_ChipID 	= IC_ST3375;		/* 驱动芯片ID */
u16 g_LcdHeight = 240;			    /* 显示屏分辨率-高度 */
u16 g_LcdWidth 	= 240;			    /* 显示屏分辨率-宽度 */

u8  s_ucBright;						/* 背光亮度参数 */
u8  g_LcdDirection;					/* 显示方向.0，1，2，3 */

// u8  FrameBuff[240*240*2];			/* 帧缓冲区 */
void *pFrameBuff = NULL;			/* 帧缓冲区指针 */

//注意函数内定义大数组时有长度的限制,定义为256没什么问题,但定义为512就不行,这样的话就要把数组定义为全局变量或定义为静态变量才行
//u8  a_dot[DOT_NUM];			  	/* 只暂存储48号汉字点阵数据 */

static void LCD_CtrlLinesConfig(void);
static void LCD_HardReset(void);

static void time_delay(u32 cnt);

void LCD_PWMBackLight_Init(void);

void LCD_SetPwmBackLight(u8 _bright);

void LCD_SetDirection(u8 _dir);

void LCD_GetChipDescribe(char *_str);

/*
*********************************************************************************************************
*	函 数 名: time_delay
*	功能说明: 延时函数。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void time_delay(u32 cnt)
{
    u32 i = 0;
    u32 j = 0; 
    for(i=0;i<cnt;i++)
        for(j=0;j<1000000;j++);  
}

/*
*********************************************************************************************************
*	函 数 名: LCD_InitHard
*	功能说明: 初始化LCD
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitHard(void)
{
//	u32 id;

	/* 配置LCD控制口线GPIO */
	LCD_CtrlLinesConfig();

	/* 配置SPI接口 */
//	LCD_SPIConfig();					/* 已经在bsp_spi_bus中初始化SPI接口了 */        

	LCD_HardReset();					/* 硬件复位，针对其他GPIO控制LCD复位的产品  */

//	id = ST3375_ReadID();				/* 注意SPI接口的屏没有SOMI引脚,所以不能读ID */
//	g_ChipID = IC_ST3375;				/* 无法读取芯片ID,默认为ST3375 */

	/* 申请一个LCD显示缓冲区 */
    if (pFrameBuff != NULL)
        {
            Ql_MEM_Free(pFrameBuff);
            pFrameBuff = NULL;
        }
    pFrameBuff = Ql_MEM_Alloc(240*240*2);
	if(pFrameBuff == NULL)
	{
		while(1);   /* 锁死 */
	}

	ST3375_InitHard();

	LCD_SetDirection(0);    			/* 设置LCD屏方向 */

	LCD_ClrScr(CL_BLACK);				/* 清屏，显示全黑 */
       
	LCD_DispOn();						/* lcd内部显示开 */

	LCD_SetBackLight(BRIGHT_DEFAULT);	 /* 打开背光，设置为缺省亮度 */

}

/*
*********************************************************************************************************
*	函 数 名: LCD_HardReset
*	功能说明: 硬件复位. 针对复位口线由GPIO控制的产品。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_HardReset(void)
{	
	/* 复位LCD */
	LCD_SetRES(0);
	time_delay(10000000);		/* 延时 */
	LCD_SetRES(1);

}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetChipDescribe
*	功能说明: 读取LCD驱动芯片的描述符号，用于显示
*	形    参: char *_str : 描述符字符串填入此缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_GetChipDescribe(char *_str)
{
	switch (g_ChipID)
	{
		case IC_ST3375:
			strcpy(_str, CHIP_STR_ST3375);
			break;
		default:
			strcpy(_str, "Unknow");
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetHeight
*	功能说明: 读取LCD分辨率之高度
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
u16 LCD_GetHeight(void)
{
	return g_LcdHeight;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetWidth
*	功能说明: 读取LCD分辨率之宽度
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
u16 LCD_GetWidth(void)
{
	return g_LcdWidth;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DispOn
*	功能说明: 打开显示
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispOn(void)
{
	if (g_ChipID == IC_ST3375)
	{
		ST3375_DispOn();
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DispOff
*	功能说明: 关闭显示
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispOff(void)
{
	if (g_ChipID == IC_ST3375)
	{
		ST3375_DispOff();
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_ClrScr
*	功能说明: 根据输入的颜色值清屏
*	形    参: _usColor : 背景色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_ClrScr(u16 _usColor)
{
    if (g_ChipID == IC_ST3375)
	{
		ST3375_ClrScr(_usColor);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DispStr
*	功能说明: 在LCD指定坐标（左上角）显示一个字符串
*	形    参:
*		_usX : X坐标
*		_usY : Y坐标
*		_ptr  : 字符串指针
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispStr(u16 _usX, u16 _usY, char *_ptr, FONT_T *_tFont)
{
	LCD_DispStrEx(_usX, _usY, _ptr, _tFont, 0, 0);
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DispStr_Align
*	功能说明: 在LCD指定坐标（左上角）显示一个字符串,带排例参数
*	形    参:
*		_usX :   X坐标
*		_usY :   Y坐标
*		_ptr  :  字符串指针
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*		_Width : 字符串显示区域的宽度. 0 表示不处理留白区域，此时_Align无效
*		_Align : 字符串在显示区域的对齐方式，
*				 ALIGN_LEFT = 0,
*				 ALIGN_CENTER = 1,
*				 ALIGN_RIGHT = 2
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispStr_Align(u16 _usX, u16 _usY, char *_ptr, FONT_T *_tFont, u16 _Width,u8 _Align)
{
	LCD_DispStrEx(_usX, _usY, _ptr, _tFont, _Width, _Align);
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetFontWidth
*	功能说明: 读取字体的宽度（像素单位)
*	形    参:
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 字体的宽度（像素单位)
*********************************************************************************************************
*/
u16 LCD_GetFontWidth(FONT_T *_tFont)
{
	u16 font_width = 16;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:
			font_width = 12;
			break;

		case FC_ST_16:			
			font_width = 16;
			break;
					
		case FC_ST_24:
			font_width = 24;
			break;
			
		case FC_ST_32:	
			font_width = 32;
			break;
		
		case FC_ST_40:
            font_width = 40;
			break;

   		case FC_ST_48:
            font_width = 48;
			break;  		
	}
	return font_width;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetFontHeight
*	功能说明: 读取字体的高度（像素单位)
*	形    参:
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 字体的宽度（像素单位)
*********************************************************************************************************
*/
u16 LCD_GetFontHeight(FONT_T *_tFont)
{
	u16 height = 16;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:
			height = 12;
			break;

		case FC_ST_16:			
			height = 16;
			break;
						
		case FC_ST_24:
			height = 24;
			break;
			
		case FC_ST_32:	
			height = 32;
			break;
		
		case FC_ST_40:
            height = 40;
			break;

   		case FC_ST_48:
            height = 48;
			break; 		
	}
	return height;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetStrWidth
*	功能说明: 计算字符串宽度(像素单位)
*	形    参:
*		_ptr  : 字符串指针
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 无
*********************************************************************************************************
*/
u16 LCD_GetStrWidth(char *_ptr, FONT_T *_tFont)
{
	char *p = _ptr;
	u16 width = 0;
	u8 code1, code2;
	u16 font_width;

	font_width = LCD_GetFontWidth(_tFont);

	while (*p != 0)
	{
		code1 = *p;	/* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
		if (code1 < 0x80)	/* ASCII */
		{
			switch(_tFont->FontCode)
			{
				case FC_ST_12:
					font_width = 6;
					break;

				case FC_ST_16:		
					font_width = 8;
					break;
					
				case FC_ST_24:			
					font_width = 12;
					break;
					
				case FC_ST_32:
					font_width = 16;
					break;
				
				default:
					font_width = 8;
					break;					
			}
			
		}
		else	/* 汉字 */
		{
			code2 = *++p;
			if (code2 == 0)
			{
				break;
			}
			font_width = LCD_GetFontWidth(_tFont);
			
		}
		width += font_width;
		p++;
	}

	return width;
}

/*
*********************************************************************************************************
*	函 数 名: _LCD_ReadAsciiDot
*	功能说明: 读取1个ASCII字符的点阵数据
*	形    参:
*		_code : ASCII字符的编码，1字节。1-128
*		_fontcode ：字体代码
*		_pBuf : 存放读出的字符点阵数据
*	返 回 值: 文字宽度
*********************************************************************************************************
*/
static void _LCD_ReadAsciiDot(u8 _code, u8 _fontcode, u8 *_pBuf)
{
	const u8 *pAscDot;
	u16 font_bytes = 0;

	pAscDot = 0;
	switch (_fontcode)
	{

		case FC_ST_12:				/* 12点阵 */
			/* 默认为16点阵 ,如果实际使用到12点阵要修改 */
			font_bytes = 16;
			pAscDot = ascii_8x16;
			break;
		
		case FC_ST_16:		        /* 16点阵 */
			font_bytes = 16;
			pAscDot = ascii_8x16;	
			break;
        
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = ascii_12x24;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = ascii_16x32;
			break;
        
		case FC_ST_40:
			font_bytes = 120;
			pAscDot = ascii_20x40;
			break;

   		case FC_ST_48:
			font_bytes = 288;
			pAscDot = ascii_24x48;
			break;
	}	

	/* 将CPU内部Flash中的ascii字符点阵复制到buf */
//	memcpy(_pBuf, &pAscDot[_code * (font_bytes / 2)], (font_bytes / 2));
	memcpy(_pBuf, &pAscDot[(_code-0x20) * font_bytes], font_bytes);	
}

/*
*********************************************************************************************************
*	函 数 名: _LCD_ReadHZDot
*	功能说明: 读取1个汉字的点阵数据
*	形    参:
*		_code1, _cod2 : 汉字内码. GB2312编码
*		_fontcode ：字体代码
*		_pBuf : 存放读出的字符点阵数据
*	返 回 值: 无
*********************************************************************************************************
*/
static void _LCD_ReadHZDot(u8 _code1, u8 _code2,  u8 _fontcode, u8 *_pBuf)
{
#ifdef USE_SMALL_FONT	/* 使用CPU 内部Flash 小字库 */
		u8 *pDot;
		u8 font_bytes = 0;
		u32 address;
		u16 m;

		pDot = 0;	/* 仅仅用于避免告警 */
		switch (_fontcode)
		{
			case FC_ST_16:
				font_bytes = 32;
				pDot = (u8 *)g_Hz16;
				break;
	
			case FC_ST_24:
				font_bytes = 72;
				pDot = (u8 *)g_Hz24;
				break;			
				
			case FC_ST_32:	
				font_bytes = 128;
				pDot = (u8 *)g_Hz32;
				break;						
			
//			case FC_RA8875_16:
//			case FC_RA8875_24:
//			case FC_RA8875_32:
//				return;
		}	

		m = 0;
		while(1)
		{
			address = m * (font_bytes + 2);
			m++;
			if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
			{
				address += 2;
				memcpy(_pBuf, &pDot[address], font_bytes);
				break;
			}
			else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
			{
				/* 字库搜索完毕，未找到，则填充全FF */
				memset(_pBuf, 0xFF, font_bytes);
				break;
			}
		}
#else	/* 用全字库 */
		u32 offset = 0;

		u16 font_bytes = 0;
			
		switch (_fontcode)
		{
//			case FC_ST_12:		/* 12点阵 */
//				font_bytes = 24;
//				pDot = (u8 *)HZK12_ADDR;	
//				break;
			
			case FC_ST_16:
				font_bytes = 32;
				offset = FONT_16_OFFSET;					    //字库中地址偏移量;
				break;
	
			case FC_ST_24:
				font_bytes = 72;
				offset = FONT_24_OFFSET;
				break;			
				
			case FC_ST_32:	
				font_bytes = 128;
				offset = FONT_32_OFFSET;
				break;						
            case FC_ST_40:
            case FC_ST_48:  
				return;
		}			
	
//		/* 此处需要根据字库文件存放位置进行修改 */
//		if (_code1 >=0xA1 && _code1 <= 0xA9 && _code2 >=0xA1)
//		{
//			pDot += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
//		}
//		else if (_code1 >=0xB0 && _code1 <= 0xF7 && _code2 >=0xA1)
//		{
//			pDot += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
//		}
//		memcpy(_pBuf, pDot, font_bytes);

	if(_code2 < 0x7f)
			_code2 -= 0x40;
	else
			_code2 -= 0x41;
	_code1 -= 0x81;
	sf_ReadBuffer(_pBuf,(offset+(190*(_code1)+(_code2))*font_bytes),font_bytes);  //GBK字库解码        
        
#endif
}
			
/*
*********************************************************************************************************
*	函 数 名: LCD_DispStrEx
*	功能说明: 在LCD指定坐标（左上角）显示一个字符串。 增强型函数。支持左\中\右对齐，支持定长清屏。
*	形    参:
*		_usX : X坐标
*		_usY : Y坐标
*		_ptr  : 字符串指针
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数。可以指定RA8875字库显示汉字
*		_Width : 字符串显示区域的宽度. 0 表示不处理留白区域，此时_Align无效
*		_Align :字符串在显示区域的对齐方式，
*				ALIGN_LEFT = 0,
*				ALIGN_CENTER = 1,
*				ALIGN_RIGHT = 2
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispStrEx(u16 _usX, u16 _usY, char *_ptr, FONT_T *_tFont, u16 _Width,
	u8 _Align)
{
	u32 i;
	u8 code1;
	u8 code2;
	u8 buf[32 * 32 / 8];		/* 最大支持32点阵汉字 */
	u8 width;
	u16 m;
	u8 font_width = 0;
	u8 font_height = 0;
	u16 x, y;
	u16 offset;
	u16 str_width;				/* 字符串实际宽度 */
	u16 dis_width=0;			/* 已经显示的字符宽度,用于显示字符时超过允许的宽度(_Width)就不再显示了  */
	u8 row_bytes = 1; 			/*在扫描点阵时每一行所占用的字节数,用于显示时跳到下一个数据位置 */

	switch (_tFont->FontCode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_height = 12;
			font_width = 12;
			break;
		
		case FC_ST_16:
			font_height = 16;
			font_width = 16;
			break;

		case FC_ST_24:
			font_height = 24;
			font_width = 24;
			break;
						
		case FC_ST_32:	
			font_height = 32;
			font_width = 32;
			break;
		
		case FC_ST_40:
			font_height =40;
			font_width = 40;
			break;	
        
		case FC_ST_48:  
			font_height =48;
			font_width = 48;
			break;	
		
//		case FC_RA8875_16:
//			ra8875_font_code = RA_FONT_16;
//			ra8875_use = 1;	/* 表示用RA8875字库 */
//			break;
//			
//		case FC_RA8875_24:
//			ra8875_font_code = RA_FONT_24;
//			ra8875_use = 1;	/* 表示用RA8875字库 */
//			break;
//						
//		case FC_RA8875_32:
//			ra8875_font_code = RA_FONT_32;
//			ra8875_use = 1;	/* 表示用RA8875字库 */
//			break;
		
		default:
			return;
	}
	
	str_width = LCD_GetStrWidth(_ptr, _tFont);	/* 计算字符串实际宽度(RA8875内部ASCII点阵宽度为变长 */
	offset = 0;
	if (_Width > str_width)
	{
		if (_Align == ALIGN_RIGHT)	/* 右对齐 */
		{
			offset = _Width - str_width;
		}
		else if (_Align == ALIGN_CENTER)	/* 左对齐 */
		{
			offset = (_Width - str_width) / 2;
		}
		else	/* 左对齐 ALIGN_LEFT */
		{
			;
		}
	}

	/* 左侧填背景色, 中间对齐和右边对齐  */
	if (offset > 0)
	{
        if (_tFont->BackColor != CL_MASK)	/* 透明色 */
        {
            LCD_Fill_Rect(_usX, _usY, LCD_GetFontHeight(_tFont), offset,  _tFont->BackColor);
        }
		_usX += offset;
	}
	
	/* 右侧填背景色 */
	if (_Width > str_width)
	{
        if (_tFont->BackColor != CL_MASK)	/* 透明色 */
        {
            LCD_Fill_Rect(_usX + str_width, _usY, LCD_GetFontHeight(_tFont), _Width - str_width - offset,  _tFont->BackColor);
        }
	}
	
//	if (ra8875_use == 1)	/* 使用RA8875外挂的字库芯片 */
//	{
//		RA8875_SetFrontColor(_tFont->FrontColor);			/* 设置字体前景色 */
//		RA8875_SetBackColor(_tFont->BackColor);				/* 设置字体背景色 */
//		RA8875_SetFont(ra8875_font_code, 0, _tFont->Space);	/* 字体代码，行间距，字间距 */
//		RA8875_DispStr(_usX, _usY, _ptr);
//	}
//	else	/* 使用CPU内部字库. 点阵信息由CPU读取 */
	{
		/* 开始循环处理字符 */
		while (*_ptr != 0)
		{
			if(_Width)							/* 在允许显示宽度不为0时才去检查 */
			{
				if(dis_width >= _Width)return;	/*如果已显示宽度超过允许宽度时,退出 */
			}
			code1 = *_ptr;	/* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
			if (code1 < 0x80)
			{
				/* 将ascii字符点阵复制到buf */
				//memcpy(buf, &pAscDot[code1 * (font_bytes / 2)], (font_bytes / 2));
				_LCD_ReadAsciiDot(code1, _tFont->FontCode, buf);	/* 读取ASCII字符点阵 */
				width = font_width / 2;
                if(width % 8)                                 //显示宽度
                {
                    row_bytes = (width/8)+1;                  //显示一行数据占用的字节数,如24号英文字符12X24 一行就占用两个字节
                }
                else
                {
                    row_bytes = width/8;     
                }
			}
			else
			{
				code2 = *++_ptr;
				if (code2 == 0)
				{
					break;
				}
				/* 读1个汉字的点阵 */
				_LCD_ReadHZDot(code1, code2, _tFont->FontCode, buf);
				width = font_width;
				
				row_bytes = width/8;//显示汉字时都是整除的,所以直接除以8,得到一行的字节数
			}
	
			y = _usY;
			/* 开始刷LCD */
			for (m = 0; m < font_height; m++)	/* 字符高度 */
			{
				x = _usX;
				for (i = 0; i < width; i++)	/* 字符宽度 */
				{
#if 0               
					if ((buf[m * ((2 * width) / font_width) + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
					{
						LCD_PutPixel(x, y, _tFont->FrontColor);	/* 设置像素颜色为文字色 */
					}
					else
					{
						if (_tFont->BackColor != CL_MASK)	/* 透明色 */
						{
							LCD_PutPixel(x, y, _tFont->BackColor);	/* 设置像素颜色为文字背景色 */
						}
					}
#else               //如果使用自己做的字库就要用下面的代码才能正常显示大于16号字体的字符串
                    if ((buf[m * (row_bytes) + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
                    {
                        LCD_PutPixel(x, y, _tFont->FrontColor);/* 设置像素颜色为文字色 */
                    }
                    else
                    {
                        if (_tFont->BackColor != CL_MASK)	/* 透明色 */
                        {
                            LCD_PutPixel(x, y, _tFont->BackColor);	/* 设置像素颜色为文字背景色 */
                        }
                    }
#endif                    
					x++;
				}
				y++;
			}
	
			if (_tFont->Space > 0)
			{
				/* 如果文字底色按_tFont->usBackColor，并且字间距大于点阵的宽度，那么需要在文字之间填充(暂时未实现) */
			}
			_usX += width + _tFont->Space;	/* 列地址递增 */
			dis_width += width + _tFont->Space;		/*实际已经显示了多少像素宽度 */			
			_ptr++;			/* 指向下一个字符 */
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_PutPixel
*	功能说明: 画1个像素
*	形    参:
*			_usX,_usY : 像素坐标
*			_usColor  : 像素颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_PutPixel(u16 _usX, u16 _usY, u16 _usColor)
{
	if (g_ChipID == IC_ST3375)
	{
		ST3375_PutPixel(_usX, _usY, _usColor);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetPixel
*	功能说明: 读取1个像素
*	形    参:
*			_usX,_usY : 像素坐标
*			_usColor  : 像素颜色
*	返 回 值: RGB颜色值
*********************************************************************************************************
*/
u16 LCD_GetPixel(u16 _usX, u16 _usY)
{
	u16 usRGB = 0;
	/* SPI接口液晶屏无法读取像素 */
	// if (g_ChipID == IC_ST3375)
	// {
	// 	usRGB = ST3375_GetPixel(_usX, _usY);
	// }

	return usRGB;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawLine
*	功能说明: 采用 Bresenham 算法，在2点间画一条直线。
*	形    参:
*			_usX1, _usY1 : 起始点坐标
*			_usX2, _usY2 : 终止点Y坐标
*			_usColor     : 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawLine(u16 _usX1 , u16 _usY1 , u16 _usX2 , u16 _usY2 , u16 _usColor)
{
	if (g_ChipID == IC_ST3375)
	{
		ST3375_DrawLine(_usX1 , _usY1 , _usX2, _usY2 , _usColor);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawPoints
*	功能说明: 采用 Bresenham 算法，绘制一组点，并将这些点连接起来。可用于波形显示。
*	形    参:
*			x, y     : 坐标数组
*			_usColor : 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void LCD_DrawPoints(u16 *x, u16 *y, u16 _usSize, u16 _usColor)
{
	u16 i;

	for (i = 0 ; i < _usSize - 1; i++)
	{
		LCD_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
	}
}
#endif

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawRect
*	功能说明: 绘制水平放置的矩形。
*	形    参:
*			_usX,_usY: 矩形左上角的坐标
*			_usHeight : 矩形的高度
*			_usWidth  : 矩形的宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawRect(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u16 _usColor)
{
	if (g_ChipID == IC_ST3375)
	{
		ST3375_DrawRect( _usX, _usY,  _usHeight, _usWidth, _usColor);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_Fill_Rect
*	功能说明: 用一个颜色值填充一个矩形。【emWin 中有同名函数 LCD_FillRect，因此加了下划线区分】
*	形    参:
*			_usX,_usY: 矩形左上角的坐标
*			_usHeight : 矩形的高度
*			_usWidth  : 矩形的宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Fill_Rect(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u16 _usColor)
{
	if (g_ChipID == IC_ST3375)
	{
		ST3375_FillRect( _usX, _usY, _usHeight, _usWidth, _usColor );
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawCircle
*	功能说明: 绘制一个圆，笔宽为1个像素
*	形    参:
*			_usX,_usY  : 圆心的坐标
*			_usRadius  : 圆的半径
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawCircle(u16 _usX, u16 _usY, u16 _usRadius, u16 _usColor)
{
    if (g_ChipID == IC_ST3375)
	{
		ST3375_DrawCircle( _usX, _usY, _usRadius, _usColor);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawBMP
*	功能说明: 在LCD上显示一个BMP位图，位图点阵扫描次序: 从左到右，从上到下
*	形    参:
*			_usX, _usY : 图片的坐标
*			_usHeight  : 图片高度
*			_usWidth   : 图片宽度
*			_ptr       : 图片点阵指针
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void LCD_DrawBMP(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u16 *_ptr)
{
	if (g_ChipID == IC_8875)
	{
		RA8875_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
	}
	else if (g_ChipID == IC_9488)
	{
		ILI9488_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
	}
	else
	{
		SPFD5420_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
	}
}
#endif

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawWin
*	功能说明: 在LCD上绘制一个窗口
*	形    参: 结构体指针
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawWin(WIN_T *_pWin)
{
#if 1
	u16 TitleHegiht;

    switch (_pWin->Font->FontCode) //标题栏字体大小
    {   //设置标题栏高度
        case FC_ST_16:
            TitleHegiht = 20; 
            break;
        case FC_ST_24:
            TitleHegiht = 28;             
            break;
        case FC_ST_32:
            TitleHegiht = 36;  
            break;
        case FC_ST_40:
            TitleHegiht = 44;  
            break;
        case FC_ST_48:
            TitleHegiht = 52;  
            break;        
        default:
            TitleHegiht = 20; 
            break;
    }

	/* 绘制窗口外框 */
	LCD_DrawRect(_pWin->Left, _pWin->Top, _pWin->Height, _pWin->Width, WIN_BORDER_COLOR);
	LCD_DrawRect(_pWin->Left + 1, _pWin->Top + 1, _pWin->Height - 2, _pWin->Width - 2, WIN_BORDER_COLOR);

	/* 窗口标题栏 */
	LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + 2, TitleHegiht, _pWin->Width - 4, WIN_TITLE_COLOR);

	/* 窗体填充 */
	LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + TitleHegiht + 2, _pWin->Height - 4 - TitleHegiht,
		_pWin->Width - 4, WIN_BODY_COLOR);

	LCD_DispStr(_pWin->Left + 3, _pWin->Top + 2, _pWin->pCaption, _pWin->Font);
#else
	if (g_ChipID == IC_8875)
	{
		u16 TitleHegiht;

		TitleHegiht = 28;

		/* 绘制窗口外框 */
		RA8875_DrawRect(_pWin->Left, _pWin->Top, _pWin->Height, _pWin->Width, WIN_BORDER_COLOR);
		RA8875_DrawRect(_pWin->Left + 1, _pWin->Top + 1, _pWin->Height - 2, _pWin->Width - 2, WIN_BORDER_COLOR);

		/* 窗口标题栏 */
		RA8875_FillRect(_pWin->Left + 2, _pWin->Top + 2, TitleHegiht, _pWin->Width - 4, WIN_TITLE_COLOR);

		/* 窗体填充 */
		RA8875_FillRect(_pWin->Left + 2, _pWin->Top + TitleHegiht + 2, _pWin->Height - 4 - TitleHegiht, _pWin->Width - 4, WIN_BODY_COLOR);

		//RA8875_SetFont(_pWin->Font.FontCode, 0, 0);
		RA8875_SetFont(RA_FONT_24, 0, 0);

		RA8875_SetBackColor(WIN_TITLE_COLOR);
		RA8875_SetFrontColor(WIN_CAPTION_COLOR);
		RA8875_DispStr(_pWin->Left + 3, _pWin->Top + 2, _pWin->Caption);
	}
	else
	{
		;
	}
#endif
}


/*
*********************************************************************************************************
*	函 数 名: LCD_DrawIcon
*	功能说明: 在LCD上绘制一个图标，四角自动切为弧脚
*	形    参: _pIcon : 图标结构
*			  _tFont : 字体属性
*			  _ucFocusMode : 焦点模式。0 表示正常图标  1表示选中的图标
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, u8 _ucFocusMode)
{
	const u16 *p;
	u16 usNewRGB;
	u16 x, y;		/* 用于记录窗口内的相对坐标 */

	p = _tIcon->pBmp;
	for (y = 0; y < _tIcon->Height; y++)
	{
		for (x = 0; x < _tIcon->Width; x++)
		{
			usNewRGB = *p++;	/* 读取图标的颜色值后指针加1 */
			/* 将图标的4个直角切割为弧角，弧角外是背景图标 */
			if ((y == 0 && (x < 6 || x > _tIcon->Width - 7)) ||
				(y == 1 && (x < 4 || x > _tIcon->Width - 5)) ||
				(y == 2 && (x < 3 || x > _tIcon->Width - 4)) ||
				(y == 3 && (x < 2 || x > _tIcon->Width - 3)) ||
				(y == 4 && (x < 1 || x > _tIcon->Width - 2)) ||
				(y == 5 && (x < 1 || x > _tIcon->Width - 2))	||

				(y == _tIcon->Height - 1 && (x < 6 || x > _tIcon->Width - 7)) ||
				(y == _tIcon->Height - 2 && (x < 4 || x > _tIcon->Width - 5)) ||
				(y == _tIcon->Height - 3 && (x < 3 || x > _tIcon->Width - 4)) ||
				(y == _tIcon->Height - 4 && (x < 2 || x > _tIcon->Width - 3)) ||
				(y == _tIcon->Height - 5 && (x < 1 || x > _tIcon->Width - 2)) ||
				(y == _tIcon->Height - 6 && (x < 1 || x > _tIcon->Width - 2))
				)
			{
				;
			}
			else
			{
				if (_ucFocusMode != 0)	/* 1表示选中的图标 */
				{
					/* 降低原始像素的亮度，实现图标被激活选中的效果 */
					u16 R,G,B;
					u16 bright = 15;

					/* rrrr rggg gggb bbbb */
					R = (usNewRGB & 0xF800) >> 11;
					G = (usNewRGB & 0x07E0) >> 5;
					B =  usNewRGB & 0x001F;
					if (R > bright)
					{
						R -= bright;
					}
					else
					{
						R = 0;
					}
					if (G > 2 * bright)
					{
						G -= 2 * bright;
					}
					else
					{
						G = 0;
					}
					if (B > bright)
					{
						B -= bright;
					}
					else
					{
						B = 0;
					}
					usNewRGB = (R << 11) + (G << 5) + B;
				}

				LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
			}
		}
	}

	/* 绘制图标下的文字 */
	{
		u16 len;
		u16 width;

		len = strlen(_tIcon->Text);

		if  (len == 0)
		{
			return;	/* 如果图标文本长度为0，则不显示 */
		}

		/* 计算文本的总宽度 */
		if (_tFont->FontCode == FC_ST_12)		/* 12点阵 */
		{
			width = 6 * (len + _tFont->Space);
		}
		else	/* FC_ST_16 */
		{
			width = 8 * (len + _tFont->Space);
		}


		/* 水平居中 */
		x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
		y = _tIcon->Top + _tIcon->Height + 2;
		LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
	}
}
#endif
/*
*********************************************************************************************************
*	函 数 名: LCD_Blend565
*	功能说明: 对像素透明化 颜色混合
*	形    参: src : 原始像素
*			  dst : 混合的颜色
*			  alpha : 透明度 0-32
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
u16 LCD_Blend565(u16 src, u16 dst, u8 alpha)
{
	u32 src2;
	u32 dst2;

	src2 = ((src << 16) |src) & 0x07E0F81F;
	dst2 = ((dst << 16) | dst) & 0x07E0F81F;
	dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
	return (dst2 >> 16) | dst2;
}
#endif
/*
*********************************************************************************************************
*	函 数 名: LCD_DrawIcon32
*	功能说明: 在LCD上绘制一个图标, 带有透明信息的位图(32位， RGBA). 图标下带文字
*	形    参: _pIcon : 图标结构
*			  _tFont : 字体属性
*			  _ucFocusMode : 焦点模式。0 表示正常图标  1表示选中的图标
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void LCD_DrawIcon32(const ICON_T *_tIcon, FONT_T *_tFont, u8 _ucFocusMode)
{
	const u8 *p;
	u16 usOldRGB, usNewRGB;
	int16_t x, y;		/* 用于记录窗口内的相对坐标 */
	u8 R1,G1,B1,A;	/* 新像素色彩分量 */
	u8 R0,G0,B0;	/* 旧像素色彩分量 */

	p = (const u8 *)_tIcon->pBmp;
	p += 54;		/* 直接指向图像数据区 */

	/* 按照BMP位图次序，从左至右，从上至下扫描 */
	for (y = _tIcon->Height - 1; y >= 0; y--)
	{
		for (x = 0; x < _tIcon->Width; x++)
		{
			B1 = *p++;
			G1 = *p++;
			R1 = *p++;
			A = *p++;	/* Alpha 值(透明度)，0-255, 0表示透明，1表示不透明, 中间值表示透明度 */

			if (A == 0x00)	/* 需要透明,显示背景 */
			{
				;	/* 不用刷新背景 */
			}
			else if (A == 0xFF)	/* 完全不透明， 显示新像素 */
			{
				usNewRGB = RGB(R1, G1, B1);
				if (_ucFocusMode == 1)
				{
					usNewRGB = LCD_Blend565(usNewRGB, CL_YELLOW, 10);
				}
				LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
			}
			else 	/* 半透明 */
			{
				/* 计算公式： 实际显示颜色 = 前景颜色 * Alpha / 255 + 背景颜色 * (255-Alpha) / 255 */
				usOldRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
				//usOldRGB = 0xFFFF;
				R0 = RGB565_R(usOldRGB);
				G0 = RGB565_G(usOldRGB);
				B0 = RGB565_B(usOldRGB);

				R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
				G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
				B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
				usNewRGB = RGB(R1, G1, B1);
				if (_ucFocusMode == 1)
				{
					usNewRGB = LCD_Blend565(usNewRGB, CL_YELLOW, 10);
				}
				LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
			}
		}
	}

	/* 绘制图标下的文字 */
	{
		u16 len;
		u16 width;

		len = strlen(_tIcon->Text);

		if  (len == 0)
		{
			return;	/* 如果图标文本长度为0，则不显示 */
		}

		/* 计算文本的总宽度 */
		if (_tFont->FontCode == FC_ST_12)		/* 12点阵 */
		{
			width = 6 * (len + _tFont->Space);
		}
		else	/* FC_ST_16 */
		{
			width = 8 * (len + _tFont->Space);
		}


		/* 水平居中 */
		x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
		y = _tIcon->Top + _tIcon->Height + 2;
		LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
	}
}
#endif
/*
*********************************************************************************************************
*	函 数 名: LCD_DrawBmp32
*	功能说明: 在LCD上绘制一个32位的BMP图, 带有透明信息的位图(32位， RGBA)
*	形    参: _usX, _usY : 显示坐标
*			  _usHeight, _usWidth : 图片高度和宽度
*			  _pBmp : 图片数据（带BMP文件头）
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void LCD_DrawBmp32(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u8 *_pBmp)
{
	const u8 *p;
	u16 usOldRGB, usNewRGB;
	int16_t x, y;		/* 用于记录窗口内的相对坐标 */
	u8 R1,G1,B1,A;	/* 新像素色彩分量 */
	u8 R0,G0,B0;	/* 旧像素色彩分量 */

	p = (const u8 *)_pBmp;
	p += 54;		/* 直接指向图像数据区 */

	/* 按照BMP位图次序，从左至右，从上至下扫描 */
	for (y = _usHeight - 1; y >= 0; y--)
	{
		for (x = 0; x < _usWidth; x++)
		{
			B1 = *p++;
			G1 = *p++;
			R1 = *p++;
			A = *p++;	/* Alpha 值(透明度)，0-255, 0表示透明，1表示不透明, 中间值表示透明度 */

			if (A == 0x00)	/* 需要透明,显示背景 */
			{
				;	/* 不用刷新背景 */
			}
			else if (A == 0xFF)	/* 完全不透明， 显示新像素 */
			{
				usNewRGB = RGB(R1, G1, B1);
				//if (_ucFocusMode == 1)
				//{
				//	usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
				//}
				LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
			}
			else 	/* 半透明 */
			{
				/* 计算公式： 实际显示颜色 = 前景颜色 * Alpha / 255 + 背景颜色 * (255-Alpha) / 255 */
				usOldRGB = LCD_GetPixel(x + _usX, y + _usY);
				R0 = RGB565_R(usOldRGB);
				G0 = RGB565_G(usOldRGB);
				B0 = RGB565_B(usOldRGB);

				R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
				G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
				B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
				usNewRGB = RGB(R1, G1, B1);
				//if (_ucFocusMode == 1)
				//{
				//	usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
				//}
				LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
			}
		}
	}
}
#endif
/*
*********************************************************************************************************
*	函 数 名: LCD_DrawLabel
*	功能说明: 绘制一个文本标签
*	形    参: 结构体指针
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawLabel(LABEL_T *_pLabel)
{
#if 1
	char dispbuf[256];
	u16 i;
	u16 NewLen;

	NewLen = strlen(_pLabel->pCaption);

	if (NewLen > _pLabel->MaxLen)
	{
		LCD_DispStr(_pLabel->Left, _pLabel->Top, _pLabel->pCaption, _pLabel->Font);
		_pLabel->MaxLen = NewLen;
	}
	else
	{
		for (i = 0; i < NewLen; i++)
		{
			dispbuf[i] = _pLabel->pCaption[i];
		}
		for (; i < _pLabel->MaxLen; i++)
		{
			dispbuf[i] = ' ';		/* 末尾填充空格 */
		}
		dispbuf[i] = 0;
		LCD_DispStr(_pLabel->Left, _pLabel->Top, dispbuf, _pLabel->Font);
	}
#else
	if (g_ChipID == IC_8875)
	{
		RA8875_SetFont(_pLabel->Font->FontCode, 0, 0);	/* 设置32点阵字体，行间距=0，字间距=0 */

		RA8875_SetBackColor(_pLabel->Font->BackColor);
		RA8875_SetFrontColor(_pLabel->Font->FrontColor);

		RA8875_DispStr(_pLabel->Left, _pLabel->Top, _pLabel->Caption);
	}
	else
	{

	}
#endif
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawCheckBox
*	功能说明: 绘制一个检查框
*	形    参: 结构体指针
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawCheckBox(CHECK_T *_pCheckBox)
{
#if 1
	u16 width,x, y;
	/* 目前只做了16点阵汉字的大小 */
	  
	/* 绘制外框 */
	x = _pCheckBox->Left;
	LCD_DrawRect(x, _pCheckBox->Top, CHECK_BOX_H, CHECK_BOX_W, CHECK_BOX_BORDER_COLOR);
	LCD_DrawRect(x + 1, _pCheckBox->Top + 1, CHECK_BOX_H - 2, CHECK_BOX_W - 2, CHECK_BOX_BORDER_COLOR);
	LCD_Fill_Rect(x + 2, _pCheckBox->Top + 2, CHECK_BOX_H - 4, CHECK_BOX_W - 4, CHECK_BOX_BACK_COLOR);

     switch (_pCheckBox->Font->FontCode) //字体宽度
    {  
        case FC_ST_16:
            width = 8; 
            break;
        case FC_ST_24:
            width = 12;             
            break;
        case FC_ST_32:
            width = 16;  
            break;
        case FC_ST_40:
            width = 20;  
            break;
        case FC_ST_48:
            width = 24;  
            break;        
        default:
            width = 8; 
            break;
    } 
	/* 绘制文本标签 */
	x = _pCheckBox->Left + CHECK_BOX_W + 2;
	y = _pCheckBox->Top + CHECK_BOX_H / 2 - width;
	LCD_DispStr(x, y, _pCheckBox->pCaption, _pCheckBox->Font);

	if (_pCheckBox->Checked)
	{
		FONT_T font;

	    font.FontCode = FC_ST_16;
		font.BackColor = CL_MASK;
		font.FrontColor = CHECK_BOX_CHECKED_COLOR;	/* 钩的颜色 */
		font.Space = 0;
		x = _pCheckBox->Left;
		LCD_DispStr(x + 3, _pCheckBox->Top + 3, "√", &font);
	}
#else
	if (g_ChipID == IC_8875)
	{
		u16 x;

		RA8875_SetFont(_pCheckBox->Font.FontCode, 0, 0);	/* 设置32点阵字体，行间距=0，字间距=0 */

		/* 绘制标签 */
		//RA8875_SetBackColor(_pCheckBox->Font.BackColor);
		RA8875_SetBackColor(WIN_BODY_COLOR);
		RA8875_SetFrontColor(_pCheckBox->Font.FrontColor);
		RA8875_DispStr(_pCheckBox->Left, _pCheckBox->Top, _pCheckBox->Caption);

		/* 绘制外框 */
		x = _pCheckBox->Left + _pCheckBox->Width - CHECK_BOX_W;
		RA8875_DrawRect(x, _pCheckBox->Top, CHECK_BOX_H, CHECK_BOX_W, CHECK_BOX_BORDER_COLOR);
		RA8875_DrawRect(x + 1, _pCheckBox->Top + 1, CHECK_BOX_H - 2, CHECK_BOX_W - 2, CHECK_BOX_BORDER_COLOR);
		RA8875_FillRect(x + 2, _pCheckBox->Top + 2, CHECK_BOX_H - 4, CHECK_BOX_W - 4, CHECK_BOX_BACK_COLOR);

		if (_pCheckBox->Checked)
		{
			RA8875_SetBackColor(CHECK_BOX_BACK_COLOR);
			RA8875_SetFrontColor(CL_RED);
			RA8875_DispStr(x + 3, _pCheckBox->Top + 3, "√");
		}
	}
	else
	{

	}
#endif

}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawEdit
*	功能说明: 在LCD上绘制一个编辑框
*	形    参: _pEdit 编辑框结构体指针
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawEdit(EDIT_T *_pEdit)
{
#if 1
	u16    /*len, */x, y;
	u8     width;
    u16    EditWidth;

	/* 仿XP风格，平面编辑框 */
	LCD_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR);
	LCD_Fill_Rect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2,EDIT_BACK_COLOR);
   

	/* 文字居中 */
    switch (_pEdit->Font->FontCode) //字体宽度
    {  
        case FC_ST_16:
            width = 8; 
            break;
        case FC_ST_24:
            width = 12;             
            break;
        case FC_ST_32:
            width = 16;  
            break;
        case FC_ST_40:
            width = 20;  
            break;
        case FC_ST_48:
            width = 24;  
            break;        
        default:
            width = 8; 
            break;
    }
    
//	len = strlen(_pEdit->pCaption);
//	x = _pEdit->Left +  (_pEdit->Width - len * width) / 2;
    x = _pEdit->Left+2;  //留出2个点的距离,使文字与编辑框不会太近                         
	y = _pEdit->Top + _pEdit->Height / 2 - width;

    EditWidth = _pEdit->Width - 4; //编辑框的宽度要减去留出的两边的间距
    
//	LCD_DispStr(x, y, _pEdit->pCaption, _pEdit->Font);
    /*显示字符串*/
    LCD_DispStr_Align(x,y,_pEdit->pCaption,_pEdit->Font,EditWidth,_pEdit->Align);
#else
	if (g_ChipID == IC_8875)
	{
		u16 len, x;

		/* 仿XP风格，平面编辑框 */
		RA8875_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR);
		RA8875_FillRect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BACK_COLOR);

		RA8875_SetFont(_pEdit->Font.FontCode, 0, 0);	/* 设置32点阵字体，行间距=0，字间距=0 */
		RA8875_SetFrontColor(_pEdit->Font.FrontColor);
		RA8875_SetBackColor(EDIT_BACK_COLOR);

		/* 文字居中 */
		len = strlen(_pEdit->Caption);
		x = (_pEdit->Width - len * 16) / 2;

		RA8875_DispStr(_pEdit->Left + x, _pEdit->Top + 3, _pEdit->Caption);
	}
	else
	{
		//SPFD5420_DrawCircle(_usX, _usY, _usRadius, _usColor);
	}
#endif
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawEdit
*	功能说明: 在LCD上绘制一个编辑框
*	形    参:
*			_usX, _usY : 图片的坐标
*			_usHeight  : 图片高度
*			_usWidth   : 图片宽度
*			_ptr       : 图片点阵指针
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawButton(BUTTON_T *_pBtn)
{
#if 1
	u16 x, y, h;
	FONT_T font;	/* 按钮激活时，需要更改字体设置，因此需要一个变量来保存 */

	font.FontCode = _pBtn->Font->FontCode;
	font.FrontColor = _pBtn->Font->FrontColor;
	font.BackColor = _pBtn->Font->BackColor;
	font.Space = _pBtn->Font->Space;	
			
	if (_pBtn->Focus == 1)
	{
		font.BackColor = BUTTON_ACTIVE_COLOR;
	}
	else
	{
		/* 按钮的背景色统一管理，不能单独指定 */
		font.BackColor = BUTTON_BACK_COLOR;
	}
	
	/* 仿XP风格，平面编辑框 */
	LCD_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
	LCD_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
	LCD_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);

	h =  LCD_GetFontHeight(&font);
	x = _pBtn->Left + 3;
	y = _pBtn->Top + _pBtn->Height / 2 - h / 2;		
	
	LCD_Fill_Rect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, font.BackColor);	/* 选中后的底色 */
	LCD_DispStrEx(x, y, _pBtn->pCaption, &font, _pBtn->Width - 6, ALIGN_CENTER);	/* 水平居中 */	
#else
	if (g_ChipID == IC_8875)
	{
		u16 len, x, y;

		if (_pBtn->Focus == 1)
		{
			/* 仿XP风格，平面编辑框 */
			RA8875_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
			RA8875_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
			RA8875_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);

			RA8875_FillRect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, BUTTON_ACTIVE_COLOR);

			RA8875_SetBackColor(BUTTON_ACTIVE_COLOR);
		}
		else
		{
			/* 仿XP风格，平面编辑框 */
			RA8875_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
			RA8875_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
			RA8875_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);

			RA8875_FillRect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, BUTTON_BACK_COLOR);

			RA8875_SetBackColor(BUTTON_BACK_COLOR);
		}

		#if 1	/* 按钮文字字体和颜色固定 */
			if (strcmp(_pBtn->Caption, "←") == 0)	/* 退格键特殊处理 */
			{
				/* 退格键符号是单像素笔画，太细了不协调，因此特殊处理 */
				RA8875_SetFont(RA_FONT_16, 0, 0);
				RA8875_SetFrontColor(CL_BLACK);
				RA8875_SetTextZoom(RA_SIZE_X2, RA_SIZE_X2);	/* 放大2倍 */
			}
			else
			{
				RA8875_SetFont(RA_FONT_16, 0, 0);
				RA8875_SetFrontColor(CL_BLACK);
				RA8875_SetTextZoom(RA_SIZE_X1, RA_SIZE_X1);	/* 放大1倍 */
			}
		#else	/* 按钮文字字体和颜色有应用程序指定 */
			RA8875_SetFont(_pBtn->Font.FontCode, 0, 0);
			RA8875_SetFrontColor(_pBtn->Font.FrontColor);
		#endif

		/* 文字居中 */
		len = strlen(_pBtn->Caption);

		/* 此处统计不等宽字符有问题。暂时特殊处理下 */
		if (len != 3)
		{
			x = _pBtn->Left + (_pBtn->Width - len * 16) / 2;
		}
		else
		{
			x = _pBtn->Left + (_pBtn->Width - len * 20) / 2;
		}

		/* 对特殊字符特殊处理 */
		if ((len == 1) && (_pBtn->Caption[0] == '.'))
		{
			y = _pBtn->Top + 3;
			x += 3;
		}
		else
		{
			y = _pBtn->Top + 3;
		}

		RA8875_DispStr(x, y, _pBtn->Caption);

		RA8875_SetTextZoom(RA_SIZE_X1, RA_SIZE_X1);	/* 还原放大1倍 */
	}
#endif
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawGroupBox
*	功能说明: 在LCD上绘制一个分组框
*	形    参: _pBox 分组框
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawGroupBox(GROUP_T *_pBox)
{
	u16 x, y;

	/* 画阴影线 */
	LCD_DrawRect(_pBox->Left + 1, _pBox->Top + 5, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER2);

	/* 画主框线 */
	LCD_DrawRect(_pBox->Left, _pBox->Top + 4, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER1);

	/* 显示分组框标题（文字在左上角） */
	x = _pBox->Left + 9;
	y = _pBox->Top;
	LCD_DispStr(x, y, _pBox->pCaption, _pBox->Font);
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DispControl
*	功能说明: 绘制控件
*	形    参: _pControl 控件指针
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispControl(void *_pControl)
{
	u8 id;

	id = *(u8 *)_pControl;	/* 读取ID */

	switch (id)
	{
		case ID_ICON:
			//void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, u8 _ucFocusMode);
			break;

		case ID_WIN:
			LCD_DrawWin((WIN_T *)_pControl);
			break;

		case ID_LABEL:
			LCD_DrawLabel((LABEL_T *)_pControl);
			break;

		case ID_BUTTON:
			LCD_DrawButton((BUTTON_T *)_pControl);
			break;

		case ID_CHECK:
			LCD_DrawCheckBox((CHECK_T *)_pControl);
			break;

		case ID_EDIT:
			LCD_DrawEdit((EDIT_T *)_pControl);
			break;

		case ID_GROUP:
			LCD_DrawGroupBox((GROUP_T *)_pControl);
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_CtrlLinesConfig
*	功能说明: 配置LCD控制口线
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void LCD_CtrlLinesConfig(void)
{
	/* 配置CS(片选)引脚 (输出,上拉,高电平) */
	Ql_GPIO_Init(PINNAME_LCD_CS,PINDIRECTION_OUT,PINLEVEL_HIGH,PINPULLSEL_PULLUP);

	/* 配置DC(数据/命令)GPIP口 */
	Ql_GPIO_Init(PINNAME_LCD_DC,PINDIRECTION_OUT,PINLEVEL_HIGH,PINPULLSEL_PULLUP);

	/* 配置RES(复位)GPIP口 */
	Ql_GPIO_Init(PINNAME_LCD_RES,PINDIRECTION_OUT,PINLEVEL_LOW,PINPULLSEL_PULLUP);

#ifdef BRIGHT_CNL_PWM
	/* 初始化背光引脚 */
	LCD_PWMBackLight_Init();
#endif
}

/*
*********************************************************************************************************
*	函 数 名: LCD_SetCS
*	功能说明: 设置液晶屏CS引脚。
*	形    参: 无
	返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetCS(u8 _level)
{
	if (_level == 0)
	{
			/* 如果FLASH与tft液晶的SPI接口参数不一样时,要重新配置一下SPI接口 */
			// s32 Ql_SPI_Config(u32 chnnlNo, bool isHost, bool cpol, bool cpha, u32 clkSpeed);
        
		Ql_GPIO_SetLevel(PINNAME_LCD_CS,PINLEVEL_LOW);
	}
	else
	{
		Ql_GPIO_SetLevel(PINNAME_LCD_CS,PINLEVEL_HIGH);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_SetDC
*	功能说明: 设置液晶屏DC引脚。
*	形    参: 无
	返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetDC(u8 _level)
{
	if (_level == 0)
	{
		Ql_GPIO_SetLevel(PINNAME_LCD_DC,PINLEVEL_LOW);
	}
	else
	{
		Ql_GPIO_SetLevel(PINNAME_LCD_DC,PINLEVEL_HIGH);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_SetRES
*	功能说明: 设置液晶屏RES(复位)引脚。
*	形    参: 无
	返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetRES(u8 _level)
{
	if (_level == 0)
	{
		Ql_GPIO_SetLevel(PINNAME_LCD_RES,PINLEVEL_LOW);
	}
	else
	{
		Ql_GPIO_SetLevel(PINNAME_LCD_RES,PINLEVEL_HIGH);
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_SPIConfig
*	功能说明: 配置LCD SPI接口
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// static void LCD_SPIConfig(void)
// {
// /* 已经在bsp_spi_bus中初始化SPI接口了 */ 
// }

/*
*********************************************************************************************************
*	函 数 名: LCD_PWMBackLight_Init
*	功能说明: PWM背光初始化。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#ifdef BRIGHT_CNL_PWM

// #define PWM_SRC_CLK		 
// #define PWM_DIV
// #define PWM_LOW_PULSE_NUM
// #define PWM_HIGH_PULSE_NUM

void LCD_PWMBackLight_Init(void)
{

	/* 配置背光引脚 (输出,上拉,高电平) */
	Ql_GPIO_Init(PINNAME_LCD_PWM,PINDIRECTION_OUT,PINLEVEL_HIGH,PINPULLSEL_PULLUP);
	APP_DEBUG("\r\n<-- Initialize LCD_PWM pin (PINNAME_NETLIGHT): output, HIGH level, PULL up -->\r\n");

//	s32 ret;
	/* PWM frequency(频率)   	= (pwmSrcClk/pwmDiv)/(lowPulseNum+highPulseNum)	*/
	/* PWM Duty cycle(占空比) 	= highPulseNum/( lowPulseNum+highPulseNum )		*/
    // ret = Ql_PWM_Init(PINNAME_LCD_PWM,Ql_atoi(pwmSrcClk),Ql_atoi(pwmDiv),Ql_atoi(lowPulseNum),Ql_atoi(highPulseNum));
    // if(ret < 0 )
    // {
    //     APP_DEBUG("<--Init PWM faliure,pin=%d,pwmSrcClk=%d,pwmDiv=%d,lowPulseNum=%d,highPulseNum=%d,ret=%d-->\r\n",PinName,Ql_atoi(pwmSrcClk), Ql_atoi(pwmDiv), Ql_atoi(lowPulseNum),Ql_atoi(highPulseNum), ret);
    // }else
    // {
    // 	APP_DEBUG("<--Init PWM successfully,pin=%d,pwmSrcClk=%d,pwmDiv=%d,lowPulseNum=%d,highPulseNum=%d-->\r\n",PinName,Ql_atoi(pwmSrcClk), Ql_atoi(pwmDiv), Ql_atoi(lowPulseNum),Ql_atoi(highPulseNum));
	// }	
}
#endif

/*
*********************************************************************************************************
*	函 数 名: LCD_SetPwmBackLight
*	功能说明: 初始化控制LCD背景光的GPIO,配置为PWM模式。
*			当关闭背光时，将CPU IO设置为浮动输入模式（推荐设置为推挽输出，并驱动到低电平)；将TIM3关闭 省电
*	形    参:  _bright 亮度，0是灭，255是最亮
*	返 回 值: 无
*********************************************************************************************************
*/
#ifdef BRIGHT_CNL_PWM
void LCD_SetPwmBackLight(u8 _bright)
{
	if(!_bright)
	{
		Ql_GPIO_SetLevel(PINNAME_LCD_PWM,PINLEVEL_LOW);	
		APP_DEBUG("\r\n<-- PIN_LCD_PWM OUT_LOW -->\r\n");
	}
	else
	{
		/* 直接设置背光引脚为高电平,点亮LCD */
		Ql_GPIO_SetLevel(PINNAME_LCD_PWM,PINLEVEL_HIGH);
		APP_DEBUG("\r\n<-- PIN_LCD_PWM OUT_HIGH -->\r\n");		
	}
}
#endif

/*
*********************************************************************************************************
*	函 数 名: LCD_SetBackLight
*	功能说明: 初始化控制LCD背景光的GPIO,配置为PWM模式。
*			当关闭背光时，将CPU IO设置为浮动输入模式（推荐设置为推挽输出，并驱动到低电平)；将TIM3关闭 省电
*	形    参: _bright 亮度，0是灭，255是最亮
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetBackLight(u8 _bright)
{
	s_ucBright =  _bright;	/* 保存背光值 */
	
#ifdef BRIGHT_CNL_PWM

	LCD_SetPwmBackLight(_bright);

#else

	if ( _bright)
		LIGHT_ON;
	else
		LIGHT_OFF;
	
#endif
}
/*
*********************************************************************************************************
*	函 数 名: LCD_GetBackLight
*	功能说明: 获得背光亮度参数
*	形    参: 无
*	返 回 值: 背光亮度参数
*********************************************************************************************************
*/
u8 LCD_GetBackLight(void)
{
	return s_ucBright;
}
/*
*********************************************************************************************************
*	函 数 名: LCD_SetDirection
*	功能说明: 设置显示屏显示方向（横屏 竖屏）
*	形    参: 显示方向代码 0 横屏正常, 1=横屏180度翻转, 2=竖屏, 3=竖屏180度翻转
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetDirection(u8 _dir)
{
	g_LcdDirection =  _dir;		/* 保存在全局变量 */

    if (g_ChipID == IC_ST3375)
	{
		ST3375_SetDirection(_dir);
	}
}


/*
*********************************************************************************************************
*	函 数 名: LCD_ButtonTouchDown
*	功能说明: 判断按钮是否被按下. 检查触摸坐标是否在按钮的范围之内。并重绘按钮。
*	形    参:  _btn : 按钮对象
*			  _usX, _usY: 触摸坐标
*	返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
u8 LCD_ButtonTouchDown(BUTTON_T *_btn, u16 _usX, u16 _usY)
{
	if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
		&& (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
	{
		_btn->Focus = 1;
		LCD_DrawButton(_btn);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_ButtonTouchRelease
*	功能说明: 判断按钮是否被触摸释放. 并重绘按钮。在触摸释放事件中被调用。
*	形    参:  _btn : 按钮对象
*			  _usX, _usY: 触摸坐标
*	返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
u8 LCD_ButtonTouchRelease(BUTTON_T *_btn, u16 _usX, u16 _usY)
{
    /*因为触摸释放时有可以不在当前按钮上,所以都要重绘按钮*/
	_btn->Focus = 0;
	LCD_DrawButton(_btn);

	if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
		&& (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: LCD_InitButton
*	功能说明: 初始化按钮结构体成员。
*	形    参:  _x, _y : 坐标
*			  _h, _w : 高度和宽度
*			  _pCaption : 按钮文字
*			  _pFont : 按钮字体
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitButton(BUTTON_T *_btn, u16 _x, u16 _y, u16 _h, u16 _w, char *_pCaption, FONT_T *_pFont)
{
	_btn->Left = _x;
	_btn->Top = _y;
	_btn->Height = _h;
	_btn->Width = _w;
	_btn->pCaption = _pCaption;	
	_btn->Font = _pFont;
	_btn->Focus = 0;
}



/*******************************************************************/
/* 测试 */
void Test_FillRec(void)
{
		LCD_Fill_Rect(10,10,100,100,CL_BLUE);
}
/**************************************** 华宏软件 (END OF FILE) ****************************************/
