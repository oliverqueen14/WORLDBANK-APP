import json
import sys

def jsonToCsv(countryId, indicatorId):
	jsonFile = 'data/' + countryId + '/' + indicatorId + '.json'
	csvFile = 'data/' + countryId + '/' + indicatorId + '.csv'
	with open(jsonFile, 'r') as file:
		data = json.load(file)
	if data[0]['total'] == 0:
		with open(csvFile, 'w') as file:
			file.write('')
		return
	data = data[1]
	text = ''
	for line in data:
		if line['value'] == None or line['date'] == None:
			continue
		if line['unit'] == None:
			line['unit'] = 'NULL'
		text += str(line['date']) + '|' + str(line['value']) + '|' + str(line['unit']) + '\n'
	with open(csvFile, 'w') as file:
		file.write(text)

jsonToCsv(sys.argv[1], sys.argv[2])