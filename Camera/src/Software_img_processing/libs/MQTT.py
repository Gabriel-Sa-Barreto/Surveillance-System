from libs.logger_config import get_logger
import paho.mqtt.client as mqtt
import numpy as np
import cv2
import queue

class MQTTClient:
    def __init__(self, broker, port, keepaLive, frame_queue):
        self.BROKER                = broker
        self.PORT                  = port
        self.KEEPALIVE             = keepaLive
        self.USERNAME              = "RaspCam_1"
        self.PASSWORD              = "RaspCam_1"
        self.MAX_NUMBER_OF_MSG     = 1
        self.client                = mqtt.Client()
        self.MQTT_TOPIC_SEND_IMAGE = "cam/img/"
        # Queue to transfer frames between MQTT thread and the processing thread.
        self.frame_queue           = frame_queue
        # Callbacks configurations
        self.client.on_connect    = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_subscribe  = self.on_subscribe
        self.client.on_publish    = self.on_publish
        self.client.on_message    = self.on_message
        # ========
        self.last_frame = None
        self.logger = get_logger("MQTT-Client")

    def _check_error(self, method_name, rc):
        """
        Check the return code (rc) of the Paho operations.
        Use the native error constants and strings.
        """
        if rc == mqtt.MQTT_ERR_SUCCESS:
            return True
        else:
            error_msg = mqtt.error_string(rc)
            self.logger.warning(f"'{method_name}' operation failed: {error_msg} (Code: {rc})")
            return False

    def on_connect(self, client, userdata, flags, rc):
        # No on_connect, 'rc' é o Connect Return Code
        if self._check_error("Connect", rc):
            self.logger.info(f"Success: Connected to broker {self.BROKER}")
            # Keeps only the last message in the queue to avoid backlog.
            self.client.max_queued_messages_set(self.MAX_NUMBER_OF_MSG)              
            res, mid = self.client.subscribe(self.MQTT_TOPIC_SEND_IMAGE)
            self._check_error("Subscribe Request", res)

    def on_disconnect(self, client, userdata, rc):
        if rc != mqtt.MQTT_ERR_SUCCESS:
            self._check_error("Unexpected Disconnect", rc)
        else:
            self.logger.info("Successfully disconnected from the broker.")
    
    def on_subscribe(self, client, userdata, mid, granted_qos):
        self.logger.info(f"Subscription confirmed on the topic: {self.MQTT_TOPIC_SEND_IMAGE} (MID: {mid})")

    def on_publish(self, client, userdata, mid):
        pass
    
    def on_message(self, client, userdata, msg):
        try:
            # 1. Convert the payload (bytes) to a NumPy array
            nparr = np.frombuffer(msg.payload, np.uint8)
            # 2. Decode the JPG image to OpenCV (BGR) format
            frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
            if frame is None:
                self.logger.error("The opencv frame is None.")
            else:
                self.frame_queue.put(frame)
        except Exception as e:
            self.logger.error(f"Error decoding image: {e}")

    def start(self):
        try:
            self.client.username_pw_set(self.USERNAME, self.PASSWORD)
            rc = self.client.connect(self.BROKER, self.PORT, self.KEEPALIVE)
            if self._check_error("Initial Connection", rc):
                # Begins a thread to manage the MQTT's network process 
                self.client.loop_start()
        except Exception as e:
            self.logger.error(f"Unable to connect to the broker.: {e}")

    def stop(self):
        self.client.loop_stop()
        self.client.disconnect()
        cv2.destroyAllWindows()