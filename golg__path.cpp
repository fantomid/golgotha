/********************************************************************** <BR>
   This file is part of Crack dot Com's free source code release of
   Golgotha. <a href="http://www.crack.com/golgotha_release"> <BR> for
   information about compiling & licensing issues visit this URL</a>
   <PRE> If that doesn't help, contact Jonathan Clark at
   golgotha_source@usa.net (Subject should have "GOLG" in it)
 ***********************************************************************/
#include "pch.h"
#include "path.h"
#include "memory/malloc.h"
#include "g1_limits.h"
#include "saver_id.h"
#include "global_id.h"
//#include <string.h>

g1_path_manager_class g1_path_manager;

enum {
	VERSION=1
};

class g1_path_class
{
protected:
	class point
	{
public:
		i4_float x,y;
	} *node;

	w8 nodes;
	w8 current;
public:
	i4_bool get_pos(int n, i4_float &x, i4_float &y) const
	{
		int i=current+n;

		if (i<0 || i>=nodes)
		{
			return i4_F;
		}

		x = node[i].x;
		y = node[i].y;
		return i4_T;
	}
	i4_float x() const
	{
		return node[current].x;
	}
	i4_float y() const
	{
		return node[current].y;
	}

	i4_bool last() const
	{
		return current == nodes-1;
	}

	void alloc()
	{
		if (nodes>0)
		{
			node = (point *)I4_MALLOC(sizeof(point)*(nodes), "alloc_path");
		}
		else
		{
			node = 0;
		}
	}

	g1_path_class(w32 t_positions, i4_float * positions)
	{
		nodes = (w8)t_positions;
		current = 0;
		alloc();
		for (w32 i=0; i<nodes; i++)
		{
			//find_path returns paths in reverse order. this will fix that
			node[nodes-1-i].x = *positions++;
			node[nodes-1-i].y = *positions++;
		}
	}

	g1_path_class(g1_loader_class * fp)
	{
		w16 ver=0,data_size=0;


		if (fp)
		{
			ver=fp->read_16();
			I4_ASSERT(ver==VERSION,"ERROR: Unknown version of path data encountered");
			data_size=fp->read_16();
			nodes = fp->read_8();
			current = fp->read_8();
			alloc();
			for (w8 i=0; i<nodes; i++)
			{
				node[i].x = fp->read_float();
				node[i].y = fp->read_float();
			}

		}
		else
		{
			nodes = 0;
			current = 0;
		}
	}

	void save(g1_saver_class * fp)
	{
		//Since this is saved trough an object, we must not use start_version/end_version,
		//because that mechanism does not support recursion.
		//fp->start_version(VERSION);
		fp->write_16(VERSION);
		fp->write_16(nodes*8+6); //Number of bytes
		fp->write_8(nodes);
		fp->write_8(current);
		for (w8 i=0; i<nodes; i++)
		{
			fp->write_float(node[i].x);
			fp->write_float(node[i].y);
		}
		//fp->end_version();
	}

	~g1_path_class()
	{
		if (node)
		{
			//delete node;
			i4_free(node);
		}
	}

	void advance_path()
	{
		current++;
	}
};

g1_path_handle g1_path_manager_class::alloc_path(w32 t_positions,
												 i4_float * positions)
{
	return new g1_path_class(t_positions,positions);
}

void g1_path_manager_class::get_position(g1_path_handle path, i4_float &x, i4_float &y)
{
	x=path->x();
	y=path->y();
}

i4_bool g1_path_manager_class::get_nth_position(g1_path_handle path, int n,
												i4_float &x, i4_float &y)
{
	if (!path)
	{
		return i4_F;
	}
	return path->get_pos(n,x,y);
}

//The caller is supposed to do something like
//path=g1_path_manager->advance_path(path)
//and test if path is still valid afterwards
g1_path_handle g1_path_manager_class::advance_path(g1_path_handle path)
{
	if (!path)
	{
		return 0;
	}
	if (path->last())
	{
		delete path;
		return 0;
	}

	path->advance_path();
	return path;
}


i4_bool g1_path_manager_class::is_last_path_point(g1_path_handle path)
{
	return path->last();
}

void g1_path_manager_class::free_path(g1_path_handle &path)
{
	delete path;
}

void g1_path_manager_class::save(g1_saver_class * fp, g1_path_handle path)
{
	path->save(fp);
}

g1_path_handle g1_path_manager_class::load(g1_loader_class * fp)
{
	return new g1_path_class(fp);
}
