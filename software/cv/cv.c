#include "cv.h"
#include "cv_config.h"

CV_camerapipe CV_getcamera(const char *camera_name, const char *filters) {
    char cmd[512];
    if (filters && filters[0]) { // Use filters if provided.
        snprintf(cmd, sizeof(cmd),
                 "ffmpeg -f v4l2 -video_size %dx%d -i %s -vf \"%s\" -f rawvideo -pix_fmt rgb24 - 2>/dev/null", // Redirect stderr to /dev/null. This hides the error messages when closing the pipe.
                 CV_FRAME_WIDTH, CV_FRAME_HEIGHT, camera_name, filters);
    } else {
        snprintf(cmd, sizeof(cmd),
                 "ffmpeg -f v4l2 -video_size %dx%d -i %s -f rawvideo -pix_fmt rgb24 - 2>/dev/null", // Redirect stderr to /dev/null. This hides the error messages when closing the pipe.
                 CV_FRAME_WIDTH, CV_FRAME_HEIGHT, camera_name);
    }

    CV_camerapipe pipe = popen(cmd, "r");

    sleep(1); // Sleep for 1 second to allow the camera to initialise.

    if (!pipe) {
        fprintf(stderr, "CV Error - Failed to open camera: %s\n", camera_name);
        return NULL;
    }

    return pipe;
}

unsigned char CV_clamp(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (unsigned char)value;
}

void CV_enhanceframe(CV_frame buffer, float contrast, int brightness) {
    unsigned char blurred[CV_FRAME_HEIGHT][CV_FRAME_WIDTH][3];

    float kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}};
    float kernel_sum = 16.0f;

    for (int y = 0; y < CV_FRAME_HEIGHT; y++) {
        for (int x = 0; x < CV_FRAME_WIDTH; x++) {
            float sum_r = 0, sum_g = 0, sum_b = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int iy = y + ky;
                    int ix = x + kx;
                    if (iy < 0) iy = 0;
                    if (iy >= CV_FRAME_HEIGHT) iy = CV_FRAME_HEIGHT - 1;
                    if (ix < 0) ix = 0;
                    if (ix >= CV_FRAME_WIDTH) ix = CV_FRAME_WIDTH - 1;

                    float weight = kernel[ky + 1][kx + 1];
                    sum_r += weight * buffer[iy][ix][0];
                    sum_g += weight * buffer[iy][ix][1];
                    sum_b += weight * buffer[iy][ix][2];
                }
            }

            blurred[y][x][0] = CV_clamp((int)(sum_r / kernel_sum));
            blurred[y][x][1] = CV_clamp((int)(sum_g / kernel_sum));
            blurred[y][x][2] = CV_clamp((int)(sum_b / kernel_sum));
        }
    }

    for (int y = 0; y < CV_FRAME_HEIGHT; y++) {
        for (int x = 0; x < CV_FRAME_WIDTH; x++) {
            int r = (int)(contrast * blurred[y][x][0] + brightness);
            int g = (int)(contrast * blurred[y][x][1] + brightness);
            int b = (int)(contrast * blurred[y][x][2] + brightness);

            buffer[y][x][0] = CV_clamp(r);
            buffer[y][x][1] = CV_clamp(g);
            buffer[y][x][2] = CV_clamp(b);
        }
    }

    long total_brightness = 0;
    for (int y = 0; y < CV_FRAME_HEIGHT; y++) {
        for (int x = 0; x < CV_FRAME_WIDTH; x++) {
            total_brightness += (buffer[y][x][0] + buffer[y][x][1] + buffer[y][x][2]) / 3;
        }
    }

    float avg_brightness = (float)total_brightness / (CV_FRAME_HEIGHT * CV_FRAME_WIDTH);
    float brightness_adjust = 128.0f - avg_brightness;

    for (int y = 0; y < CV_FRAME_HEIGHT; y++) {
        for (int x = 0; x < CV_FRAME_WIDTH; x++) {
            buffer[y][x][0] = CV_clamp((int)(buffer[y][x][0] + brightness_adjust));
            buffer[y][x][1] = CV_clamp((int)(buffer[y][x][1] + brightness_adjust));
            buffer[y][x][2] = CV_clamp((int)(buffer[y][x][2] + brightness_adjust));
        }
    }
}

int CV_getRGBframe(CV_frame buffer, const CV_camerapipe camera) {
    size_t n = fread(buffer, 1, CV_FRAME_HEIGHT * CV_FRAME_WIDTH * 3, camera);
    if (n != CV_FRAME_HEIGHT * CV_FRAME_WIDTH * 3) {
        fprintf(stderr, "CV Error - Failed to read frame: got %zu bytes, expected %d.\n", n, CV_FRAME_HEIGHT * CV_FRAME_WIDTH * 3);
        return 0;
    }

    return 1;
}

int CV_getHSVframe(CV_frame buffer, const CV_camerapipe camera) {
    if (!CV_getRGBframe(buffer, camera)) {
        return 0; // If reading the RGB frame fails, return 0.
    }

    CV_enhanceframe(buffer, CONTRAST, BRIGHTNESS);

    // Convert each pixel from RGB to HSV.
    CV_FOREACH(x, y) {
        CV_RGBtoHSV(buffer[y][x], buffer[y][x]); // Reuse the buffer for HSV values.
    }
    return 1; // Return 1 to indicate success.
}

void CV_closecamera(const CV_camerapipe camera) {
    if (camera) {
        fflush(camera);
        pclose(camera);
    }
}

