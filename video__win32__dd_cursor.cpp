/********************************************************************** <BR>
   This file is part of Crack dot Com's free source code release of
   Golgotha. <a href="http://www.crack.com/golgotha_release"> <BR> for
   information about compiling & licensing issues visit this URL</a>
   <PRE> If that doesn't help, contact Jonathan Clark at
   golgotha_source@usa.net (Subject should have "GOLG" in it)
 ***********************************************************************/
#include "pch.h"
#include "video/win32/dd_cursor.h"
#include "video/win32/dx9_util.h"
#include "video/win32/dx9_error.h"
#include "image/context.h"

ddraw_thread_cursor_class::ddraw_thread_cursor_class(const i4_pal * screen_pal,
													 HWND window_handle,
													 sw32 clip_x1, sw32 clip_y1,
													 sw32 clip_x2, sw32 clip_y2)
	: dx_threaded_mouse_class(screen_pal,window_handle,clip_x1,clip_y1,clip_x2,clip_y2)
{
	cursor.pict = 0;
	mouse_save  = 0;
	cursor_version=0;
	current_mouse_x = 0;
	current_mouse_y = 0;

	save_mouse_x    = 0;
	save_mouse_y    = 0;

	visible = i4_F;

	use_back_buffer = i4_F;
}

void ddraw_thread_cursor_class::set_cursor(i4_cursor_class * c)
{
	draw_lock.lock();

	remove();

	if (cursor.pict)
	{
		delete cursor.pict;
		cursor.pict = 0;
	}

	if (mouse_save)
	{
		delete mouse_save;
		mouse_save = 0;
	}

	if (c && c->pict)
	{
		int cw=c->pict->width(), ch=c->pict->height();

		i4_draw_context_class context(0,0, cw-1, ch-1);

		i4_dx9_image_class * dx9_image = new i4_dx9_image_class(cw, ch, DX9_VRAM);
		cursor_version=5;
		//setup room for the mouse save
		mouse_save = new i4_dx9_image_class(cw,ch,DX9_VRAM);

		//setup the color-key value for the mouse (black)
		/*DDCOLORKEY colorkey;
		memset(&colorkey,0,sizeof(DDCOLORKEY));

		colorkey.dwColorSpaceLowValue  = c->trans;
		colorkey.dwColorSpaceHighValue = c->trans;

		dx9_image->surface->SetColorKey(DDCKEY_SRCBLT,&colorkey);*/

		cursor      = *c;
		cursor.pict = dx9_image;

		dx9_image->lock();
		c->pict->put_image_trans(cursor.pict, 0,0, c->trans, context);
		dx9_image->unlock();

		display();
	}

	draw_lock.unlock();
}

void ddraw_thread_cursor_class::save()
{
	if (!mouse_save)
	{
		return;
	}

	RECT r;

	r.left   = save_mouse_x;
	r.top    = save_mouse_y;
	r.right  = r.left + mouse_save->width();
	r.bottom = r.top  + mouse_save->height();



	IDirect3DSurface9 * framebuffer = (use_back_buffer) ? (dx9_common.back_surface) : (dx9_common.front_surface);

	i4_dx9_image_class * im5=(i4_dx9_image_class *)mouse_save;
	HRESULT res =
		im5->surface->BltFast(0,
							  0,
							  framebuffer,
							  &r,
							  DDBLTFAST_NOCOLORKEY |
							  DDBLTFAST_WAIT);
	i4_dx5_check(res);

}

void ddraw_thread_cursor_class::remove()
{
	i4_dx5_image_class * im5=(i4_dx5_image_class *) mouse_save;

	if (!visible || !mouse_save || !im5->surface)
	{
		return;
	}


	IDirectDrawSurface3 * framebuffer = (use_back_buffer) ? (dx5_common.back_surface) : (dx5_common.primary_surface);
	HRESULT res =
		framebuffer->BltFast(save_mouse_x,
							 save_mouse_y,
							 im5->surface,
							 0,
							 DDBLTFAST_NOCOLORKEY |
							 DDBLTFAST_WAIT);
	i4_dx5_check(res);

	visible = i4_F;

}

void ddraw_thread_cursor_class::display()
{
	i4_dx5_image_class * im5=(i4_dx5_image_class *)mouse_save;

	if (!cursor.pict || !mouse_save || !im5->surface)
	{
		return;
	}



	save_mouse_x = current_mouse_x;
	save_mouse_y = current_mouse_y;

	RECT r;

	r.left   = save_mouse_x;
	r.top    = save_mouse_y;
	r.right  = r.left + mouse_save->width();
	r.bottom = r.top + mouse_save->height();


	IDirectDrawSurface3 * framebuffer = (use_back_buffer) ? (dx5_common.back_surface) : (dx5_common.primary_surface);
	HRESULT res =
		im5->surface->BltFast(0,
							  0,
							  framebuffer,
							  &r,
							  DDBLTFAST_NOCOLORKEY |
							  DDBLTFAST_WAIT);
	i4_dx5_check(res);

	IDirectDrawSurface3 * mouse_draw_surface = ((i4_dx5_image_class *)cursor.pict)->surface;

	res =
		framebuffer->BltFast(save_mouse_x,
							 save_mouse_y,
							 mouse_draw_surface,
							 0,
							 DDBLTFAST_SRCCOLORKEY |
							 DDBLTFAST_WAIT);

	i4_dx5_check(res);


	visible = i4_T;
}

void ddraw_thread_cursor_class::use_backbuffer(i4_bool use)
{
	use_back_buffer = use;
}
