import json
import sys

with open('data/indicators.json', 'r') as file:
	data = json.load(file)[1]

text = ''
for indicator in data:
	text += indicator['id'] + '|' + indicator['name'] +'|'+ indicator['sourceNote'] + '|' + '\n'

with open('data/indicators.csv', 'w') as file:
	file.write(text)