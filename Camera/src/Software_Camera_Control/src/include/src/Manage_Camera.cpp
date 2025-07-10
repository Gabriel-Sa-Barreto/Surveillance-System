#include "Manage_Camera.h"

using namespace libcamera;
using namespace libcamera::controls;
using namespace std;

// Shared pointer variable for the camera to support the event call back.
static shared_ptr<Camera>   camera;

// Definition of the static atribute
unique_ptr<CameraManager>                      ManageCamera::CamManager    = nullptr;
unique_ptr<CameraConfiguration>                ManageCamera::CamConfig     = nullptr;
FrameBufferAllocator*                          ManageCamera::allocator     = nullptr;
vector<unique_ptr<Request>>                    ManageCamera::videoRequests = {};

bool ManageCamera::initCamera()
{
    CamManager = make_unique<CameraManager>();
    if(!CamManager->start())
        log_info("Camera Manager started succesfully.");
    else
    {
        log_error("Error on Camera Manager initialization.");
        return false;
    }

    auto availableCameras = CamManager->cameras();
    if (availableCameras.empty())
    {
        log_error("No cameras were identified on the system.");
        CamManager->stop();
        return false;
    }else
    {
        // Show the list of cameras detected.
        for (auto const &cm : availableCameras)
        cout << cm->id() << endl;
    }
    // Get the first camera id on list.
    string cameraId = availableCameras[0]->id();
    camera          = CamManager->get(cameraId);

    //Now, the application needs to acquire an exclusive lock to it so no other application can use it.
    return (!camera->acquire()) ? true : false;
}

bool ManageCamera::configureCam()
{
    CamConfig = camera->generateConfiguration({ StreamRole::Viewfinder } );
    if(CamConfig == nullptr)
    {
        log_error("It's not possible set the request roles for camera configuration.");
        return false;
    }

    StreamConfiguration &config = CamConfig->at(0);
    config.pixelFormat          = formats::RGB888;
    config.size.width           = 800;
    config.size.height          = 600;

    for(StreamConfiguration &streamConfig : *CamConfig)
    {
        cout << "Parameters of role: " << streamConfig.toString() << endl;
    }

    CameraConfiguration::Status validateStatus = CamConfig->validate(); 
    switch(validateStatus)
    {
        case CameraConfiguration::Status::Valid:
            cout << "VALIDATION STATUS: Valid" << endl;
            break;
        case CameraConfiguration::Status::Invalid:
            cout << "VALIDATION STATUS: Invalid" << endl;
            break;
        case CameraConfiguration::Status::Adjusted:
            cout<< "VALIDATION STATUS: Adjusted" << endl;
            break;
        default:
            log_error("Error on camera configuration validate.");
    }

    if((validateStatus != CameraConfiguration::Status::Valid) && (validateStatus != CameraConfiguration::Status::Adjusted))
        return false;

    if(!camera->configure(CamConfig.get()))
        log_info("Camera configured!!");


    allocator = new FrameBufferAllocator(camera);
    for (StreamConfiguration &cfg : *CamConfig)
    {
        int ret = allocator->allocate(cfg.stream());
        if (ret < 0)
        {
            log_error("Can't allocate buffers");
            return false;
        }

        size_t allocated = allocator->buffers(cfg.stream()).size();
        std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
    }

    /* 
        Each configuration (Viewfinder) will have its own buffer.
    */
    StreamConfiguration &streamConfig              = CamConfig->at(0);
    Stream *stream                                 = streamConfig.stream();
    const vector<unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
    log_info("Creating first requests....");
    for(unsigned int i = 0; i < buffers.size(); i++)
    {
        unique_ptr<Request> request = camera->createRequest();
        if (!request)
        {
            log_error("Can't create request");
            return false;
        }
        const std::unique_ptr<FrameBuffer> &bf = buffers[i];
        int ret = request->addBuffer(streamConfig.stream(), bf.get());
        if (ret < 0)
        {
            log_error("Can't set buffer for request");
            return false;
        }
        request->controls().set(controls::ExposureTime, 33167);  // 33.167 ms
        request->controls().set(controls::AnalogueGain, 8.0);    // 8×

        videoRequests.push_back(move(request));
    }

    log_info("First requests created.");

    camera->requestCompleted.connect(requestComplete);

    return true;
}

void ManageCamera::startCamera()
{
    camera->start();
    if(!videoRequests.empty())
    {
        // Queues the first requests.
        for (unique_ptr<Request> &request : videoRequests)
            camera->queueRequest(request.get());
    }
}

void ManageCamera::stopCameraProcess()
{
    camera->stop();
    for(StreamConfiguration &cfg : *CamConfig)
        allocator->free(cfg.stream());
    delete allocator;
    camera->release();
    camera.reset();
    log_info("Released camera.");
    CamManager->stop();
    log_info("Camera Manager stopped.");
}

void ManageCamera::requestComplete(Request *request)
{
    if ((request->status() == Request::RequestCancelled) || (request->status() == Request::RequestPending))
        return;
    // Request completed
    const std::map<const Stream *, FrameBuffer *> &buffersCompleted = request->buffers();
    StreamConfiguration &streamConfig  = CamConfig->at(0);
    //log_debug("New request is ready!!");
    for (auto bufferPair : buffersCompleted)
    {
        FrameBuffer *buffer = bufferPair.second;
        
        const void *data = mmap(nullptr, buffer->planes()[0].length, PROT_READ, MAP_SHARED, buffer->planes()[0].fd.get(), 0);
        if (data == MAP_FAILED)
            continue;
        cv::Mat rgb_frame(streamConfig.size.height, streamConfig.size.width, CV_8UC3, const_cast<void *>(data));
        cv::imshow("Camera", rgb_frame);


        munmap((void *)data, buffer->planes()[0].length);
    }
    request->reuse(Request::ReuseBuffers);
    camera->queueRequest(request);
}

