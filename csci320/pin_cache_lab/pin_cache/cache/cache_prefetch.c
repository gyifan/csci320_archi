/**
 * Cache simulation using a functional system simulator.
 *
 * Course: Advanced Computer Architecture, Uppsala University
 * Course Part: Lab assignment 1
 *
 * Original authors: UART 1.0(?)
 * Modified by: Andreas Sandberg <andreas.sandberg@it.uu.se>
 *
 * $Id: cache.c 14 2011-08-24 09:55:20Z ansan501 $
 */

#include "cache.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>


#define AVDC_MALLOC(nelems, type) malloc(nelems * sizeof(type))
#define AVDC_FREE(p) free(p)
#define LRU	0
#define RANDOM	1
#define FIFO	2

/**
 * Cache block information.
 *
 * HINT: You will probably need to change this structure
 */
struct avdc_cache_line {
	avdc_tag_t tag;
	int        valid;

	// Question 3 implementing associativity
	int counter;	
};

/**
 * Extract the cache line tag from a physical address.
 *
 * You probably don't want to change this function, instead you may
 * want to change how the tag_shift field is calculated in
 * avdc_resize().
 */
	static inline avdc_pa_t
tag_from_pa(avdark_cache_t *self, avdc_pa_t pa)
{
	return pa >> self->tag_shift;
}

/**
 * Calculate the cache line index from a physical address.
 *
 * Feel free to experiment and change this function
 */
	static inline int
index_from_pa(avdark_cache_t *self, avdc_pa_t pa)
{

	
//	   printf("self->block_size_log2 = %d, self->number_of_sets-1 = %x\n", self->block_size_log2, self->number_of_sets-1);
//	   fflush(stdout);
	
	return (pa >> self->block_size_log2) & (self->number_of_sets - 1);
}

/**
 * Computes the log2 of a 32 bit integer value. Used in dc_init
 *
 * Do NOT modify!
 */
	static int
log2_int32(uint32_t value)
{
	int i;

	for (i = 0; i < 32; i++) {
		value >>= 1;
		if (value == 0)
			break;
	}
	return i;
}

/**
 * Check if a number is a power of 2. Used for cache parameter sanity
 * checks.
 *
 * Do NOT modify!
 */
	static int
is_power_of_two(uint64_t val)
{
	return ((((val)&(val-1)) == 0) && (val > 0));
}

	void
avdc_dbg_log(avdark_cache_t *self, const char *msg, ...)
{
	va_list ap;

	if (self->dbg) {
		const char *name = self->dbg_name ? self->dbg_name : "AVDC";
		fprintf(stderr, "[%s] dbg: ", name);
		va_start(ap, msg);
		vfprintf(stderr, msg, ap);
		va_end(ap);
	}
}


	void
