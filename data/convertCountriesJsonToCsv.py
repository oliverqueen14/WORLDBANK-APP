import json
import sys

with open('data/countries.json', 'r') as file:
	data = json.load(file)[1]

text = ''
for country in data:
	if country['capitalCity'] == '' or country['capitalCity'] == None:
		continue
	else:
		text += country['id'] + '|' + country['name'] +'|'+ country['region']['value'] + '|' + '\n'

with open('data/countries.csv', 'w') as file:
	file.write(text)