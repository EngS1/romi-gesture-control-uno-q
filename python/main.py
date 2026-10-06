# SPDX-FileCopyrightText: Copyright (C) ARDUINO SRL (http://www.arduino.cc)
# SPDX-FileCopyrightText: Copyright (C) 2026 Daouda SYLLA
#
# SPDX-License-Identifier: MPL-2.0
#
# Fichier dérivé de l'exemple Arduino App Lab « Detect Hands on Smartphone Camera ».
# Modifié : ajout de send_message_to_romi() (traduction des détections en commandes
# pour le robot ROMI) et de la mise à jour du curseur « volant » dans l'interface.

import secrets
import string
from datetime import datetime, UTC

from arduino.app_utils import *
from arduino.app_bricks.web_ui import WebUI
from arduino.app_bricks.video_objectdetection import VideoObjectDetection
from arduino.app_peripherals.camera import WebSocketCamera
previousCode = ""
code = ""
occurrences = {"VG":0, "VD": 0, "AV": 0, "FR": 0}
volant = 0

def send_message_to_romi(message):
    global code
    global previousCode
    global occurrences
    global volant
    previousCode = code
    code = ""
    match message:
        case "Volant Gauche":
            code = "VG"
            
        case "Volant Droit":
            code = "VD"

        case "Avancer":
            code = "AV"

        case "Freiner":
            code = "FR"
        case _:
            print('No datat to send to ROMI')

    print(code)
    if code != previousCode:
        Bridge.call("ai_command", code)
        occurrences[previousCode] = 0
        previousCode = code
        match code:
            case "VG":
                if volant >= -90:
                    volant -= 10
            
            case "VD":
                if volant <= 90:
                    volant += 10

            case _:
                pass
                
        ui.send_message('slider_update', {'valeur': volant})
        
    else:
        occurrences[code] += 1
        if occurrences[code] == 5:
            Bridge.call("ai_command", code)
            occurrences[code] = 0
            match code:
                case "VG":
                    if volant >= -90:
                        volant -= 10
            
                case "VD":
                    if volant <= 90:
                        volant += 10

                case _:
                    pass
                    
            ui.send_message('slider_update', {'valeur': volant})
        


def generate_secret() -> str:
  characters = string.digits
  return ''.join(secrets.choice(characters) for _ in range(6))

secret = generate_secret()

ui = WebUI()  # set use_tls=True to enable TLS encryption for HTTPS
camera = WebSocketCamera(secret=secret, encrypt=True)
camera.on_status_changed(lambda evt_type, data: ui.send_message(evt_type, data))

detection = VideoObjectDetection(camera, confidence=0.5, debounce_sec=0.0)

ui.on_connect(lambda sid: ui.send_message("welcome", {"client_name": camera.name, "secret": secret, "status": camera.status, "protocol": camera.protocol, "ip": camera.ip, "port": camera.port}))
ui.on_message("override_th", lambda sid, threshold: detection.override_threshold(threshold))

# Register a callback for when all objects are detected
def send_detections_to_ui(detections: dict):
  for key, values in detections.items():
    for value in values:
      entry = {
        "content": key,
        "confidence": value.get("confidence"),
        "timestamp": datetime.now(UTC).isoformat()
      }
      send_message_to_romi(key)
      ui.send_message("detection", entry)

detection.on_detect_all(send_detections_to_ui)

App.run()
