#!/usr/bin/python3

import os
import requests

def uploadChunks(filePath, url, chunkSize=600):
	totalSize = os.path.getsize(filePath)
	numChunks = totalSize // chunkSize + (1 if totalSize % chunkSize != 0 else 0)

	with open(filePath, 'rb') as file:
		for chunkNumber in range(numChunks):
			# startByte = chunkNumber * chunkSize
			# endByte = min((chunkNumber + 1) * chunkSize, totalSize)
			chunkData = file.read(chunkSize)
			chunkData = f"{len(chunkData):X}\r\n{chunkData}\r\n"

			headers = {'Transfer-Encoding': 'chunked'}
			data = {'filename': 'picture.png'}
			if (chunkNumber == 0):
				response = requests.post(url, data=chunkData, headers=headers, files=data)
			else:
				response = requests.post(url, data=chunkData)

			if response.status_code != 200:
				print(f"Error uploading chunk {chunkNumber + 1}. Status code: {response.status_code}")
				return

			print(f"Uploaded chunk {chunkNumber + 1}/{numChunks}")

filePath = "/mnt/g/webserve/picture.png"
url = "http://localhost:8014/cgi-bin/upload.py"

uploadChunks(filePath, url)
