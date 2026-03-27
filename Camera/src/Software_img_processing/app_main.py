from libs.MQTT import MQTTClient
from libs.logger_config import get_logger
import time
import cv2
import queue # This queue class is Thread-Safe.

if __name__ == "__main__":
    logger      = get_logger("Main")
    frame_queue = queue.Queue()
    my_client   = MQTTClient("raspberrypi.local", 1883, 60, frame_queue) # raspberrypi.local broker.emqx.io
    my_client.start()
    try:
        while True:
            try:
                frame = frame_queue.get(block = True, timeout = 1.0)
                if frame is None:
                    logger.error("Frame is None.")
                else:
                    cv2.imshow("Monitoring - Python Process", frame)
                    if cv2.waitKey(1) & 0xFF == ord('q'):
                        logger.info("Closing screen by key 'q'...")
                        break
                # The item has been processed.
                frame_queue.task_done()
            except queue.Empty:
                logger.debug("Frame queue is empty. Waiting....")
                continue
    except KeyboardInterrupt:
        my_client.stop()