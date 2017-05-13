

#include "Display.h"
#include "stdio.h"
#include "math.h"
static pPaint paint;

#define checkpPoint(array,p)		((p->heigh<=array->heigh-1)&&(p->width<=array->width-1))
#define checkPoint(array,p)			((p.heigh<=array->heigh-1)&&(p.width<=array->width-1))
#define absInt(a)								(((a)>0)?(a):-(a))
#define board(array,p0,p1)			if((checkPoint(array,p0)&&checkPoint(array,p1)) ==0){	\
																		if(array->heigh <=  p0.heigh)p0.heigh = array->heigh-1;\
																		if(array->heigh <=  p1.heigh)p1.heigh = array->heigh-1;\
																		if(array->width <=  p0.width)p0.width = array->width-1;\
																		if(array->width <=  p1.width)p1.width = array->width-1;\
																	}


#define putPixelRGB(array,x,y,color)	(*((pRGB)array->array +((x)*array->width +(y)))) =  color
#define putPixelRGBW(array,x,y,color) (*((pRGBW)array->array +(x*array->width +y))) =  color
#define putPixelRGBIndex(array,x,color) 

static  void drawLineRGB(pArray array,Point p0,Point p1);
static  void drawLineRGBW(pArray array,Point p0,Point p1);
static  void moveScreenRGB(pArray array,int row,int column);
static  void moveScreenRGBW(pArray array,int row,int column);			
static  void moveScreenHeighRGB(pArray array,int heigh);
static  void moveScreenHeighRGBW(pArray array,int heigh);
static  void moveScreenWidthRGB(pArray array,int width);
static  void moveScreenWidthRGBW(pArray array,int width);	
static  void clearSrceenRGB(pArray array);
static  void clearScreenRGBW(pArray array);																	
static  void clearAreaRGB(pArray array,Int16 xStart,Int16 xEnd,Int16 yStart,Int16 yEnd);
static  void chearAreaRGBW(pArray array,Int16 xStart,Int16 xEnd,Int16 yStart,Int16 yEnd);																	
																	
void    setPaint(pPaint	_paint)
{
	paint = _paint;
}

pPaint  getPaint(void)
{
	return paint;
}
void    drawLineXY(pArray array,Int16 x0,Int16 y0,Int16 x1,Int16 y1)
{
	Point p0,p1;
	p0.width = x0;p0.heigh = y0;
	p1.width = x1;p1.heigh = y1;
	drawLine(array,p0,p1);
}
void		drawLine(pArray array,Point p0,Point p1)
{
	board(array,p0,p1);
	if(array->type == DATATYPE_RGBW){
		drawLineRGBW(array,p1,p0);
	}else{
		drawLineRGB(array,p1,p0);
	}
//	switch(array->type){
//		case DATATYPE_GRB:
//		case DATATYPE_RBG:
//		case DATATYPE_RGB:{
//			drawLineRGB(array,p1,p0);
//		}break;
//		case DATATYPE_RGBW:{
//			drawLineRGBW(array,p1,p0);
//		}break;
//	}
	
}
void    drawTriangle(pArray array,pTriangle	triangle)
{
	drawLine(array,triangle->a,triangle->b);
	drawLine(array,triangle->b,triangle->c);
	drawLine(array,triangle->a,triangle->c);
}
void    drawTriangleFull(pArray array,pTriangle triangle)
{
	
}
void    drawCircleXY(pArray array,Int16 x,Int16 y,Int16 R)
{
	Point p={x,y};
	drawCircle(array,p,R);
}

