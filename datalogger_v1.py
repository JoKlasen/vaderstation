# -*- coding: utf-8 -*-
"""
Created on Sat May 16 14:33:03 2020

@author: erik
"""
import time
import csv
import serial


filename_time = time.strftime("%Y-%m-%d_%H-%M-%S",time.localtime())

path = '/home/erik/Dokument/python/Loggar/'


filename = path+'RainGaugeLogg_'+filename_time+'.csv'

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=2)
ser.flushInput()
#ser.flushOutput()

with open(filename, 'w') as csvfile:
    csvwriter = csv.writer(csvfile, delimiter=',')
    t_start = time.time()
    while True:
        data_raw = ser.readline()
#        data = float(data_raw.decode("utf-8"))
#        print(data_raw)
#        csvwriter.writerow([time.time()-t_start,data_raw])
#        if len(data_raw)==0:
#            csvwriter.writerow([time.time()-t_start,'NaN'])
#        else:
#            if float(data_raw[0])==0:
#                csvwriter.writerow([time.time()-t_start,0])
#            elif float(data_raw[0])==1:
#                csvwriter.writerow([time.time()-t_start,1])
#            else:
#                csvwriter.writerow([time.time()-t_start,'NaN'])

        try:
            csvwriter.writerow([time.time()-t_start,float(data_raw[0])])
        except:
            csvwriter.writerow([time.time()-t_start,'NaN'])

