#include "mm.h"
#include "kernel.h"


pid_t MM;
static CR3 user_cr3[USER_NUM];											// user CR3
static PDE user_pdir[USER_NUM][NR_PDE] align_to_page;						// user page directory
static PTE user_ptable[USER_NUM][NR_PTE] align_to_page;		// user page tables
static map[PHY_MEM / PAGE_SIZE];
static bool hash[USER_NUM];

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
		hash[i] = true;
		user_cr3[i].val=0;
		user_cr3[i].page_directory_base = (uint32_t)va_to_pa(user_pdir + i) >> 12;
	}
}

inline CR3* get_usr_cr3() {
    static int last_used = 0;
    int i = last_used;
    for(; i < USR_PROC_MAX; i ++) {
        if(empty[i]) {
            last_used = i;
            empty[i] = false;
            break;
        }
        if(i == USR_PROC_MAX - 1) i = -1;
    }
    return usr_cr3 + last_used;
}
static int free_phy_mem()
{
	int i;
	for (i=0; i < PHY_MEM / PAGE_SIZE; i++)
	if (!map[i])
	{
		map[i]=true;
		return i*PAGE_SIZE;
	}
}
static void init_mm(void)
{
	PCB *p = create_kthread(mm_thread);
	MM = p->pid;
	wakeup(p);
}
static void mm_thread(void)
{
	static Msg m;
	init_mem();
	while (true) {
		receive(ANY, &m);
		switch (m.type) {
			case MM_NEW_PROC:
				
				break;
			case MM_NEW_PAGE:
				int *va;
				va = (uint8_t *)m.i[0];
				int memsz = (int) m.i[1];
				PDE *pdir = (PDE *)(*(uint32_t *)cr3 & ~0xfff);
                		PTE *ptable = usr_ptable[3 * (int)(pdir - usr_pde[0])];

				
				uint32_t pde = ((uint32_t *)(cr3 & ~0xfff))[va >> 22];
				uint32_t pte = ((uint32_t *)(pde & ~0xfff))[(va >> 12) & 0x3ff];
				uint32_t pa = (pte & ~0xfff) | (va & 0xfff);
				m.dest = m.src;
				m.src = MM;
				m.ret = pa;
				send(m.dest, &m);
				break;
			default: assert(0);
		}
		m.dest = m.src;
		m.src = MM;
		send(m.dest,&m);
	}
}