avdc_access(avdark_cache_t *self, avdc_pa_t pa, avdc_access_type_t type)
{
	/* HINT: You will need to update this function */
	avdc_tag_t tag = tag_from_pa(self, pa);
	int index = index_from_pa(self, pa);
	int hit = 0;
	int i;

	avdc_cache_line_t* tempLine = NULL;
	avdc_cache_line_t* tempReplace = NULL;
	avdc_cache_line_t* tempLRU = NULL;
	int lineIndex = -1;
	int tempMax = -1;
	int lru = -1; // Index of least recent used
	int mru = -1; // Index of most recent used
	int addEntry = 0;

	int* temp;

	switch (self->policy) {
		case LRU:
	// Variables LRU
	/* DEBUG */
	if(self->dbg){
		printf("********************************************************** index: %d\n", index);
	}
	i = 0;
	while (i<self->assoc){
		
		lineIndex = index*self->assoc+i;
		tempLine = &self->lines[lineIndex];
		temp = &tempLine->counter;
		if(tempLine->valid){
			
			if(tempMax < *temp){
				tempMax = *temp;
				lru = i;
				tempLRU = tempLine;
			}

			// If no replacement found and the tag matches, 
			// record it as a hit.
			//if(NULL == tempReplace && tempLine->tag == tag){
			if(tempLine->tag == tag){
				/* DEBUG */	
				if(self->dbg){
					printf("It is a Hit!! at %d\n",i);
				}
				tempReplace = tempLine;
				hit = 1;
				*temp = 0;
			}else
				*temp = *temp + 1;
		}else{
			// If no replace found and there is available invalid entry,
			// Make that as replacement, increment the tempMax.
			if(NULL == tempReplace){
				/* DEBUG */	
				if(self->dbg){
					printf("Replace invalid line! at %d\n",i);
				}
				tempReplace = tempLine;
				addEntry = 1;
			}
		}
		i ++;
		/* DEBUG */	
		if(self->dbg){
			printf("==========================================");
			printf("i = %d, self->assoc = %d, tempLine->counter = %d\n", i, self->assoc, tempLine->counter);
			printf("tempMax = %d, lru = %d, mru = %d\n", tempMax, lru, mru);
			fflush(stdout);
		}
	}
	if(NULL == tempReplace){
		/* DEBUG */	
		if(self->dbg){
			printf("Replace the entry at %d\n", lru);
		}
		tempReplace = tempLRU;
	}
	/* DEBUG */	
	if(self->dbg){
		printf("mru = %d; tempMax = %d; lineIndex = %d\n", mru, tempMax, lineIndex);
		//		printf("tempReplace->tag = %d \n", tempReplace->tag);  
		fflush(stdout);
	}

	// Update the cache line.
	tempReplace->valid = 1;
	tempReplace->counter = 0;
	tempReplace->tag = tag;

		break;
		case RANDOM:
	

	for (i=0;i<self->assoc;i++){

		lineIndex = index*self->assoc+i;
		tempLine = &self->lines[index*self->assoc+i];
		if(tempLine->valid){
			// If no replacement found and the tag matches, 
			// record it as a hit.
			//if(NULL == tempReplace && tempLine->tag == tag){
			if(tempLine->tag == tag){
				/* DEBUG */	
				if(self->dbg){
					printf("It is a Hit!! at %d\n",i);
				}
				tempReplace = tempLine;
				hit = 1;
			}
		}else{
			if(NULL == tempReplace){
				/* DEBUG */	
				if(self->dbg){
					printf("Replace invalid line! at %d\n",i);
				}
				tempReplace = tempLine;
				addEntry = 1;
			}
		}
		/* DEBUG */	
		if(self->dbg){
			printf("==========================================");
			printf("i = %d, self->assoc = %d, tempLine->counter = %d\n", i, self->assoc, tempLine->counter);
			printf("tempMax = %d, lru = %d, mru = %d\n", tempMax, lru, mru);
			fflush(stdout);
		}
	}
	if(NULL == tempReplace){
		lru = rand() % self->assoc;
		/* DEBUG */	
		if(self->dbg){
			printf("Replace the entry at %d\n", lru);
		}
		tempReplace = &self->lines[index*self->assoc+lru];
	}
	/* DEBUG */	
	if(self->dbg){
		printf("mru = %d; tempMax = %d; lineIndex = %d\n", mru, tempMax, lineIndex);
		//		printf("tempReplace->tag = %d \n", tempReplace->tag);  
		fflush(stdout);
	}

	// Update the cache line.
	tempReplace->valid = 1;
	tempReplace->counter = 0;
	tempReplace->tag = tag;
		break;
		case FIFO:
	
//	printf("index: %d\n", index);
	for (i=0;i<self->assoc;i++){

		lineIndex = index*self->assoc+i;
		tempLine = &self->lines[index*self->assoc+i];
		if(tempLine->valid){
			// If no replacement found and the tag matches, 
			// record it as a hit.
			//if(NULL == tempReplace && tempLine->tag == tag){
			if(tempLine->tag == tag){
				/* DEBUG */	
				if(self->dbg){
					printf("It is a Hit!! at %d\n",i);
				}
				tempReplace = tempLine;
				hit = 1;
			}
		}else{
			// If no replace found and there is available invalid entry,
			// Make that as replacement, increment the tempMax.
			if(NULL == tempReplace){
				/* DEBUG */	
				if(self->dbg){
					printf("Replace invalid line! at %d\n",i);
				}
				tempReplace = tempLine;
				addEntry = 1;
			}
		}
	}
	if(NULL == tempReplace){
		/* DEBUG */	
		if(self->dbg){
			printf("Replace the entry at %d\n", self->index[index]);
		}
		tempReplace = &self->lines[index*self->assoc+self->index[index]];
		self->index[index] = (self->index[index] + 1) % self->assoc;
	}

	// Update the cache line.
	tempReplace->valid = 1;
	tempReplace->counter = 0;
	tempReplace->tag = tag;
			
		break;
	}

	switch (type) {
		case AVDC_READ: /* Read accesses */
			avdc_dbg_log(self, "read: pa: 0x%.16lx, tag: 0x%.16lx, index: %d, hit: %d\n",
					(unsigned long)pa, (unsigned long)tag, index, hit);
			self->stat_data_read += 1;
			if (!hit)
				self->stat_data_read_miss += 1;
			break;

		case AVDC_WRITE: /* Write accesses */
			avdc_dbg_log(self, "write: pa: 0x%.16lx, tag: 0x%.16lx, index: %d, hit: %d\n",
					(unsigned long)pa, (unsigned long)tag, index, hit);
			self->stat_data_write += 1;
			if (!hit)
				self->stat_data_write_miss += 1;
			break;
	}

	if(hit){

//	printf("Prefetching!!!!!!!!!!!!!========================******************\n");	
	avdc_pa_t pa_n = pa + 64;
	tag = tag_from_pa(self, pa_n);
	index = index_from_pa(self, pa);

	tempLine = NULL;
	tempReplace = NULL;
	tempLRU = NULL;
	lineIndex = -1;
	tempMax = -1;
	lru = -1; // Index of least recent used
	mru = -1; // Index of most recent used
	addEntry = 0;

	switch (self->policy) {
		case LRU:
	// Variables LRU
	/* DEBUG */
	if(self->dbg){
		printf("********************************************************** index: %d\n", index);
	}
	i = 0;
	while (i<self->assoc){
		
		lineIndex = index*self->assoc+i;
		tempLine = &self->lines[lineIndex];
		temp = &tempLine->counter;
		if(tempLine->valid){
			
			if(tempMax < *temp){
				tempMax = *temp;
				lru = i;
				tempLRU = tempLine;
			}

			// If no replacement found and the tag matches, 
			// record it as a hit.
			//if(NULL == tempReplace && tempLine->tag == tag){
			if(tempLine->tag == tag){
				/* DEBUG */	
				if(self->dbg){
					printf("It is a Hit!! at %d\n",i);
				}
				tempReplace = tempLine;
				*temp = 0;
			}else
				*temp = *temp + 1;
		}else{
			// If no replace found and there is available invalid entry,
			// Make that as replacement, increment the tempMax.
			if(NULL == tempReplace){
				/* DEBUG */	
				if(self->dbg){
					printf("Replace invalid line! at %d\n",i);
				}
				tempReplace = tempLine;
				addEntry = 1;
			}
		}
		i ++;
		/* DEBUG */	
		if(self->dbg){
			printf("==========================================");
			printf("i = %d, self->assoc = %d, tempLine->counter = %d\n", i, self->assoc, tempLine->counter);
			printf("tempMax = %d, lru = %d, mru = %d\n", tempMax, lru, mru);
			fflush(stdout);
		}
	}
	if(NULL == tempReplace){
		/* DEBUG */	
		if(self->dbg){
			printf("Replace the entry at %d\n", lru);
		}
		tempReplace = tempLRU;
	}
	/* DEBUG */	
	if(self->dbg){
		printf("mru = %d; tempMax = %d; lineIndex = %d\n", mru, tempMax, lineIndex);
		//		printf("tempReplace->tag = %d \n", tempReplace->tag);  
		fflush(stdout);
	}

	// Update the cache line.
	tempReplace->valid = 1;
	tempReplace->counter = 0;
	tempReplace->tag = tag;

		break;
		case RANDOM:
	

	for (i=0;i<self->assoc;i++){

		lineIndex = index*self->assoc+i;
		tempLine = &self->lines[index*self->assoc+i];
		if(tempLine->valid){
			// If no replacement found and the tag matches, 
			// record it as a hit.
			//if(NULL == tempReplace && tempLine->tag == tag){
			if(tempLine->tag == tag){
				/* DEBUG */	
				if(self->dbg){
					printf("It is a Hit!! at %d\n",i);
				}
				tempReplace = tempLine;
			}
		}else{
			if(NULL == tempReplace){
				/* DEBUG */	
				if(self->dbg){
					printf("Replace invalid line! at %d\n",i);
				}
				tempReplace = tempLine;
				addEntry = 1;
			}
		}
		/* DEBUG */	
		if(self->dbg){
			printf("==========================================");
			printf("i = %d, self->assoc = %d, tempLine->counter = %d\n", i, self->assoc, tempLine->counter);
			printf("tempMax = %d, lru = %d, mru = %d\n", tempMax, lru, mru);
			fflush(stdout);
		}
	}
	if(NULL == tempReplace){
		lru = rand() % self->assoc;
		/* DEBUG */	
		if(self->dbg){
			printf("Replace the entry at %d\n", lru);
		}
		tempReplace = &self->lines[index*self->assoc+lru];
	}
	/* DEBUG */	
	if(self->dbg){
		printf("mru = %d; tempMax = %d; lineIndex = %d\n", mru, tempMax, lineIndex);
		//		printf("tempReplace->tag = %d \n", tempReplace->tag);  
		fflush(stdout);
	}

	// Update the cache line.
	tempReplace->valid = 1;
	tempReplace->counter = 0;
	tempReplace->tag = tag;
		break;
		case FIFO:
	
//	printf("index: %d\n", index);
	for (i=0;i<self->assoc;i++){

		lineIndex = index*self->assoc+i;
		tempLine = &self->lines[index*self->assoc+i];
		if(tempLine->valid){
			// If no replacement found and the tag matches, 
			// record it as a hit.
			//if(NULL == tempReplace && tempLine->tag == tag){
			if(tempLine->tag == tag){
				/* DEBUG */	
				if(self->dbg){
					printf("It is a Hit!! at %d\n",i);
				}
				tempReplace = tempLine;
			}
		}else{
			// If no replace found and there is available invalid entry,
			// Make that as replacement, increment the tempMax.
			if(NULL == tempReplace){
				/* DEBUG */	
				if(self->dbg){
					printf("Replace invalid line! at %d\n",i);
				}
				tempReplace = tempLine;
				addEntry = 1;
			}
		}
	}
	if(NULL == tempReplace){
		/* DEBUG */	
		if(self->dbg){
			printf("Replace the entry at %d\n", self->index[index]);
		}
		tempReplace = &self->lines[index*self->assoc+self->index[index]];
		self->index[index] = (self->index[index] + 1) % self->assoc;
	}

	// Update the cache line.
	tempReplace->valid = 1;
	tempReplace->counter = 0;
	tempReplace->tag = tag;
			
		break;
	}
		
	}
	/* DEBUG */
//	if(self->dbg){
//		printf("Read miss: %d; write miss: %d\n", self->stat_data_read_miss, self->stat_data_write_miss);
//	}
}

	void
