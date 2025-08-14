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
 *
 * CV_RGB_pixel[0] = Red (0-255)
 *
 * CV_RGB_pixel[1] = Green (0-255)
 *
 * CV_RGB_pixel[2] = Blue (0-255)
 */
typedef unsigned char CV_pixel_RGB[3];

/**
 * @brief A type representing a single HSV pixel.
 *
 * CV_pixel_HSV[0] = Hue (0-36; in decadegrees)
 *
 * CV_pixel_HSV[1] = Saturation (0-100; in percentage)
 *
 * CV_pixel_HSV[2] = Value (0-100; in percentage)
 */
typedef unsigned char CV_pixel_HSV[3];

/**
 * @brief A video frame type for storing a single frame of video.
 */
typedef unsigned char CV_frame[CV_FRAME_HEIGHT][CV_FRAME_WIDTH][3];

/**
 * @brief A type representing a camera pipe, equivalent to a file pointer.
 */
typedef FILE *CV_camerapipe;

/**
 * @brief A type representing a player pipe, equivalent to a file pointer.
 */
typedef FILE *CV_playerpipe;

/**
 * @brief A type representing a binary mask for a frame.
 */
typedef unsigned char CV_mask[CV_FRAME_HEIGHT][CV_FRAME_WIDTH];

/**
 * @brief A struct that defines a bounding box. Coordinates are relative to the top-left corner of the frame.
 */
typedef struct {
    unsigned int x[2]; // x[0] is the leftmost x-coordinate, x[1] is the rightmost x-coordinate).
    unsigned int y[2]; // y[0] is the topmost y-coordinate, y[1] is the bottommost y-coordinate).
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
 *
 * @param x The variable that will get assigned the x-coordinate of the pixel.
 * @param y The variable that will get assigned the y-coordinate of the pixel.
 */
#define CV_FOREACH(x, y)                      \
    for (int y = 0; y < CV_FRAME_HEIGHT; ++y) \
        for (int x = 0; x < CV_FRAME_WIDTH; ++x)

/**
 * @brief Converts an RGB pixel to HSV, and stores the result in the provided HSV pixel.
 *
 * @param rgb The RGB pixel to convert.
 * @param hsv The HSV pixel to store the converted values.
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

/**
 * @brief Opens a camera using the specified camera name.
 *
 * @param camera_name The name of the camera to open (e.g., "/dev/video0").
 * @param filters Optional filters to apply to the camera stream (e.g., "gblur=0.5" for gaussian blur with sigma=0.5). See https://ffmpeg.org/ffmpeg-filters.html for more details.
 * @return Returns a pipe to the camera if successful, otherwise returns NULL.
 */
CV_camerapipe CV_getcamera(const char *camera_name, const char *filters);

unsigned char CV_clamp(int value);

void CV_enhanceframe(CV_frame buffer, float contrast, int brightness);

/**
 * @brief Retrieves a RGB frame from the camera and stores it in the provided buffer.
 *
 * @param buffer The buffer where the frame will be stored.
 * @param camera The camera pipe from which to read the frame.
 * @return Returns 1 if the frame was successfully read, otherwise returns 0.
 */
int CV_getRGBframe(CV_frame buffer, const CV_camerapipe camera);

/**
 * @brief Retrieves a HSV frame from the camera and stores it in the provided buffer.
 *
 * @param buffer The buffer where the HSV frame will be stored.
 * @param camera The camera pipe from which to read the frame.
 * @return Returns 1 if the frame was successfully read and converted, otherwise returns 0.
 */
int CV_getHSVframe(CV_frame buffer, const CV_camerapipe camera);

/**
 * @brief Closes the camera pipe.
 *
 * @param camera The camera pipe to close.
 */
void CV_closecamera(const CV_camerapipe camera);

/**
 * @brief Opens a player pipe to play video frames using ffplay.
 *
 * @return Returns a pipe to the player if successful, otherwise returns NULL.
 */
CV_playerpipe CV_getplayer();

/**
 * @brief Plays a frame using the specified player pipe.
 *
 * @param ffplay_pipe The player pipe to write the frame to.
 * @param frame The frame to play.
 */
void CV_playframe(CV_playerpipe player, const CV_frame frame);

/**
 * @brief Draws bounding boxes on a frame.
 *
 * @param frame The frame to draw on.
 * @param bboxes The list of bounding boxes to draw.
 * @param color The color to use for the bounding boxes (RGB format).
 */
void CV_drawbb(CV_frame frame, CV_bounding_box_list *bboxes, const unsigned char color[3]);

/**
 * @brief Closes the player pipe.
 *
 * @param player The player pipe to close.
 */
void CV_closeplayer(const CV_playerpipe player);

/**
 * @brief Generates a binary mask for chroma keying based on the specified hue, saturation, and value ranges.
 *
 * @param mask The mask to fill with chroma key values.
 * @param hsv_frame The HSV frame to process.
 * @param h An array of two unsigned chars representing the hue range (inclusive).
 * @param s An array of two unsigned chars representing the saturation range (inclusive).
 * @param v An array of two unsigned chars representing the value range (inclusive).
 * @note The hue range is specified in decadegrees (0-36), saturation and value ranges are specified in percentage (0-100). If the first value in the hue range is greater than the second, the interval wraps around (e.g., h[0] = 350, h[1] = 10 means hue from 350 to 10 degrees).
 */
void CV_chromakey(CV_mask mask, const CV_frame hsv_frame, const unsigned char h[2], const unsigned char s[2], const unsigned char v[2]);

/**
 * @brief Tracks colors in an image using a flood fill algorithm. It identifies contiguous regions of pixels that match specified color criteria and returns their bounding boxes.
 *
 * @param bboxes A pointer to a bounding box list where the detected bounding boxes will be stored.
 * @param frame The frame to process.
 * @param selector A color selector to match against.
 * @param minsize The minimum area percentage (0-100) of the frame that a detected region must occupy to be considered valid.
 */
void CV_masktracker(CV_bounding_box_list *bboxes, const CV_mask mask, const unsigned int minsize);

#endif // CV_H