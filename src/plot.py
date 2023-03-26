import datetime as dt
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial

# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = []
ys = []

ser = serial.Serial('COM6', 9600)
# import random
def get_data():
    ser.write(bytes("c", 'utf-8'))# c,s,b for current, shunt voltage, bus voltage respectively
    recieved = ser.read_all().strip()
    print(recieved, end="")
    if recieved == b'':
        return 0
    data = int(recieved)
    return data
    # return random.randint(0, 100)


# This function is called periodically from FuncAnimation
def animate(i, xs, ys):

    point = get_data()

    # Add x and y to lists
    xs.append(dt.datetime.now().strftime('%H:%M:%S.%f'))
    ys.append(point)

    # Limit x and y lists to 20 items
    xs = xs[-20:]
    ys = ys[-20:]

    # Draw x and y lists
    ax.clear()
    ax.plot(xs, ys)

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.title('data over Time')
    plt.ylabel('data')

# Set up plot to call animate() function periodically
ani = animation.FuncAnimation(fig, animate, fargs=(xs, ys), interval=1000)#interval is time resolution of graph
plt.show()