void		drawCircle(pArray array,Point p0,Int16 R)
{
	int   tx = 0;
	int   ty = R;
	int   d = 1-R;
	pRGB  prgb;

	if(array == NULL)return;
	if(array->type == DATATYPE_RGBW){
		while(tx <= ty){
			if(p0.width+tx <= array->width&&p0.heigh+ty<=array->heigh) *(pRGBW)(prgb+p0.width+tx+(p0.heigh+ty)*array->width) = paint->color.rgbw;
			if(p0.width+tx <= array->width&&p0.heigh-ty>=0) 								 *(pRGBW)(prgb+p0.width+tx+(p0.heigh-ty)*array->width) = paint->color.rgbw;
			if(p0.width-tx >=0&&p0.heigh+ty<=array->heigh) 							   *(pRGBW)(prgb+p0.width-tx+(p0.heigh+ty)*array->width) = paint->color.rgbw;
			if(p0.width-tx >=0&&p0.heigh-ty>=0) 																 *(pRGBW)(prgb+p0.width-tx+(p0.heigh-ty)*array->width) = paint->color.rgbw;	
			
			if(p0.width+ty <= array->width&&p0.heigh+tx<=array->heigh) *(pRGBW)(prgb+p0.width+ty+(p0.heigh+tx)*array->width) = paint->color.rgbw;
			if(p0.width+ty <= array->width&&p0.heigh-tx>=0) 								 *(pRGBW)(prgb+p0.width+ty+(p0.heigh-tx)*array->width) = paint->color.rgbw;
			if(p0.width-ty >=0&&p0.heigh+tx<=array->heigh) 								 *(pRGBW)(prgb+p0.width-ty+(p0.heigh+tx)*array->width) = paint->color.rgbw;
			if(p0.width-ty >=0&&p0.heigh-tx>=0) 																 *(pRGBW)(prgb+p0.width-ty+(p0.heigh-tx)*array->width) = paint->color.rgbw;	
			
			if(d <0)d+= (tx<<1)+3;
			else d+=((tx-ty)<<1)+5,ty--;
			tx++;
		}		
	}else{
		prgb = (pRGB)array->array;
		while(tx <= ty){
				if(p0.width+tx <= array->width&&p0.heigh+ty<=array->heigh) *(pRGB)(prgb+p0.width+tx+(p0.heigh+ty)*array->width) = paint->color.rgb;
				if(p0.width+tx <= array->width&&p0.heigh-ty>=0) 								 *(pRGB)(prgb+p0.width+tx+(p0.heigh-ty)*array->width) = paint->color.rgb;
				if(p0.width-tx >=0&&p0.heigh+ty<=array->heigh) 							   *(pRGB)(prgb+p0.width-tx+(p0.heigh+ty)*array->width) = paint->color.rgb;
				if(p0.width-tx >=0&&p0.heigh-ty>=0) 																 *(pRGB)(prgb+p0.width-tx+(p0.heigh-ty)*array->width) = paint->color.rgb;	
				
				if(p0.width+ty <= array->width&&p0.heigh+tx<=array->heigh) *(pRGB)(prgb+p0.width+ty+(p0.heigh+tx)*array->width) = paint->color.rgb;
				if(p0.width+ty <= array->width&&p0.heigh-tx>=0) 								 *(pRGB)(prgb+p0.width+ty+(p0.heigh-tx)*array->width) = paint->color.rgb;
				if(p0.width-ty >=0&&p0.heigh+tx<=array->heigh) 								 *(pRGB)(prgb+p0.width-ty+(p0.heigh+tx)*array->width) = paint->color.rgb;
				if(p0.width-ty >=0&&p0.heigh-tx>=0) 																 *(pRGB)(prgb+p0.width-ty+(p0.heigh-tx)*array->width) = paint->color.rgb;	
				
				if(d <0)d+= (tx*2)+3;
				else d+=((tx-ty)*2)+5,ty--;
				tx++;
			}
	}
//	switch(array->type){
//		case DATATYPE_GRB:
//		case DATATYPE_RBG:
//		case DATATYPE_RGB:{
//			prgb = (pRGB)array->array;
//			while(tx <= ty){
//					if(p0.width+tx <= array->width&&p0.heigh+ty<=array->heigh) *(pRGB)(prgb+p0.width+tx+(p0.heigh+ty)*array->width) = paint->color.rgb;
//					if(p0.width+tx <= array->width&&p0.heigh-ty>=0) 								 *(pRGB)(prgb+p0.width+tx+(p0.heigh-ty)*array->width) = paint->color.rgb;
//					if(p0.width-tx >=0&&p0.heigh+ty<=array->heigh) 							   *(pRGB)(prgb+p0.width-tx+(p0.heigh+ty)*array->width) = paint->color.rgb;
//					if(p0.width-tx >=0&&p0.heigh-ty>=0) 																 *(pRGB)(prgb+p0.width-tx+(p0.heigh-ty)*array->width) = paint->color.rgb;	
//					
//					if(p0.width+ty <= array->width&&p0.heigh+tx<=array->heigh) *(pRGB)(prgb+p0.width+ty+(p0.heigh+tx)*array->width) = paint->color.rgb;
//					if(p0.width+ty <= array->width&&p0.heigh-tx>=0) 								 *(pRGB)(prgb+p0.width+ty+(p0.heigh-tx)*array->width) = paint->color.rgb;
//					if(p0.width-ty >=0&&p0.heigh+tx<=array->heigh) 								 *(pRGB)(prgb+p0.width-ty+(p0.heigh+tx)*array->width) = paint->color.rgb;
//					if(p0.width-ty >=0&&p0.heigh-tx>=0) 																 *(pRGB)(prgb+p0.width-ty+(p0.heigh-tx)*array->width) = paint->color.rgb;	
//					
//					if(d <0)d+= (tx*2)+3;
//					else d+=((tx-ty)*2)+5,ty--;
//					tx++;
//				}
//		}break;
//		case DATATYPE_RGBW:{

//			while(tx <= ty){
//					if(p0.width+tx <= array->width&&p0.heigh+ty<=array->heigh) *(pRGBW)(prgb+p0.width+tx+(p0.heigh+ty)*array->width) = paint->color.rgbw;
//					if(p0.width+tx <= array->width&&p0.heigh-ty>=0) 								 *(pRGBW)(prgb+p0.width+tx+(p0.heigh-ty)*array->width) = paint->color.rgbw;
//					if(p0.width-tx >=0&&p0.heigh+ty<=array->heigh) 							   *(pRGBW)(prgb+p0.width-tx+(p0.heigh+ty)*array->width) = paint->color.rgbw;
//					if(p0.width-tx >=0&&p0.heigh-ty>=0) 																 *(pRGBW)(prgb+p0.width-tx+(p0.heigh-ty)*array->width) = paint->color.rgbw;	
//					
//					if(p0.width+ty <= array->width&&p0.heigh+tx<=array->heigh) *(pRGBW)(prgb+p0.width+ty+(p0.heigh+tx)*array->width) = paint->color.rgbw;
//					if(p0.width+ty <= array->width&&p0.heigh-tx>=0) 								 *(pRGBW)(prgb+p0.width+ty+(p0.heigh-tx)*array->width) = paint->color.rgbw;
//					if(p0.width-ty >=0&&p0.heigh+tx<=array->heigh) 								 *(pRGBW)(prgb+p0.width-ty+(p0.heigh+tx)*array->width) = paint->color.rgbw;
//					if(p0.width-ty >=0&&p0.heigh-tx>=0) 																 *(pRGBW)(prgb+p0.width-ty+(p0.heigh-tx)*array->width) = paint->color.rgbw;	
//					
//					if(d <0)d+= (tx<<1)+3;
//					else d+=((tx-ty)<<1)+5,ty--;
//					tx++;
//				}
//		}break;
//	}
	
}
void    drawCircleFull(pArray array,Point p0,Int16 R)
{
	while(R--)drawCircle(array,p0,R);
}

