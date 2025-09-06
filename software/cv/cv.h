#ifndef CV_H
#define CV_H

#include "cv_config.h"
#include <stdio.h>
#include <unistd.h>

#if !defined(CV_FRAME_HEIGHT) || !defined(CV_FRAME_WIDTH)
#define CV_FRAME_HEIGHT 72 // Default height
#define CV_FRAME_WIDTH 88  // Default width
#warning "Using default resolution: 72x88 (QQCIF). Define both CV_FRAME_HEIGHT and CV_FRAME_WIDTH in cv_config.h for a specific resolution."
#endif

#ifndef CV_MAX_BOUNDING_BOXES
#define CV_MAX_BOUNDING_BOXES 100 // Default maximum number of bounding boxes
#warning "Using default maximum bounding boxes: 100. Define CV_MAX_BOUNDING_BOXES in cv_config.h for a specific value."
#endif

/**
 * @brief A type representing a single RGB pixel.
 */
typedef unsigned char CV_pixel_RGB[3];

/**
 * @brief A type representing a single HSV pixel.
 */
typedef unsigned char CV_pixel_HSV[3];

/**
 * @brief A video frame type for storing a single frame of video.
 */
typedef unsigned char CV_frame[CV_FRAME_HEIGHT][CV_FRAME_WIDTH][3];

/**
 * @brief A type representing a player pipe, equivalent to a file pointer.
 */
typedef FILE *CV_playerpipe;

/**
 * @brief A type representing a binary mask for a frame.
 */
typedef unsigned char CV_mask[CV_FRAME_HEIGHT][CV_FRAME_WIDTH];

/**
 * @brief A struct that defines a bounding box.
 */
typedef struct {
    unsigned int x[2];
    unsigned int y[2];
} CV_bounding_box;

/**
 * @brief A struct that defines a bounding box list.
 */
typedef struct {
    CV_bounding_box boxes[CV_MAX_BOUNDING_BOXES];
    size_t count;
} CV_bounding_box_list;

/**
 * @brief A struct that defines what data is read from the camera.
 */
struct CV_CameraData {
    int obstacle_spotted;
    int obstacle_x;
    int obstacle_y;
    int obstacle_colour;
};

/**
 * @brief A macro to iterate over each pixel in a frame.
 */
#define CV_FOREACH(x, y)                      \
    for (int y = 0; y < CV_FRAME_HEIGHT; ++y) \
        for (int x = 0; x < CV_FRAME_WIDTH; ++x)

/**
 * @brief Converts an RGB pixel to HSV.
 */
static inline void CV_RGBtoHSV(const CV_pixel_RGB rgb, CV_pixel_HSV hsv) {
    unsigned char r = rgb[0], g = rgb[1], b = rgb[2];
    unsigned char max = r > g ? (r > b ? r : b) : (g > b ? g : b);
    unsigned char min = r < g ? (r < b ? r : b) : (g < b ? g : b);
    unsigned char delta = max - min;

    hsv[2] = max * 100 / 255;
    if (max == 0) {
        hsv[0] = hsv[1] = hsv[2] = 0;
        return;
    }
    if (max == min) {
        hsv[0] = 0;
        hsv[1] = 0;
        return;
    }
    hsv[1] = delta * 100 / max;
    int h;
    if (max == r) {
        h = 60 * (g - b) / delta;
        if (h < 0) h += 360;
    } else if (max == g) {
        h = 60 * (b - r) / delta + 120;
    } else {
        h = 60 * (r - g) / delta + 240;
    }
    hsv[0] = h / 10;
}

unsigned char CV_clamp(int value);
void CV_enhanceframe(CV_frame buffer, float contrast, int brightness);

/**
 * @brief Captures a single RGB frame from the camera.
 *
 * @param buffer Buffer to store the RGB frame.
 * @param camera_name Device path (e.g., "/dev/video0").
 * @param filters Optional ffmpeg filters (may be NULL).
 * @return 1 if successful, 0 otherwise.
 */
int CV_captureRGBframe(CV_frame buffer, const char *camera_name, const char *filters);

/**
 * @brief Captures a single HSV frame from the camera.
 *
 * @param buffer Buffer to store the HSV frame.
 * @param camera_name Device path (e.g., "/dev/video0").
 * @param filters Optional ffmpeg filters (may be NULL).
 * @return 1 if successful, 0 otherwise.
 */
int CV_captureHSVframe(CV_frame buffer, const char *camera_name, const char *filters);

/**
 * @brief Opens a player pipe to play video frames using ffplay.
 */
CV_playerpipe CV_getplayer();

/**
 * @brief Plays a frame using the specified player pipe.
 */
void CV_playframe(CV_playerpipe player, const CV_frame frame);

/**
 * @brief Closes the player pipe.
 */
void CV_closeplayer(const CV_playerpipe player);

/**
 * @brief Draws bounding boxes on a frame.
 */
void CV_drawbb(CV_frame frame, CV_bounding_box_list *bboxes, const unsigned char color[3]);

/**
 * @brief Generates a binary mask for chroma keying.
 */
void CV_chromakey(CV_mask mask, const CV_frame hsv_frame, const unsigned char h[2], const unsigned char s[2], const unsigned char v[2]);

/**
 * @brief Tracks colors in an image using a flood fill algorithm.
 */
void CV_masktracker(CV_bounding_box_list *bboxes, const CV_mask mask, const unsigned int minsize);

#endif // CV_H
