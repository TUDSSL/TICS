#ifndef _LIBMSP_MEM_H
#define _LIBMSP_MEM_H

/* The linker script needs to allocate these sections into FRAM region. */
#define __nv    __attribute__((section(".nv_vars")))
#define __ro_nv __attribute__((section(".ro_nv_vars")))

#endif // _LIBMSP_MEM_H