void    drawCircleFullXY(pArray array,Int16 x,Int16 y,Int16 R)
{
	Point p={x,y};
	drawCircleFull(array,p,R);
}

void    drawPixelObject(pArray array,Point p0)
{
	if(array != NULL){
		if(checkPoint(array,p0)==1){
			if(array->type == DATATYPE_RGBW){
				(*((pRGBW)array->array +(p0.width*array->width +p0.heigh))) =  paint->color.rgbw;
			}else{
				(*((pRGB)array->array +(p0.width*array->width +p0.heigh))) =  paint->color.rgb;
			}
//			switch(array->type){
//				case DATATYPE_GRB:
//				case DATATYPE_RBG:
//				case DATATYPE_RGB:{
//					(*((pRGB)array->array +(p0.width*array->width +p0.heigh))) =  paint->color.rgb;
//				}break;
//				case DATATYPE_RGBW:{
//					(*((pRGBW)array->array +(p0.width*array->width +p0.heigh))) =  paint->color.rgbw;
//				}break;
//					
//			}
		}
	}
}

void    drawPixel(pArray array,Int16 x,Int16 y)
{
	if(array != NULL){
		if(((y<=array->heigh)&&(x<=array->width))){
			if(array->type == DATATYPE_RGBW){
				(*((pRGBW)array->array +(x*array->width +y))) =  paint->color.rgbw;
			}else{
				(*((pRGB)array->array +(x*array->width +y))) =  paint->color.rgb;
			}
//			switch(array->type){
//				case DATATYPE_GRB:
//				case DATATYPE_RBG:
//				case DATATYPE_RGB:{
//					(*((pRGB)array->array +(x*array->width +y))) =  paint->color.rgb;
//				}break;
//				case DATATYPE_RGBW:{
//					(*((pRGBW)array->array +(x*array->width +y))) =  paint->color.rgbw;
//				}break;
//			}
		}
	}
}



