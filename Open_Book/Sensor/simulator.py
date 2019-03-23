import time
import random
import threading 
import datetime


# Storage in the CSV Format for the following fields , per sensor.=> 
#T  = Temperature, <Value>, H = Humidity, <Value> , M = Methane, <Value> , C = CarbonMONO, <Value>, P = Propane, <Value> per sensor.

def Sensors(): 
    print("Sensor1:") 
    print(datetime.datetime.now())
    sensor1 = open(r"sensor1.txt","w")
    s1= random.randint(50,101)  
    sensor1.write("T,"+ str(s1)) 
    s1= random.randint(150,1101)
    sensor1.write(",H,"+ str(s1)) 
    s1= random.randint(200,1101)
    sensor1.write(",M,"+ str(s1)) 
    s1= random.randint(300,1101)
    sensor1.write(",C,"+ str(s1)) 
    s1= random.randint(400,1101)
    sensor1.write(",P,"+ str(s1)) 
    sensor1.close() #to change file access modes  

    sensor2 = open(r"sensor2.txt","w")
    print("Sensor2:") 
    print(datetime.datetime.now())
    s2= random.randint(50,90)
    sensor2.write("T,"+ str(s2)) 
    s2= random.randint(155,1101)
    sensor2.write(",H,"+ str(s2)) 
    s2= random.randint(205,1101)
    sensor2.write(",M,"+ str(s2)) 
    s2= random.randint(305,1101)
    sensor2.write(",C,"+ str(s2)) 
    s2= random.randint(405,1101)
    sensor2.write(",P,"+ str(s2)) 
    sensor2.close() #to change file access modes  
  
    sensor3 = open(r"sensor3.txt","w")
    print("Sensor3:") 
    print(datetime.datetime.now())
    s3= random.randint(1150,1190)
    sensor3.write("T,"+ str(s3)) 
    s3= random.randint(255,1101)
    sensor3.write(",H,"+ str(s3)) 
    s3= random.randint(305,1101)
    sensor3.write(",M,"+ str(s3)) 
    s3= random.randint(505,1101)
    sensor3.write(",C,"+ str(s3)) 
    s3= random.randint(605,1101)
    sensor3.write(",P,"+ str(s3)) 
    sensor3.close() #to change file access modes  

    sensor4 = open(r"sensor4.txt","w")
    print("Sensor4:") 
    print(datetime.datetime.now())
    s4= random.randint(2050,2090)
    sensor4.write("T,"+ str(s4)) 
    s4= random.randint(655,1101)
    sensor4.write(",H,"+ str(s4)) 
    s4= random.randint(755,1101)
    sensor4.write(",M,"+ str(s4)) 
    s4= random.randint(855,1101)
    sensor4.write(",C,"+ str(s4)) 
    s4= random.randint(405,1101)
    sensor4.write(",P,"+ str(s4)) 
    sensor4.close() #to change file access modes  
  

    sensor5 = open(r"sensor5.txt","w")
    print("Sensor5:") 
    print(datetime.datetime.now())
    s5 = random.randint(3050,3090)
    sensor5.write("T,"+ str(s5)) 
    s5= random.randint(755,1101)
    sensor5.write(",H,"+ str(s5)) 
    s5= random.randint(855,1101)
    sensor5.write(",M,"+ str(s5)) 
    s5= random.randint(955,1101)
    sensor5.write(",C,"+ str(s5)) 
    s5= random.randint(1005,1101)
    sensor5.write(",P,"+ str(s5)) 
    sensor5.close() #to change file access modes  
 
    return 

while True :
    time.sleep(2) 
    print("-----------------------\n")
    Sensors()
print("Exit\n") 
