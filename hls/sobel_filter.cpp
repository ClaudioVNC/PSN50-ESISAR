#include "sobel_filter.hpp"

#include <ap_int.h>

static ap_uint<8> rgb_to_gray(ap_uint<24> pixel) {
#pragma HLS INLINE
    // Converte RGB em escala de cinza usando pesos aproximados da luminancia.
    // Convert RGB to grayscale using approximate luminance weights.
    const ap_uint<8> red = pixel.range(23, 16);
    const ap_uint<8> green = pixel.range(15, 8);
    const ap_uint<8> blue = pixel.range(7, 0);
    const ap_uint<16> weighted =
        (ap_uint<16>)red * 77 + (ap_uint<16>)green * 150 +
        (ap_uint<16>)blue * 29 + 128;
    return weighted >> 8;
}

void sobel_filter(hls::stream<axis_pixel_t>& in_stream,
                  hls::stream<axis_pixel_t>& out_stream) {
#pragma HLS INTERFACE axis port=in_stream
#pragma HLS INTERFACE axis port=out_stream
#pragma HLS INTERFACE ap_ctrl_none port=return

    // line_0 guarda a linha anterior; line_1 guarda a linha duas linhas atras.
    // line_0 stores the previous row; line_1 stores the row from two rows ago.
    // A cada pixel, o valor antigo de line_0 passa para line_1, e line_0 recebe
    // o pixel atual. Assim, ha duas linhas atrasadas mais o pixel atual.
    // For each pixel, the old line_0 value moves to line_1, and line_0 receives
    // the current pixel. This provides two delayed rows plus the current pixel.
    static ap_uint<8> line_0[SOBEL_MAX_WIDTH];
    static ap_uint<8> line_1[SOBEL_MAX_WIDTH];

    // A janela desloca-se horizontalmente a cada pixel recebido. A coluna da
    // direita recebe line_1, line_0 e o pixel atual, respectivamente.
    // The window shifts horizontally for each input pixel. Its right column
    // receives line_1, line_0, and the current pixel, respectively.
    static ap_uint<8> window[3][3];
#pragma HLS ARRAY_PARTITION variable=window complete dim=0

    // Coordenadas do pixel atual dentro do quadro.
    // Coordinates of the current pixel within the frame.
    static ap_uint<16> x = 0;
    static ap_uint<32> y = 0;

    while (true) {
#pragma HLS PIPELINE II=1
        axis_pixel_t input_pixel = in_stream.read();
        axis_pixel_t output_pixel = input_pixel;

        // user marca o inicio do quadro (Start of Frame).
        // user marks the start of frame (Start of Frame).
        if (input_pixel.user) {
            x = 0;
            y = 0;
            for (int row = 0; row < 3; ++row) {
#pragma HLS UNROLL
                for (int col = 0; col < 3; ++col) {
#pragma HLS UNROLL
                    window[row][col] = 0;
                }
            }
        }

        // Por padrao, pixels sem vizinhanca 3x3 completa saem como fundo preto.
        // By default, pixels without a complete 3x3 neighborhood are black.
        output_pixel.data = 0;

        if (x < SOBEL_MAX_WIDTH) {
            const ap_uint<8> gray = rgb_to_gray(input_pixel.data);
            const ap_uint<8> top = line_1[x];
            const ap_uint<8> middle = line_0[x];

            line_1[x] = middle;
            line_0[x] = gray;

            // Desloca a janela e insere a nova coluna de pixels.
            // Shift the window and insert the new column of pixels.
            for (int row = 0; row < 3; ++row) {
#pragma HLS UNROLL
                window[row][0] = window[row][1];
                window[row][1] = window[row][2];
            }
            window[0][2] = top;
            window[1][2] = middle;
            window[2][2] = gray;

            if (x >= 2 && y >= 2) {
                const ap_int<12> p00 = window[0][0];
                const ap_int<12> p01 = window[0][1];
                const ap_int<12> p02 = window[0][2];
                const ap_int<12> p10 = window[1][0];
                const ap_int<12> p12 = window[1][2];
                const ap_int<12> p20 = window[2][0];
                const ap_int<12> p21 = window[2][1];
                const ap_int<12> p22 = window[2][2];

                const ap_int<12> gx =
                    -p00 + p02 - 2 * p10 + 2 * p12 - p20 + p22;
                const ap_int<12> gy =
                    -p00 - 2 * p01 - p02 + p20 + 2 * p21 + p22;

                // Aproxima a magnitude por |Gx| + |Gy| e compara com o limiar.
                // Approximate magnitude with |Gx| + |Gy| and compare to threshold.
                const ap_uint<12> abs_gx =
                    gx < 0 ? (ap_uint<12>)(-gx) : (ap_uint<12>)gx;
                const ap_uint<12> abs_gy =
                    gy < 0 ? (ap_uint<12>)(-gy) : (ap_uint<12>)gy;
                const ap_uint<12> magnitude = abs_gx + abs_gy;

                if (magnitude >= SOBEL_EDGE_THRESHOLD) {
                    output_pixel.data = 0xFFFFFF;
                }
            }
        }

        // Copiar a palavra AXI preserva user, last e os demais sinais laterais;
        // somente data e substituido pelo resultado RGB binario.
        // Copying the AXI word preserves user, last, and the other sidebands;
        // only data is replaced with the binary RGB result.
        out_stream.write(output_pixel);

        // last marca o fim da linha (End of Line): reinicia x e a janela.
        // last marks the end of line (End of Line): reset x and the window.
        if (input_pixel.last) {
            x = 0;
            if (y != 0xFFFFFFFF) {
                ++y;
            }
            for (int row = 0; row < 3; ++row) {
#pragma HLS UNROLL
                for (int col = 0; col < 3; ++col) {
#pragma HLS UNROLL
                    window[row][col] = 0;
                }
            }
        } else if (x != 0xFFFF) {
            ++x;
        }
    }
}