void    clearScreen(pArray array)
{
	if(array != NULL){
		if(array->type == DATATYPE_RGBW){
			clearScreenRGBW(array);
		}else{
			clearSrceenRGB(array);
		}
//		switch(array->type){
//			case DATATYPE_GRB:
//			case DATATYPE_RBG:
//			case DATATYPE_RGB:{
//				clearSrceenRGB(array);
//			}break;
//			case DATATYPE_RGBW:{
//				clearScreenRGBW(array);
//			}break;
//		}
		
	}
}
void    adjectArrayLight(pArray array,float k)
{
	pRGBW p0;
	pRGB  p1;
	int i,j;
	if(array != NULL){
		if(array->type == DATATYPE_RGBW){
			p0 = (pRGBW)array->array;
			for(i=0;i<array->heigh;i++){
				for(j =0; j< array->width;j++){
					p0->red  *=k;
					p0->green*=k;
					p0->blue *=k;
					p0->white*=k;
					p0++;
				}
			}
			
		}else{
			p1 = (pRGB)array->array;
			for(i=0;i<array->heigh;i++){
				for(j =0; j< array->width;j++){
					p1->red  *=k;
					p1->green*=k;
					p1->blue *=k;
					p1++;
				}
			}
		}
	}
}

void    clearArea(pArray array,Int16 xStart,Int16 xEnd,Int16 yStart,Int16 yEnd)
{
	xStart --;yEnd++;
	if(array!= NULL){
		if(xStart >xEnd || yStart > yEnd)return;
		if(xEnd>=array->width)xEnd = array->width-1;
		if(yEnd>=array->heigh)yEnd = array->heigh-1;
	}
	if(array->type == DATATYPE_RGBW){
		chearAreaRGBW(array,xStart,xEnd,yStart,yEnd);
	}else{
		clearAreaRGB(array,xStart,xEnd,yStart,yEnd);
	}
//	switch(array->type){
//		case DATATYPE_GRB:
//		case DATATYPE_RBG:
//		case DATATYPE_RGB:
//				clearAreaRGB(array,xStart,xEnd,yStart,yEnd);
//		break;
//		case DATATYPE_RGBW:
//				chearAreaRGBW(array,xStart,xEnd,yStart,yEnd);
//		break;
//	}
//	
}
void    clearLine(pLineNode line)
{
	Int16 len;
	DataType*pDest;
	if(line == NULL)return;
	len   = line->len;
	pDest = line->Data;
	if(line->type == DATATYPE_RGBW){
		
		while(len--){
			*(RGBW*)pDest++ = paint->background.rgbw;
		}
	}else{
		while(len--){
			*(RGB*)pDest = paint->background.rgb;
			pDest = (DataType*)(((pRGB)pDest )+1);
		}
	}
}
void    adjectLineLight(pLineNode line,float k)
{
	pRGBW p0;
	pRGB  p1;
	if(line == NULL)return;
	int len   = line->len;
	if(line->type == DATATYPE_RGBW){
		p0 = (pRGBW)line->Data;
		while(len--){
			p0->red   *= k;
			p0->green *= k;
			p0->blue  *= k;
			p0->white *= k;
			p0++;
		}
	}else{
		p1 = (pRGB)line->Data;
		while(len--){
			p1->red   *= k;
			p1->green *= k;
			p1->blue  *= k;
			p1++;
		}
	}
}
void    showArray(pArray array)
{
	int i,j;
	DataType *p;
	printf("ans = [\n");
	if(array != NULL){
		p = array ->array;
		if(array->type == DATATYPE_RGBW){
			for(i=0;i<array->heigh;i++){
					for(j=0;j<array->width;j++){
						
						if(p->rgb.red!= 0)printf("o");
						else printf(" ");
//						printf("%x%x%x\t",p->rgb.red,p->rgb.green,p->rgb.blue);
						p 		= (DataType*)(((char*)p)+sizeof(RGB));
					}
					printf("\n");
				}
		}else{
			for(i=0;i<array->width;i++){
					for(j=0;j<array->heigh;j++){
						if(p->rgb.red!= 0)printf("o");
						else printf(" ");
//						printf("%x%x%x%x\t",p->rgb.red,p->rgb.green,p->rgb.blue,p->rgbw.white);
						p 		 = (DataType*)(((char*)p)+sizeof(RGBW));
					}
					printf("\n");
				}
		}
//		switch(array->type){
//			case DATATYPE_GRB:
//			case DATATYPE_RBG:
//			case DATATYPE_RGB:{
//				for(i=0;i<array->heigh;i++){
//					for(j=0;j<array->width;j++){
//						
//						if(p->rgb.red!= 0)printf("o");
//						else printf(" ");
////						printf("%x%x%x\t",p->rgb.red,p->rgb.green,p->rgb.blue);
//						p 		= (DataType*)(((char*)p)+sizeof(RGB));
//					}
//					printf("\n");
//				}
//			}break;
//			case DATATYPE_RGBW:{
//				for(i=0;i<array->width;i++){
//					for(j=0;j<array->heigh;j++){
//						if(p->rgb.red!= 0)printf("o");
//						else printf(" ");
////						printf("%x%x%x%x\t",p->rgb.red,p->rgb.green,p->rgb.blue,p->rgbw.white);
//						p 		 = (DataType*)(((char*)p)+sizeof(RGBW));
//					}
//					printf("\n");
//				}
//			}break;
//		}
	}
	printf("]\n");
}

