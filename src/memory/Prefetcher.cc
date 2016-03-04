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


    //JGMonge: TODO FIXME!!!
    /*
    Missing things to do:
    Add prefetcher function calls in system events.
    Create prefetcher functions correctly
    Make sure connection is correct and working
    		/// JGMonge: Get the block prefetched variable
		// FIXME: Need to implement this correctly in SystemEvents (set variable prefetch in a block)
		unsigned getPrefetch() const { return prefetch; } 
		
    Add prefetcher statistics
    
    */   
/*
    if (type != PrefetcherAlways || type != PrefetcherMiss)
	{
	    assert(ghb_size >= 1 && it_size >= 1);
    	ghb_head = -1;
    }
    */

namespace mem
{

/// Constructor
///	Based on the prefetcher type, variables are created accordingly. 
Prefetcher::Prefetcher(PrefetcherType type,
		int global_history_buffer_lookup_depth,
		int global_history_buffer_size,
		int global_history_buffer_index_table_size)
		: 
		type(type),
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

void Prefetcher::prefetcher_do_prefetch (esim::Frame *esim_frame, 
		unsigned int prefetch_address)
{
	/*int set1, tag1, set2, tag2;*/

	Frame *frame = misc::cast<Frame *>(esim_frame);
	
	Module *module = frame->getModule();
	//Module *target_module = frame->target_module;
	//Cache *cache = module->getCache();
	
	assert(prefetch_address > 0);
    
    /*FIXME: All of this has to change*/
    /*
    * Predicted prefetch_addr can go horribly wrong
	 * sometimes. Since prefetches aren't supposed to
	 * cause any kind of faults/exceptions, return. */
	/*if (!mod_serves_address(mod, prefetch_addr))
	{
		mem_debug("  miss_addr 0x%x, prefetch_addr 0x%x, %s : illegal prefetch\n", stack->addr,
			  prefetch_addr, mod->name);
		return;
	}*/

	/* If the prefetch_addr is in the same block as the missed address
	 * there is no point in prefetching. One scenario where this may
	 * happen is when we see a stride smaller than block size because
	 * of an eviction between the two accesses. */
	 /* This check should be grabbed in later stage as a useless prefetch*/

	/* I'm not passing back the mod_client_info structure. If this needs to be 
	 * passed in the future, make sure a copy is made (since the one that is
	 * pointed to by stack->client_info may be freed early. */
	/*mem_debug("  miss_addr 0x%x, prefetch_addr 0x%x, %s : prefetcher\n", stack->addr,
		  prefetch_addr, mod->name);

Module::Access(AccessType access_type,
		unsigned address,
		int *witness,
		esim::Event *return_event)
    EventPrefetchHandler(esim::Event *event, esim::Frame *esim_frame)
	mod_access(mod, mod_access_prefetch, prefetch_addr, NULL, NULL, NULL, NULL);*/
	
	// Record access
	//module->StartAccess(frame, Module::AccessLoad);
	module->Access(Module::AccessPrefetch, prefetch_address, NULL, NULL);
}

void Prefetcher::prefetcher_access_miss (esim::Frame *esim_frame)
{

}

void Prefetcher::prefetcher_access_hit  (esim::Frame *esim_frame)
{
	Frame *frame = misc::cast<Frame *>(esim_frame);
	
	Module *module = frame->getModule();
	//Module *target_module = frame->target_module;
	Cache *cache = module->getCache();
	
	if (cache != NULL) {
		switch (cache->getPrefetcherType()){
			case Empty:
				break;
			case ConstantStrideGlobalHistoryBuffer: break;
			case DeltaCorrelationGlobalHistoryBuffer: break;
			case Miss: break;
			case Always: {
				unsigned int prefetch_address = frame->getAddress() 
					+ (this->getGlobalHistoryBufferLookupDepth()
					 - 1)*module->getBlockSize();
				if (prefetch_address > 0)
					prefetcher_do_prefetch(frame,
						prefetch_address);
				break;
			}
			default:
				printf("Illegal.");
		}
	}
	
}
			
int Prefetcher::prefetcherUpdateTables (esim::Frame *frame)
{
	return 0;
}

}

