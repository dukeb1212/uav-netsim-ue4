import zmq, cv2, numpy as np
import json, time
import tensorflow as tf
from ultralytics import YOLO

# Load YOLO
model = YOLO('dji.pt')
model.eval()

# Load TFLite Classifier
#interpreter = tf.lite.Interpreter(model_path='Resnet8_72_128.tflite')
#interpreter.allocate_tensors()
#input_details = interpreter.get_input_details()
#output_details = interpreter.get_output_details()
#input_height, input_width = input_details[0]['shape'][1:3]
#labels = ['F', 'L', 'O', 'R']

# ZMQ SUB for receiving frames
ctx = zmq.Context()
sub = ctx.socket(zmq.SUB)
sub.connect("tcp://192.168.1.12:5557")
sub.setsockopt_string(zmq.SUBSCRIBE, '')
sub.RCVTIMEO = 10

# ZMQ PUB for sending result
pub = ctx.socket(zmq.PUB)
pub.bind("tcp://*:557")

latest_frame = None

def now_ns():
    return time.time_ns()

while True:
    try:
        while True:
            msg = sub.recv(flags=zmq.NOBLOCK)
            latest_frame = msg
    except zmq.Again:
        pass

    if latest_frame:
        if len(latest_frame) < 8:
            print("[DEBUG] Frame too short to contain metadata. Length:", len(latest_frame))
            continue

        # --- Extract Header ---
        frame_number = int.from_bytes(latest_frame[0:8], byteorder='little', signed=True)
        jpeg_data = latest_frame[8:]

        ai_start_ns = now_ns()

        # Decode image
        img = cv2.imdecode(np.frombuffer(jpeg_data, np.uint8), cv2.IMREAD_COLOR)
        if img is None:
            print("[ERROR] Failed to decode JPEG image.")
            continue

        # --- YOLO Detection ---
        results = model.predict(img, imgsz=640, conf=0.5, verbose=False, device='cuda')
        boxes_out = []
        for box in results[0].boxes:
            cls_id = int(box.cls)
            conf = float(box.conf)
            if model.names[cls_id] == "person":
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                boxes_out.append({
                    "b": [x1, y1, x2, y2],
                    "c": round(conf, 3)
                })

        # --- Classifier ---
        #resized = cv2.resize(img, (input_width, input_height))
        #input_data = np.expand_dims(resized.astype(np.float32) / 255.0, axis=0)
        #interpreter.set_tensor(input_details[0]['index'], input_data)
        #interpreter.invoke()
        #output_data = interpreter.get_tensor(output_details[0]['index'])
        #class_idx = int(np.argmax(output_data))
        #class_label = labels[class_idx]
        #class_confidence = round(float(output_data[0][class_idx]), 3)

        ai_end_ns = now_ns()
        ai_latency_ns = ai_end_ns - ai_start_ns

        # --- Compose Result ---
        result = {
            "frame": frame_number,
            "ai_latency_ns": ai_latency_ns,
            "boxes": boxes_out
        }

        topic = "ai"
        message = json.dumps(result)
        pub.send_string(f"{topic} {message}")

        print(
            f"[Result] Frame {frame_number} | "
            f"Boxes: {len(boxes_out)} | "
            f"AI latency: {ai_latency_ns / 1e6:.2f} ms"
        )
