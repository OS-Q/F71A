#ifndef _GUI2D_H
#define _GUI2D_H
#include <stm32f10x_lib.h>

typedef struct { s16 x,y; } GUI_POINT;
/* Define minimum and maximum coordinates in x and y */
#define GUI_XMIN -4095
#define GUI_XMAX  4095
#define GUI_YMIN -4095
#define GUI_YMAX  4095

#define GUI_45DEG  512
#define GUI_90DEG  (2*GUI_45DEG)
#define GUI_180DEG (4*GUI_45DEG)
#define GUI_360DEG (8*GUI_45DEG)

/**********************************
*      Line styles				  *
**********************************/
#define GUI_LS_SOLID        (0)
#define GUI_LS_DASH         (1)
#define GUI_LS_DOT          (2)
#define GUI_LS_DASHDOT      (3)
#define GUI_LS_DASHDOTDOT   (4)

/**********************************
*      Pen shapes				  *
**********************************/
#define GUI_PS_ROUND        (0)
#define GUI_PS_FLAT         (1)
#define GUI_PS_SQUARE       (2)

extern u16 *tempReshow;
extern u32 tempOff;
extern u16 PinAlpha;
extern u8 DrawFuc;
extern u8 PenSize;
extern u8 LineStyle;
extern u8 PenShape;
extern u8 SWAlpha;

void GL_FillPolygon(const GUI_POINT*paPoint, int NumPoints, int xOff, int yOff);
void GL_DrawPolygon(const GUI_POINT*pPoints, int NumPoints, int x0, int y0);
void GL_DrawPolygonM(const GUI_POINT*pPoints, int NumPoints, int x0, int y0);
void GUI_RotatePolygon(GUI_POINT* pDest, const GUI_POINT* pSrc, int NumPoints, float Angle);
void GL_DrawPoint(int x, int y);
void GL_DrawLine(int x0, int y0, int x1, int y1);
void GL_DrawLineRelNM(int dx, int dy);
void GL_DrawLineTo(int x, int y);
void GL_DrawLineToNM(int x, int y);
void GL_MoveRel(int dx, int dy);
void GL_MoveTo(int x, int y);
void SMG_display(u16 sx, u16 sy, u8 num);
void SMG_ShowNum(u16 x,u16 y,u16 num,u8 len);

#endif
