#include "paging.h"

static page_directory_t* kernel_directory = 0;
static page_directory_t* current_directory = 0;

static uint32_t* frames;
static uint32_t num_frames;

extern uint32_t kmem_placement_address;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_addr)
{
   uint32_t frame = frame_addr / 0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   frames[idx] &= ~(0x1 << off);
}

//static uint32_t test_frame(uint32_t frame_addr)
//{
//    uint32_t frame = frame_addr/0x1000;
//    uint32_t idx = INDEX_FROM_BIT(frame);
//    uint32_t off = OFFSET_FROM_BIT(frame);
//    return (frames[idx] & (0x1 << off));
//}

static uint32_t first_frame()
{
    for (uint32_t i = 0; i < INDEX_FROM_BIT(num_frames); i++)
    {
        if (frames[i] != 0xFFFFFFFF)
        {
            for (uint32_t j = 0; j < 32; j++)
            {
                uint32_t toTest = 0x1 << j;
                if (!(frames[i] & toTest))
                {
                    return i*4*8+j;
                }
            }
        }
    }
    return 0;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0)
    {
        return;
    }
    else
    {
        uint32_t idx = first_frame();
        if (idx == (uint32_t)-1)
        {
            //PANIC("No free frames!");
            for(;;);
        }
        set_frame(idx * 0x1000);
        page->present = 1;
        page->rw = (is_writeable) ? 1 : 0;
        page->user = (is_kernel) ? 0 : 1;
        page->frame = idx;
    }
}

void free_frame(page_t *page)
{
   uint32_t frame;
   if (!(frame = page->frame))
   {
       return;
   }
   else
   {
       clear_frame(frame);
       page->frame = 0x0;
   }
}

void init_paging()
{
    // The size of physical memory. For the moment we
    // assume it is 16MB big.
    uint32_t mem_end_page = 0x1000000;

    num_frames = mem_end_page / 0x1000;
    frames = (uint32_t*)kmem_alloc(INDEX_FROM_BIT(num_frames));
    memset(frames, 0, INDEX_FROM_BIT(num_frames));

    // Let's make a page directory.
    kernel_directory = (page_directory_t*)kmem_alloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    int i = 0;
    while (i < kmem_placement_address)
    {
        // Kernel code is readable but not writeable from userspace.
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    // Before we enable paging, we must register our page fault handler.
    // register_interrupt_handler(14, page_fault);

    // Now, enable paging!
    switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir)
{
   current_directory = dir;
   asm volatile("mov %0, %%cr3":: "r"(&dir->tables_physical));
   uint32_t cr0;
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000;
   asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
    address /= 0x1000;
   
    uint32_t table_idx = address / 1024;
    if (dir->tables[table_idx]) 
    {
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    else if(make)
    {
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t*)kmem_alloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, 0x1000);
        dir->tables_physical[table_idx] = tmp | 0x7;
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
    {
        return 0;
    }
} 