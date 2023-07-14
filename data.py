import pandas as pd
import matplotlib.pyplot as plt
import sys
import numpy as np
import pylab

def drift(folder, files):
    #load the csv file

    i = 0
    for file in files:
        print("File: ", file)
        x = 0
        if file == 'DriftTime.csv' or file == 'TotalDriftTime.csv' or 'DriftTime_1s.csv' or file == 'TotalDriftTime_1s.csv' or 'DriftTime_001s.csv' or file == 'TotalDriftTime_001.csv' or file == 'DriftTime_01s.csv' or file == 'TotalDriftTime_01s.csv':
            x = 1
        #create a path to the file
        fileoriginal = file
        file = folder + "/" + file
        
        df = pd.read_csv(file)
        
        number = df.shape[0]
        
        df = df[x:number]
        number = number - x
        
        
        #plot the drift data
        #get the mean of the data
        mean = df['data'].mean()
        print("Mean is: ",mean)
        std = df['data'].std()
        print("Std is: ",std)
        print("Max is: ",df['data'].max())
        print("Min is: ",df['data'].min())

        
        titles = ["", " without outliers"]
        data1 = []
        data2 = []
        data_ar = [data1, data2]

        for z in range(x, number):
            data1.append(df['data'][z])
            if df['data'][z] < mean + 2*std and df['data'][z] > mean - 2*std: 
                data2.append(df['data'][z])

        
        
        std1 = (mean + std)
        std2 = (mean - std)
        mean = round(mean, 3)
        std1 = round(std1, 3)
        std2 = round(std2, 3)
        for title, data in zip(titles, data_ar):
        
            plt.title(file + title)
            
            fig = pylab.gcf()
            fig.canvas.manager.set_window_title(fileoriginal + title)
            plt.plot([i for i in range(len(data))],  data, color='blue') 
            plt.xlabel("Execution")
            plt.ylabel("Time (us)")
            #plot in red color
            plt.plot([i for i in range(len(data))], [mean for i in range(len(data))], color='red', label= ("Mean: " + str(mean) + " us."))
            #get standard deviation
            
            plt.plot([i for i in range(len(data))], [mean + df['data'].std() for i in range(len(data))], color='green', label= ("Mean + Std:  " + str(std1) + " us."))
            plt.plot([i for i in range(len(data))], [mean - df['data'].std() for i in range(len(data))], color='green',  label= ("Mean - Std:  " + str(std2) + " us."))
            #bbox_to_anchor=(1.05, 1.0)
            plt.legend(loc='upper right')

            plt.tight_layout()
            plt.savefig(folder + "/Charts/" + fileoriginal + title + ".png")
            
            plt.close()

            

        data = []
        
        
               
               

files = ['DriftTime.csv', 'TotalDriftTime.csv', 'FunctionExecTime.csv', 'JobWaitTime.csv', 'QueueInsertTime.csv']
files2 = ['DriftTime_1s.csv', 'DriftTime_01s.csv', 'DriftTime_001s.csv','TotalDriftTime_1s.csv','TotalDriftTime_01s.csv','TotalDriftTime_001s.csv', 'FunctionExecTime.csv', 'JobWaitTime.csv', 'QueueInsertTime.csv']
files2 = ['QueueInsertTime.csv','FunctionExecTime.csv']
    

drift("data 0.01s", files)