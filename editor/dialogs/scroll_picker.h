/********************************************************************** <BR>
   This file is part of Crack dot Com's free source code release of
   Golgotha. <a href="http://www.crack.com/golgotha_release"> <BR> for
   information about compiling & licensing issues visit this URL</a>
   <PRE> If that doesn't help, contact Jonathan Clark at
   golgotha_source@usa.net (Subject should have "GOLG" in it)
 ***********************************************************************/

#ifndef G1_SCROLL_PICKER_HH
#define G1_SCROLL_PICKER_HH


#include "window/window.h"
#include "window/colorwin.h"
#include "memory/array.h"
#include "render/r1_win.h"

class i4_graphical_style_class;
class i4_scroll_bar;
class i4_menu_item_class;

struct g1_scroll_picker_info
{
	int scroll_offset;
	int object_size;
	int max_object_size, min_object_size;
	int max_objects_down;
	int win_x, win_y;


	g1_scroll_picker_info() {
		scroll_offset=0;
		object_size=64;
		max_object_size=128;
		min_object_size=16;
		max_objects_down=3;
		win_x=win_y=-1;
	}
};

class g1_scroll_picker_class :
	public i4_color_window_class
{
protected:
	i4_array<i4_menu_item_class *> windows;
	i4_array<r1_render_window_class *> render_windows;

	int start_y, show_area_w;
	i4_scroll_bar * scroll_bar;
	g1_scroll_picker_info * info;



	// this should return 0 if scroll_object_num is too big
	virtual i4_menu_item_class *create_window(w16 w, w16 h, int scroll_object_num) = 0;
	virtual void change_window_object_num(i4_window_class * win, int new_scroll_object_num) = 0;
	virtual int total_objects() = 0;
	virtual void rotate()
	{
		;
	}
	virtual void mirror()
	{
		;
	}
	virtual void add(i4_str name)
	{
	}
	virtual i4_bool remove(i4_menu_item_class * window)
	{
		return i4_F;
	}
	// Return i4_T if something changed.
	virtual i4_bool edit(i4_menu_item_class * window)
	{
		return i4_F;
	}

public:
	enum {
		ROTATE, MIRROR, GROW, SHRINK, SCROLL, ADD, REMOVE, EDIT
	};

	void refresh(i4_bool list_has_changed=i4_F);
	void create_windows();
	// option_flags can include (1<<ROTATE) | (1<<MIRROR) | (1<<GROW) | (1<<SHRINK) | (1<<SCROLL))
	g1_scroll_picker_class(i4_graphical_style_class * style,
						   w32 option_flags,
						   g1_scroll_picker_info * info,
						   int total_objects);

	void receive_event(i4_event * ev);
	void parent_draw(i4_draw_context_class &context);
	void change_current_select();
	void name(char * buffer)
	{
		static_name(buffer,"scroll_picker_class");
	}
};


#endif
