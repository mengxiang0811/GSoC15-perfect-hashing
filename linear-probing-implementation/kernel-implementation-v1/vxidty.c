#include <net/xia_vxidty.h>

/* The current perfect hashing mechanism used here is linear probing
 */

#define ULONG_SIZE_IN_BIT	(sizeof(unsigned long) * 8)

atomic_t tot_element = ATOMIC_INIT(0);

static DEFINE_MUTEX(vxt_mutex);
static unsigned long allocated_vxt[(XIP_MAX_XID_TYPES + ULONG_SIZE_IN_BIT - 1)
	/ ULONG_SIZE_IN_BIT];

static struct xip_vxt_entry map1[XIP_VXT_TABLE_SIZE] __read_mostly;
static struct xip_vxt_entry map2[XIP_VXT_TABLE_SIZE] __read_mostly;
const struct xip_vxt_entry *xip_virtual_xid_types __read_mostly = map1;

#define MAP_SIZE_IN_BYTE	(sizeof(map1))

static inline struct xip_vxt_entry *writable_current_map(void)
{
	return (struct xip_vxt_entry *)xip_virtual_xid_types;
}

static inline struct xip_vxt_entry *get_entry_reg_locked(struct xip_vxt_entry *map,
						     xid_type_t ty)
{
	__u32 hash_index = 0;
	struct xip_vxt_entry *entry;

	hash_index = (__be32_to_cpu(ty) & (XIP_VXT_TABLE_SIZE - 1));
	entry = &(rcu_dereference(map)[hash_index]);
	
	BUILD_BUG_ON_NOT_POWER_OF_2(XIP_VXT_TABLE_SIZE);
	BUILD_BUG_ON(XIP_VXT_TABLE_SIZE < XIP_MAX_XID_TYPES);

	while (likely(entry->marker == 1)) {
		hash_index = ((hash_index + 1) & (XIP_VXT_TABLE_SIZE - 1));
		
		entry = &(rcu_dereference(map)[hash_index]);
		if (entry->xid_type == ty) {
			return entry;
		}
	}

	return entry;
}

static inline struct xip_vxt_entry *get_entry_unreg_locked(struct xip_vxt_entry *map,
						     xid_type_t ty)
{
	__u32 count = 0;
	__u32 hash_index = 0;
	struct xip_vxt_entry *entry;

	hash_index = (__be32_to_cpu(ty) & (XIP_VXT_TABLE_SIZE - 1));
	entry =	&(rcu_dereference(map)[hash_index]);

	BUILD_BUG_ON_NOT_POWER_OF_2(XIP_VXT_TABLE_SIZE);
	BUILD_BUG_ON(XIP_VXT_TABLE_SIZE < XIP_MAX_XID_TYPES);

	pr_info("In get_entry_unreg_locked!\n");
	
	while (likely(entry->marker != 0) && count <= XIP_VXT_TABLE_SIZE) {
		if (entry->xid_type == ty) {
			pr_info("The XID Type 0x%x will be deleted!\n", __be32_to_cpu(ty));
			pr_info("---XID Type: 0x%x!\n", __be32_to_cpu(entry->xid_type));
			pr_info("---Index: %d!\n", entry->index);
			pr_info("---Marker: %d!\n", entry->marker);
			return entry;
		}
		
		hash_index = ((hash_index + 1) & (XIP_VXT_TABLE_SIZE - 1));
		entry = &(rcu_dereference(map)[hash_index]);
		count++;
	}

	return entry;
}

static inline struct xip_vxt_entry *next_map(void)
{
	return xip_virtual_xid_types == map1 ? map2 : map1;
}

int vxt_register_xidty(xid_type_t ty)
{
	struct xip_vxt_entry *entry, *old_map, *new_map;
	int ret = -ENOSPC;

	mutex_lock(&vxt_mutex);

	if (likely(XIP_VXT_TABLE_SIZE == atomic_read(&tot_element))) {
		pr_info("Can't perform Registration..Hash Table is full!!\n");
		goto out;
	}

	/* Check that everything is ready. */
	old_map = writable_current_map(); /* get_entry_locked() requires it. */
	entry = get_entry_reg_locked(old_map, ty);
	if (entry->xid_type == ty) {
		ret = -EEXIST;
		goto out;
	} else if (entry->xid_type) {
		ret = -EINVAL;
		goto out;
	}
	ret = find_first_zero_bit(allocated_vxt, XIP_MAX_XID_TYPES);

	/*To test the return value of the XID index*/
	/*By Qiaobin Fu*/
	pr_info("Add XID_TYPE 0x%x to bucket %d\n", __be32_to_cpu(ty), ret);

	if (ret >= XIP_MAX_XID_TYPES) {
		ret = -ENOSPC;
		goto out;
	}

	/* Cook a new map. */
	__set_bit(ret, allocated_vxt);
	new_map = next_map();
	memmove(new_map, old_map, MAP_SIZE_IN_BYTE);
	entry = get_entry_reg_locked(new_map, ty);

	entry->xid_type = ty;
	entry->index = ret;
	entry->marker = 1;
	
	/* Stat for the total entries */
	atomic_inc(&tot_element);

	/* Publish the new map. */
	rcu_assign_pointer(xip_virtual_xid_types, new_map);
	synchronize_rcu();

out:
	mutex_unlock(&vxt_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(vxt_register_xidty);

int vxt_unregister_xidty(xid_type_t ty)
{
	struct xip_vxt_entry *entry, *old_map, *new_map;
	int ret = -EINVAL;

	mutex_lock(&vxt_mutex);
	
	if (likely(0 == atomic_read(&tot_element))) {
		pr_info("Can't perform Deletion..Hash Table is empty!!\n");
		goto out;
	}

	pr_info("To unload XID Type 0x%x!\n", __be32_to_cpu(ty));
	/* Check that everything is ready. */
	old_map = writable_current_map(); /* get_entry_locked() requires it. */
	entry = get_entry_unreg_locked(old_map, ty);
	if (entry->xid_type != ty) {
		ret = -EINVAL;
		goto out;
	}
	ret = 0;

	/* Cook a new map. */
	BUG_ON(!__test_and_clear_bit(entry->index, allocated_vxt));
	new_map = next_map();
	memmove(new_map, old_map, MAP_SIZE_IN_BYTE);
	entry = get_entry_unreg_locked(new_map, ty);
	
	/* set marker to -1 during deletion operation */
	memset(entry, 0, sizeof(*entry));
	entry->marker = -1;
	
	/* Stat for the total entries */
	atomic_dec(&tot_element);

	/* Publish the new map. */
	rcu_assign_pointer(xip_virtual_xid_types, new_map);
	synchronize_rcu();

out:
	mutex_unlock(&vxt_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(vxt_unregister_xidty);
