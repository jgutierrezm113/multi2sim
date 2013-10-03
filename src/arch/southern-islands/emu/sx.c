/*
 *  Multi2Sim
 *  Copyright (C) 2012  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <lib/mhandle/mhandle.h>
#include <lib/util/debug.h>
#include <lib/util/list.h>
#include <arch/southern-islands/emu/work-item.h>
#include <driver/opengl/si-pa.h>

#include "sx.h"

/*
 * Private Functions
 */

static void SISXPositionCreate(SISX *self)
{
	int i;
	for (i = 0; i < SI_POS_COUNT; ++i)
	{
		self->pos[i] = list_create();
	}
}

static void SISXPositionDestroy(SISX *self)
{
	int i;
	int j;
	float *pos;

	/* Free */
	for (i = 0; i < SI_POS_COUNT; ++i)
	{
		LIST_FOR_EACH(self->pos[i], j)
		{
			pos = list_get(self->pos[i], j);
			free(pos);
		}
		list_free(self->pos[i]);
	}

}

static void SISXPosReset(SISX *self)
{
	SISXPositionDestroy(self);
	SISXPositionCreate(self);
}

static void SISXParamCreate(SISX *self)
{
	int i;
	for (i = 0; i < SI_PARAM_COUNT; ++i)
	{
		self->param[i] = list_create();
	}
}

static void SISXParamDestroy(SISX *self)
{
	int i;
	int j;
	float *param;

	/* Free */
	for (i = 0; i < SI_PARAM_COUNT; ++i)
	{
		LIST_FOR_EACH(self->param[i], j)
		{
			param = list_get(self->param[i], j);
			free(param);
		}
		list_free(self->param[i]);
	}

}

static void SISXParamReset(SISX *self)
{
	SISXParamDestroy(self);
	SISXParamCreate(self);
}

static SISXPSInitMeta *SISXPSInitMetaCreate(float brctrc_i, float brctrc_j)
{
	SISXPSInitMeta *meta;

	/* Allocate */
	meta = xcalloc(1, sizeof(SISXPSInitMeta));

	/* Initialize */
	meta->brctrc_i = brctrc_i;
	meta->brctrc_j = brctrc_j;

	/* Return */
	return meta;
}

static void SISXPSInitMetaDestroy(SISXPSInitMeta *meta)
{
	free(meta);
}

static void SISXPSInitCreate(SISX *self)
{
	self->ps_init_meta = list_create();
}

static void SISXPSInitDestroy(SISX *self)
{
	SISXPSInitMeta *meta;
	int i;

	LIST_FOR_EACH(self->ps_init_meta, i)
	{
		meta = list_get(self->ps_init_meta, i);
		SISXPSInitMetaDestroy(meta);
	}
	list_free(self->ps_init_meta);
}

static void SISXPSInitReset(SISX *self)
{
	SISXPSInitDestroy(self);
	SISXPSInitCreate(self);
}

/*
 * Public Functions
 */

void SISXCreate(SISX *self, SIEmu *emu)
{
	/* Initialize */
	self->emu = emu;
	SISXPositionCreate(self);
	SISXParamCreate(self);
	SISXPSInitCreate(self);
}

void SISXDestroy(SISX *self)
{
	/* Free */
	SISXPositionDestroy(self);
	SISXParamDestroy(self);
	SISXPSInitDestroy(self);
}

void SISXReset(SISX *self)
{
	/* Reset */
	SISXPosReset(self);
	SISXParamReset(self);
	SISXPSInitReset(self);
}

void SISXExportPosition(SISX *self, unsigned int target, unsigned int id, 
	float x, float y, float z, float w)
{
	struct list_t *pos_lst;
	float *pos;

	pos = xcalloc(1, 4 * sizeof(float));

	pos[0] = (float)x;
	pos[1] = (float)y;
	pos[2] = (float)z;
	pos[3] = (float)w;

	pos_lst = self->pos[target];
	list_insert(pos_lst, id, pos);
}


void SISXExportParam(SISX *self, unsigned int target, unsigned int id, 
	float x, float y, float z, float w)
{
	struct list_t *param_lst;
	float *param;

	param = xcalloc(1, 4 * sizeof(float));

	param[0] = (float)x;
	param[1] = (float)y;
	param[2] = (float)z;
	param[3] = (float)w;

	param_lst = self->param[target];
	list_insert(param_lst, id, param);
}

void SISXPSInitMetaAdd(SISX *self, float brctrc_i, float brctrc_j)
{
	struct list_t *meta_list;
	SISXPSInitMeta *meta;

	meta_list = self->ps_init_meta;
	meta = SISXPSInitMetaCreate(brctrc_i, brctrc_j);
	list_enqueue(meta_list, meta);
}