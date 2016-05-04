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
#include "Cache.h"
#include "Frame.h"
#include "System.h"	
#include "Prefetcher.h"


    //JGMonge Notes
    /*
	The prefetcher cant know which type it is? Has to be set in the
	constructor somehow.
	Should Prefetcher have different constructor and functions?
	Using inheritance?
	Create a blank prefetcher and then fill in the functions based on 
	which type was set?
    */


namespace mem
{

/// Constructor
///	Based on the prefetcher type, variables are created accordingly. 
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
	
	// Index Table	 creation		
	index_table = misc::new_unique_array
			<IndexTableElement>(global_history_buffer_index_table_size);

}

void Prefetcher::PrefetchAction (esim::Frame *esim_frame, 
		unsigned int prefetch_address)
{

	Frame *frame = misc::cast<Frame *>(esim_frame);
	
	Module *module = frame->getModule();
	
	assert(prefetch_address > 0);
    
	module->Access(Module::AccessPrefetch, prefetch_address, NULL, NULL);
}

void Prefetcher::AccessOnMiss (esim::Frame *esim_frame)
{
	Frame *frame = misc::cast<Frame *>(esim_frame);
	
	Module *module = frame->getModule();
	//Module *target_module = frame->target_module;
	Cache *cache = module->getCache();
	
	if (cache != NULL) {
		
		// FIXME: New prefetched address should be based on the type of prefetcher.
		unsigned int prefetch_address = frame->getAddress() 
			+ module->getBlockSize();
		if (prefetch_address > 0)
			PrefetchAction(frame, prefetch_address);
	}
	
}

void Prefetcher::AccessOnHit  (esim::Frame *esim_frame)
{
	Frame *frame = misc::cast<Frame *>(esim_frame);
	
	Module *module = frame->getModule();
	//Module *target_module = frame->target_module;
	Cache *cache = module->getCache();
	
	if (cache != NULL) {
		
		// FIXME: New prefetched address should be based on the type of prefetcher.
		unsigned int prefetch_address = frame->getAddress() 
			+ module->getBlockSize();
		if (prefetch_address > 0)
			PrefetchAction(frame, prefetch_address);
	}
	
}
			
int Prefetcher::UpdateTables (esim::Frame *frame)
{
	return 0;
}

}

