/*
 *  Multi2Sim
 *  Copyright (C) 2016  Julian Gutierrez (gutierrez.jul@husky.neu.edu)
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
 *  
 */

#include <lib/esim/Event.h>
#include <lib/esim/Queue.h>
#include <network/EndNode.h>

#include "Memory.h"
#include "Module.h"
#include "System.h"	
#include "Prefetcher.h"

namespace mem
{

Prefetcher::Prefetcher(
		int global_history_buffer_lookup_depth,
		int global_history_buffer_size,
		int global_history_buffer_index_table_size)
		: 
		global_history_buffer_lookup_depth(
			global_history_buffer_lookup_depth),
		global_history_buffer_size(
			global_history_buffer_size),
		global_history_buffer_index_table_size(
			global_history_buffer_index_table_size)
{

	// Global History Buffer creation
	global_history_buffer = misc::new_unique_array
			<GlobalHistoryBufferElement>(global_history_buffer_size);
	
	// Index Table creation		
	index_table = misc::new_unique_array
			<IndexTableElement>(global_history_buffer_index_table_size);

}

void Prefetcher::PrefetchAction (Module *module, unsigned int prefetch_address)
{
	assert(prefetch_address > 0);
    
	module->Access(Module::AccessPrefetch, prefetch_address, NULL, NULL);
}

void Prefetcher::AccessOnMiss (Module *module, unsigned int address)
{
	unsigned int prefetch_address = address	+ module->getBlockSize();
	if (prefetch_address > 0)
		PrefetchAction(module, prefetch_address);
}

void Prefetcher::AccessOnHit (Module *module, unsigned int address)
{
	unsigned int prefetch_address = address	+ module->getBlockSize();
	if (prefetch_address > 0)
		PrefetchAction(module, prefetch_address);
}
			
int Prefetcher::UpdateTables (Module *module, unsigned int address)
{
	return 0;
}

}