void    moveScreen(pArray array,int row,int column)
{
	if(array == NULL)return;
	if(array->type == DATATYPE_RGBW){
		moveScreenRGBW(array,row,column);
	}else{
		moveScreenRGB(array,row,column);
	}
//	switch(array->type){
//		case DATATYPE_GRB:
//		case DATATYPE_RBG:
//		case DATATYPE_RGB:moveScreenRGB(array,row,column);break;
//		case DATATYPE_RGBW:moveScreenRGBW(array,row,column);break;
//	}
}
void    moveScreenWidth(pArray array,int width)
{
	// 决定移动方向
	if(array == NULL)return;
	if(array -> type == DATATYPE_RGBW){
		moveScreenWidthRGBW(array,width);
	}else{
		moveScreenWidthRGB(array,width);
	}
//	switch(array->type){
//		case DATATYPE_GRB:
//		case DATATYPE_RBG:
//		case DATATYPE_RGB:moveScreenWidthRGB(array,width);break;
//		case DATATYPE_RGBW:moveScreenWidthRGBW(array,width);break;
//	}
}

void    moveScreenHeigh(pArray array,int heigh)
{
	if(array == NULL)return;
	if(array->type == DATATYPE_RGBW){
		moveScreenHeighRGBW(array,heigh);
	}else{
		moveScreenHeighRGB(array,heigh);
	}
//	switch(array->type){
//		case DATATYPE_GRB:
//		case DATATYPE_RBG:
//		case DATATYPE_RGB:moveScreenHeighRGB(array,heigh);break;
//		case DATATYPE_RGBW:moveScreenHeighRGBW(array,heigh);break;
//	}
}

