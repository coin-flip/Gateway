
from sys import argv
import urllib
import json


filename , searchQuery = argv

encoded = urllib.quote(searchQuery)

rawData = urllib.urlopen('http://ajax.googleapis.com/ajax/services/search/web?v=1.0&q=' + encoded).read()

jsonData = json.loads(rawData)

searchResults = jsonData['responseData']['results']

output = "["

i = 0;
for result in searchResults:

	if i > 0:
		output = output + ","
	
	title = result['title']
	link = result['url']
	i = i+ 1
	output = output + "{"
	output = output + "\"title\" : " + '"' + title + '"' + ","
	output = output +"\"link\" : " + '"' + link + '"'
	output = output +"}"
	


output = output + "]"

print output.encode('utf-8')

