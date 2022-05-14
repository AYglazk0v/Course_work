from kivy.uix.screenmanager import Screen
from kivy.lang import Builder
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import StringProperty
from kivy.utils import platform

from kivymd.uix.dialog import MDDialog
from kivymd.uix.button import MDFlatButton
from kivymd.uix.filemanager import MDFileManager
from kivymd.toast import toast

from datetime import datetime
import socket
import ssl
import pathlib
import time
import os

if platform == 'android':
	from android.permissions import request_permissions, Permission

Builder.load_string("""
<ReceiverPropmtContent>:
	orientation: 'vertical'
	spacing: "12dp"
	size_hint_y: None
	height: "50dp"
	MDTextField:
		id: receiver_ip
		hint_text: "Receiver IP"
		helper_text: "Find the ip of the reciever in the reciever's screen"
		helper_text_mode: 'on_focus'

<ClientScreen>:
	id: send_screen
	name: 'send_screen'
	MDLabel:
		orientation: 'vertical'
		size_hint_y: None
		pos_hint: {'top': 0.9}
		text: 'Steps2Hell'
		font_style: 'H2'
		halign: 'center'
	MDRaisedButton:
		text: "Two way sync"
		elevation: 10
		size_hint: 1,None 
		pos_hint: {'center_x': 0.5, 'center_y': 0.4}
		on_release: root.open_filemanager(1)
	MDRaisedButton:
		text: "One-way sync: phone"
		elevation: 10
		size_hint: 1,None
		pos_hint: {'center_x': 0.5, 'center_y': 0.5}
		on_release: root.open_filemanager(2)
	MDRaisedButton:
		text: "One-way sync: computer"
		elevation: 10
		size_hint: 1, None
		pos_hint: {'center_x': 0.5, 'center_y': 0.6}
		on_release: root.open_filemanager(3)
	MDRaisedButton:
		text: "Exit"
		elevation: 10
		size_hint: 1, None
		pos_hint: {'center_x': 0.5, 'center_y': 0.3}
		on_release: root.exit(4)
	MDRaisedButton:
		text: "Exit and kill server"
		elevation: 10
		size_hint: 1, None
		pos_hint: {'center_x': 0.5, 'center_y': 0.2}
		on_release: root.exit(5)
""")

class Connection:
	port = 3389
	address = None
	socket = None

	def __init__(self, address):
		self.address = address

	def connect(self):
		if platform == 'android':
			ca_path = "/storage/emulated/0/.Steps2Hell/CA/"
		else:
			ca_path = "./CA/"
		self.sslSettings = ssl.SSLContext(protocol=ssl.PROTOCOL_TLS_CLIENT);
		self.sslSettings.verify_mode = ssl.CERT_REQUIRED;
		self.sslSettings.load_verify_locations(ca_path + "ca.crt");
		self.sslSettings.load_cert_chain(certfile = ca_path + "client.crt", keyfile= ca_path + "client.key");
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
		self.socket = self.sslSettings.wrap_socket(s, server_hostname="server");
		s.close();
		self.socket.connect((self.address, self.port))
		try:
			self.socket.do_handshake()
			toast("SSL handshake successful")
		except:
			toast("Could not perform SSL handshake")
			self.disconnect()
			exit()
		return self.socket

	def disconnect(self):
		self.socket.close()

class ReceiverPropmtContent(BoxLayout):
	pass


