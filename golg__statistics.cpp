/********************************************************************** <BR>
   This file is part of Crack dot Com's free source code release of
   Golgotha. <a href="http://www.crack.com/golgotha_release"> <BR> for
   information about compiling & licensing issues visit this URL</a>
   <PRE> If that doesn't help, contact Jonathan Clark at
   golgotha_source@usa.net (Subject should have "GOLG" in it)
 ***********************************************************************/

#include "pch.h"
#include "statistics.h"
#include "window/window.h"
#include "window/style.h"
#include "font/font.h"
#include "error/error.h"
#include "time/timedev.h"
#include "app/app.h"
#include "tick_count.h"
#include "device/key_man.h"

static i4_event_handler_reference_class<i4_parent_window_class> g1_stat_win;

//Need to declare these here (flaw of MSVC compiler...)
const int g1_statistics_counter_class::perframe[LAST] = {
	1, 1, 1, 1, 1, 1, 1, 0, 0
};
const char * g1_statistics_counter_class::strs[LAST]=
{
	"Polys",
	"Object polys",
	"Terrain polys",
	"Objects",
	"Terrain",
	"Sprites",
	"Sfx mixed",
	"Frames total",
	"AI steps"
};

const int g1_statistics_counter_class::max_perframe[LAST]={
	1,1,1,1,1,1,1,0,0
};
const char * g1_statistics_counter_class::max_strs[LAST]=
{
	"Drawn Quads",
	"Object total quads",
	"Object total vertices",
	"Objects",
	"<unused>",
	"<unused>",
	"<unused>",
	"Frames total",
	"<unused>"
};


class g1_stat_win_class :
	public i4_window_class
{
	int t_strings;
	i4_graphical_style_class * style;
	i4_time_device_class::id poll_id;

public:
	int x2()
	{
		return 50;
	}
	g1_stat_win_class(i4_graphical_style_class * style)
		: i4_window_class(0,0),
		  style(style)

	{
		int min_w=0, min_h=0;

		t_strings=0;
		i4_font_class * fnt=style->font_hint->small_font;

		min_w=fnt->width("Object polygons");
		min_h=fnt->largest_height() * g1_statistics_counter_class::LAST;

		min_h+=fnt->largest_height(); // save room for fps (first line)
		min_w+=x2();             // add room for numbers

		resize(min_w, min_h);

		i4_user_message_event_class poll(0);
		poll_id=i4_time_dev.request_event(this, &poll, 2000); // update once every 2 secs
	}

	void draw_float(int x, int y, float v, i4_font_class * fnt, i4_draw_context_class &context)
	{
		char buf[30];
		int ipart, fpart;

		ipart=(int)v;
		fpart=(int)(v * 10) - ipart*10;

		sprintf(buf, "%d.%d", ipart, fpart);

		for (char * c=buf; *c; c++)
		{
			i4_char ch(* c);
			fnt->put_character(local_image, x,y, ch, context);
			x+=fnt->width(ch);
		}
	}

	void draw(i4_draw_context_class &context)
	{
		local_image->clear(0, context);

		//if (g1_stat_counter.t_frames)
		//{
		i4_font_class * fnt=style->font_hint->small_font;
		fnt->set_color(0xffff00);

		float oo_tframes = 1.0f/(float)g1_stat_counter.t_frames;

		i4_time_class now;
		draw_float(0,0, (float)g1_stat_counter.t_frames /
				   (now.milli_diff(g1_stat_counter.last_update_time)/1000.0f),
				   fnt, context);

		fnt->put_string(local_image, x2(), 0, i4gets("fps"), context);
		int y=fnt->largest_height();

		//Bind this setting to the tick_counter
		g1_stat_counter.set_value(g1_statistics_counter_class::AISTEPS,g1_tick_counter);
		i4_bool maxtool_mode=i4_F;
		if (strcmp(i4_key_man.context_name(i4_key_man.current_context()),"maxtool")==0)
		{
			maxtool_mode=i4_T;
		}
		for (int i=0; i<g1_statistics_counter_class::LAST; i++)
		{
			i4_bool isperframe=i4_F;
			if (maxtool_mode)
			{
				isperframe=g1_stat_counter.max_perframe[i];
			}
			else
			{
				isperframe=g1_stat_counter.perframe[i];
			}
			if (isperframe)
			{

				draw_float(0, y, g1_stat_counter.counter_array[i] * oo_tframes, fnt, context);
			}
			else
			{

				draw_float(0, y, (float)g1_stat_counter.counter_array[i], fnt, context);

			}
			if (maxtool_mode)
			{
				fnt->put_string(local_image, x2(),y, g1_stat_counter.max_strs[i],context);
			}
			else
			{
				fnt->put_string(local_image, x2(), y, g1_stat_counter.strs[i], context);
			}
			y+=fnt->height(g1_stat_counter.strs[i]);
		}
		//}

		g1_stat_counter.reset();

	}

	void receive_event(i4_event * ev)
	{
		if (ev->type()==i4_event::USER_MESSAGE) // update statics
		{
			request_redraw();
			i4_user_message_event_class poll(0);
			poll_id=i4_time_dev.request_event(this, &poll, 2000); // update once every 2 secs
		}
	}

	~g1_stat_win_class()
	{
		i4_time_dev.cancel_event(poll_id);
	}

	void name(char * buffer)
	{
		static_name(buffer,"stat_win");
	}
};

void g1_statistics_counter_class::show()
{
	if (!g1_stat_win.get())
	{
		i4_graphical_style_class * style=i4_current_app->get_style();
		i4_window_class * swin = new g1_stat_win_class(style);

		int wpos=i4_current_app->get_root_window()->width() - swin->width()-20;
		g1_stat_win = style->create_mp_window(wpos, 20, swin->width(), swin->height(),
											  i4gets("stat_win_title"));

		g1_stat_win->add_child(0, 0, swin);
	}
}
