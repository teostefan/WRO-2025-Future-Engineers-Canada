#include <stdio.h>

#include "cv.h"
#include "cv_config.h"

int main() {
    CV_camerapipe camera = CV_getcamera("/dev/video0", "gblur=0.5");
    if (!camera) return 0;

    CV_playerpipe player = CV_getplayer();
    if (!player) return 0;

    CV_frame frame;
    int frame_count = 0;

    CV_mask mask;
    CV_bounding_box_list bboxes = {0};

    while (frame_count++ < 100) {                     // Limit to 100 frames for demonstration
        if (!CV_getHSVframe(frame, camera)) return 0; // Load an HSV frame.

        CV_chromakey(mask, frame, (unsigned char[]){33, 3}, (unsigned char[]){20, 80}, (unsigned char[]){20, 80});

        CV_masktracker(&bboxes, mask, 50);

        if (!CV_getRGBframe(frame, camera)) return 0; // Load an RGB frame for display.

        CV_drawbb(frame, &bboxes, (unsigned char[]){255, 0, 0}); // Draw bounding boxes in red.

        CV_playframe(player, frame); // Play the frame.
        /*
        printf("Frame %d: Found %zu bounding boxes\n", frame_count, bboxes.count);
        for (size_t i = 0; i < bboxes.count; i++) {
            CV_bounding_box *box = bboxes.boxes[i];
            printf("Box %zu: x=[%d, %d], y=[%d, %d]\n", i, box->x[0], box->x[1], box->y[0], box->y[1]);
        }*/
    }

    CV_closecamera(camera);

    CV_closeplayer(player);

    return 0;
}