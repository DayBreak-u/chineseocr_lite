#!/usr/bin/env python
# encoding: utf-8
# author:alisen
# time: 2020/5/29 10:48

import logging
from logging.handlers import RotatingFileHandler
import os
import datetime

BASE_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

LOGGER_ROOT_NAME = 'TrWebOCRLog'
logger = logging.getLogger(LOGGER_ROOT_NAME)
logger.setLevel(logging.INFO)
# Formatter
formatter = logging.Formatter('%(asctime)s - %(name)s - [%(levelname)s] | %(message)s',
                              datefmt='%Y/%m/%d %H:%M:%S')

logfile_name = datetime.date.today().__format__('%Y-%m-%d.log')
logfile_path = os.path.join(BASE_PATH, f'logs/')
if not os.path.exists(logfile_path):
    os.mkdir(logfile_path)

handler_logfile = RotatingFileHandler(logfile_path + logfile_name,
                                      maxBytes=1 * 1024 * 1024,
                                      backupCount=3)
handler_logfile.setLevel(logging.INFO)
handler_logfile.setFormatter(formatter)

console_output = logging.StreamHandler()
console_output.setLevel(logging.INFO)
console_output.setFormatter(formatter)

logger.addHandler(handler_logfile)
logger.addHandler(console_output)
