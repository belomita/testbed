from threading import Thread
import time
from Queue import Queue
import random

import os
import os.path

class Worker(Thread):
	def __init__(self, queue):
		Thread.__init__(self)
		self.queue = queue
	
	def run(self):
		while True:
			item = self.queue.get()
			#time.sleep(random.random() * 2) # seconds
			print('-------------start job ' + item)
			os.system(item)
			self.queue.task_done()

class Dispatcher:
	def __init__(self):
		self.queue = Queue()
		self.worker_num = 1
		self.jobs = []
	
	def prepare_jobs(self):
		for i in xrange(16):
			self.jobs.append(
				'ping -n '+ str(random.randint(5, 10)) + 
				' www.baidu.com > ' + str(i) + '.txt')
	
	def create_workers(self):
		for i in xrange(self.worker_num):
			t = Worker(self.queue)
			t.daemon = True
			t.start()
	def do_jobs(self):
		print('----------------start do jobs')
		for job in self.jobs:
			self.queue.put(job)
		
		self.queue.join()
		print('----------------all jobs done')

def test():
	d = Dispatcher()
	d.prepare_jobs()
	d.create_workers()
	d.do_jobs()

if __name__ == '__main__':
	from timeit import Timer
	t1=Timer("test()","from __main__ import test")
	print t1.timeit(1)
