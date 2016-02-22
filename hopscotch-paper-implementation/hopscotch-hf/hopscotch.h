/*
 * Hopscotch Hashing Implementation
 *
 * Author: Qiaobin Fu <qiaobinf@bu.edu>
 *
 * The implementation is based on the original paper:
 *  Herlihy, Maurice, Nir Shavit, and Moran Tzafrir. "Hopscotch hashing." 
 *  Distributed Computing. Springer Berlin Heidelberg, 2008. 350-364.
 * 
 * You can find the paper here: http://people.csail.mit.edu/shanir/publications/disc2008_submission_98.pdf
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "hash_function.h"

//#include <linux/types.h>
//#include <asm/byteorder.h>

#define HASH_FUNC_NUM 16
#define XIP_VXT_TABLE_SIZE 128 //XIP_VXT_TABLE_SIZE
#define HOP_RANGE 32
#define ADD_RANGE 128

int BUSY = -1;
int current_idx = 0;

typedef unsigned int xid_type_t;

/*---------------------------------*/

struct xip_vxt_entry{
	xid_type_t xid_type;

	int index;

	unsigned int hop_info;
};

struct xip_vxt_entry * B;

static uint (* hash_func[HASH_FUNC_NUM])(const unsigned char * str, uint len) = 
{BOB1, JSHash, OCaml, OAAT, PJWHash, RSHash,  SDBM, Simple, SML, STL,
    APHash, BKDR, DEKHash, DJBHash, FNV32, Hsieh};

int xt_to_vxt_rcu(xid_type_t ty);
void resize();
void find_closer_bucket(struct xip_vxt_entry **,int *);
int vxt_unregister_xidty(xid_type_t ty);

/* Insert function: insert the xid_type into the (map) hash table */
int vxt_register_xidty(xid_type_t ty,int reentrant) {
	//unsigned int hash = ty & (XIP_VXT_TABLE_SIZE - 1);
    unsigned int hash = hash_func[0]((const unsigned char *)&ty, sizeof(ty)) % XIP_VXT_TABLE_SIZE;
	
	struct xip_vxt_entry * start_bucket = &B[hash];
	
	struct xip_vxt_entry * free_bucket = start_bucket;
	int free_distance =0;
	
	for (free_distance = 0; free_distance < ADD_RANGE; ++free_distance) {
		if (0 == free_bucket->xid_type)
			break;
		
		free_bucket++;
	}

	if (free_distance < ADD_RANGE) {
		do{
			if (free_distance < HOP_RANGE) {
				printf("Find the location free_distance = %d, hop_info = %d!\n", free_distance, start_bucket->hop_info);
				start_bucket->hop_info |= (1 << free_distance);
				free_bucket->xid_type = ty;
				free_bucket->index = current_idx++;

				printf("The new hop info = %d\n", start_bucket->hop_info);

				return free_bucket->index;
			}

			find_closer_bucket(&free_bucket, &free_distance);
		} while( NULL!= free_bucket);
	}

	printf("There is no empty space left, need to resize!!!\n");

	resize();

	return vxt_register_xidty(ty,0);
}

void find_closer_bucket(struct xip_vxt_entry **free_bucket, int * free_distance) {
	printf("In find_closer_bucket()!\n");

	struct xip_vxt_entry * move_bucket = *free_bucket - (HOP_RANGE -1);
	
	if (move_bucket < (&B[0])) {
		printf("Impossible!!!\n");
		return;
		move_bucket = &B[0];
	}
	
	int free_dist;
	for(free_dist = (HOP_RANGE -1); free_dist > 0; --free_dist) {
		unsigned start_hop_info = move_bucket->hop_info;
		int move_free_distance = -1;
		unsigned int mask = 1;
		int i;
		for(i = 0; i < free_dist; ++i, mask <<= 1) {
			if (mask & start_hop_info) {
				move_free_distance = i;
				printf("move_free_distance = %d\n", i);
				break;
			}
		}

		if (-1 != move_free_distance) {
			if (start_hop_info ==  move_bucket->hop_info) {
				struct xip_vxt_entry * new_free_bucket = move_bucket + move_free_distance;
				move_bucket->hop_info |= (1 << free_dist);
				(*free_bucket)->xid_type = new_free_bucket->xid_type;
				(*free_bucket)->index = new_free_bucket->index;
				//++(move_bucket->timestamp);
				new_free_bucket->xid_type = BUSY;
				new_free_bucket->index = BUSY;
				move_bucket->hop_info &= ~(1 << move_free_distance);

				*free_bucket = new_free_bucket;
				*free_distance -=free_dist;

				printf("New free_distance = %d\n", *free_distance);

				return;
			}
		}

		move_bucket++;
	}

	*free_bucket = NULL; 
	*free_distance = 0;
}


void resize(){
	//TODO: Not required when number of principal are limited Table size can be incresed.
	printf(" @resize \n");
}

