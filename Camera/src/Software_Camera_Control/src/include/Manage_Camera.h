#ifndef __MANAGE_CAMERA_H
    #define __MANAGE_CAMERA_H

    #include <iomanip>
    #include <iostream>
    #include <memory>
    #include <vector>
    #include <map>
    #include <libcamera/libcamera.h>
    #include "log.h"
    #include <opencv2/opencv.hpp>
    #include <sys/mman.h>

    using namespace libcamera;
    using namespace std;

    class ManageCamera 
    {
        private:
            static unique_ptr<CameraManager>        CamManager;
            static unique_ptr<CameraConfiguration>  CamConfig;
            static FrameBufferAllocator            *allocator;

            static void requestComplete(Request *request);

        public:
            static vector<unique_ptr<Request>> videoRequests;

            ManageCamera(){}
            ~ManageCamera(){}

            bool initCamera();
            bool configureCam();
            void stopCameraProcess();
            void startCamera();
    };
#endif