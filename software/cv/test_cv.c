#include <stdio.h>

#include "cv.h"
#include "cv_config.h"

int main() {
    CV_playerpipe player = CV_getplayer();
    if (!player) return 0;

    CV_frame redframe;
    CV_frame greenframe;
    int frame_count = 0;

    while (frame_count++ < 40) {
        CV_mask redmask;
        CV_mask greenmask;

        CV_bounding_box_list redbboxes = {0};
        CV_bounding_box_list greenbboxes = {0};

        // Take snapshots directly in HSV
        if (!CV_captureHSVframe(greenframe, "/dev/video0", "gblur=0.5")) return 0;
        if (!CV_captureHSVframe(redframe, "/dev/video0", "gblur=0.5")) return 0;

        // Detect green objects
        CV_chromakey(greenmask, greenframe, H_HM_GREEN, S_HM_GREEN, V_HM_GREEN);
        CV_masktracker(&greenbboxes, greenmask, 15);

        CV_bounding_box *biggestgreenbox = &greenbboxes.boxes[0];
        for (size_t i = 0; i < greenbboxes.count; i++) {
            CV_bounding_box *box = &greenbboxes.boxes[i];
            if (((box->x[1] - box->x[0]) * (box->y[1] - box->y[0])) >=
                ((biggestgreenbox->x[1] - biggestgreenbox->x[0]) *
                 (biggestgreenbox->y[1] - biggestgreenbox->y[0]))) {
                *biggestgreenbox = *box;
            }
        }
        printf("Green box: x=[%d, %d], y=[%d, %d]\n",
               biggestgreenbox->x[0], biggestgreenbox->x[1],
               biggestgreenbox->y[0], biggestgreenbox->y[1]);

        // Detect red objects
        CV_chromakey(redmask, redframe, H_HM_RED, S_HM_RED, V_HM_RED);
        CV_masktracker(&redbboxes, redmask, 15);

        CV_bounding_box *biggestredbox = &redbboxes.boxes[0];
        for (size_t i = 0; i < redbboxes.count; i++) {
            CV_bounding_box *box = &redbboxes.boxes[i];
            if (((box->x[1] - box->x[0]) * (box->y[1] - box->y[0])) >=
                ((biggestredbox->x[1] - biggestredbox->x[0]) *
                 (biggestredbox->y[1] - biggestredbox->y[0]))) {
                *biggestredbox = *box;
            }
        }
        printf("Red box: x=[%d, %d], y=[%d, %d]\n",
               biggestredbox->x[0], biggestredbox->x[1],
               biggestredbox->y[0], biggestredbox->y[1]);

        if ((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) <
            (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) {
            printf("\nred box detected\n\n");
        } else {
            printf("\ngreen box detected\n\n");
        }
    }

    CV_closeplayer(player);

    return 0;
}
