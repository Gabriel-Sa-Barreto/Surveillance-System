import paho.mqtt.client as mqtt
import numpy as np
import cv2
import time

class MQTTClient:
    def __init__(self, broker, port, keepaLive):
        self.BROKER                = broker
        self.PORT                  = port
        self.KEEPALIVE             = keepaLive
        self.USERNAME              = "RaspCam_1"
        self.PASSWORD              = "RaspCam_1"
        self.MAX_NUMBER_OF_MSG     = 1
        self.client                = mqtt.Client()
        self.MQTT_TOPIC_SEND_IMAGE = "cam/img/"
        
        # Callbacks configurations
        self.client.on_connect    = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_subscribe  = self.on_subscribe
        self.client.on_publish    = self.on_publish
        self.client.on_message    = self.on_message
        
        self.last_frame = None

    def _check_error(self, method_name, rc):
        """
        Check the return code (rc) of the Paho operations.
        Use the native error constants and strings.
        """
        if rc == mqtt.MQTT_ERR_SUCCESS:
            return True
        else:
            error_msg = mqtt.error_string(rc)
            print(f"[ERROR] '{method_name}' operation failed: {error_msg} (Code: {rc})")
            return False

    def on_connect(self, client, userdata, flags, rc):
        # No on_connect, 'rc' é o Connect Return Code
        if self._check_error("Connect", rc):
            print(f"Success: Connected to broker {self.BROKER}")
            # Keeps only the last message in the queue to avoid backlog.
            self.client.max_queued_messages_set(self.MAX_NUMBER_OF_MSG)              
            res, mid = self.client.subscribe(self.MQTT_TOPIC_SEND_IMAGE)
            self._check_error("Subscribe Request", res)

    def on_disconnect(self, client, userdata, rc):
        if rc != mqtt.MQTT_ERR_SUCCESS:
            self._check_error("Unexpected Disconnect", rc)
        else:
            print("Successfully disconnected from the broker.")
    
    def on_subscribe(self, client, userdata, mid, granted_qos):
        print(f"Subscription confirmed on the topic: {self.MQTT_TOPIC_SEND_IMAGE} (MID: {mid})")

    def on_publish(self, client, userdata, mid):
        pass
    
    def on_message(self, client, userdata, msg):
        try:
            # 1. Convert the payload (bytes) to a NumPy array
            nparr = np.frombuffer(msg.payload, np.uint8)
            # 2. Decode the JPG image to OpenCV (BGR) format
            frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
            if frame is None:
                print("Erro na decodificação da imagem.")
            if frame is not None:
                self.process_frame(frame)
        except Exception as e:
            print(f"Error decoding image: {e}")

    def process_frame(self, frame):
        cv2.imshow("Monitoramento - Processo Python", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            self.stop()

    def start(self):
        try:
            self.client.username_pw_set(self.USERNAME, self.PASSWORD)
            rc = self.client.connect(self.BROKER, self.PORT, self.KEEPALIVE)
            if self._check_error("Initial Connection", rc):
                # Begins a thread to manage the MQTT's network process 
                self.client.loop_start()
        except Exception as e:
            print(f"Unable to connect to the broker.: {e}")

    def stop(self):
        self.client.loop_stop()
        self.client.disconnect()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    my_client = MQTTClient("raspberrypi.local", 1883, 60) # raspberrypi.local broker.emqx.io
    my_client.start()
    
    try:
        while True:
            time.sleep(1) # Mantém o processo principal vivo
    except KeyboardInterrupt:
        my_client.stop()