/* Lookup function: returns the virtual XID if found and -1 if not */
int xt_to_vxt_rcu(xid_type_t ty){

	//unsigned int hash = (ty) & (XIP_VXT_TABLE_SIZE - 1);
    unsigned int hash = hash_func[0]((const unsigned char *)&ty, sizeof(ty)) % XIP_VXT_TABLE_SIZE;

	struct xip_vxt_entry * start_bucket  = &B[hash];

	/*
	   int try_counter = 0;
	   int timestamp;
	 */

	//TODO:include the fast path too

	int i;

	struct xip_vxt_entry * check_bucket = start_bucket;

	for(i = 0; i < HOP_RANGE; ++i){

		if((start_bucket->hop_info & (1 << i)) && ty == (check_bucket->xid_type))
			return check_bucket->index;

		check_bucket++;
	}

	return -1;
}

/* Delete function: remove the xid @ty from the maps */
int vxt_unregister_xidty(xid_type_t ty) {
	//unsigned int hash = (ty) & (XIP_VXT_TABLE_SIZE - 1);
    unsigned int hash = hash_func[0]((const unsigned char *)&ty, sizeof(ty)) % XIP_VXT_TABLE_SIZE;

	struct xip_vxt_entry * start_bucket  = &B[hash];
	unsigned int hop_info = start_bucket->hop_info;
	unsigned int mask = 1;
	int i;
	for (i = 0; i < HOP_RANGE; ++i, mask <<= 1) {
		if(mask & hop_info){
			struct xip_vxt_entry* check_bucket = start_bucket + i;

			if(ty == (check_bucket->xid_type)) {
				check_bucket->xid_type = 0;
				check_bucket->index = -1;
				start_bucket->hop_info &= ~(1 << i);
				return 0;
			}
		}
	}

	return -1;
}

#if 0
int main(int argc, char * argv[]) {
	B = (struct xip_vxt_entry *) calloc(XIP_VXT_TABLE_SIZE, sizeof(struct xip_vxt_entry));

#define TEST
/*
*/
#ifdef TEST
	int reg_num = 5;
	int unreg_num = 3;
	int lp_num = 6;

	xid_type_t reg_ty[10] = {
		__cpu_to_be32(12), 
		__cpu_to_be32(13),
		__cpu_to_be32(14), 
		__cpu_to_be32(15), 
		__cpu_to_be32(76),
	};

	xid_type_t unreg_ty[10] = {
		__cpu_to_be32(12), 
		__cpu_to_be32(13),
		__cpu_to_be32(76),
	};

	xid_type_t lp_ty[10] = {
		__cpu_to_be32(12), 
		__cpu_to_be32(13),
		__cpu_to_be32(14), 
		__cpu_to_be32(15), 
		__cpu_to_be32(16), 
		__cpu_to_be32(76),
	};
#else
	int reg_num = 7;
	int unreg_num = 3;
	int lp_num = 9;

	xid_type_t reg_ty[10] = {
		XIDTYPE_AD, 
		XIDTYPE_HID,
		XIDTYPE_U4ID, 
		XIDTYPE_XDP,
		XIDTYPE_SRVCID,
		XIDTYPE_FLOWID,
		XIDTYPE_ZF
	};

	xid_type_t unreg_ty[10] = {
		XIDTYPE_AD, 
		XIDTYPE_HID,
		XIDTYPE_U4ID, 
	};
	
	xid_type_t lp_ty[10] = {
		XIDTYPE_AD, 
		XIDTYPE_HID,
		XIDTYPE_U4ID, 
		XIDTYPE_XDP,
		XIDTYPE_SRVCID,
		XIDTYPE_FLOWID,
		XIDTYPE_ZF,
		__cpu_to_be32(0x1),
		__cpu_to_be32(0x2)
	};
#endif
	int i = 0;

	printf("\nRegister xid_type: 12, ret = %d\n", vxt_register_xidty(__cpu_to_be32(12), 0));
	printf("\nRegister xid_type: 76, ret = %d\n", vxt_register_xidty(__cpu_to_be32(76), 0));

	for (i = 14; i < (12 + 35); i++) {
		printf("\nRegister xid_type: %u, ret = %d\n", i, vxt_register_xidty(__cpu_to_be32(i), 0));
	}

	printf("\nRegister xid_type: 140, ret = %d\n", vxt_register_xidty(__cpu_to_be32(140), 0));

	for(i = 0; i < XIP_VXT_TABLE_SIZE; i++) printf("i = %d key = %u, index = %d, hop_info = %u\n",i , __be32_to_cpu(B[i].xid_type), B[i].index, B[i].hop_info);

#if 0
	for (i = 0; i < unreg_num; i++) {
		printf("Unregister xid_type: %u, ret = %d\n", __be32_to_cpu(unreg_ty[i]), vxt_unregister_xidty(unreg_ty[i]));
	}

	for(i = 0; i < XIP_VXT_TABLE_SIZE; i++) printf("i = %d key = %u, index = %d\n",i , __be32_to_cpu(B[i].xid_type), B[i].index);
#endif
	for (i = 0; i < XIP_VXT_TABLE_SIZE; i++) {
		printf("Lookup xid_type: %u, ret = %d\n", i, xt_to_vxt_rcu(__cpu_to_be32(i)));
	}
	printf("Lookup xid_type: 140, ret = %d\n", xt_to_vxt_rcu(__cpu_to_be32(140)));

	return 1;
}
#endif
