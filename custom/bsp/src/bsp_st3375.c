/*
*********************************************************************************************************
*
*	模块名称 : ST3375片驱动模块
*	文件名称 : LCD_ST3375.c
*	版    本 : V1.0
*	说    明 : ST3375底层驱动函数集。
*	修改记录 :
*		版本号  日期        作者    说明
*       v1.0   2020-04-29 孟凡忠  正式发布
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/

#include "bsp.h"


/* 延时函数 */
void TimeDelay(unsigned int i)
{
#if 0  /* 打开时bsp_InitDWT()函数要放在LCD_InitHard()函数之前才能使用 */ 
	bsp_DelayMS(i);			/* 使用硬件延时 */
#else
    unsigned int a;
    unsigned int b;
    for(b=0;b<i;b++)
    for(a=0;a<500;a++);	
#endif
}


/*
*********************************************************************************************************
*	函 数 名: ST3375_WR_REG
*	功能说明: 写入8位命令到LCD
*	形    参: data : 写入的命令
*	返 回 值: 无
*********************************************************************************************************
*/
static void ST3375_WR_REG(u8 data)
{ 
	/* 片选设置为低 */
	LCD_SetCS(0); 
	/* DC设置为低 写命令 */    
	LCD_SetDC(0);	
	/* 写入命令 */ 
	bsp_spiWrite(data); 
   /* 片选设置为高 */
   	LCD_SetCS(1);	
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_WR_DATA
*	功能说明: 写入8位数据到LCD
*	形    参: data : 写入的值
*	返 回 值: 无
*********************************************************************************************************
*/
static void ST3375_WR_DATA(u8 data)
{
	/* 片选设置为低 */
	LCD_SetCS(0); 
	/* DC设置为高,写数据 */    
	LCD_SetDC(1);	
	/* 写入命令 */ 
	bsp_spiWrite(data); 
   /* 片选设置为高 */
   	LCD_SetCS(1);
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_WriteReg
*	功能说明: 写数据到寄存器
*	形    参: LCD_Reg : 寄存器地址
*			  LCD_RegValue : 写入的值
*	返 回 值: 无
*********************************************************************************************************
*/
static void ST3375_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	ST3375_WR_REG(LCD_Reg);  
	ST3375_WR_DATA(LCD_RegValue);	    		 
}	   

/*
*********************************************************************************************************
*	函 数 名: ST3375_WriteRAM_Prepare
*	功能说明: 准备写RAM指令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/ 
static void ST3375_WriteRAM_Prepare(void)
{
	ST3375_WR_REG(0x2C);
}	 

/*
*********************************************************************************************************
*	函 数 名: ST3375_WriteData_16Bit
*	功能说明: 写入16位数据到LCD
*	形    参: Data: 写入的数据
*	返 回 值: 无
*********************************************************************************************************
*/ 
static void ST3375_WriteData_16Bit(u16 Data)
{	
	/* 片选设置为低 */
	LCD_SetCS(0); 
	/* DC设置为高,写数据 */     
	LCD_SetDC(1);	
	/* 写入数据 */ 
	bsp_spiWrite(Data>>8); 
	bsp_spiWrite(Data); 
   /* 片选设置为高 */
   	LCD_SetCS(1);
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_Set_windows
*	功能说明: 设置显示窗口
*	形    参: Data: 写入的数据
*	返 回 值: 无
*********************************************************************************************************
*/ 
void ST3375_Set_windows(u16 xs,u16 xe,u16 ys,u16 ye)
{
	u16 xoffset = 0, yoffset = 0;

	ST3375_WR_REG(0x2A);	
	ST3375_WR_DATA((xs+xoffset)>>8);
	ST3375_WR_DATA(xs+xoffset);		
	ST3375_WR_DATA((xe+xoffset)>>8);
	ST3375_WR_DATA(xe+xoffset);

	ST3375_WR_REG(0x2B);	
	ST3375_WR_DATA((ys+yoffset)>>8);
	ST3375_WR_DATA(ys+yoffset);		
	ST3375_WR_DATA((ye+yoffset)>>8);
	ST3375_WR_DATA(ye+yoffset);

	ST3375_WriteRAM_Prepare();			//开始写入GRAM	
}	

/*
*********************************************************************************************************
*	函 数 名: LCD_SetCursor
*	功能说明: 设置光标位置
*	形    参: Xpos,Ypos: 坐标
*	返 回 值: 无
*********************************************************************************************************
*/ 
void ST3375_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	ST3375_Set_windows(Xpos,Ypos,Xpos,Ypos);	
} 

/*
*********************************************************************************************************
*	函 数 名: ST3375_InitHard
*	功能说明: 初始化ST3375
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_InitHard(void)
{	
//************* ST7789初始化**********//
	APP_DEBUG("\r\n<-- Initialize ST3375  -->\r\n");

	ST3375_WR_REG(0x36); 
	ST3375_WR_DATA(0x00);

	ST3375_WR_REG(0x3A); 
	ST3375_WR_DATA(0x05);

	ST3375_WR_REG(0xB2);
	ST3375_WR_DATA(0x0C);
	ST3375_WR_DATA(0x0C);
	ST3375_WR_DATA(0x00);
	ST3375_WR_DATA(0x33);
	ST3375_WR_DATA(0x33);

	ST3375_WR_REG(0xB7); 
	ST3375_WR_DATA(0x35);  

	ST3375_WR_REG(0xBB);
	ST3375_WR_DATA(0x19);

	ST3375_WR_REG(0xC0);
	ST3375_WR_DATA(0x2C);

	ST3375_WR_REG(0xC2);
	ST3375_WR_DATA(0x01);

	ST3375_WR_REG(0xC3);
	ST3375_WR_DATA(0x12);   

	ST3375_WR_REG(0xC4);
	ST3375_WR_DATA(0x20);  

	ST3375_WR_REG(0xC6); 
	ST3375_WR_DATA(0x0F);    

	ST3375_WR_REG(0xD0); 
	ST3375_WR_DATA(0xA4);
	ST3375_WR_DATA(0xA1);

	ST3375_WR_REG(0xE0);
	ST3375_WR_DATA(0xD0);
	ST3375_WR_DATA(0x04);
	ST3375_WR_DATA(0x0D);
	ST3375_WR_DATA(0x11);
	ST3375_WR_DATA(0x13);
	ST3375_WR_DATA(0x2B);
	ST3375_WR_DATA(0x3F);
	ST3375_WR_DATA(0x54);
	ST3375_WR_DATA(0x4C);
	ST3375_WR_DATA(0x18);
	ST3375_WR_DATA(0x0D);
	ST3375_WR_DATA(0x0B);
	ST3375_WR_DATA(0x1F);
	ST3375_WR_DATA(0x23);

	ST3375_WR_REG(0xE1);
	ST3375_WR_DATA(0xD0);
	ST3375_WR_DATA(0x04);
	ST3375_WR_DATA(0x0C);
	ST3375_WR_DATA(0x11);
	ST3375_WR_DATA(0x13);
	ST3375_WR_DATA(0x2C);
	ST3375_WR_DATA(0x3F);
	ST3375_WR_DATA(0x44);
	ST3375_WR_DATA(0x51);
	ST3375_WR_DATA(0x2F);
	ST3375_WR_DATA(0x1F);
	ST3375_WR_DATA(0x1F);
	ST3375_WR_DATA(0x20);
	ST3375_WR_DATA(0x23);

	ST3375_WR_REG(0x21); 

	ST3375_WR_REG(0x11); 

//	TimeDelay(100000);

//	ST3375_WR_REG(0x29); 

	APP_DEBUG("\r\n<-- end Initialize ST3375  -->\r\n");		
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_SetDirection
*	功能说明: 设置显示屏显示方向（横屏 竖屏）
*	形    参: 显示方向代码 0 横屏正常, 1=横屏180度翻转, 2=竖屏, 3=竖屏180度翻转
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_SetDirection(u8 _dir)
{			
	// lcddev.setxcmd=0x2A;
	// lcddev.setycmd=0x2B;
	// lcddev.wramcmd=0x2C;
	switch(_dir)
	{		  
		case 0:						 	 		
			// lcddev.width=LCD_W;
			// lcddev.height=LCD_H;	
			// lcddev.xoffset=0;
			// lcddev.yoffset=0;
			ST3375_WriteReg(0x36,0);//BGR==1,MY==0,MX==0,MV==0
		break;
		case 2:
			// lcddev.width=LCD_H;
			// lcddev.height=LCD_W;
			// lcddev.xoffset=0;
			// lcddev.yoffset=0;
			ST3375_WriteReg(0x36,(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 1:						 	 		
	// 		lcddev.width=LCD_W;
	// 		lcddev.height=LCD_H;
    //   lcddev.xoffset=0;
	// 		lcddev.yoffset=80;			
			ST3375_WriteReg(0x36,(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			// lcddev.width=LCD_H;
			// lcddev.height=LCD_W;
			// lcddev.xoffset=80;
			// lcddev.yoffset=0;
			ST3375_WriteReg(0x36,(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_DispOn
*	功能说明: 打开显示
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_DispOn(void)
{
     ST3375_WR_REG(0x29);
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_DispOff
*	功能说明: 关闭显示
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_DispOff(void)
{
     ST3375_WR_REG(0x28);
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_PutPixel
*	功能说明: 画1个像素
*	形    参:
*			_usX,_usY : 像素坐标
*			_usColor  :像素颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_PutPixel(u16 _usX, u16 _usY, u16 _usColor)
{	
	ST3375_SetCursor(_usX,_usY);			//设置光标位置 
	ST3375_WriteData_16Bit(_usColor); 
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_ClrScr
*	功能说明: 根据输入的颜色值清屏
*	形    参:  _usColor : 背景色
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_ClrScr(u16 _usColor)
{ 
  	u16 i,m,temp, *pFBuf;

	pFBuf =  (u16 *)pFrameBuff;
	/* 填充帧缓冲区数据 */
	for(i=0;i<(g_LcdWidth * g_LcdHeight );i++)
	{
			/* 高低位交换 */
		temp = 0;
		temp = 	_usColor >> 8; 
		temp |= _usColor << 8; 

		Ql_memcpy(pFBuf,&temp,2);
		pFBuf++;
	}
	

	ST3375_Set_windows(0,g_LcdWidth-1,0,g_LcdHeight-1); 
	/* 片选设置为低 */
	LCD_SetCS(0); 
	/* DC设置为高  */    
	LCD_SetDC(1);


	Ql_SPI_Write(1,(u8 *)pFrameBuff,(g_LcdWidth * g_LcdHeight * 2));

// 	for(i=0;i<g_LcdHeight;i++)
// 	{
// 		for(m=0;m<g_LcdWidth;m++)
// 		{	
// 			/* 一次发送一帧数据 */
// 			Ql_SPI_Write(1,(u8 *)pFrameBuff,(g_LcdWidth * g_LcdHeight));

// //			Ql_SPI_Write(1,&temp,2);  /* 同时传输2个字节 刷屏时间 8秒*/

// 				/* 每次只传送一个字节,分两次发送 刷屏时间 17秒 */
// 			//  bsp_spiWrite(_usColor>>8); 
// 			//  bsp_spiWrite(_usColor);
// 		}
// 	}
   /* 片选设置为高 */
   	LCD_SetCS(1);
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_DrawLine
*	功能说明: 采用 Bresenham 算法，在2点间画一条直线。
*	形    参:  
*			_usX1, _usY1 ：起始点坐标
*			_usX2, _usY2 ：终止点Y坐标
*			_usColor     ：颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_DrawLine(u16 _usX1 , u16 _usY1 , u16 _usX2 , u16 _usY2 , u16 _usColor)
{
 	u16 x,y;
 	u16 dx;
 	u16 dy;

	if(_usY1==_usY2)
	{
		if(_usX1<=_usX2)
		{
			x=_usX1;
		}
		else
		{
			x=_usX2;
			_usX2=_usX1;
		}
		ST3375_Set_windows(x,_usX2,_usY1,_usY1);				//设置更新区域 
  		while(x <= _usX2)
  		{
				ST3375_WriteData_16Bit(_usColor);
   			x++;
  		}
  		return;
	}
	else if(_usY1>_usY2)
	{
		dy=_usY1-_usY2;
	}
	else
	{
		dy=_usY2-_usY1;
	}
 
 	if(_usX1==_usX2)
	{
		if(_usY1<=_usY2)
		{
			y=_usY1;
		}
		else
		{
			y=_usY2;
			_usY2=_usY1;
		}
		ST3375_Set_windows(_usX1,_usX1,y,_usY2);								//设置更新区域 
  		while(y <= _usY2)
  		{
			ST3375_WriteData_16Bit(_usColor);
   			y++;
  		}
  		return;
	}
	else if(_usX1 > _usX2)
 	{
		dx=_usX1-_usX2;
  		x = _usX2;
  		_usX2 = _usX1;
  		y = _usY2;
  		_usY2 = _usY1;
 	}
 	else
 	{
		dx=_usX2-_usX1;
  		x = _usX1;
  		y = _usY1;
 	}

 	if(dx == dy)
 	{
  		while(x <= _usX2)
  		{

   			x++;
			if(y>_usY2)
			{
				y--;
			}
			else
			{
   				y++;
			}
   			ST3375_PutPixel(x,y,_usColor);
  		}
 	}
 	else
 	{
 		ST3375_PutPixel(x, y, _usColor);
  		if(y < _usY2)
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
    			s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < _usX2)
    			{
     				x++;
     				if(p < 0)
     				{
      					p += twoDy;
     				}
     				else
     				{
      					y++;
      					p += twoDyMinusDx;
     				}
     				ST3375_PutPixel(x, y,_usColor);
    			}
   			}
   			else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
    			s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y < _usY2)
    			{
     				y++;
     				if(p < 0)
     				{
      					p += twoDx;
     				}
     				else
     				{
      					x++;
      					p+= twoDxMinusDy;
     				}
     				ST3375_PutPixel(x, y, _usColor);
    			}
   			}
  		}
  		else
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
	    		s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < _usX2)
    			{
     				x++;
     				if(p < 0)
	     			{
    	  				p += twoDy;
     				}
     				else
     				{
      					y--;
	      				p += twoDyMinusDx;
    	 			}
     				ST3375_PutPixel(x, y,_usColor);
    			}
   			}
	   		else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
	    		s16 twoDxMinusDy = 2 * (dx - dy);
    			while(_usY2 < y)
    			{
     				y--;
     				if(p < 0)
	     			{
    	  				p += twoDx;
     				}
     				else
     				{
      					x++;
	      				p+= twoDxMinusDy;
    	 			}
     				ST3375_PutPixel(x, y,_usColor);
    			}
   			}
  		}
 	}
    
    
 //使用下面这个显示慢   
//	int32_t dx , dy ;
//	int32_t tx , ty ;
//	int32_t inc1 , inc2 ;
//	int32_t d , iTag ;
//	int32_t x , y ;

//	/* 采用 Bresenham 算法，在2点间画一条直线 */

//	ST3375_PutPixel(_usX1 , _usY1 , _usColor);

//	/* 如果两点重合，结束后面的动作。*/
//	if ( _usX1 == _usX2 && _usY1 == _usY2 )
//	{
//		return;
//	}

//	iTag = 0 ;
//	/* dx = abs ( _usX2 - _usX1 ); */
//	if (_usX2 >= _usX1)
//	{
//		dx = _usX2 - _usX1;
//	}
//	else
//	{
//		dx = _usX1 - _usX2;
//	}

//	/* dy = abs ( _usY2 - _usY1 ); */
//	if (_usY2 >= _usY1)
//	{
//		dy = _usY2 - _usY1;
//	}
//	else
//	{
//		dy = _usY1 - _usY2;
//	}

//	if ( dx < dy )   /*如果dy为计长方向，则交换纵横坐标。*/
//	{
//		u16 temp;

//		iTag = 1 ;
//		temp = _usX1; _usX1 = _usY1; _usY1 = temp;
//		temp = _usX2; _usX2 = _usY2; _usY2 = temp;
//		temp = dx; dx = dy; dy = temp;
//	}
//	tx = _usX2 > _usX1 ? 1 : -1 ;    /* 确定是增1还是减1 */
//	ty = _usY2 > _usY1 ? 1 : -1 ;
//	x = _usX1 ;
//	y = _usY1 ;
//	inc1 = 2 * dy ;
//	inc2 = 2 * ( dy - dx );
//	d = inc1 - dx ;
//	while ( x != _usX2 )     /* 循环画点 */
//	{
//		if ( d < 0 )
//		{
//			d += inc1 ;
//		}
//		else
//		{
//			y += ty ;
//			d += inc2 ;
//		}
//		if ( iTag )
//		{
//			ST3375_PutPixel ( y , x , _usColor) ;
//		}
//		else
//		{
//			ST3375_PutPixel ( x , y , _usColor) ;
//		}
//		x += tx ;
//	}
}

/****************************************************************************
* 名    称：void ST3375_DispPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
            pic        图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：TFT_DispPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void ST3375_DispPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
{	
	u32 index=0;
	u32 totalpoint;

	totalpoint = (EndX - StartX)*(EndY - StartY) ; 			//得到总点数

	ST3375_Set_windows(StartX,EndX,StartY,EndY);			//设置更新区域 
	ST3375_WriteRAM_Prepare();     							//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		ST3375_WriteData_16Bit(*pic++);	   
	}
}


// /****************************************************************************
// * 名    称：void ST3375_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
// * 功    能：在指定座标画圆，可填充
// * 入口参数：
// * 出口参数：
// * 说    明：
// * 调用方法：
// ****************************************************************************/
// void ST3375_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
// {
// 	u16 x,y;
// 	s16 delta,tmp;
// 	x=0;
// 	y=r;
// 	delta=3-(r<<1);

// 	while(y>x)
// 	{
// 		if(fill)
// 		{
// 			ST3375_DrawLine(cx+x,cy+y,cx-x,cy+y,color);
// 			ST3375_DrawLine(cx+x,cy-y,cx-x,cy-y,color);
// 			ST3375_DrawLine(cx+y,cy+x,cx-y,cy+x,color);
// 			ST3375_DrawLine(cx+y,cy-x,cx-y,cy-x,color);
// 		}
// 		else
// 		{
// 			ST3375_PutPixel(cx+x,cy+y,color);
// 			ST3375_PutPixel(cx-x,cy+y,color);
// 			ST3375_PutPixel(cx+x,cy-y,color);
// 			ST3375_PutPixel(cx-x,cy-y,color);
// 			ST3375_PutPixel(cx+y,cy+x,color);
// 			ST3375_PutPixel(cx-y,cy+x,color);
// 			ST3375_PutPixel(cx+y,cy-x,color);
// 			ST3375_PutPixel(cx-y,cy-x,color);
// 		}
// 		x++;
// 		if(delta>=0)
// 		{
// 			y--;
// 			tmp=(x<<2);
// 			tmp-=(y<<2);
// 			delta+=(tmp+10);
// 		}
// 		else
// 		{
// 			delta+=((x<<2)+6);
// 		}
// 	}
// }

/*
*********************************************************************************************************
*	函 数 名: ST3375_DrawRect
*	功能说明: 绘制水平放置的矩形。
*	形    参:
*			_usX,_usY：矩形左上角的坐标
*			_usHeight ：矩形的高度
*			_usWidth  ：矩形的宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_DrawRect(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u16 _usColor)
{
	/*
	 ---------------->---
	|(_usX，_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/

	ST3375_DrawLine(_usX, _usY, _usX + _usWidth - 1, _usY, _usColor);									/* 顶 */
	ST3375_DrawLine(_usX, _usY + _usHeight - 1, _usX + _usWidth - 1, _usY + _usHeight - 1, _usColor);	/* 底 */

	ST3375_DrawLine(_usX, _usY, _usX, _usY + _usHeight - 1, _usColor);									/* 左 */
	ST3375_DrawLine(_usX + _usWidth - 1, _usY, _usX + _usWidth - 1, _usY + _usHeight, _usColor);		/* 右 */
}

/*
*********************************************************************************************************
*	函 数 名: ST3375_FillRect
*	功能说明: 填充矩形。
*	形    参:
*			_usX,_usY：矩形左上角的坐标
*			_usHeight ：矩形的高度
*			_usWidth  ：矩形的宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_FillRect(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u16 _usColor)
{
//	u32 i;

	/*
	 ---------------->---
	|(_usX，_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/

	// ILI9488_SetDispWin(_usX, _usY, _usHeight, _usWidth);

	// ILI9488_REG = 0x2C;
	// for (i = 0; i < _usHeight * _usWidth; i++)
	// {
	// 	ILI9488_RAM = _usColor;
	// }
}

/*
*********************************************************************************************************
*	函 数 名: ILI9488_DrawCircle
*	功能说明: 绘制一个圆，笔宽为1个像素
*	形    参:
*			_usX,_usY  ：圆心的坐标
*			_usRadius  ：圆的半径
*	返 回 值: 无
*********************************************************************************************************
*/
void ST3375_DrawCircle(u16 _usX, u16 _usY, u16 _usRadius, u16 _usColor)
{
	s32  D;			/* Decision Variable */
	u32  CurX;		/* 当前 X 值 */
	u32  CurY;		/* 当前 Y 值 */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{
		ST3375_PutPixel(_usX + CurX, _usY + CurY, _usColor);
		ST3375_PutPixel(_usX + CurX, _usY - CurY, _usColor);
		ST3375_PutPixel(_usX - CurX, _usY + CurY, _usColor);
		ST3375_PutPixel(_usX - CurX, _usY - CurY, _usColor);
		ST3375_PutPixel(_usX + CurY, _usY + CurX, _usColor);
		ST3375_PutPixel(_usX + CurY, _usY - CurX, _usColor);
		ST3375_PutPixel(_usX - CurY, _usY + CurX, _usColor);
		ST3375_PutPixel(_usX - CurY, _usY - CurX, _usColor);

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}


// //显示使用Img2Lcd软件转换的图片
// //x,y为开始显示座标,addr为图片数据在FLASH中保存的位置
// u8 ST3375_ShowPicture(u16 x,u16 y,u32 addr)
// {	
// 	u32 i,n,num,j;      
// 	u32 totalpoint;
//    	u16 w;			   //图片宽度
//    	u16 h;			   //图片高度
// 	sf_ReadBuffer(a_dot,addr,8);							//读取图像数据头
// 	if(a_dot[0] != 0)return 0;									//扫描模式不正确时退出
// 	if(a_dot[1] != 0x10)return 0;								//灰度值不是16位时退出
// 	w = *(u16 *)&a_dot[2];
// 	h = *(u16 *)&a_dot[4];
// 	if(((x+w)>g_LcdWidth)||((y+h)>g_LcdHeight))return 0;	//超出范围退出
// 	if(a_dot[6] != 0x01)return 0;								//R G B颜色分量565
// 	if(a_dot[7] != 0x1B)return 0;								//描述R G B颜色分量的排列顺序
// 	totalpoint = w * h;  		   								//总点数
	
// 	ST3375_Set_windows(x,x+w-1,y,y+h-1);						//设置更新区域
// 	ST3375_WriteRAM_Prepare();     							//开始写入GRAM	 	  
// 	for(i=0,n=0;i<totalpoint;)
// 	{
// 		num = totalpoint - i;
// 		if(num >= DOT_NUM/2)num = DOT_NUM/2;
// 		sf_ReadBuffer(a_dot,addr+8+n,num*2);				//读取图像数据
// 		for(j=0;j<num;j++)
// 		{
// 			ST3375_WriteData(*(u16 *)&a_dot[j*2]);
// 			i++;
// 		}  						
// 		n+=num*2;
// 	}
// 	return 1;
// }

/************************************************************************************************
**函数功能:清空指定的矩形区域
//参数说明:x:行,y:列,w:矩形的宽,h:矩形的高,data:要填充的数据
************************************************************************************************/
#if 0
void ST3375_ClrRect(u32 x0,u32 y0,u32 x1,u32 y1)
{
	u32 tmp;
	u32 index=0;      
	u32 totalpoint;

	if(x0>x1){  tmp = x0;x0 = x1;x1 = tmp;}	  				//如果x0大于x1,交换x0和x1
	if(y0>y1){  tmp = y0;y0 = y1;y1 = tmp;}

	totalpoint=x1-x0;
	totalpoint*=y1-y0;										//得到总点数
	
	ST3375_Set_windows(x0,x1,y0,y1);								//设置更新区域 
	ST3375_WriteRAM_Prepare();     							//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		ST3375_WriteData(bkColor);	   							//使用背景色来填充
	}
}
#endif
/************************************************************************************************
**函数功能:清空指定的矩形区域,可设置填充颜色
//参数说明:x:行,y:列,w:矩形的宽,h:矩形的高,data:要填充的数据
************************************************************************************************/
// void ST3375_ClrRect_color(u32 x0,u32 y0,u32 x1,u32 y1,u16 color)
// {
// 	u32 tmp;
// 	u32 index=0;      
// 	u32 totalpoint;

// 	if(x0>x1){  tmp = x0;x0 = x1;x1 = tmp;}	  				//如果x0大于x1,交换x0和x1
// 	if(y0>y1){  tmp = y0;y0 = y1;y1 = tmp;}

// 	totalpoint=x1-x0;
// 	totalpoint*=y1-y0;										//得到总点数
	
// 	ST3375_Set_windows(x0,x1,y0,y1);								//设置更新区域 
// 	ST3375_WriteRAM_Prepare();     							//开始写入GRAM	 	  
// 	for(index=0;index<totalpoint;index++)
// 	{
// 		ST3375_WriteData(color);	   						
// 	}
// }


/**************************************** 华宏软件 (END OF FILE) ****************************************/
