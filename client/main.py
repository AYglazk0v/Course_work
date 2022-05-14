from kivymd.app import MDApp
from kivy.uix.screenmanager import Screen, ScreenManager
from screens.send import ClientScreen

class MainScreen(Screen):
	pass

class MainApp(MDApp):
	def build(self):
		self.sm = ScreenManager()
		self.sm.add_widget(MainScreen())
		self.sm.add_widget(ClientScreen())
		self.sm.current = 'send_screen'
		return (self.sm)

if __name__ == '__main__':
	MainApp().run()
