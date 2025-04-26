import zmq, cv2, numpy as np
import json
import tflite_runtime.interpreter as tflite
from ultralytics import YOLO

# Load YOLO
model = YOLO('dji.pt')
model.eval()

# Load TFLite Classifier
interpreter = tflite.Interpreter(model_path='Resnet8_72_128.tflite')
interpreter.allocate_tensors()
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()
input_height, input_width = input_details[0]['shape'][1:3]
labels = ['F', 'L', 'O', 'R']

# ZMQ SUB for receiving frames
ctx = zmq.Context()
sub = ctx.socket(zmq.SUB)
sub.connect("tcp://192.168.1.12:5557")
sub.setsockopt_string(zmq.SUBSCRIBE, '')
sub.RCVTIMEO = 10

# ZMQ PUB for sending result
pub = ctx.socket(zmq.PUB)
pub.bind("tcp://*:5557")  # Adjust for your network

latest_frame = None

while True:
    try:
        while True:
            msg = sub.recv(flags=zmq.NOBLOCK)
            latest_frame = msg
    except zmq.Again:
        pass

    if latest_frame:
        img = cv2.imdecode(np.frombuffer(latest_frame, np.uint8), cv2.IMREAD_COLOR)
        if img is None:
            continue

        # --- YOLO Detection ---
        results = model.predict(img, imgsz=640, conf=0.5, verbose=False, device='cpu')
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
        resized = cv2.resize(img, (input_width, input_height))
        input_data = np.expand_dims(resized.astype(np.float32) / 255.0, axis=0)
        interpreter.set_tensor(input_details[0]['index'], input_data)
        interpreter.invoke()
        output_data = interpreter.get_tensor(output_details[0]['index'])
        class_idx = int(np.argmax(output_data))
        class_label = labels[class_idx]
        class_confidence = round(float(output_data[0][class_idx]), 3)

        # --- Compose Result ---
        result = {
            "c": class_label,
            "pc": class_confidence,
            "boxes": boxes_out
        }

        topic = "ai"
        message = json.dumps(result)
        pub.send_string(f"{topic} {message}")

        print(f"[Result] Classifier: {class_label} ({class_confidence}), Boxes: {boxes_out}")