avdc_flush_cache(avdark_cache_t *self)
{
	int i;
	/* HINT: You will need to update this function */
	for (i = 0; i < self->number_of_sets * self->assoc; i++) {
		self->lines[i].valid = 0;
		self->lines[i].tag = 0;
		self->lines[i].counter = 0;
	}
	for (i=0; i<self->number_of_sets; i++)
		self->index[i] = 0;
}


	int
avdc_resize(avdark_cache_t *self,
		avdc_size_t size, avdc_block_size_t block_size, avdc_assoc_t assoc, avdc_policy_t policy)
{
	/* HINT: This function precomputes some common values and
	 * allocates the self->lines array. You will need to update
	 * this to reflect any changes to how this array is supposed
	 * to be allocated.
	 *
	 */

	/* Verify that the parameters are sane */
	if (!is_power_of_two(size) ||
			!is_power_of_two(block_size) ||
			!is_power_of_two(assoc)) {
		fprintf(stderr, "size, block-size and assoc all have to be powers of two and > zero\n");
		return 0;
	}

	/* Update the stored parameters */
	self->size = size;
	self->block_size = block_size;
	self->assoc = assoc;
	self->policy = policy;

	/* Cache some common values */
	self->number_of_sets = (self->size / self->block_size) / self->assoc;
	self->block_size_log2 = log2_int32(self->block_size);
	self->tag_shift = self->block_size_log2 + log2_int32(self->number_of_sets);

	/* (Re-)Allocate space for the tags array */
	if (self->lines)
		AVDC_FREE(self->lines);
	/* HINT: If you change this, you may have to update
	 * avdc_delete() to reflect changes to how thie self->lines
	 * array is allocated. */
	self->lines = AVDC_MALLOC(self->number_of_sets*self->assoc, avdc_cache_line_t);
	self->index = AVDC_MALLOC(self->number_of_sets, int);

	int j;
	for (j=0; j<self->number_of_sets; j++)
		self->index[j] = 0;
	/* Flush the cache, this initializes the tag array to a known state */
	avdc_flush_cache(self);

	return 1;
}

	void