void    moveLine(pLineNode pline,int number)
{
	DataType* pDest;
	DataType* pSrc;
	DataType  src;
	src.rgbw.blue = 0;
	src.rgbw.green = 0;
	src.rgbw.red = 0;
	src.rgbw.white =0;
	Int16     i;
	if(pline == NULL||number == 0 )return;
	if(number >0){
		if(number >pline->len){clearLine(pline);return;}
		
		if(pline->type == DATATYPE_RGBW){
			pDest = (DataType*)((pRGBW)pline->Data) + pline->len-1;
			pSrc  = (DataType*)((pRGBW)pDest) - number;
			
			for(i=0;i<pline->len-number;i++){
				*pDest= *pSrc;
				pDest =(DataType*)(((pRGBW)pDest) -1);
				pSrc  =(DataType*)(((pRGBW)pSrc)  -1);
			}
			for(i=0;i<number;i++){													// 清屏
				*pDest = src;
				pDest =(DataType*)(((pRGBW)pDest) -1);
			}
			
		}else{
			pDest = (DataType*)(((pRGB)pline->Data) + pline->len-1);
			pSrc  = (DataType*)(((pRGB)pDest) - number);
			
			for(i=0;i<pline->len-number;i++){
				*(pRGB)pDest = *(pRGB)pSrc;
				pDest =(DataType*)(((pRGB)pDest) -1);
				pSrc  =(DataType*)(((pRGB)pSrc)  -1);
			}
			for(i=0;i<number;i++){													// 清屏
				*(pRGB)pDest = src.rgb;
				pDest =(DataType*)(((pRGB)pDest) -1);
			}
		}
	}else{
		number = -number;
		if(number >pline->len){clearLine(pline);return;}
		
		if(pline->type == DATATYPE_RGBW){
			pDest = (DataType*)((pRGBW)pline->Data);
			pSrc  = (DataType*)((pRGBW)pline->Data) + number;
			
			for(i=0;i<pline->len-number;i++){
				*pDest= *pSrc;
				pDest =(DataType*)(((pRGBW)pDest) +1);
				pSrc  =(DataType*)(((pRGBW)pSrc)  +1);
			}
			for(i=0;i<number;i++){													// 清屏
				*pDest = src;
				pDest =(DataType*)(((pRGBW)pDest) +1);
			}
			
		}else{
			pDest = (DataType*)(((pRGB)pline->Data));
			pSrc  = (DataType*)(((pRGB)pline->Data)+ number);
			
			for(i=0;i<pline->len-number;i++){
				*(pRGB)pDest = *(pRGB)pSrc;
				pDest =(DataType*)(((pRGB)pDest) +1);
				pSrc  =(DataType*)(((pRGB)pSrc)  +1);
			}
			for(i=0;i<number;i++){													// 清屏
				*(pRGB)pDest = src.rgb;
				pDest =(DataType*)(((pRGB)pDest) +1);
			}
		}
	}
}

void    moveLineRGB(pRGB pData,Int16 len,int number)
{
	pRGB pSrc,pDest;
	RGB  zeros={0,0,0};
	Int16 count;
	if(number >0){
		if(number >= len){
			while(len--)*pData ++ = zeros;
		}else{
			pDest = pData + len -1;
			pSrc  = pDest - number;
			count = len - number;
			while(count --)*pDest-- = *pSrc--;
			while(number--)*pDest-- = zeros;
		}
	}else{
		number = - number;
		if(number >= len){
			while(len--)*pData ++ = zeros;
		}else{
			pDest = pData;
			pSrc  = pDest+number;
			count = len - number;
			while(count --)*pDest++ = *pSrc++;
			while(number--)*pDest++ = zeros;
		}
	}
	
}

// 减少乘除法，提高运算效率
static  void drawLineRGB(pArray array,Point p0,Point p1)
{
	int   move;
	int   fmove;
	int   count;
	int   hd = absInt(p0.heigh- p1.heigh);
	int   wd = absInt(p0.width - p1.width);
	
	Int16 dlt;
	Int16 err;
	pRGB  pData ;
	
	if(hd ==0&& wd == 0)return;
	
	pData  = (pRGB)array->array + p0.heigh*array->width + p0.width;
	
	err  =0;
	// 点数跟最多的一个维度一致
	if(hd > wd){
		if(p0.heigh < p1.heigh)move  = array->width;
		else move = -array->width;
		if(p0.width < p1.width) fmove = 1;
		else fmove = -1;
	
		count = hd;
		dlt   = (wd<<12)/hd;
		
	}else {
		if(p0.heigh < p1.heigh) fmove = array->width;
		else fmove = -array->width;
		
		if(p0.width < p1.width) move = 1;
		else move  = -1;

		count = wd;
		dlt   = (hd<<12)/wd;
	}
	count ++;
	// 需要判定move和fmove的符号
	while(count --){
		*pData = paint->color.rgb;
		// pData需要怎么移动？
		err   += dlt; // dlt充当小数点部分
		if(err >0xfff){
			err &= 0xfff;
			pData += fmove;
		}
		pData += move;
	}
}

