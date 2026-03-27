import logging
import sys
from logging.handlers import RotatingFileHandler

def get_logger(name):
    logger = logging.getLogger(name)
    # If the logger already has handlers, do not add new ones (this avoids duplicate logs).
    if not logger.handlers:
        logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        # Handler for Console
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setFormatter(formatter)
        console_handler.setLevel(logging.DEBUG)
        # Handler with Automatic Rotation (File). 
        # It limits the file size to 5MB and creates "backups", automatically deleting the oldest ones and leaving only the 3 most recent.
        # maxBytes = 5*1024*1024 (5MB)
        file_handler = RotatingFileHandler(
            "./log/log_img_processing.txt", maxBytes = 5*1024*1024, backupCount = 3
        )
        file_handler.setFormatter(formatter)
        file_handler.setLevel(logging.DEBUG)

        logger.addHandler(console_handler)
        logger.addHandler(file_handler)
        
    return logger