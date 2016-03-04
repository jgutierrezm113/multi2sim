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

#ifndef MEM_SYSTEM_PREFETCHER_H
#define MEM_SYSTEM_PREFETCHER_H

#include <lib/esim/Event.h>
#include <lib/esim/Queue.h>


// Used on GHB prefetcher: 
// Doesn't really make sense to have a big lookup depth
#define MAX_PREFETCHER_LOOKUP_DEPTH 4

enum PrefetcherType
{
	Invalid = 0,
	Empty,
	ConstantStrideGlobalHistoryBuffer,
	DeltaCorrelationGlobalHistoryBuffer,
	Always,
	Miss
};


namespace mem
{
 

class Prefetcher
{

private:

	// Identifies which prefetcher was selected by user
	PrefetcherType type = Empty;

	// GHB lookup depth
	int global_history_buffer_lookup_depth = 1;
	
	// Size of the array for the GHB.
	int global_history_buffer_size = 0;
    	
	// Size of the index table (array) for the GHB.
	int global_history_buffer_index_table_size = 0;
	
	// Object containing global address of the miss, and pointer to
	// next and previous element.
	class GlobalHistoryBufferElement
	{
	
	private:
		// The global address miss this entry corresponds to
		unsigned int address;
		
		// Next element in the linked list : -1 implies none 
		int next;
		
		// Previous element in the linked list : -1 implies none
		int previous;
	
	public:	
		/// Whether the previous element is a GHB entry or a pointer
		///	to the index table. 
		enum PrefetcherPointerType
		{
			PrefetcherPointerGlobalHistoryBuffer = 0,
			PrefetcherPointerIndexTable,
		} PreviousElementType;
		
		/// Get Address
		unsigned int getAddress () const { return address; }
		
		/// Get Next
		int getNext () const { return next; }
		
		/// Get Previous
		int getPrevious () const { return previous; }
		
		/// Get PreviousElementType
		PrefetcherPointerType getPreviousElementType () const 
		{ return PreviousElementType; }

		/// Set Address
		void setAddress (unsigned int value) { address = value; }
		
		/// Set Next
		void setNext (int value) { next = value; }
		
		/// Set Previous
		void setPrevious (int value) { previous = value; }
		
		/// Set PreviousElementType
		void setPreviousElementType (PrefetcherPointerType 
				value)  
		{ PreviousElementType = value; } 
	
	};

	// Object containing global address of the miss, and pointer to
	// next and previous element.
	class IndexTableElement
	{
	
	private:
		// The tag to compare to before indexing into the table
		unsigned int tag;
		
		// Pointer into the GHB. -1 implies no entry in GHB.
	   	int pointer;
	   	
	public:
		/// Get the way index of this block
		//unsigned getWayId() const { return way_id; }

		/// Get Tag
		unsigned int getTag() const { return tag; }
		
		/// Get Pointer
		int getPointer() const { return pointer; }
		
		/// Set Tag
		void setTag (unsigned int value) { tag = value; }
		
		/// Set Pointer
		void setPointer (int value) { pointer = value; }
	};
	
	// Global History Buffer
	std::unique_ptr<GlobalHistoryBufferElement[]> global_history_buffer;
	
	// Index Table
	std::unique_ptr<IndexTableElement[]> index_table;

public:
	/// Constructor
	Prefetcher(PrefetcherType prefetcher_type,
			int global_history_buffer_lookup_depth,
			int global_history_buffer_size,
			int global_history_buffer_index_table_size);
	
	/// Function that is called if the prefetcher will generate an 
	/// access to higher module based on the System Events.
	///
	/// \param esim_frame
	///	Has all the information of the data in motion.
	///
	/// \param prefetch_address
	///	Has the actual address that needs to be prefetched.
	void prefetcher_do_prefetch(esim::Frame *esim_frame, 
			unsigned int prefetch_address);
	
	/// Function in charge of deciding what to do with the access miss
	/// depending of the prefetcher type.
	///
	/// \param esim_frame
	///	Has all the information of the data in motion.
	void prefetcher_access_miss(esim::Frame *esim_frame);

	/// Function in charge of deciding what to do with the access hit
	/// depending of the prefetcher type.
	///
	/// \param esim_frame
	///	Has all the information of the data in motion.
	void prefetcher_access_hit(esim::Frame *esim_frame);


	/// Get Prefetcher Type
	///
	/// \return
	///	Returns the type of prefetcher in use on the module.
	PrefetcherType getPrefetcherType () const { return type; }

	/// Get Global History Buffer Lookup Depth
	///
	/// \return
	///	Returns the lookup depth set by the user for the Global
	///	History Buffer.	
	int getGlobalHistoryBufferLookupDepth () const
	{ return global_history_buffer_lookup_depth; }

	/// Get Global History Buffer Size
	///
	/// \return
	///	Returns the size of the Global History Buffer.
	int getGlobalHistoryBufferSize () const
	{ return global_history_buffer_size; }

	/// Get Global History Buffer Index Table Size
	///
	/// \return
	///	Returns the size of the Index Table for the Global 
	///	History Buffer.	
	int getGlobalHistoryBufferIndexTableSize () const
	{ return global_history_buffer_index_table_size; }

	/// Set Prefetcher Type
	///
	/// \param type
	///	Prefetcher Type for the module.
	void setPrefetcherType(PrefetcherType value) { type = value; }

	/// Set Global History Buffer Lookup Depth
	///
	///	\param global_history_buffer_lookup_depth
	///		The lookup depth that will be used for the GHB 
	///		algorithm.
	///
	/// 	\return
	///		NULL.	
	void setGlobalHistoryBufferLookupDepth ( int value ) 
	{ global_history_buffer_lookup_depth = value; }

	/// Set Global History Buffer Size
	///
	///	\param global_history_buffer_size
	///		Size of the GHB.
	///
	/// 	\return
	///		NULL.
	void setGlobalHistoryBufferSize ( int value ) 
	{ global_history_buffer_size = value; }

	/// Set Global History Buffer Index Table Size
	///
	///	\param global_history_buffer_index_table_size
	///		Size of the GHB Index Table.
	///
	/// 	\return
	///		NULL.	
	void setGlobalHistoryBufferIndexTableSize ( int value ) 
	{ global_history_buffer_index_table_size = value; }
		
	/// Prefetcher Update Tables
	///
	///	\param esim_frame
	///		Container of all the information related to the
	///		transaction
	///
	///	\return
	///		it_index >= 0 if any valid update is made, 
	///		negative otherwise.
	int prefetcherUpdateTables (esim::Frame *esim_frame);
	
};

}
#endif

