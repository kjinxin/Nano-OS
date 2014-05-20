#include "mm.h"
#include "kernel.h"


pid_t MM;
static CR3 user_cr3[USER_NUM];											// user CR3
static PDE user_pdir[USER_NUM][NR_PDE] align_to_page;						// user page directory
static PTE user_ptable[USER_NUM*3][NR_PTE] align_to_page;		// user page tables
static bool map[PHY_MEM / PAGE_SIZE];
static bool hash[USER_NUM];

CR3 *cr3;
static void init_mem(void)
{
        /* initialize the map*/
	int i;
	for (i = 0; i < KMEM / PAGE_SIZE; i++)
		map[i] = true;
	for (i = KMEM / PAGE_SIZE; i < PHY_MEM / PAGE_SIZE; i++)
		map[i] = false;
	/* initialize the user pdir*/
	PDE *pdir;
	PTE *ptable;
	int pdir_idx;
	for (i = 0; i< USER_NUM; i++)
	{
		pdir = user_pdir[i];
		for (pdir_idx = 0; pdir_idx < NR_PDE; pdir_idx++)
			make_invalid_pde(pdir + pdir_idx);
		/* set the kernel reflection */
        	ptable = (PTE *)va_to_pa(get_kptable());
        	for (pdir_idx = 0; pdir_idx < KMEM / PD_SIZE; pdir_idx ++) 
		{
            		make_pde(&pdir[pdir_idx + KOFFSET / PD_SIZE], ptable);
            		ptable += NR_PTE;
        	}
		/* make CR3 to be the entry of user page directory */
		hash[i] = true;
		user_cr3[i].val=0;
		user_cr3[i].page_directory_base = (uint32_t)va_to_pa(user_pdir + i) >> 12;
	}
}

inline CR3* get_user_cr3() {
    int i;
    for(i = 0; i < USER_NUM; i ++) {
        if(hash[i]) {
            hash[i] = false;
            cr3 = user_cr3 + i;
	    break;
        }
    }
    return cr3;
}
static void* free_phy_mem()
{
	int i,j=0;
	for (i=0; i < PHY_MEM / PAGE_SIZE; i++)
	if (!map[i])
	{
		map[i] = true;
		j=i;
		break;
	}
	return (void*) (j*PAGE_SIZE);
}
void init_mm(void)
{
	PCB *p = create_kthread(mm_thread);
	MM = p->pid;
	wakeup(p);
}

void mm_thread(void)
{
	static Msg m;
	uint8_t *va;
	init_mem();
	while (true) {
		receive(ANY, &m);
		switch (m.type) {
			case MM_NEW_PROC:
				get_user_cr3();	
				printk("mm_thread MM_NEW_PROC%d\n",cr3->val);
				break;
			case MM_NEW_PAGE:
				va = (uint8_t *)m.i[0];
				int memsz = (int) m.i[1];
				PDE *pdir = (PDE *)(*(uint32_t *)cr3 & ~0xfff);
                		PTE *ptable = user_ptable[3 * (int)(pdir - user_pdir[0])];
					
				/* calculate where to lacate */
                		int pdir_base = (uint32_t)va >> 22;
                		int ptable_base = ((uint32_t)va >> 12) & 0x3ff;
                		int ptable_num = (memsz - 1) / PAGE_SIZE + 1;
				
                		int pdir_num = (ptable_num - 1) / NR_PTE + 1;
				
                		if(memsz == 0) ptable_num= pdir_num = 0;
                		//assert(ptable_num < 3 * USR_PROC_MAX);

                		/* fill pdir */
                		int pdir_idx, ptable_idx;
                		for(pdir_idx = 0; pdir_idx < pdir_num; pdir_idx ++) 
				{
                    			if(ptable_num == 0) break;
                    			make_pde(pdir + pdir_idx + pdir_base, (PTE *)va_to_pa(ptable));
                    			if(ptable_num + ptable_base < NR_PTE) 
					{
                        			ptable += ptable_base;
                        			for(ptable_idx = ptable_base; ptable_idx < ptable_base + ptable_num; ptable_idx ++) 
						{
                        	    			if(ptable->val != 0) continue;
                        	    			make_pte(ptable, free_phy_mem());
                        	    			ptable ++;
                        			}
                        			break;
                    			} else 
					{
                        			ptable += ptable_base;
                        			for(ptable_idx = ptable_base; ptable_idx < NR_PTE; ptable_idx ++) 
						{
                        	    			if(ptable->val != 0) continue;
                        	    			make_pte(ptable, free_phy_mem());
                        	    			ptable ++;
                        			}
                        			ptable_num = ptable_num + ptable_base - NR_PTE;
                        			ptable_base = 0;
                    			}
                		}
				
				uint32_t pde = ((uint32_t *)(*(uint32_t *)cr3 & ~0xfff))[(uint32_t)va >> 22];
                		uint32_t pte = ((uint32_t *)(pde & ~0xfff))[((uint32_t)va >> 12) & 0x3ff];
                		uint8_t *pa = (uint8_t *)((pte & ~0xfff) | ((uint32_t)va & 0xfff));
				m.dest = m.src;
				m.src = MM;
				m.ret = (int) pa;
				send(m.dest, &m);
				break;
			default: 
				assert(0);
		}
	}
}