CV_playerpipe CV_getplayer() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "ffplay -window_title \"cv.c - camera 1\" -loglevel quiet -f rawvideo -pixel_format rgb24 -video_size %dx%d -",
             CV_FRAME_WIDTH, CV_FRAME_HEIGHT);
    FILE *ffplay_pipe = popen(cmd, "w");
    if (!ffplay_pipe) {
        fprintf(stderr, "CV Error - Failed to open ffplay pipe.");
        return NULL;
    }
    return ffplay_pipe;
}

void CV_playframe(CV_playerpipe player, const CV_frame frame) {
    fwrite(frame, 1, CV_FRAME_WIDTH * CV_FRAME_HEIGHT * 3, player);
    fflush(player);
}

void CV_closeplayer(const CV_playerpipe player) {
    sleep(1); // Sleep for 1 second to allow the player to finish processing the last frame.
    if (player) {
        fflush(player);
        pclose(player);
    }
}

void CV_drawbb(CV_frame frame, CV_bounding_box_list *bboxes, const unsigned char color[3]) {
    for (size_t i = 0; i < bboxes->count; i++) {
        CV_bounding_box *box = &bboxes->boxes[i];
        int x0 = box->x[0], x1 = box->x[1];
        int y0 = box->y[0], y1 = box->y[1];

        // clang-format off
        // Top and bottom edges
        for (int x = x0; x <= x1; x++) {
            frame[y0][x][0] = color[0]; frame[y0][x][1] = color[1]; frame[y0][x][2] = color[2];
            frame[y1][x][0] = color[0]; frame[y1][x][1] = color[1]; frame[y1][x][2] = color[2];
        }
        // Left and right edges
        for (int y = y0; y <= y1; y++) {
            frame[y][x0][0] = color[0]; frame[y][x0][1] = color[1]; frame[y][x0][2] = color[2];
            frame[y][x1][0] = color[0]; frame[y][x1][1] = color[1]; frame[y][x1][2] = color[2];
        }
        // clang-format on
    }
}

void CV_chromakey(CV_mask mask, const CV_frame hsv_frame, const unsigned char h[2], const unsigned char s[2], const unsigned char v[2]) {
    CV_FOREACH(x, y) {
        unsigned char h_value = hsv_frame[y][x][0];
        unsigned char s_value = hsv_frame[y][x][1];
        unsigned char v_value = hsv_frame[y][x][2];

        // Check if the pixel is within the specified hue, saturation, and value ranges.
        if (((h_value >= h[0] && h_value <= h[1]) || (h[0] > h[1] && (h_value >= h[0] || h_value <= h[1]))) &&
            s_value >= s[0] && s_value <= s[1] &&
            v_value >= v[0] && v_value <= v[1]) {
            mask[y][x] = 1; // Mark the pixel as part of the chroma key.
        } else {
            mask[y][x] = 0; // Mark the pixel as not part of the chroma key.
        }
    }
}

void CV_masktracker(CV_bounding_box_list *bboxes, const CV_mask mask, const unsigned int minsize, const unsigned int maxsize) {
    bboxes->count = 0;                                           // Reset the bounding box count to dereference any previous boxes.
    unsigned int visited[CV_FRAME_HEIGHT][CV_FRAME_WIDTH] = {0}; // Visited pixels to avoid re-checking.
    CV_FOREACH(x, y) {
        if (!visited[y][x] && mask[y][x]) {
            // Flood fill algorithm to get the color region's bounding box.

            unsigned int left = x, right = x, top = y, bottom = y;
            // Mark the pixel as visited.
            visited[y][x] = 1;

            typedef struct {
                unsigned int x, y;
            } CV_Point;
            CV_Point stack[CV_FRAME_HEIGHT * CV_FRAME_WIDTH]; // Stack for flood fill.
            unsigned int stack_size = 0;
            stack[stack_size++] = (CV_Point){x, y};

            while (stack_size > 0) {
                CV_Point p = stack[--stack_size];
                unsigned int cx = p.x, cy = p.y;

                // Update the bounding box coordinates.
                if (cx < left) left = cx;
                if (cx > right) right = cx;
                if (cy < top) top = cy;
                if (cy > bottom) bottom = cy;

                // 4-connected neighbors: up, down, left, right
                if (cy > 0 && !visited[cy - 1][cx] && mask[cy - 1][cx]) {
                    visited[cy - 1][cx] = 1;
                    stack[stack_size++] = (CV_Point){cx, cy - 1};
                }
                if (cy + 1 < CV_FRAME_HEIGHT && !visited[cy + 1][cx] && mask[cy + 1][cx]) {
                    visited[cy + 1][cx] = 1;
                    stack[stack_size++] = (CV_Point){cx, cy + 1};
                }
                if (cx > 0 && !visited[cy][cx - 1] && mask[cy][cx - 1]) {
                    visited[cy][cx - 1] = 1;
                    stack[stack_size++] = (CV_Point){cx - 1, cy};
                }
                if (cx + 1 < CV_FRAME_WIDTH && !visited[cy][cx + 1] && mask[cy][cx + 1]) {
                    visited[cy][cx + 1] = 1;
                    stack[stack_size++] = (CV_Point){cx + 1, cy};
                }
            }

            CV_bounding_box box = {
                .x = {left, right},
                .y = {top, bottom}};
            // Check if the bounding box meets the minimum size requirement.
            if ((box.x[1] - box.x[0] + 1) < minsize || (box.y[1] - box.y[0] + 1) < minsize || (box.x[1] - box.x[0] + 1) > maxsize) {
                continue; // Skip this box if it is smaller than the minimum size.
            }
            if (bboxes->count < CV_MAX_BOUNDING_BOXES) {
                bboxes->boxes[bboxes->count++] = box; // Add the bounding box to the list.
            } else {
                fprintf(stderr, "CV Error - Bounding box list is full, cannot add more boxes.\n");
                return;
            }
            break; // Stop checking after the first region.
        }
    }
}