class ClientScreen(Screen):
	receiver_ip = StringProperty("")

	def	__init__(self, *args, **kwargs):
		super(ClientScreen, self).__init__(*args, **kwargs)
		self.type = None
		self.file_manager = MDFileManager(
			exit_manager=self.on_filemanager_exit,
			select_path=self.on_select_path,
			search="dirs"
		)

	def set_receiver_ip(self, value):
		self.receiver_ip = value
		self.connection = Connection(self.receiver_ip)
		try:
			self.socket = self.connection.connect()
			self.receiver_prompt.dismiss()
		except:
			toast("Error: incorrect IP")
			exit()

	def on_enter(self):
		def callback(permission, resault):
			if all([res for res in resault]):
				toast("Permission received")
			else:
				toast("No access to certificates")
				exit()
		if platform == 'android':
				request_permissions([Permission.READ_EXTERNAL_STORAGE,
						Permission.WRITE_EXTERNAL_STORAGE], callback)
		self.receiver_prompt = MDDialog(
			title="Enter Receiver IP",
			type="custom",
			auto_dismiss=False,
			content_cls=ReceiverPropmtContent(),
			buttons=[
				MDFlatButton(
					text="OK",
					on_release=lambda x: self.set_receiver_ip(
						self.receiver_prompt.content_cls.ids.receiver_ip.text
					)
				)
			])
		self.receiver_prompt.open()

	def open_filemanager(self, type_sync, *args):
		self.type = type_sync
		path = "/"
		if platform == 'android':
			path = '/storage/emulated/0/'
		self.file_manager.show(path)

	def on_select_path(self, paths):
		self.sync_file(paths)
		self.file_manager.close()

	def on_filemanager_exit(self, *args):
		self.file_manager.close()

	def exit(self, type_sync):
		self.type = type_sync;
		self.socket.sendall(str(self.type).encode('utf-8'))
		self.connection.disconnect()
		exit()

	def	setTime(self):
		name = self.socket.recv(4096).decode().rstrip("\x00")
		times = self.socket.recv(15).decode().rstrip("\x00")
		n_ctime = datetime(int(times[0:4]),
							int(times[4:6]),
							int(times[6:8]),
							int(times[8:10]),
							int(times[10:12]),
							int(times[12:14]))
		modTime = time.mktime(n_ctime.timetuple())
		os.utime(name, (modTime, modTime))

	def	give_c_files(self):
		lists=os.listdir(path=".")
		lists.sort()
		with open(".c_files", 'w') as f:
			for lst in lists:
				if (lst != '.c_files'):
					f.write(lst
							+ ' '
							+ datetime.fromtimestamp(os.path.getmtime(lst)).strftime('%Y%m%d%H%M%S'))
					if (os.path.isdir(lst)):
						f.write(" DIR")
					f.write("\n")
		with open(".c_files", 'rb') as f:
			i = self.socket.sendfile(f)
			if (i == 0):
				j = self.socket.send(b'\x00')
		os.remove(".c_files")

	def	sndFiles(self):
		name = self.socket.recv(4096).decode().rstrip("\x00")
		with open(name, 'rb') as f:
			i = self.socket.sendfile(f)
			if (i == 0):
				j = self.socket.send(b'\x00')

	def	recFiles(self):
		name = self.socket.recv(4096).decode().rstrip("\x00")
		with open (name, 'wb') as f:
			while True:
				packeg = self.socket.recv(4096)
				if packeg == b'ENDFILE\x00':
					break
				f.write(packeg)

	def	changeDir(self):
		dirs = self.socket.recv(4096).decode().rstrip("\x00")
		os.chdir(dirs)

	def makeDir(self):
		dirs = self.socket.recv(4096).decode().rstrip("\x00")
		os.mkdir(dirs, 0o755)
		os.chdir(dirs)

	def printConfilct(self):
		name = self.socket.recv(4096).decode().rstrip("\x00")
		toast(name)

	def	checkCmd(self, cmd):
		if (cmd == "give_c"):
			self.give_c_files()
		elif (cmd == "send"):
			self.sndFiles()
		elif (cmd == "rec"):
			self.recFiles()
		elif (cmd == "set_time"):
			self.setTime()
		elif (cmd == "cd"):
			self.changeDir()
		elif (cmd == "mkdir && cd"):
			self.makeDir()
		elif (cmd == "err"):
			self.printConfilct()

	def sync_file(self, path):
		self.socket.sendall(str(self.type).encode('utf-8'))
		if platform == 'android':
			self.socket.sendall(str(pathlib.Path(path)).encode('utf-8')[20:])
		else:
			self.socket.sendall(str(pathlib.Path(path)).encode('utf-8'))
		os.chdir(path)
		while True:
			cmd = self.socket.recv(4096).decode().rstrip("\x00")
			if (cmd == "bye"):
				break
			self.checkCmd(cmd)
		toast("Synchronization completed!")