avdc_print_info(avdark_cache_t *self)
{
	fprintf(stderr, "Cache Info\n");
	fprintf(stderr, "size: %d, assoc: %d, line-size: %d\n",
			self->size, self->assoc, self->block_size);
}

	void
avdc_print_internals(avdark_cache_t *self)
{
	int i;

	fprintf(stderr, "Cache Internals\n");
	fprintf(stderr, "size: %d, assoc: %d, line-size: %d\n",
			self->size, self->assoc, self->block_size);

	for (i = 0; i < self->number_of_sets; i++){
		for(i=0; i<self->assoc; i++)
			fprintf(stderr, "tag: <0x%.16lx> valid: %d\n",
					(long unsigned int)self->lines[i].tag,
					self->lines[i].valid);
	}
}

	void
avdc_reset_statistics(avdark_cache_t *self)
{
	self->stat_data_read = 0;
	self->stat_data_read_miss = 0;
	self->stat_data_write = 0;
	self->stat_data_write_miss = 0;
}

	avdark_cache_t *
avdc_new(avdc_size_t size, avdc_block_size_t block_size,
		avdc_assoc_t assoc, avdc_policy_t policy)
{
	avdark_cache_t *self;

	self = AVDC_MALLOC(1, avdark_cache_t);

	memset(self, 0, sizeof(*self));
	self->dbg = 0;

	if (!avdc_resize(self, size, block_size, assoc, policy)) {
		AVDC_FREE(self);
		return NULL;
	}

	return self;
}

	void
avdc_delete(avdark_cache_t *self)
{
	if (self->lines)
		AVDC_FREE(self->lines);

	AVDC_FREE(self);
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * indent-tabs-mode: nil
 * c-file-style: "linux"
 * compile-command: "make -k -C ../../"
 * End:
 */
