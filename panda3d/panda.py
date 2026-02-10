from direct.showbase.ShowBase import ShowBase
from direct.task import Task
import socket
import select

class MyApp(ShowBase):
	def __init__(self):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.sock.bind(('', 5005))

		ShowBase.__init__(self)

		# Disable the camera trackball controls, if you want a static view
		# self.disableMouse()

		# Load the model
		self.model = loader.loadModel("models/ball")
		
		# Reparent the loaded model to render so it is visible in the scene
		self.model.reparentTo(render)

		# Position the model in the world
		self.model.setPos(0, 10, 0) # X, Y, Z coordinates
		self.model.setScale(6.0)

		# Apply a texture
		# I borrowed the texture from here.
		# https://science.nasa.gov/earth/earth-observatory/blue-marble-next-generation/base-map/
		tex = loader.loadTexture("texture/world.200401.3x5400x2700.jpg") # January
		#tex = loader.loadTexture("texture/world.200408.3x5400x2700.jpg") # August
		self.model.setTexture(tex, 1)

		self.currentYaw = 0.0
		self.taskMgr.add(self.MotionTask, "MotionTask")

	def MotionTask(self, task):
		#print("MotionTask self.yaw={}".format(self.yaw));
		result = select.select([self.sock],[],[],1)
		#print(result[0], type(result[0]), len(result[0]))
		if (len(result[0]) != 0):
			line = result[0][0].recv(1024)
			if (type(line) is bytes):
				line=line.decode('utf-8')
			#print("line={}".format(line))
			yaw = float(line.split('y')[1])
			pitch = float(line.split('p')[1])
			roll = float(line.split('r')[1])
			differenceYaw= abs(self.currentYaw - yaw)
			print("yaw={} currentYaw={} differenceYaw={}".format(yaw, self.currentYaw, differenceYaw))
			if (differenceYaw> 2.0):
				self.model.setHpr(yaw, pitch, roll)
				self.currentYaw = yaw

		return Task.cont

app = MyApp()
app.run()

