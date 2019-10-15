/* VL FEAT memory type used by vl_malloc_init */
enum vl_mem_type_ {L1, L2} ;
typedef enum vl_mem_type_ vl_mem_type ;

VL_EXPORT void  vl_set_mem_type(vl_mem_type type);
