#include <kernel.h>
#include <graph.h>
#include <draw.h>
#include <gs_psm.h>
#include <dma.h>
#define GS_REG_PRIM   0x00
#define GS_REG_RGBAQ  0x01
#define GS_REG_XYZ2   0x05
#define PRIM_TRIANGLE  0x03
#define GS_XY_OFFSET 2048
#define PX(px) ((px) * 16)
static qword_t packet[16] __attribute__((aligned(16)));
static inline void set_qword(qword_t *q, u64 lo, u64 hi) {
    q->dw[0] = lo;
    q->dw[1] = hi;
}
int main(void) {
    framebuffer_t fb;
    qword_t *q = packet;
    graph_shutdown();
    graph_set_mode(GRAPH_MODE_INTERLACED, GRAPH_MODE_NTSC, GRAPH_MODE_FIELD, GRAPH_ENABLE);
    fb.width   = 640;
    fb.height  = 448;
    fb.mask    = 0;
    fb.psm     = GS_PSM_32;
    fb.address = graph_vram_allocate(fb.width, fb.height, fb.psm, GRAPH_ALIGN_PAGE);
    graph_set_screen(0, 0, fb.width, fb.height);
    graph_set_bgcolor(0, 0, 0);
    graph_set_framebuffer_filtered(fb.address, fb.width, fb.psm, 0, 0);
    graph_enable_output();
    set_qword(q,
        (u64)7          |
        ((u64)1 << 15)  |
        ((u64)0 << 46)  |
        ((u64)0 << 58)  |
        ((u64)1 << 60),
        0xEULL
    );
    q++;
    set_qword(q, PRIM_TRIANGLE, GS_REG_PRIM);
    q++;
    set_qword(q, 0xFF0000FF, GS_REG_RGBAQ); q++;
    set_qword(q,
        ((u64)(GS_XY_OFFSET + PX(0))    & 0xFFFF) |
        (((u64)(GS_XY_OFFSET + PX(-100)) & 0xFFFF) << 16) |
        ((u64)0 << 32),
        GS_REG_XYZ2);
    q++;
    set_qword(q, 0x00FF00FF, GS_REG_RGBAQ); q++;
    set_qword(q,
        ((u64)(GS_XY_OFFSET + PX(-100)) & 0xFFFF) |
        (((u64)(GS_XY_OFFSET + PX(100)) & 0xFFFF) << 16) |
        ((u64)0 << 32),
        GS_REG_XYZ2);
    q++;
    set_qword(q, 0x0000FFFF, GS_REG_RGBAQ); q++;
    set_qword(q,
        ((u64)(GS_XY_OFFSET + PX(100)) & 0xFFFF) |
        (((u64)(GS_XY_OFFSET + PX(100)) & 0xFFFF) << 16) |
        ((u64)0 << 32),
        GS_REG_XYZ2);
    q++;
    dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
    dma_channel_send_normal(DMA_CHANNEL_GIF, packet, q - packet, 0, 0);
    dma_wait_fast();
    while (1) {
        graph_wait_vsync();
    }
    return 0;
}
