# -*- coding: utf-8 -*-
"""
Created on Sat May 16 18:42:37 2020

@author: erik
"""

import csv
import matplotlib.pyplot as plt

filename = 'RainGaugeLogg_2020-05-18_15-41-40.csv'
path = '/home/erik/Dokument/python/Loggar/'

with open(path+filename, 'r') as csvfile:
    df = csv.reader(csvfile, delimiter=',')
#    n=0
    data, time, value = {}, [], []
    for n, row in enumerate(df):
        data[n]  = [float(row[0]), float(row[1])]
        time.append(float(row[0]))
        value.append(float(row[1]))

plt.close('all')  

#plt.figure()
#plt.clf()
#plt.plot(time, value, '*')


#%%

#h_cnt=0
#cntr_val =0
#cntr, h_cntr = [], []
#for n,v in enumerate(value):
#    if v==0 and h_cnt>10:
#        h_cnt = 0
#        cntr_val += 1 
#    elif v==0 and h_cnt<=10:
#        h_cnt = 0
#    elif v==1:
#        h_cnt +=1
#    cntr.append(cntr_val)
#    h_cntr.append(h_cnt)

#plt.figure()
#plt.clf()
#plt.subplot(2,1,1)
#plt.plot(time, value, '*')
#plt.subplot(2,1,2)
#plt.plot(time, cntr, 'r')
#plt.plot(time, h_cntr, 'b')

#%%

state = 0
B_SwInProg = 0
t_tresh = 0.25
D_FlipCounter = 0
t_LatestSw = 0
D_FlipSmplCntr = 0
D_SmplTresh = 20

vec_B_SwInProg = []
vec_t_LatestSw = []
vec_state = []
vec_D_FlipCounter = []

for n, v in enumerate(value):
    if value[n] == 0 or value[n] == 1:
        if value[n] != state and B_SwInProg == 0:
            t_SwStart = time[n]
            B_SwInProg = 1
            D_FlipSmplCntr += 1
        elif value[n] != state and B_SwInProg == 1:
            if time[n]-t_SwStart > t_tresh:
                state = value[n]
                B_SwInProg = 0
                D_FlipCounter += 1
                D_FlipSmplCntr = 0
                t_LatestSw = time[n]
            else:
                D_FlipSmplCntr += 1
        elif value[n]==state:
            B_SwInProg = 0
            D_FlipSmplCntr = 0
    vec_B_SwInProg.append(B_SwInProg)
    vec_t_LatestSw.append(t_LatestSw)
    vec_state.append(state)
    vec_D_FlipCounter.append(D_FlipCounter)

plt.figure('Rain gauge measurement')
plt.clf()
plt.title('Rain Gauge measure data')
plt.subplot(2,1,1)
plt.plot(time, value, 'b*', label='Raw signal')
plt.plot(time, vec_B_SwInProg, 'r', label='Switch in progress')
plt.plot(time, vec_state, 'b', label='Estimated state')
plt.legend()
plt.ylim([-0.1, 1.1])
plt.ylabel('Signal Value [bool]')
plt.subplot(2,1,2)
plt.plot(time, vec_D_FlipCounter, 'k', label='Counter')
plt.xlabel('Time [s]')
plt.ylabel('Counter [-]')
