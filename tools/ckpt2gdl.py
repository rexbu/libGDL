#-*- coding: utf-8 -*- 
'''
 * file :	read.py
 * author :	Rex
 * create :	2017-12-11 11:03
 * func : 
 * history:
'''

import os, sys, getopt, struct, math
import numpy as np
from tensorflow.python import pywrap_tensorflow
'''
conv_layers = []
for op, value in opts:
	if op == "-i":
		input_file = value
	elif op == "-o":
		output_file = value
	elif op == '-l':
		layers = value.split(",")
		for layer in layers:
			ls = layer.split(":")
			names = ls[0].split("|")
			print names
			# 3个元素分别为卷积层名字、bias名字、是否为全连接层
			conv_layers.append((names[0], names[1], ls[1]))
	elif op == "-h":
		print "python ckpt2gdl -i [input] -o [output] -l \"[covn1_w|conv1_b:c,covn1_w|conv1_b:f...]\" \
		\ncovn1_w:卷积层名字 \
		\nconv1_b:bias名字 \
		\nc:表示卷积层 \
		\nf:表示全连接层"
		sys.exit()
	else:
		print op+":"+value+" undefined command"
'''

def write_conv(model_file, wei, name):
	kernel_size = len(wei)
	channel_size = len(wei[0][0])
	kernel_count = len(wei[0][0][0])
	print "kernel_size[%d] channel[%d] kernel_count[%d]" % (kernel_size, channel_size, kernel_count)
	model_file.write(struct.pack("%ss"%len(name), name))
	model_file.write(struct.pack("c", "\0"))
	# 卷积核大小
	model_file.write(struct.pack("i", kernel_size))
	model_file.write(struct.pack("i", kernel_size))
	# 通道数
	model_file.write(struct.pack("i", channel_size))
	# 卷积核数量
	model_file.write(struct.pack("i", kernel_count))
	#卷积类型
	model_file.write(struct.pack("i", 0))
	# 卷积核遍历
	# for k in range(0, kernel_count):
	# 	# 通道遍历
	# 	for c in range(0, channel_size):
	# 		for h in range(0, kernel_size):
	# 			for w in range(0, kernel_size):
	# 				model_file.write(struct.pack("f", wei[h][w][c][k]))
	for h in range(0, kernel_count*kernel_size):
		k = h/kernel_size
		y = h%kernel_size
		for w in range(0, int(math.ceil(channel_size/4.0))*kernel_size):
			c = w/kernel_size
			x = w%kernel_size
			for ci in range(c*4, c*4+4):
				if ci < channel_size:
					model_file.write(struct.pack("f", wei[y][x][ci][k]))

def write_bias(model_file, bias):
	print "bias[%d] %f" % (len(bias), bias[0])
	for b in bias:
		model_file.write(struct.pack('f', b))

def write_tensor_layer(model_file, wei):
	if len(wei.shape)==4:
		# 1*h*w*c
		print "channel:%d size:[%d,%d]" % (len(wei[0][0][0]), len(wei[0][0]), len(wei[0]))
		for c in range(0, len(wei[0][0][0])):
			for h in range(0, len(wei[0])):
				for w in range(0, len(wei[0][h])):
					model_file.write(struct.pack("f", wei[0][h][w][c]))
	# 全连接
	elif len(wei.shape)==2:
		for c in range(0, len(wei)):
			for i in range(0, len(wei[0])):
				model_file.write(struct.pack("f", wei[c][i]))

def write_tensor_file(file, wei):
	model_file = open(file, "wb")
	write_tensor_layer(model_file, wei)
	model_file.close()

# active: 0不做任何处理, 1为relu, 2为tan
def write_convfull(model_file, wei, channel, name, active=1):
	kernel_size = len(wei)
	kernel_count = len(wei[0])
	print "kernel_size[%d] channel[%d] kernel_count[%d]" % (kernel_size, 1, kernel_count)
	model_file.write(struct.pack("%ss"%len(name), name))
	model_file.write(struct.pack("c", "\0"))
	# 卷积核大小
	model_file.write(struct.pack("i", kernel_size))
	# 全连接层卷积核是1维向量
	model_file.write(struct.pack("i", 1))
	# 通道数
	model_file.write(struct.pack("i", active))
	# 卷积核数量
	model_file.write(struct.pack("i", kernel_count))
	#卷积类型
	model_file.write(struct.pack("i", 1))
	'''
	for k in range(0, kernel_count):
		for c in range(0, kernel_size):
			model_file.write(struct.pack("f", wei[c][k]))
	'''
	
	c_width = channel[0]
	c_height = channel[1]
	c_x_count = channel[2]
	c_y_count = channel[3]
	c_count = c_x_count*c_y_count*4
	for k in range(0, kernel_count):
		for h in range(0, c_height*c_y_count):
			for w in range(0, c_width*c_x_count):
				# 属于第几个通道
				c_h = h/c_height
				c_w = w/c_width
				c = (c_h*c_x_count + c_w)*4
				# 在通道内的坐标
				c_y = h%c_height
				c_x = w%c_width
				for ci in range(c, c+4):
					# tensorflow的reshape函数会将一个点的不同通道放到一起
					i = c_y*c_width*c_count + c_x*c_count + ci
					model_file.write(struct.pack("f", wei[i][k]))


def write_full(model_file, wei, name, active=1):
	kernel_size = len(wei)
	kernel_count = len(wei[0])
	print "kernel_size[%d] channel[%d] kernel_count[%d]" % (kernel_size, 1, kernel_count)
	model_file.write(struct.pack("%ss"%len(name), name))
	model_file.write(struct.pack("c", "\0"))
	# 卷积核大小
	model_file.write(struct.pack("i", kernel_size))
	# 全连接层卷积核是1维向量
	model_file.write(struct.pack("i", 1))
	# 通道数
	model_file.write(struct.pack("i", active))
	# 卷积核数量
	model_file.write(struct.pack("i", kernel_count))
	#卷积类型
	model_file.write(struct.pack("i", 1))

	for k in range(0, kernel_count):
		for i in range(0, kernel_size):
			model_file.write(struct.pack("f", wei[i][k]))

if __name__ == '__main__':
	opts, args = getopt.getopt(sys.argv[1:], "hl:i:o:")
	input_file = None
	output_file = "./mnist.gdl"
	if input_file==None:
		input_file = 'models/model.ckpt'
	# Read data from checkpoint file
	reader = pywrap_tensorflow.NewCheckpointReader(input_file)
	# 用于遍历key
	var_to_shape_map = reader.get_variable_to_shape_map()
	model_file = open(output_file, "wb")

	wei = reader.get_tensor('W_conv1')
	bias = reader.get_tensor('b_conv1')
	write_conv(model_file, wei, 'conv1')
	write_bias(model_file, bias)

	wei = reader.get_tensor('W_conv2')
	bias = reader.get_tensor('b_conv2')
	write_conv(model_file, wei, 'conv2')
	write_bias(model_file, bias)

	wei = reader.get_tensor('W_fc1')
	bias = reader.get_tensor('b_fc1')
	write_convfull(model_file, wei, (7,7,4,4), 'fc1')
	write_bias(model_file, bias)

	wei = reader.get_tensor('W_fc2')
	bias = reader.get_tensor('b_fc2')
	write_full(model_file, wei, 'fc2')
	write_bias(model_file, bias)