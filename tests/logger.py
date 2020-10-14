import logging
import sys
logging.basicConfig()
logging.root.setLevel(logging.INFO)


def myLogger():
    logger = logging.getLogger()
    if not logger.handlers:

        logger.addHandler(logging.StreamHandler(sys.stdout))
        logger.propagate = False
    return logger
