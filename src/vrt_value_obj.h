
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <libcork/core.h>
#include <libcork/helpers/errors.h>
#include <vrt.h>


/* --------------------------------------------------------------
 * x-trader varon-t value and type
 */

enum HybridData {
	BESTANDDEEP = 0, 
	ORDERSTATICS, 
	PENDING_SIGNAL, 
	TUNN_RPT, 
};

struct vrt_hybrid_value {
	struct vrt_value  parent;
	enum HybridData data;
	int32_t index;
};

static struct vrt_value *
vrt_hybrid_value_new(struct vrt_value_type *type)
{
	struct vrt_hybrid_value  *self = cork_new(struct vrt_hybrid_value);
	return &self->parent;
}

static void
vrt_hybrid_value_free(struct vrt_value_type *type, struct vrt_value *vself)
{
	struct vrt_hybrid_value *iself =
		cork_container_of(vself, struct vrt_hybrid_value, parent);
	free(iself);
}

static struct vrt_value_type  _vrt_hybrid_value_type = {
	vrt_hybrid_value_new,
	vrt_hybrid_value_free
};

static struct vrt_value_type *
vrt_hybrid_value_type(void)
{
	return &_vrt_hybrid_value_type;
}
