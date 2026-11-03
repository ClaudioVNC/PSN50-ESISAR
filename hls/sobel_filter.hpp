#ifndef SOBEL_FILTER_HPP
#define SOBEL_FILTER_HPP

#include <ap_axi_sdata.h>
#include <hls_stream.h>

#ifndef SOBEL_MAX_WIDTH
#define SOBEL_MAX_WIDTH 1920
#endif

#ifndef SOBEL_EDGE_THRESHOLD
#define SOBEL_EDGE_THRESHOLD 512
#endif

typedef ap_axiu<24, 1, 1, 1> axis_pixel_t;

void sobel_filter(hls::stream<axis_pixel_t>& in_stream,
                  hls::stream<axis_pixel_t>& out_stream);

#endif