static  void drawLineRGBW(pArray array,Point p0,Point p1)
{
	int   move;
	int   fmove;
	int   count;
	int   hd = absInt(p0.heigh- p1.heigh);
	int   wd = absInt(p0.width - p1.width);
	
	Int16 dlt;
	Int16 err;
	pRGBW  pData ;
	
	if(hd ==0&& wd == 0)return;
	
	pData  = (pRGBW)array->array + p0.heigh*array->width + p0.width;
	
	err  =0;
	// 点数跟最多的一个维度一致
	if(hd > wd){
		if(p0.heigh < p1.heigh)move  = array->width;
		else move = -array->width;
		if(p0.width < p1.width) fmove = 1;
		else fmove = -1;
	
		count = hd;
		dlt   = (wd<<12)/hd;
		
	}else {
		if(p0.heigh < p1.heigh) fmove = array->width;
		else fmove = -array->width;
		
		if(p0.width < p1.width) move = 1;
		else move  = -1;

		count = wd;
		dlt   = (hd<<12)/wd;
	}
	
	count ++;
	// 需要判定move和fmove的符号
	while(count --){
		*pData = paint->color.rgbw;
		// pData需要怎么移动？
		err   += dlt; // dlt充当小数点部分
		if(err >0xfff){
			err &= 0xfff;
			pData += fmove;
		}
		pData += move;
	}
}
static  void moveScreenRGB(pArray array,int width,int heigh)
{
	moveScreenHeighRGB(array,heigh);
	moveScreenWidthRGB(array,width);
	
}

static  void moveScreenRGBW(pArray array,int width,int heigh)
{
	moveScreenHeighRGBW(array,heigh);
	moveScreenWidthRGBW(array,width);
}

static  void moveScreenWidthRGB(pArray array,int width)
{
	pRGB pdest,psrc;
	int  size;
	int  row;
	int  count;
	int  column;
	if(width >= array->width){
		pdest = (pRGB)array->array;
		count = array->heigh*array->width;
		while(count--)*pdest++ = paint->background.rgb;
		return;
	}
	
	if(width > 0){
		
		pdest  = (pRGB)array->array +array->width;
		
		psrc   = pdest-width;// 边界处理
		count   = array->width  - width;
		size    = count + array->width;
		for(row = 0;row<array->heigh;row++){
			
			for (column = 0; column<count;column++){
				*pdest-- = *psrc--;
			}
			pdest += size;
			psrc  += size;
		}

		pdest = (pRGB)array->array;
		count = width+1;
		size  = array->width - count;
		for(row = 0; row < array->heigh;row++){
			
			for(column = 0;column<count;column++){
				*pdest ++ = paint->background.rgb;
			}
			pdest += size;
		}
	}else{
		width = - width;			// 变为正数
		pdest  = (pRGB)array->array;
		psrc   = pdest+width-1;// 边界处理
		count  = array->width - width;
		size   = array->width - count;
		for(row = 0;row<array->heigh;row++){
			for (column = 0; column<count;column++){
				*pdest++ = *psrc++;
			}
			pdest += size;
			psrc  += size;
		}

		pdest = (pRGB)array->array + array->width-width;
		count = width;
		size  = array->width - count;
		for(row = 0; row < array->heigh;row++){
			for(column = 0;column<count;column++){
				*pdest ++ = paint->background.rgb;
			}
			pdest += size;
		}
	}
	
}

static  void moveScreenWidthRGBW(pArray array,int width)
{
	pRGBW pdest,psrc;
	int  size;
	int  row;
	int  count;
	int  column;
	if(width >= array->width){
		pdest = (pRGBW)array->array;
		count = array->heigh*array->width;
		while(count--)*pdest++ = paint->background.rgbw;
		return;
	}
	
	if(width > 0){
		
		pdest  = (pRGBW)array->array +array->width;
		
		psrc   = pdest-width;// 边界处理
		count   = array->width  - width;
		size    = count + array->width;
		for(row = 0;row<array->heigh;row++){
			
			for (column = 0; column<count;column++){
				*pdest-- = *psrc--;
			}
			pdest += size;
			psrc  += size;
		}

		pdest = (pRGBW)array->array;
		count = width+1;
		size  = array->width - count;
		for(row = 0; row < array->heigh;row++){
			
			for(column = 0;column<count;column++){
				*pdest ++ = paint->background.rgbw;
			}
			pdest += size;
		}
	}else{
		width = - width;			// 变为正数
		pdest  = (pRGBW)array->array;
		psrc   = pdest+width-1;// 边界处理
		count  = array->width - width;
		size   = array->width - count;
		for(row = 0;row<array->heigh;row++){
			for (column = 0; column<count;column++){
				*pdest++ = *psrc++;
			}
			pdest += size;
			psrc  += size;
		}

		pdest = (pRGBW)array->array + array->width-width;
		count = width;
		size  = array->width - count;
		for(row = 0; row < array->heigh;row++){
			for(column = 0;column<count;column++){
				*pdest ++ = paint->background.rgbw;
			}
			pdest += size;
		}
	}
}

static  void moveScreenHeighRGB(pArray array,int heigh)
{
	pRGB pdest,psrc;
	int  count;
	if(heigh >= array->heigh ||-heigh >=array->heigh){
		pdest = (pRGB)array->array;
		count = array->heigh*array->width;
		while(count--)*pdest++ = paint->background.rgb;
		return;
	}
	if(heigh >0){
		count = (array->heigh- heigh)*array->width;
		pdest =  (((pRGB)array->array)+array->heigh*array->width);
		psrc  = pdest - heigh*array->width;
		while(count--){
			*pdest-- = *psrc--;
		}

		count = heigh*array->width;
		pdest = (pRGB)array->array;
		while(count--)*pdest++ = paint->background.rgb; // 将其与的屏幕刷成背景色
	}else{
		heigh = - heigh;
		count = (array->heigh-heigh)*array->width;
		pdest = (pRGB)array->array;
		psrc  = pdest + heigh*array->width;
		while(count--){
			*pdest++ = *psrc++;
		}
		count = heigh*array->width;								// 可以直接绘制
		while(count--){
			*pdest++ = paint->background.rgb;
		}
	}
}

static  void moveScreenHeighRGBW(pArray array,int heigh)
{
	pRGBW pdest,psrc;
	int  count;
	if(heigh >= array->heigh ||-heigh >=array->heigh){
		pdest = (pRGBW)array->array;
		count = array->heigh*array->width;
		while(count--)*pdest++ = paint->background.rgbw;
		return;
	}
	if(heigh >0){
		count = (array->heigh- heigh)*array->width;
		pdest =  (((pRGBW)array->array)+array->heigh*array->width);
		psrc  = pdest - heigh*array->width;
		while(count--){
			*pdest-- = *psrc--;
		}

		count = heigh*array->width;
		pdest = (pRGBW)array->array;
		while(count--)*pdest++ = paint->background.rgbw; // 将其与的屏幕刷成背景色
	}else{
		heigh = - heigh;
		count = (array->heigh-heigh)*array->width;
		pdest = (pRGBW)array->array;
		psrc  = pdest + heigh*array->width;
		while(count--){
			*pdest++ = *psrc++;
		}
		count = heigh*array->width;								// 可以直接绘制
		while(count--){
			*pdest++ = paint->background.rgbw;
		}
	}
}

static  void clearSrceenRGB(pArray array)
{
	Int16 i,j;
	pRGB p = (pRGB)array->array;
	for(i=0;i<array->heigh;i++){
		for(j=0;j<array->width;j++){
			*p++= paint->background.rgb;
		}
	}
}
static  void clearScreenRGBW(pArray array)
{
	Int16 i,j;
	pRGBW p = (pRGBW)array->array;
	for(i=0;i<array->heigh;i++){
		for(j=0;j<array->width;j++){
			*p++= paint->background.rgbw;
		}
	}
}
static  void clearAreaRGB(pArray array,Int16 xStart,Int16 xEnd,Int16 yStart,Int16 yEnd)
{
	pRGB  p;
	Int16     count;
	int i ;
	count = xEnd - xStart;
	p = (pRGB)(array->array +yStart*array->width + xStart);
		
	for(; yStart < yEnd; yStart++){
		
		for(i = xStart;i< xEnd;i++){
			*p++= paint->background.rgb;
		}
		p = p - count + array->width;
	}
	
}
static  void chearAreaRGBW(pArray array,Int16 xStart,Int16 xEnd,Int16 yStart,Int16 yEnd)
{
	pRGBW  p;
	Int16     count;
	int i ;
	count = xEnd - xStart;
	p = (pRGBW)(array->array +yStart*array->width + xStart);
		
	for(; yStart < yEnd; yStart++){
		
		for(i = xStart;i< xEnd;i++){
			*p++= paint->background.rgbw;
		}
		p -= count;
		p += array->width;
	}
}


